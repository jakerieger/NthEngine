/*
 *  Filename: Game.hpp
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

#pragma once

#include "EngineCommon.hpp"
#include "AudioEngine.hpp"
#include "EngineConfig.hpp"
#include "FrameAllocator.hpp"
#include "Scene.hpp"
#include "ScriptEngine.hpp"
#include "Window.hpp"
#include "Rendering/DebugInterface.hpp"
#include "Rendering/ImGuiDebugLayer.hpp"
#include "Rendering/PhysicsDebugLayer.hpp"
#include "Rendering/RenderTarget.hpp"

namespace Astera {
    /// @brief Base class where all engine systems meet and execute
    ///
    /// Extends Window to add game-specific functionality including rendering, audio,
    /// scripting, and scene management. Handles the main game loop and communication
    /// between the OS, graphics API, and game client code. User games should inherit
    /// from this class and override lifecycle methods to implement custom behavior.
    class Game : public Window {
    public:
        /// @brief Default window width in pixels
        static constexpr u32 kDefaultWidth {800};

        /// @brief Default window height in pixels
        static constexpr u32 kDefaultHeight {600};

        ASTERA_CLASS_PREVENT_MOVES_COPIES(Game)

        /// @brief Default constructor - creates a game with default window settings
        Game() : Window({}), mFrameAllocator(1_MB) {}

        /// @brief Constructs a game with custom window properties
        /// @param title Window title
        /// @param width Window width in pixels
        /// @param height Window height in pixels
        Game(const string& title, u32 width, u32 height)
            : Window({.title = title, .width = width, .height = height}), mFrameAllocator(1_MB) {}

        /// @brief Virtual destructor for proper cleanup of derived classes
        ~Game() override;

        /// @brief Requests the game to quit
        ///
        /// Sets the running flag to false, which will exit the main loop
        /// and trigger cleanup on the next iteration.
        void Quit();

        /// @brief Toggles between fullscreen and windowed mode
        void ToggleFullscreen();

        // Getters

        /// @brief Gets the main render target
        /// @return Reference to the main render target
        ASTERA_KEEP RenderTarget& GetMainRenderTarget() {
            return *mMainRenderTarget;
        }

        /// @brief Gets the render context from the main render target
        /// @return Reference to the RenderContext managing graphics rendering
        ASTERA_KEEP RenderContext& GetRenderContext() {
            return mMainRenderTarget->GetContext();
        }

        /// @brief Gets the currently active scene
        /// @return Pointer to the active Scene instance
        ASTERA_KEEP Scene* GetActiveScene() const {
            return mActiveScene.get();
        }

        /// @brief Gets the script engine
        /// @return Reference to the ScriptEngine for Lua scripting
        ASTERA_KEEP ScriptEngine& GetScriptEngine() {
            return mScriptEngine;
        }

        /// @brief Gets the audio engine
        /// @return Reference to the AudioEngine for audio playback
        ASTERA_KEEP AudioEngine& GetAudioEngine() {
            return mAudioEngine;
        }

        /// @brief Gets the debug manager
        /// @return Reference to the DebugManager instance
        ASTERA_KEEP DebugManager& GetDebugManager() {
            return mDebugManager;
        }

        /// @brief Gets the frame allocator
        /// @return Reference to the frame allocator instance
        ASTERA_KEEP FrameAllocator& GetFrameAllocator() {
            return mFrameAllocator;
        }

        /// @brief Checks if the window is in fullscreen mode
        /// @return True if fullscreen, false if windowed
        ASTERA_KEEP bool GetFullscreen() const {
            return mFullscreen;
        }

    protected:
        virtual void LoadContent() {}

        virtual void UnloadContent() {}

        /// @brief Called once when the game starts, before the main loop
        /// Override this to initialize game systems and load resources
        void OnAwake() override;

        /// @brief Called every frame for game logic updates
        /// @param clock Clock object containing delta time and frame information
        void OnUpdate(const Clock& clock) override;

        /// @brief Called every frame after OnUpdate, typically for rendering
        void OnLateUpdate() override;

        /// @brief Called when the window is resized
        /// @param width New window width
        /// @param height New window height
        void OnResize(u32 width, u32 height) override;

        /// @brief Called when the game is shutting down
        void OnDestroyed() override;

        /// @brief Renders the current frame
        ///
        /// Called internally each frame to render the scene, debug layers,
        /// and swap buffers.
        void Render();

        /// @brief Whether the window is in fullscreen mode
        bool mFullscreen {false};

    private:
        /// @brief Initializes the Lua script engine and registers bindings
        /// @return True if initialization succeeded, false otherwise
        bool InitializeScriptEngine();

        // Core rendering

        /// @brief Main render target for the game window
        unique_ptr<RenderTarget> mMainRenderTarget;

        // Internal systems

        /// @brief Debug rendering manager
        DebugManager mDebugManager;

        /// @brief Lua script execution engine
        ScriptEngine mScriptEngine;

        /// @brief Audio playback and management engine
        AudioEngine mAudioEngine;

        /// @brief Frame allocator for temporary, fast allocations
        FrameAllocator mFrameAllocator;

        // Client systems

        /// @brief Currently active game scene
        unique_ptr<Scene> mActiveScene;

        /// @brief ImGui-based debug UI layer (optional)
        unique_ptr<ImGuiDebugLayer> mImGuiDebugLayer;

        /// @brief Physics visualization debug layer (optional)
        unique_ptr<PhysicsDebugLayer> mPhysicsDebugLayer;
    };

#define ASTERA_RUN_GAME(GAME_CLASS)                                                                                    \
    int main() {                                                                                                       \
        Content::SetRuntimeDefaults();                                                                                 \
        AssetManager::SetRuntimeDefaults();                                                                            \
        GAME_CLASS().Run();                                                                                            \
    }
}  // namespace Astera