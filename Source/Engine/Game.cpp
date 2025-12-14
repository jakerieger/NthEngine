/*
 *  Filename: Game.cpp
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

#include "Game.hpp"
#include "Coordinates.inl"
#include "Log.hpp"
#include "TextureManager.hpp"
#include "ShaderManager.hpp"
#include "ScriptTypeRegistry.hpp"
#include "Input.hpp"
#include "Math.hpp"
#include "AssetManager.hpp"
#include "JobSystem.hpp"
#include "Rendering/ImGuiDebugLayer.hpp"

#include <stb_image.h>

namespace Astera {
    using Astera::Log;

    void Game::Run() {
        mRunning = Initialize();
        if (!mRunning)
            return;
        OnAwake();
        {
            // Main loop
            // ReSharper disable once CppDFAConstantConditions
            while (mRunning && !glfwWindowShouldClose(mWindow)) {
                mClock.Tick();
                OnUpdate(mClock);

                Render();

                OnLateUpdate();
                glfwPollEvents();

                mFrameAllocator.NextFrame();
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
        if (mFullscreen) {
            // TODO: Add the GLFW code needed to actually toggle fullscreen
            mFullscreen = false;
        } else {
            mFullscreen = true;
        }
    }

    void Game::SetTitle(const string& title) {
        mTitle = title;
        glfwSetWindowTitle(mWindow, title.c_str());
    }

    void Game::Resize(u32 width, u32 height) const {
        glfwSetWindowSize(mWindow, (i32)width, (i32)height);
    }

    void Game::OnResize(u32 width, u32 height) {
        mWidth  = width;
        mHeight = height;
    }

    void Game::OnKeyDown(u32 keyCode) {
        mInputManager.UpdateKeyState(keyCode, true);
    }

    void Game::OnKeyUp(u32 keyCode) {
        mInputManager.UpdateKeyState(keyCode, false);
    }

    void Game::OnKey(u32 keyCode) {}

    void Game::OnMouseButtonDown(u32 button) {
        mInputManager.UpdateMouseButtonState(button, true);
    }

    void Game::OnMouseButtonUp(u32 button) {
        mInputManager.UpdateMouseButtonState(button, false);
    }

    void Game::OnMouseButton(u32 button) {
        ASTERA_UNUSED(button);
        // TODO: Implement
    }

    void Game::OnMouseMove(f64 dX, f64 dY) {
        mInputManager.UpdateMousePosition(dX, dY);
    }

    void Game::OnMouseScroll(f64 dX, f64 dY) {
        ASTERA_UNUSED(dX);
        ASTERA_UNUSED(dY);
        // TODO: Implement
    }

    void Game::OnAwake() {
        if (mActiveScene)
            mActiveScene->Awake(GetScriptEngine());
    }

    void Game::OnUpdate(const Clock& clock) {
        mDebugManager.Update(clock.GetDeltaTime());

        if (mActiveScene) {
            mActiveScene->Update(clock, GetScriptEngine());

            vector<Transform> transforms;
            const auto iter = mActiveScene->GetState().View<Transform>().each();
            for (auto [entity, transform] : iter) {
                transforms.push_back(transform);
            }
            mPhysicsDebugLayer->UpdateTransforms(transforms);
        }
    }

    void Game::OnLateUpdate() {
        if (mActiveScene)
            mActiveScene->LateUpdate(GetScriptEngine());
    }

    void Game::OnDestroyed() {
        if (mActiveScene)
            mActiveScene->Destroyed(GetScriptEngine());
    }

    bool Game::Initialize() {
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
            Log::Critical("Game", "Failed to create GLFW mWindow");
            return false;
        }

        glfwMakeContextCurrent(mWindow);
        glfwSetWindowUserPointer(mWindow, this);

        glfwSetFramebufferSizeCallback(mWindow, GLFWResizeCallback);
        glfwSetKeyCallback(mWindow, GLFWKeyCallback);
        glfwSetMouseButtonCallback(mWindow, GLFWMouseButtonCallback);
        glfwSetCursorPosCallback(mWindow, GLFWMouseCursorCallback);
        glfwSetScrollCallback(mWindow, GLFWMouseScrollCallback);

        if (mVsync)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        if (!mRenderContext.Initialize(mWidth, mHeight)) {
            glfwDestroyWindow(mWindow);
            glfwTerminate();
            Log::Critical("Game", "Failed to initialize render context");
            return false;
        }

        TextureManager::Initialize();
        ShaderManager::Initialize();
        mAudioEngine.Initialize();
        InitializeScriptEngine();

        // Asset manager
        if (!AssetManager::Initialize()) {
            Log::Critical("Game", "Failed to initialize asset manager");
            glfwDestroyWindow(mWindow);
            glfwTerminate();
            mRenderContext.Shutdown();
            TextureManager::Shutdown();
            ShaderManager::Shutdown();
            mAudioEngine.Shutdown();
            return false;
        }

        // Debug layers
        mImGuiDebugLayer = make_unique<ImGuiDebugLayer>(GetWindowHandle());
        mDebugManager.AttachOverlay("ImGuiDebugLayer", mImGuiDebugLayer.get());

        mPhysicsDebugLayer = make_unique<PhysicsDebugLayer>(mWidth, mHeight);
        mDebugManager.AttachOverlay("PhysicsDebugLayer", mPhysicsDebugLayer.get());

        // Create the initial scene
        mActiveScene = make_unique<Scene>(GetRenderContext());

        gJobSystem = make_unique<JobSystem>();
        gJobSystem->Initialize();

        Log::Debug(
          "Game",
          "Successfully initialized game instance:\n-- Dimensions: {}x{}\n-- V-Sync: {}\n-- Worker Threads: {}",
          mWidth,
          mHeight,
          mVsync ? "On" : "Off",
          gJobSystem->GetWorkerCount());

        return true;
    }

    void Game::Shutdown() {
        mDebugManager.DetachOverlays();
        mImGuiDebugLayer.reset();
        mPhysicsDebugLayer.reset();
        TextureManager::Shutdown();
        ShaderManager::Shutdown();
        mAudioEngine.Shutdown();
        mActiveScene.reset();
        mRenderContext.Shutdown();

        if (gJobSystem) {
            gJobSystem->Shutdown();
            gJobSystem.reset();
        }

        if (mWindow)
            glfwDestroyWindow(mWindow);
        glfwTerminate();

        Log::Shutdown();
    }

    bool Game::InitializeScriptEngine() {
        mScriptEngine.Initialize();
        if (!mScriptEngine.IsInitialized())
            return false;

        // Register globals
        auto& lua                   = mScriptEngine.GetLuaState();
        auto gameGlobal             = lua.new_usertype<Game>("Game");
        gameGlobal["Quit"]          = [this] { Quit(); };
        gameGlobal["GetScreenSize"] = [this]() -> Vec2 { return {mWidth, mHeight}; };

        lua["Scene"] = &mActiveScene->GetState();

        // Register globals
        Log::RegisterLuaGlobals(lua);
        Math::RegisterLuaGlobals(lua);
        Coordinates::RegisterLuaGlobals(lua);
        mInputManager.RegisterLuaGlobals(lua);
        mAudioEngine.RegisterLuaGlobals(lua);
        ScriptTypeRegistry::RegisterTypes(mScriptEngine);

        return true;
    }

    void Game::Render() {
        mRenderContext.BeginFrame();
        {
            // Submit drawing commands here
            if (mActiveScene) {
                mActiveScene->Render(GetRenderContext());
            }
        }
        mRenderContext.EndFrame();

        mDebugManager.Render();
        glfwSwapBuffers(mWindow);
    }

    void Game::SetWindowIcon(const fs::path& filename) const {
        i32 width, height, channels;
        u8* pixels = stbi_load(filename.string().c_str(), &width, &height, &channels, 4);  // Force RGBA

        if (pixels) {
            GLFWimage icon;
            icon.width  = width;
            icon.height = height;
            icon.pixels = pixels;
            glfwSetWindowIcon(mWindow, 1, &icon);
            stbi_image_free(pixels);
        } else {
            Log::Error("Game", "Failed to load window icon: {}", filename.string());
        }
    }

    void Game::GLFWResizeCallback(GLFWwindow* mWindow, i32 width, i32 height) {
        auto* game = CAST<Game*>(glfwGetWindowUserPointer(mWindow));
        if (game) {
            game->GetRenderContext().Resize(width, height);
            game->OnResize(width, height);
        }
    }

    void Game::GLFWKeyCallback(GLFWwindow* mWindow, i32 key, i32 scancode, i32 action, i32 mods) {
        ASTERA_UNUSED(scancode);
        ASTERA_UNUSED(mods);

        auto* game = CAST<Game*>(glfwGetWindowUserPointer(mWindow));
        if (game) {
            game->OnKey(key);
            if (action == GLFW_PRESS)
                game->OnKeyDown(key);
            else if (action == GLFW_RELEASE)
                game->OnKeyUp(key);
        }
    }

    void Game::GLFWMouseButtonCallback(GLFWwindow* mWindow, i32 button, i32 action, i32 mods) {
        ASTERA_UNUSED(mods);

        auto* game = CAST<Game*>(glfwGetWindowUserPointer(mWindow));
        if (game) {
            game->OnMouseButton(button);
            if (action == GLFW_PRESS)
                game->OnMouseButtonDown(button);
            else if (action == GLFW_RELEASE)
                game->OnMouseButtonUp(button);
        }
    }

    void Game::GLFWMouseCursorCallback(GLFWwindow* mWindow, f64 xpos, f64 ypos) {
        auto* game = CAST<Game*>(glfwGetWindowUserPointer(mWindow));
        if (game) {
            game->OnMouseMove(xpos, ypos);
        }
    }

    void Game::GLFWMouseScrollCallback(GLFWwindow* mWindow, f64 xoffset, f64 yoffset) {
        auto* game = CAST<Game*>(glfwGetWindowUserPointer(mWindow));
        if (game) {
            game->OnMouseScroll(xoffset, yoffset);
        }
    }
}  // namespace Astera