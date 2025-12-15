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
#include "../Macros.hpp"
#include "Log.hpp"

#include <imgui.h>
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

        if (mPerfOverlay)
            DrawPerformanceOverlay();

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

    void ImGuiDebugLayer::DrawPerformanceOverlay() {
        ImGui::SetNextWindowPos({0, 0});
        ImGui::Begin("Performance",
                     nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                       ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

        ImGui::Text("Frame Stats");
        ImGui::Separator();
        ImGui::TextColored(Colors::Green.To<ImVec4>(), "Frame Rate     %.1f FPS", ImGui::GetIO().Framerate);
        ImGui::TextColored(Colors::Green.To<ImVec4>(),
                           "Frame Time     %.6f ms",
                           (1.f / ImGui::GetIO().Framerate) * 1000.f);
        ImGui::TextColored(Colors::Magenta.To<ImVec4>(), "Main Thread    0 ms");
        ImGui::TextColored(Colors::Magenta.To<ImVec4>(), "Render Thread  0 ms");
        ImGui::TextColored(Colors::Cyan.To<ImVec4>(), "Draw Calls     0");
        ImGui::TextColored(Colors::Cyan.To<ImVec4>(), "Entities       0");

        ImGui::End();
    }
}  // namespace Astera