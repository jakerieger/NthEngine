// Author: Jake Rieger
// Created: 11/27/25.
//

#pragma once

#include "CommonPCH.hpp"

namespace N {
    class RenderContext {
    public:
        RenderContext()  = default;
        ~RenderContext() = default;

        N_CLASS_PREVENT_MOVES_COPIES(RenderContext)

        bool Initialize(u32 width, u32 height);
        void Shutdown() const;

        void BeginFrame() const;
        void EndFrame(GLFWwindow* window) const;

        void Resize(u32 width, u32 height);

    private:
        u32 mWidth {0};
        u32 mHeight {0};
        bool mInitialized {false};
    };
}  // namespace N
