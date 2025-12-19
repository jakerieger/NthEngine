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
#include "SceneParser.hpp"
#include "Rendering/ImGuiDebugLayer.hpp"

#include <stb_image.h>

namespace Astera {
    using Astera::Log;

    Game::~Game() = default;

    void Game::Quit() {
        Close();  // Call Window::Close() to exit the main loop
    }

    void Game::ToggleFullscreen() {
        if (mFullscreen) {
            // TODO: Add the GLFW code needed to actually toggle fullscreen
            mFullscreen = false;
        } else {
            mFullscreen = true;
        }
    }

    void Game::OnResize(u32 width, u32 height) {
        if (mMainRenderTarget) {
            mMainRenderTarget->Resize(width, height);
        }
    }

    bool Game::LoadScene(const string& name) {
        if (!mSceneCache.contains(name)) {
            Log::Error("Game", "Scene '{}' does not exist", name);
            return false;
        }

        mActiveScene->LoadDescriptor(mSceneCache[name], GetScriptEngine());
        return true;
    }

    void Game::LoadPlugins() {
        const auto pluginsPath = fs::current_path() / "Plugins";
        if (!exists(pluginsPath)) {
            Log::Warn("Game", "No plugins directory found");
        } else {
            for (const auto& entry : fs::directory_iterator(pluginsPath)) {
                if (entry.is_regular_file() && entry.path().extension() == ".plugin") {
                    Plugin plugin;
                    const auto loadResult = plugin.Load(entry.path().string());
                    if (!loadResult.has_value()) {
                        Log::Error("Game", "Error loading plugin: {}", loadResult.error());
                        continue;
                    }

                    Log::Debug("Game", "Loaded plugin: '{}' (from: {})", *loadResult, plugin.GetPluginPath().string());

                    mPlugins[*loadResult] = std::move(plugin);
                }
            }
        }
    }

    void Game::UpdateSceneCache() {
        mSceneCache.clear();
        bool loadedEntry = false;
        for (auto& scenePath : AssetManager::GetScenes()) {
            SceneDescriptor desc;
            SceneParser::DeserializeDescriptorXML(scenePath, desc);
            mSceneCache[desc.name] = desc;
            if (desc.entry && !loadedEntry) {
                mActiveScene->LoadDescriptor(desc, GetScriptEngine());
                loadedEntry = true;
            }
            Log::Debug("Game", "Added scene '{}' to cache", desc.name);
        }
    }

    void Game::LoadEngineConfigurations() {
        const auto inputMapPath = fs::current_path() / "Config" / "InputMap.ini";
        InputMap inputMap;
        if (inputMap.Load(inputMapPath)) {
            mInputManager.SetInputMap(inputMap);
        }
    }

    void Game::LoadDebugLayers(u32 width, u32 height) {
        mImGuiDebugLayer = make_unique<ImGuiDebugLayer>(GetHandle());
        mDebugManager.AttachOverlay("ImGuiDebugLayer", mImGuiDebugLayer.get());

        mPhysicsDebugLayer = make_unique<PhysicsDebugLayer>(width, height);
        mDebugManager.AttachOverlay("PhysicsDebugLayer", mPhysicsDebugLayer.get());
    }

    void Game::OnAwake() {
        Log::Info("Game", "Initializing game systems...");

        // Get window dimensions from Window base class
        u32 width, height;
        GetSize(width, height);

        // Create main render target
        mMainRenderTarget = make_unique<RenderTarget>(RenderTargetConfig {.type          = RenderTargetType::Window,
                                                                          .width         = width,
                                                                          .height        = height,
                                                                          .enableDepth   = true,
                                                                          .enableStencil = false});

        if (!mMainRenderTarget->Initialize()) {
            Log::Critical("Game", "Failed to initialize main render target");
            Close();
            return;
        }

        // Initialize asset managers
        TextureManager::Initialize();
        ShaderManager::Initialize();

        // Initialize audio engine
        mAudioEngine.Initialize();

        // Initialize script engine
        InitializeScriptEngine();

        LoadEngineConfigurations();

        // Asset manager
        if (!AssetManager::Initialize()) {
            Log::Critical("Game", "Failed to initialize asset manager");
            Close();
            return;
        }

        // Debug layers
        LoadDebugLayers(width, height);

        // Create the initial scene
        mActiveScene = make_unique<Scene>(GetRenderContext());

        UpdateSceneCache();

        // Initialize job system
        gJobSystem = make_unique<JobSystem>();
        gJobSystem->Initialize();

        LoadPlugins();

        Log::Debug("Game",
                   "Successfully initialized game instance:\n-- Dimensions: {}x{}\n-- V-Sync: {}\n-- Worker Threads: "
                   "{}\n-- Plugins: {}",
                   width,
                   height,
                   mVsync ? "On" : "Off",
                   gJobSystem->GetWorkerCount(),
                   mPlugins.size());

        for (const auto& plugin : mPlugins | std::views::values) {
            plugin->OnEngineStart(this);
        }

        // Call user's OnAwake if they have overridden it
        if (mActiveScene)
            mActiveScene->Awake(GetScriptEngine());

        for (const auto& plugin : mPlugins | std::views::values) {
            plugin->OnSceneAwake(this);
        }

        LoadContent();
    }

