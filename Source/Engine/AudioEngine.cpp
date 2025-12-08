// Author: Jake Rieger
// Created: 12/8/25.
//

#include "AudioEngine.hpp"
#include "Log.hpp"

namespace Nth {
    AudioEngine::AudioEngine() = default;

    AudioEngine::~AudioEngine() {
        Shutdown();
    }

    bool AudioEngine::Initialize() {
        if (mInitialized) {
            Log::Warn("AudioEngine", "Engine is already initialized.");
            return true;
        }

        const ma_result result = ma_engine_init(nullptr, &mEngine);
        if (result != MA_SUCCESS) {
            Log::Error("AudioEngine", "Failed to initialize audio engine: {}", (i32)result);
            return false;
        }

        mInitialized = true;
        Log::Info("AudioEngine", "Engine initialized successfully");
        return true;
    }

    void AudioEngine::Shutdown() {
        if (!mInitialized) { return; }

        for (const auto& pair : mSounds) {
            if (pair.second && pair.second->loaded) { ma_sound_uninit(&pair.second->sound); }
        }
        mSounds.clear();

        ma_engine_uninit(&mEngine);
        mInitialized = false;

        Log::Info("AudioEngine", "Engine shutdown");
    }

    SoundID AudioEngine::LoadSound(const fs::path& filename) {
        if (!mInitialized) { Initialize(); }

        if (!exists(filename)) {
            Log::Error("AudioEngine", "Attempted to load missing file: {}", filename.string());
            return kInvalidSoundID;
        }

        auto sound      = std::make_unique<Sound>();
        sound->filepath = filename.string();

        const ma_result result =
          ma_sound_init_from_file(&mEngine, filename.string().c_str(), 0, nullptr, nullptr, &sound->sound);

        if (result != MA_SUCCESS) {
            Log::Error("AudioEngine", "Failed to load sound: {} (error: {})", filename.string(), (i32)result);
            return kInvalidSoundID;
        }

        sound->loaded    = true;
        i32 soundId      = mNextSoundId++;
        mSounds[soundId] = std::move(sound);

        Log::Info("AudioEngine", "Loaded sound: {} (ID: {})", filename.string(), soundId);
        return soundId;
    }

    bool AudioEngine::PlaySound(SoundID id, bool loop) {
        auto* sound = GetSound(id);
        if (!sound) {
            Log::Error("AudioEngine", "Sound ID {} not found", id);
            return false;
        }

        ma_sound_set_looping(&sound->sound, loop ? MA_TRUE : MA_FALSE);

        ma_sound_seek_to_pcm_frame(&sound->sound, 0);

        const ma_result result = ma_sound_start(&sound->sound);
        if (result != MA_SUCCESS) {
            Log::Error("AudioEngine", "Failed to play sound ID {} (error: {})", id, (i32)result);
            return false;
        }

        return true;
    }

    void AudioEngine::StopSound(SoundID id) {
        Sound* sound = GetSound(id);
        if (!sound) {
            Log::Error("AudioEngine", "Sound ID {} not found", id);
            return;
        }

        ma_sound_stop(&sound->sound);
    }

    void AudioEngine::StopAllSounds() const {
        for (const auto& pair : mSounds) {
            if (pair.second && pair.second->loaded) { ma_sound_stop(&pair.second->sound); }
        }
    }

    void AudioEngine::SetMasterVolume(f32 vol) {
        if (!mInitialized) { Initialize(); }

        vol = std::max(0.0f, std::min(1.0f, vol));
        ma_engine_set_volume(&mEngine, vol);
    }

    void AudioEngine::SetSoundVolume(SoundID id, f32 vol) {
        Sound* sound = GetSound(id);
        if (!sound) {
            Log::Error("AudioEngine", "Sound ID {} not found", id);
            return;
        }

        vol = std::max(0.0f, std::min(1.0f, vol));
        ma_sound_set_volume(&sound->sound, vol);
    }

    AudioEngine::Sound* AudioEngine::GetSound(SoundID id) {
        const auto it = mSounds.find(id);
        if (it == mSounds.end() || !it->second->loaded) { return nullptr; }
        return it->second.get();
    }
}  // namespace Nth