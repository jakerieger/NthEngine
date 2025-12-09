// Author: Jake Rieger
// Created: 12/9/25.
//

#include "ImGuiDebugLayer.hpp"
#include "Macros.hpp"
#include "Log.hpp"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

namespace Nth {
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

        if (mPerfOverlay) DrawPerformanceOverlay();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiDebugLayer::OnEvent(const Event& event) {}

    void ImGuiDebugLayer::InitImGui(GLFWwindow* window) const {
        N_UNUSED(this);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        Log::Info("ImGuiDebugLayer", "Debug layer initialized");
    }

    void ImGuiDebugLayer::ShutdownImGui() const {
        N_UNUSED(this);
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiDebugLayer::DrawPerformanceOverlay() {
        ImGui::Begin("Performance");

        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

        static float values[90]  = {};
        static int values_offset = 0;
        values[values_offset]    = ImGui::GetIO().Framerate;
        values_offset            = (values_offset + 1) % IM_ARRAYSIZE(values);

        ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, nullptr, 0.0f, 100.0f, ImVec2(0, 80));

        ImGui::End();
    }
}  // namespace Nth