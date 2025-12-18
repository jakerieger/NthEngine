/*
 *  Filename: ImGuiDebugLayer.hpp
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
#include "DebugInterface.hpp"

#include <imgui.h>

namespace Astera {
    class ImGuiDebugLayer final : public IDebugOverlay {
    public:
        explicit ImGuiDebugLayer(GLFWwindow* window);
        ~ImGuiDebugLayer() override;

        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnEvent(const Event& event) override;

        void ShowPerformanceOverlay(bool show) {
            mPerfOverlay = show;
        }

        void UpdateFrameRate(f32 rate) {
            mFrameStats.frameRate = rate;
        }

        void UpdateFrameTime(f32 time) {
            mFrameStats.frameTime = time;
        }

        void UpdateMainThreadTime(f32 time) {
            mFrameStats.mainThreadTime = time;
        }

        void UpdateRenderThreadTime(f32 time) {
            mFrameStats.renderThreadTime = time;
        }

        void UpdateDrawCalls(u32 drawCalls) {
            mFrameStats.drawCalls = drawCalls;
        }

        void UpdateEntities(u32 entities) {
            mSceneStats.entities = entities;
        }

        void UpdateResourcePoolAllocatedBytes(u64 allocatedBytes) {
            mSceneStats.resourcePoolAllocatedBytes = allocatedBytes;
        }

        void UpdateResourcePoolUsedBytes(u64 usedBytes) {
            mSceneStats.resourcePoolUsedBytes = usedBytes;
        }

        void SetCustomText(const string& header, const vector<string>& lines) {
            mCustomText       = lines;
            mCustomTextHeader = header;
        }

    private:
        void InitImGui(GLFWwindow* window) const;
        void ShutdownImGui() const;

        bool mPerfOverlay {true};

        struct FrameStats {
            f32 frameRate {0.f};
            f32 frameTime {0.f};
            f32 mainThreadTime {0.f};
            f32 renderThreadTime {0.f};
            u32 drawCalls {0};
        } mFrameStats;

        void DrawStats();

        void DrawCustomText() const;

        struct SceneStats {
            u32 entities {0};
            u64 resourcePoolAllocatedBytes {0};
            u64 resourcePoolUsedBytes {0};
        } mSceneStats;

        ImVec2 mStatsSize;

        vector<string> mCustomText;
        string mCustomTextHeader;
    };
}  // namespace Astera
