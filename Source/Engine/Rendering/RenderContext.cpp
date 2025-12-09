/// @author Jake Rieger
/// @created 11/27/25
///
#include "RenderContext.hpp"
#include "Log.hpp"
#include "GLUtils.hpp"

namespace Nth {
    bool RenderContext::Initialize(u32 width, u32 height) {
        if (mInitialized) return true;

        mWidth  = width;
        mHeight = height;

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Log::Critical("RenderContext", "Failed to load GLAD");
            return false;
        }

        GLCall(glViewport, 0, 0, (i32)mWidth, (i32)mHeight);

        // Enable depth testing (common OpenGL setup)
        GLCall(glEnable, GL_DEPTH_TEST);

        // Enable blending for transparency
        GLCall(glEnable, GL_BLEND);
        GLCall(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    void RenderContext::EndFrame() {
        N_ASSERT(mInitialized);
        mCommandQueue.ExecuteQueue();
    }

    void RenderContext::Resize(u32 width, u32 height) {
        N_ASSERT(mInitialized);
        mWidth  = width;
        mHeight = height;
        Submit(SetViewportCommand {0, 0, width, height});
    }
}  // namespace Nth