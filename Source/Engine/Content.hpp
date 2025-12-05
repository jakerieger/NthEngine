/// @author Jake Rieger
/// @created 11/30/25
///

#pragma once

#include "CommonPCH.hpp"

namespace Nth {
    /// @brief Type of content the container holds
    enum class ContentType {
        Audio,
        Scene,
        Script,
        Shader,
        Sprite,
    };

    /// @brief Content container and helper class
    class Content {
        static constexpr std::string_view kSceneRoot  = "Scenes";
        static constexpr std::string_view kScriptRoot = "Scripts";
        static constexpr std::string_view kShaderRoot = "Shaders";
        static constexpr std::string_view kSpriteRoot = "Sprites";
        static constexpr std::string_view kAudioRoot  = "Audio";

        inline static fs::path sContentRoot;
        inline static fs::path sEngineContentRoot;

    public:
        template<ContentType type, bool engine = false>
        inline static fs::path Get(const string& filename) {
            fs::path contentPath = sContentRoot;
            if constexpr (engine) { contentPath = sEngineContentRoot; }

            if constexpr (type == ContentType::Audio) {
                return contentPath / kAudioRoot / filename;
            } else if constexpr (type == ContentType::Scene) {
                return contentPath / kSceneRoot / filename;
            } else if constexpr (type == ContentType::Script) {
                return contentPath / kScriptRoot / filename;
            } else if constexpr (type == ContentType::Shader) {
                return contentPath / kShaderRoot / filename;
            } else if constexpr (type == ContentType::Sprite) {
                return contentPath / kSpriteRoot / filename;
            } else {
                throw std::invalid_argument("Invalid content type");
            }
        }

        inline static void SetContentPath(const fs::path& path) {
            sContentRoot = path;
        }

        inline static void SetEngineContentPath(const fs::path& path) {
            sEngineContentRoot = path;
        }

        inline static void SetRuntimeDefaults() {
            sContentRoot       = fs::current_path() / "Content";
            sEngineContentRoot = sContentRoot / "EngineContent";
        }
    };
}  // namespace Nth
