// Author: Jake Rieger
// Created: 11/27/25.
//

#include "Game.hpp"
#include "Log.hpp"

namespace N {
    using N::Log;

    void Game::Run() {
        mRunning = Initialize();
        if (!mRunning) return;
        OnAwake();
        {
            // Main loop
            while (mRunning && !glfwWindowShouldClose(mWindow)) {
                mClock.Tick();
                OnUpdate(mClock);
                Render();
                OnLateUpdate();
                glfwPollEvents();
            }

            mRunning = false;
        }
        OnDestroyed();
        Shutdown();
    }

    void Game::Quit() {
        mRunning = false;
    }

    void Game::ToggleFullscreen() {
        mFullscreen = !mFullscreen;
        throw N_NOT_IMPLEMENTED;
    }

    void Game::SetTitle(const string& title) {
        mTitle = title;
        glfwSetWindowTitle(mWindow, title.c_str());
    }

    void Game::Resize(u32 width, u32 height) {
        mWidth  = width;
        mHeight = height;
        glfwSetWindowSize(mWindow, (s32)width, (s32)height);
    }

    bool Game::Initialize() {
        if (!glfwInit()) {
            Log::Critical("Failed to initialize GLFW");
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        mWindow = glfwCreateWindow((s32)mWidth, (s32)mHeight, mTitle.c_str(), nullptr, nullptr);
        if (!mWindow) {
            glfwTerminate();
            Log::Critical("Failed to create GLFW window");
            return false;
        }

        glfwMakeContextCurrent(mWindow);
        glfwSetWindowUserPointer(mWindow, this);

        glfwSetFramebufferSizeCallback(mWindow, GLFWResizeCallback);
        glfwSetKeyCallback(mWindow, GLFWKeyCallback);
        glfwSetMouseButtonCallback(mWindow, GLFWMouseButtonCallback);
        glfwSetCursorPosCallback(mWindow, GLFWMouseCursorCallback);
        glfwSetScrollCallback(mWindow, GLFWMouseScrollCallback);

        glfwSwapInterval(1);

        if (!mRenderContext.Initialize(mWidth, mHeight)) {
            glfwDestroyWindow(mWindow);
            glfwTerminate();
            Log::Critical("Failed to initialize render context");
            return false;
        }

        Log::Info("Successfully initialized game instance");

        return true;
    }

    void Game::Shutdown() const {
        mRenderContext.Shutdown();
        if (mWindow) glfwDestroyWindow(mWindow);
        glfwTerminate();

        Log::Shutdown();
    }

    void Game::Render() const {
        mRenderContext.BeginFrame();

        mRenderContext.EndFrame(mWindow);
    }

    void Game::GLFWResizeCallback(GLFWwindow* window, s32 width, s32 height) {
        auto* game = CAST<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            game->GetRenderContext().Resize(width, height);
            game->OnResize(width, height);
        }
    }

    void Game::GLFWKeyCallback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods) {
        N_UNUSED(scancode);
        N_UNUSED(mods);

        auto* game = CAST<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            game->OnKey(key);
            if (action == GLFW_PRESS) game->OnKeyDown(key);
            else if (action == GLFW_RELEASE) game->OnKeyUp(key);
        }
    }

    void Game::GLFWMouseButtonCallback(GLFWwindow* window, s32 button, s32 action, s32 mods) {
        N_UNUSED(mods);

        auto* game = CAST<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            game->OnMouseButton(button);
            if (action == GLFW_PRESS) game->OnMouseButtonDown(button);
            else if (action == GLFW_RELEASE) game->OnMouseButtonUp(button);
        }
    }

    void Game::GLFWMouseCursorCallback(GLFWwindow* window, f64 xpos, f64 ypos) {
        auto* game = CAST<Game*>(glfwGetWindowUserPointer(window));
        if (game) { game->OnMouseMove(xpos, ypos); }
    }

    void Game::GLFWMouseScrollCallback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
        auto* game = CAST<Game*>(glfwGetWindowUserPointer(window));
        if (game) { game->OnMouseScroll(xoffset, yoffset); }
    }
}  // namespace N