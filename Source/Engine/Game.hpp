// Author: Jake Rieger
// Created: 11/27/25.
//

#pragma once

#include "CommonPCH.hpp"
#include "Clock.hpp"
#include "RenderContext.hpp"

namespace N {
    /// @brief Base class where all engine systems meet and execute. Owns the system window and handles communication
    /// between the OS, graphics API, and game client code (among many other things).
    class Game {
    public:
        static constexpr u32 kDefaultWidth {800};
        static constexpr u32 kDefaultHeight {600};

        N_CLASS_PREVENT_MOVES_COPIES(Game)

        Game() = default;
        Game(string title, u32 width, u32 height) : mWidth(width), mHeight(height), mTitle(std::move(title)) {}

        virtual ~Game() = default;

        void Run();
        void Quit();
        void ToggleFullscreen();

        void SetTitle(const string& title);
        void Resize(u32 width, u32 height);

        // Window callbacks
        virtual void OnResize(u32 width, u32 height) {}
        virtual void OnKeyDown(u32 keyCode) {}
        virtual void OnKeyUp(u32 keyCode) {}
        virtual void OnKey(u32 keyCode) {}
        virtual void OnMouseButtonDown(u32 button) {}
        virtual void OnMouseButtonUp(u32 button) {}
        virtual void OnMouseButton(u32 button) {}
        virtual void OnMouseMove(f64 dX, f64 dY) {}
        virtual void OnMouseScroll(f64 dX, f64 dY) {}

        // Lifecycle hooks
        virtual void OnAwake() {}
        virtual void OnUpdate(const Clock& clock) {}
        virtual void OnLateUpdate() {}
        virtual void OnDestroyed() {}

        // Getters
        void GetWindowSize(u32& outWidth, u32& outHeight) const {
            outWidth  = mWidth;
            outHeight = mHeight;
        }

        N_ND bool GetRunning() const {
            return mRunning;
        }

        N_ND bool GetFullscreen() const {
            return mFullscreen;
        }

        N_ND const GLFWwindow* GetWindowHandle() const {
            return mWindow;
        }

        N_ND std::string_view GetWindowTitle() const {
            return mTitle;
        }

        N_ND RenderContext& GetRenderContext() {
            return mRenderContext;
        }

    protected:
        void Render();

    private:
        bool Initialize();
        void Shutdown() const;

        // GLFW callbacks
        static void GLFWResizeCallback(GLFWwindow* window, s32 width, s32 height);
        static void GLFWKeyCallback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
        static void GLFWMouseButtonCallback(GLFWwindow* window, s32 button, s32 action, s32 mods);
        static void GLFWMouseCursorCallback(GLFWwindow* window, f64 xpos, f64 ypos);
        static void GLFWMouseScrollCallback(GLFWwindow* window, f64 xoffset, f64 yoffset);

        // Window properties
        GLFWwindow* mWindow {nullptr};
        u32 mWidth {kDefaultWidth};
        u32 mHeight {kDefaultHeight};
        string mTitle {};
        bool mRunning {false};
        bool mFullscreen {false};

        // Internal systems
        Clock mClock;
        RenderContext mRenderContext;
    };
}  // namespace N
