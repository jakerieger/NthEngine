/*
 *  Filename: SoundLoader.hpp
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

#include "ResourceManager.hpp"
#include "Sound.hpp"

#include <sndfile.hh>

namespace Astera {
    class SoundLoader final : public ResourceLoader<Sound> {
        Sound LoadImpl(RenderContext& context, ArenaAllocator& allocator, const u64 id) override {
            const auto assetPath = AssetManager::GetAssetPath(id);
            if (!assetPath.has_value()) {
                throw std::runtime_error(fmt::format("Failed to get sound asset: {}", assetPath.error()));
            }

            SndfileHandle handle(assetPath->string());
            const i64 bufferSize = handle.frames() * handle.channels();
            auto* buffer         = (f32*)allocator.Allocate(bufferSize, alignof(f32));
            handle.readf(buffer, handle.frames());

            return Sound {buffer, size_t(bufferSize)};
        }
    };
}  // namespace Astera
