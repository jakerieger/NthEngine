/*
 *  Filename: ImGuiDebugLayer.cpp
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

#include "ImGuiDebugLayer.hpp"

#include "Color.hpp"
#include "Macros.hpp"
#include "Log.hpp"
#include "Math.hpp"

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

namespace Astera {
    ImGuiDebugLayer::ImGuiDebugLayer(GLFWwindow* window) {
        InitImGui(window);
    }

    ImGuiDebugLayer::~ImGuiDebugLayer() {
        ShutdownImGui();
    }

    void ImGuiDebugLayer::OnUpdate(float deltaTime) {}

    void ImGuiDebugLayer::OnRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        DrawStats();

        if (!mCustomText.empty())
            DrawCustomText();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiDebugLayer::OnEvent(const Event& event) {}

    void ImGuiDebugLayer::InitImGui(GLFWwindow* window) const {
        ASTERA_UNUSED(this);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.IniFilename = nullptr;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        Log::Info("ImGuiDebugLayer", "Debug layer initialized");
    }

    void ImGuiDebugLayer::ShutdownImGui() const {
        ASTERA_UNUSED(this);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    /// @brief Calculates the value order-of-magnitude and returns the resized value + corresponding suffix
    static f32 CalcBytesOOM(u64 value, string& suffix) {
        if (value <= 1_KB) {
            suffix = "B";
            return f32(value);
        }

        if (value <= 1_MB) {
            suffix = "KB";
            return f32(value) / 1_KB;
        }

        if (value <= 1_GB) {
            suffix = "MB";
            return f32(value) / 1_MB;
        }

        if (value <= 1_TB) {
            suffix = "GB";
            return f32(value) / 1_GB;
        }

        return f32(value);
    }

    void ImGuiDebugLayer::DrawStats() {
        ImGui::SetNextWindowPos({0, 0});
        ImGui::Begin("Performance",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

        ImGui::Text("Frame Stats");
        ImGui::Separator();
        ImGui::TextColored(Colors::Green.To<ImVec4>(),
                           "Frame Rate     %u FPS",
                           (u32)Math::Round(mFrameStats.frameRate));
        ImGui::TextColored(Colors::Green.To<ImVec4>(), "Frame Time     %.2f ms", mFrameStats.frameTime);
        ImGui::TextColored(Colors::Magenta.To<ImVec4>(), "Main Thread    %.2f ms", mFrameStats.mainThreadTime);
        ImGui::TextColored(Colors::Magenta.To<ImVec4>(), "Render Thread  %.2f ms", mFrameStats.renderThreadTime);
        ImGui::TextColored(Colors::Cyan.To<ImVec4>(), "Draw Calls     %u", mFrameStats.drawCalls);

        ImGui::Dummy({0, 20.f});
        ImGui::Text("Scene Stats");
        ImGui::Separator();
        ImGui::TextColored(Colors::Cyan.To<ImVec4>(), "Entities                   %u", mSceneStats.entities);

        string allocatedSuffix, freeSuffix, usedSuffix;
        const f32 allocatedOOM = CalcBytesOOM(mSceneStats.resourcePoolAllocatedBytes, allocatedSuffix);
        const f32 freeOOM =
          CalcBytesOOM(mSceneStats.resourcePoolAllocatedBytes - mSceneStats.resourcePoolUsedBytes, freeSuffix);
        const f32 usedOOM = CalcBytesOOM(mSceneStats.resourcePoolUsedBytes, usedSuffix);

        ImGui::TextColored(Colors::Yellow.To<ImVec4>(),
                           "Resource Pool (Allocated)  %.1f %s",
                           allocatedOOM,
                           allocatedSuffix.c_str());
        ImGui::TextColored(Colors::Yellow.To<ImVec4>(),
                           "Resource Pool (Free)       %.1f %s",
                           freeOOM,
                           freeSuffix.c_str());
        ImGui::TextColored(Colors::Yellow.To<ImVec4>(),
                           "Resource Pool (Used)       %.1f %s",
                           usedOOM,
                           usedSuffix.c_str());

        mStatsSize = ImGui::GetWindowSize();

        ImGui::End();
    }

    void ImGuiDebugLayer::DrawCustomText() const {
        ImGui::SetNextWindowPos({0, mStatsSize.y + 40});
        ImGui::Begin(mCustomTextHeader.c_str(),
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

        ImGui::Text("%s", mCustomTextHeader.c_str());
        ImGui::Separator();

        for (auto& line : mCustomText) {
            ImGui::Text("%s", line.c_str());
        }

        ImGui::End();
    }
}  // namespace Astera