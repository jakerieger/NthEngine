/// @author Jake Rieger
/// @created 11/27/25
///

#pragma once

#include "Common/CommonPCH.hpp"
#include "CommandQueue.hpp"

namespace Nth {
    class RenderContext {
    public:
        RenderContext()  = default;
        ~RenderContext() = default;

        N_CLASS_PREVENT_MOVES_COPIES(RenderContext)

        bool Initialize(u32 width, u32 height);
        void Shutdown() const;

        void BeginFrame();
        void EndFrame();

        void Resize(u32 width, u32 height);

        N_ND CommandQueue& GetCommandQueue() {
            return mCommandQueue;
        }

        /// @brief Submit a command to the render queue
        template<typename T>
        void Submit(T&& command) {
            mCommandQueue.Enqueue(std::forward<T>(command));
        }

        N_ND bool GetInitialized() const {
            return mInitialized;
        }

        void GetViewportDimensions(u32& outWidth, u32& outHeight) const {
            outWidth  = mWidth;
            outHeight = mHeight;
        }

    private:
        u32 mWidth {0};
        u32 mHeight {0};
        bool mInitialized {false};

        CommandQueue mCommandQueue;
    };
}  // namespace Nth
