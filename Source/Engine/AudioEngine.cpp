/*
 *  Filename: AudioEngine.cpp
 *  This code is part of the Astera core library
 *  Copyright 2025 Jake Rieger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include "AudioEngine.hpp"
#include "Log.hpp"
#include "Math.hpp"

#include <sol/sol.hpp>

namespace Astera {
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

        if (const ma_result result = ma_sound_start(&sound->sound); result != MA_SUCCESS) {
            Log::Error("AudioEngine", "Failed to play sound ID {} (error: {})", id, CAST<i32>(result));
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
        for (const auto& val : mSounds | std::views::values) {
            if (val && val->loaded) { ma_sound_stop(&val->sound); }
        }
    }

    void AudioEngine::SetMasterVolume(f32 vol) {
        if (!mInitialized) { Initialize(); }

        vol = Math::Max(0.0f, Math::Min(1.0f, vol));
        ma_engine_set_volume(&mEngine, vol);
    }

    void AudioEngine::SetSoundVolume(SoundID id, f32 vol) {
        Sound* sound = GetSound(id);
        if (!sound) {
            Log::Error("AudioEngine", "Sound ID {} not found", id);
            return;
        }

        vol = Math::Max(0.0f, Math::Min(1.0f, vol));
        ma_sound_set_volume(&sound->sound, vol);
    }

    AudioEngine::Sound* AudioEngine::GetSound(SoundID id) {
        const auto it = mSounds.find(id);
        if (it == mSounds.end() || !it->second->loaded) { return nullptr; }
        return it->second.get();
    }

    void AudioEngine::RegisterLuaGlobals(sol::state& lua) {
        auto engine               = lua.new_usertype<AudioEngine>("AudioEngine");
        engine["LoadSound"]       = &AudioEngine::LoadSound;
        engine["PlaySound"]       = &AudioEngine::PlaySound;
        engine["StopSound"]       = &AudioEngine::StopSound;
        engine["StopAllSounds"]   = &AudioEngine::StopAllSounds;
        engine["SetMasterVolume"] = &AudioEngine::SetMasterVolume;
        engine["SetSoundVolume"]  = &AudioEngine::SetSoundVolume;
        engine["IsInitialized"]   = &AudioEngine::IsInitialized;

        lua["AudioPlayer"] = this;
    }
}  // namespace Astera