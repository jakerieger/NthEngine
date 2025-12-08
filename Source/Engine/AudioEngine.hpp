// Author: Jake Rieger
// Created: 12/8/25.
//

#pragma once

#include "Common/CommonPCH.hpp"

#include <miniaudio.h>

namespace Nth {
    using SoundID = i32;
    static inline constexpr SoundID kInvalidSoundID {-1};

    class AudioEngine {
    public:
        AudioEngine();
        ~AudioEngine();

        bool Initialize();
        void Shutdown();

        SoundID LoadSound(const fs::path& filename);
        bool PlaySound(SoundID id, bool loop = false);
        void StopSound(SoundID id);
        void StopAllSounds() const;
        void SetMasterVolume(f32 vol);
        void SetSoundVolume(SoundID id, f32 vol);

        N_ND bool IsInitialized() const {
            return mInitialized;
        }

    private:
        struct Sound {
            ma_decoder decoder;
            ma_sound sound;
            string filepath;
            bool loaded = false;
        };

        bool mInitialized {false};
        ma_engine mEngine {};
        SoundID mNextSoundId {1};
        std::unordered_map<SoundID, unique_ptr<Sound>> mSounds;

        Sound* GetSound(SoundID id);
    };
}  // namespace Nth
