/*
 *  Filename: Asset.hpp
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

#pragma once

#include "EngineCommon.hpp"

namespace Astera {
    /// @brief Type alias for asset IDs. IDs use the highest 8-bits to store the asset type and the lower 56 bits as the
    /// unique identifier
    using AssetID = u64;

    /// @brief Single 8-bit identifier used to distinguish asset data types
    enum class AssetType : u8 {
        Audio          = 0,
        BinaryData     = 1,
        ParticleSystem = 2,
        Scene          = 3,
        Script         = 4,
        Shader         = 5,
        Sprite         = 6,
        SpriteSheet    = 7,
        TextData       = 8,
    };

    static constexpr u64 kAssetIdBitmask = 0x00FFFFFFFFFFFFFF;

    /// @brief Get the asset type from its ID
    /// @param id Asset ID
    inline AssetType AssetTypeFromID(u64 id) {
        return CAST<AssetType>(id & 0xFF);  // Mask the last 8 bits
    }
}  // namespace Astera