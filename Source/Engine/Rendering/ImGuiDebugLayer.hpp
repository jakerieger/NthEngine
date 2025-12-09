// Author: Jake Rieger
// Created: 12/9/25.
//

#pragma once

#include "CommonPCH.hpp"
#include "DebugInterface.hpp"

namespace Nth {
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

    private:
        void InitImGui(GLFWwindow* window) const;
        void ShutdownImGui() const;

        bool mPerfOverlay {true};

        static void DrawPerformanceOverlay();
    };
}  // namespace Nth
