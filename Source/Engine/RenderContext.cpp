// Author: Jake Rieger
// Created: 11/27/25.
//

#include "RenderContext.hpp"
#include "Log.hpp"

namespace N {
    bool RenderContext::Initialize(u32 width, u32 height) {
        if (mInitialized) return true;

        mWidth  = width;
        mHeight = height;

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Log::Critical("Failed to load GLAD");
            return false;
        }

        glViewport(0, 0, (s32)mWidth, (s32)mHeight);

        // Enable depth testing (common OpenGL setup)
        glEnable(GL_DEPTH_TEST);

        // Enable blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        mCommandQueue.Reserve(1000);
        mInitialized = true;

        return true;
    }

    void RenderContext::Shutdown() const {
        N_UNUSED(this);
    }

    void RenderContext::BeginFrame() {
        N_ASSERT(mInitialized);
        Submit(ClearCommand {{0.08f, 0.08f, 0.08f, 1.0f}, true, false});
    }

    void RenderContext::EndFrame(GLFWwindow* window) {
        N_ASSERT(mInitialized);
        mCommandQueue.ExecuteQueue();
        glfwSwapBuffers(window);
    }

    void RenderContext::Resize(u32 width, u32 height) {
        N_ASSERT(mInitialized);
        mWidth  = width;
        mHeight = height;
        Submit(SetViewportCommand {0, 0, width, height});
    }
}  // namespace N