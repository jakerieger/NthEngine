/// @author Jake Rieger
/// @created 11/27/25
///
#include "Game.hpp"
#include "Log.hpp"
#include "TextureManager.hpp"
#include "ShaderManager.hpp"

namespace Nth {
    using Nth::Log;

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
        glfwSetWindowSize(mWindow, (i32)width, (i32)height);
    }

    void Game::OnAwake() {
        if (mActiveScene) mActiveScene->Awake();
    }

    void Game::OnUpdate(const Clock& clock) {
        if (mActiveScene) mActiveScene->Update(clock);
    }

    void Game::OnLateUpdate() {
        if (mActiveScene) mActiveScene->LateUpdate();
    }

    void Game::OnDestroyed() {
        if (mActiveScene) mActiveScene->Destroyed();
    }

    bool Game::Initialize() {
        // Enforce X11 on Wayland systems for debugging support (RenderDoc doens't support Wayland)
#if defined(N_ENGINE_PLATFORM_LINUX_WAYLAND) || defined(N_ENGINE_PLATFORM_LINUX_X11)
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#endif

        if (!glfwInit()) {
            Log::Critical("Game", "Failed to initialize GLFW");
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        mWindow = glfwCreateWindow((i32)mWidth, (i32)mHeight, mTitle.c_str(), nullptr, nullptr);
        if (!mWindow) {
            glfwTerminate();
            Log::Critical("Game", "Failed to create GLFW window");
            return false;
        }

        glfwMakeContextCurrent(mWindow);
        glfwSetWindowUserPointer(mWindow, this);

        glfwSetFramebufferSizeCallback(mWindow, GLFWResizeCallback);
        glfwSetKeyCallback(mWindow, GLFWKeyCallback);
        glfwSetMouseButtonCallback(mWindow, GLFWMouseButtonCallback);
        glfwSetCursorPosCallback(mWindow, GLFWMouseCursorCallback);
        glfwSetScrollCallback(mWindow, GLFWMouseScrollCallback);

        if (mVsync) glfwSwapInterval(1);
        else glfwSwapInterval(0);

        if (!mRenderContext.Initialize(mWidth, mHeight)) {
            glfwDestroyWindow(mWindow);
            glfwTerminate();
            Log::Critical("Game", "Failed to initialize render context");
            return false;
        }

        TextureManager::Initialize();
        ShaderManager::Initialize();

        Log::Debug("Game",
                   "Successfully initialized game instance:\n-- Dimensions: {}x{}\n-- V-Sync: {}",
                   mWidth,
                   mHeight,
                   mVsync ? "On" : "Off");

        return true;
    }

    void Game::Shutdown() {
        TextureManager::Shutdown();
        ShaderManager::Shutdown();
        mActiveScene.reset();
        mRenderContext.Shutdown();
        if (mWindow) glfwDestroyWindow(mWindow);
        glfwTerminate();

        Log::Shutdown();
    }

    void Game::Render() {
        mRenderContext.BeginFrame();
        {
            // Submit drawing commands here
            if (mActiveScene) { mActiveScene->Render(mRenderContext); }
        }
        mRenderContext.EndFrame(mWindow);
    }

    void Game::GLFWResizeCallback(GLFWwindow* window, i32 width, i32 height) {
        auto* game = CAST<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            game->GetRenderContext().Resize(width, height);
            game->OnResize(width, height);
        }
    }

    void Game::GLFWKeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
        N_UNUSED(scancode);
        N_UNUSED(mods);

        auto* game = CAST<Game*>(glfwGetWindowUserPointer(window));
        if (game) {
            game->OnKey(key);
            if (action == GLFW_PRESS) game->OnKeyDown(key);
            else if (action == GLFW_RELEASE) game->OnKeyUp(key);
        }
    }

    void Game::GLFWMouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods) {
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
}  // namespace Nth