    void Game::OnUpdate(const Clock& clock) {
        mDebugManager.Update(clock.GetDeltaTime());
        // update debug ui
        mImGuiDebugLayer->UpdateFrameRate((f32)clock.GetFramesPerSecond());
        const auto fT = (1.f / clock.GetFramesPerSecond()) * 1000.f;
        mImGuiDebugLayer->UpdateFrameTime((f32)fT);
        // silly hack, will calculate once threading is actually implemented
        mImGuiDebugLayer->UpdateMainThreadTime((f32)fT / 2.f);
        mImGuiDebugLayer->UpdateRenderThreadTime((f32)fT / 2.f);
        mImGuiDebugLayer->UpdateEntities(mActiveScene->GetState().GetEntityCount());

        auto& resMgr = mActiveScene->GetResourceManager();
        mImGuiDebugLayer->UpdateResourcePoolAllocatedBytes(resMgr.GetAllocator().GetSize());
        mImGuiDebugLayer->UpdateResourcePoolUsedBytes(resMgr.GetAllocator().GetUsedMemory());

        if (mActiveScene) {
            mActiveScene->Update(clock, GetScriptEngine());

            vector<Transform> transforms;
            const auto iter = mActiveScene->GetState().View<Transform>().each();
            for (auto [entity, transform] : iter) {
                transforms.push_back(transform);
            }
            mPhysicsDebugLayer->UpdateTransforms(transforms);
        }

        for (const auto& plugin : mPlugins | std::views::values) {
            plugin->OnSceneUpdate(this);
        }

        // Advance frame allocator
        mFrameAllocator.NextFrame();
    }

    void Game::OnLateUpdate() {
        // Render the frame
        Render();

        // Call user's LateUpdate
        if (mActiveScene)
            mActiveScene->LateUpdate(GetScriptEngine());

        for (const auto& plugin : mPlugins | std::views::values) {
            plugin->OnSceneLateUpdate(this);
        }
    }

    void Game::OnDestroyed() {
        for (const auto& plugin : mPlugins | std::views::values) {
            plugin->OnEngineStop(this);
        }

        if (mActiveScene) {
            mActiveScene->Destroyed(GetScriptEngine());
            mActiveScene.reset();
        }

        UnloadContent();

        mDebugManager.DetachOverlays();
        mImGuiDebugLayer.reset();
        mPhysicsDebugLayer.reset();

        TextureManager::Shutdown();
        ShaderManager::Shutdown();
        mAudioEngine.Shutdown();

        if (mMainRenderTarget) {
            mMainRenderTarget->Shutdown();
            mMainRenderTarget.reset();
        }

        if (gJobSystem) {
            gJobSystem->Shutdown();
            gJobSystem.reset();
        }

        Log::Shutdown();
    }

    void Game::Render() {
        mMainRenderTarget->Bind();
        mMainRenderTarget->GetContext().BeginFrame();
        {
            // Submit drawing commands here
            if (mActiveScene) {
                mActiveScene->Render(GetRenderContext());
            }
        }
        mMainRenderTarget->GetContext().EndFrame();

        for (const auto& plugin : mPlugins | std::views::values) {
            plugin->OnSceneRender(this);
        }

        mImGuiDebugLayer->UpdateDrawCalls(CommandExecutor::gDrawCalls);
        mDebugManager.Render();
        glfwSwapBuffers(GetHandle());

        CommandExecutor::gDrawCalls = 0;
    }

    bool Game::InitializeScriptEngine() {
        mScriptEngine.Initialize();
        if (!mScriptEngine.IsInitialized())
            return false;

        // Register globals
        auto& lua                   = mScriptEngine.GetLuaState();
        auto gameGlobal             = lua.new_usertype<Game>("Game");
        gameGlobal["Quit"]          = [this] { Quit(); };
        gameGlobal["GetScreenSize"] = [this]() -> Vec2 {
            u32 w, h;
            GetSize(w, h);
            return {CAST<f32>(w), CAST<f32>(h)};
        };
        gameGlobal["LoadScene"] = [this](const sol::object&, const string& sceneName) -> bool {
            return LoadScene(sceneName);
        };

        lua["Scene"] = &mActiveScene->GetState();

        // Register globals
        Log::RegisterLuaGlobals(lua);
        Math::RegisterLuaGlobals(lua);
        Coordinates::RegisterLuaGlobals(lua);
        GetInputManager().RegisterLuaGlobals(lua);  // Use Window's InputManager
        mAudioEngine.RegisterLuaGlobals(lua);
        ScriptTypeRegistry::RegisterTypes(mScriptEngine);

        return true;
    }
}  // namespace Astera