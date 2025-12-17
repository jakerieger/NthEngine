/*
 *  Filename: AssetManager.cpp
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

#include "AssetManager.hpp"
#include "IO.hpp"
#include "Log.hpp"

#include <pugixml.hpp>

namespace Astera {
    Result<vector<u8>> AssetManager::GetAssetData(AssetID id) {
        if (!sInitialized) {
            Initialize();
        }

        if (!sAssetPaths.contains(id)) {
            return unexpected(fmt::format("Asset with given ID `{}` not found", id));
        }

        const auto assetPath = sAssetPaths[id];
        if (!exists(assetPath)) {
            return unexpected(fmt::format("Asset `{}` does not exist", assetPath.string()));
        }

        auto readResult = IO::ReadBytes(assetPath);
        if (readResult.has_value()) {
            return *readResult;
        } else {
            return unexpected(fmt::format("Failed to read asset `{}`", assetPath.string()));
        }
    }

    Result<string> AssetManager::GetAssetText(AssetID id) {
        if (!sInitialized) {
            Initialize();
        }

        if (!sAssetPaths.contains(id)) {
            return unexpected(fmt::format("Asset with given ID `{}` not found", id));
        }

        const auto assetPath = sAssetPaths[id];
        if (!exists(assetPath)) {
            return unexpected(fmt::format("Asset `{}` does not exist", assetPath.string()));
        }

        auto readResult = IO::ReadText(assetPath);
        if (readResult.has_value()) {
            return *readResult;
        } else {
            return unexpected(fmt::format("Failed to read asset `{}`", assetPath.string()));
        }
    }

    bool AssetManager::Initialize() {
        if (!exists(sWorkingDirectory)) {
            Log::Error("AssetManager", "Working directory does not exist");
            return false;
        }

        // Iterate over working directory looking for ".asset" files
        for (const auto& entry : fs::recursive_directory_iterator(sWorkingDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".asset") {
                const auto extLen = string(".asset").size();
                auto srcFilename =
                  entry.path().filename().string().substr(0, entry.path().filename().string().size() - extLen);
                const auto assetPath = entry.path().parent_path() / srcFilename;
                if (!exists(assetPath)) {
                    Log::Error("AssetManager", "Asset file `{}` does not exist", assetPath.string());
                    continue;
                }

                // Read asset ID from descriptor
                pugi::xml_document doc;
                doc.load_file(entry.path().string().c_str());
                const AssetID id = doc.child("Asset").attribute("id").as_ullong();

                sAssetPaths[id] = assetPath;

                Log::Info("AssetManager",
                          "Found asset `{}` with ID {} and type {}",
                          assetPath.string(),
                          id,
                          (u32)AssetTypeFromID(id));
            }
        }

        sInitialized = true;
        return true;
    }

    void AssetManager::Reload() {
        sInitialized = false;
        sAssetPaths.clear();
        Initialize();
    }

    void AssetManager::SetWorkingDirectory(const Path& path) {
        sWorkingDirectory = path;
    }

    void AssetManager::SetRuntimeDefaults() {
        sWorkingDirectory = fs::current_path() / "Content";
    }

    bool AssetManager::sInitialized = false;
    Path AssetManager::sWorkingDirectory {};
    unordered_map<AssetID, Path> AssetManager::sAssetPaths {};
}  // namespace Astera