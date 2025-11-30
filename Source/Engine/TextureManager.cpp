// Author: Jake Rieger
// Created: 11/28/25.
//

#include "TextureManager.hpp"
#include "Log.hpp"
#include "Rendering/GLUtils.hpp"

#include <stb_image.h>

namespace Nth {
    shared_ptr<TextureManager> TextureManager::sManager;
    unordered_map<string, TextureID> TextureManager::sCache;

    void TextureManager::Initialize() {
        sManager = make_shared<TextureManager>();
        Log::Info("TextureManager", "Initialized TextureManager");
    }

    void TextureManager::Shutdown() {
        if (!sCache.empty()) {
            for (const auto& id : sCache | std::views::values) {
                Log::Info("TextureManager", "Unloading texture id `{}`", id);
                GLCall(glDeleteTextures, 1, &id);
            }
        }
        sManager.reset();
    }

    TextureID TextureManager::GetTextureByName(const string& name) {
        return sCache.find(name)->second;
    }

    TextureID TextureManager::Load(const fs::path& filename) {
        u32 id;
        glGenTextures(1, &id);

        int w, h, channels;
        stbi_set_flip_vertically_on_load(true);  // For OpenGL
        const auto data = stbi_load(filename.string().c_str(), &w, &h, &channels, 0);
        if (!data) { Log::Error("TextureManager", "Failed to load image!"); }

        GLenum format = GL_RGBA;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        GLCall(glBindTexture, GL_TEXTURE_2D, id);
        GLCall(glTexImage2D, GL_TEXTURE_2D, 0, CAST<int>(format), w, h, 0, format, GL_UNSIGNED_BYTE, data);
        GLCall(glGenerateMipmap, GL_TEXTURE_2D);

        GLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        GLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        GLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        GLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        stbi_image_free(data);

        sCache[filename.string()] = id;
        Log::Info("TextureManager", "Loaded texture `{}` with OpenGL id `{}`", filename.string().c_str(), id);

        return id;
    }

    shared_ptr<TextureManager> TextureManager::GetManager() {
        if (!sManager) Initialize();
        return sManager;
    }
}  // namespace Nth