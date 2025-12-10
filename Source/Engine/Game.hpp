/// @author Jake Rieger
/// @created 11/27/25
///

#pragma once

#include "AudioEngine.hpp"
#include "Common/CommonPCH.hpp"
#include "Clock.hpp"
#include "Input.hpp"
#include "Scene.hpp"
#include "ScriptEngine.hpp"
#include "Rendering/DebugInterface.hpp"
#include "Rendering/ImGuiDebugLayer.hpp"
#include "Rendering/PhysicsDebugLayer.hpp"
#include "Rendering/RenderContext.hpp"

namespace Nth {
    /// @brief Base class where all engine systems meet and execute
    ///
    /// Central hub for the game engine that owns and manages all core systems including
    /// the window, rendering, input, audio, scripting, and scene management. Handles
    /// the main game loop and communication between the OS, graphics API, and game
    /// client code. User games should inherit from this class and override lifecycle
    /// methods to implement custom behavior.
    class Game {
    public:
        /// @brief Default window width in pixels
        static constexpr u32 kDefaultWidth {800};

        /// @brief Default window height in pixels
        static constexpr u32 kDefaultHeight {600};

        N_CLASS_PREVENT_MOVES_COPIES(Game)

        /// @brief Default constructor, creates a game with default window settings
        Game() : mActiveScene(make_unique<Scene>()) {}

        /// @brief Constructs a game with custom window properties
        /// @param title Window title
        /// @param width Window width in pixels
        /// @param height Window height in pixels
        Game(const string& title, u32 width, u32 height)
            : mTitle(title), mWidth(width), mHeight(height), mActiveScene(make_unique<Scene>()) {}

        /// @brief Virtual destructor for proper cleanup of derived classes
        virtual ~Game() = default;

        /// @brief Starts the main game loop
        ///
        /// Initializes all engine systems, enters the main loop that handles
        /// input, updates, and rendering, then cleans up on exit.
        void Run();

        /// @brief Requests the game to quit
        ///
        /// Sets the running flag to false, which will exit the main loop
        /// and trigger cleanup on the next iteration.
        void Quit();

        /// @brief Toggles between fullscreen and windowed mode
        void ToggleFullscreen();

        /// @brief Sets the window title
        /// @param title New window title text
        void SetTitle(const string& title);

        /// @brief Resizes the game window
        /// @param width New window width in pixels
        /// @param height New window height in pixels
        void Resize(u32 width, u32 height) const;

        // Window callbacks

        /// @brief Called when the window is resized
        /// @param width New window width in pixels
        /// @param height New window height in pixels
        virtual void OnResize(u32 width, u32 height);

        /// @brief Called when a keyboard key is pressed down
        /// @param keyCode The key code of the pressed key
        virtual void OnKeyDown(u32 keyCode);

        /// @brief Called when a keyboard key is released
        /// @param keyCode The key code of the released key
        virtual void OnKeyUp(u32 keyCode);

        /// @brief Called continuously while a key is held down
        /// @param keyCode The key code of the held key
        virtual void OnKey(u32 keyCode);

        /// @brief Called when a mouse button is pressed down
        /// @param button Mouse button code (0 = left, 1 = right, 2 = middle)
        virtual void OnMouseButtonDown(u32 button);

        /// @brief Called when a mouse button is released
        /// @param button Mouse button code (0 = left, 1 = right, 2 = middle)
        virtual void OnMouseButtonUp(u32 button);

        /// @brief Called continuously while a mouse button is held down
        /// @param button Mouse button code (0 = left, 1 = right, 2 = middle)
        virtual void OnMouseButton(u32 button);

        /// @brief Called when the mouse cursor moves
        /// @param dX Horizontal movement delta in pixels
        /// @param dY Vertical movement delta in pixels
        virtual void OnMouseMove(f64 dX, f64 dY);

        /// @brief Called when the mouse wheel is scrolled
        /// @param dX Horizontal scroll delta
        /// @param dY Vertical scroll delta
        virtual void OnMouseScroll(f64 dX, f64 dY);

        // Lifecycle hooks

        /// @brief Called once when the game is initialized, before the main loop starts
        ///
        /// Override this to perform initialization tasks such as loading assets,
        /// setting up the scene, or configuring systems.
        virtual void OnAwake();

        /// @brief Called every frame during the update phase
        /// @param clock Clock instance providing delta time and timing information
        ///
        /// Override this to implement per-frame game logic, physics updates,
        /// AI behavior, and other time-dependent operations.
        virtual void OnUpdate(const Clock& clock);

        /// @brief Called every frame after OnUpdate, before rendering
        ///
        /// Override this for operations that need to happen after all game logic
        /// has been updated but before rendering (e.g., camera updates, final transforms).
        virtual void OnLateUpdate();

        /// @brief Called when the game is shutting down
        ///
        /// Override this to perform cleanup tasks such as saving data,
        /// releasing resources, or logging final state.
        virtual void OnDestroyed();

        // Getters

        /// @brief Gets the current window dimensions
        /// @param outWidth Output parameter for window width
        /// @param outHeight Output parameter for window height
        void GetWindowSize(u32& outWidth, u32& outHeight) const {
            outWidth  = mWidth;
            outHeight = mHeight;
        }

        /// @brief Checks if the game is currently running
        /// @return True if the game loop is active, false otherwise
        N_ND bool GetRunning() const {
            return mRunning;
        }

        /// @brief Checks if the window is in fullscreen mode
        /// @return True if fullscreen, false if windowed
        N_ND bool GetFullscreen() const {
            return mFullscreen;
        }

        /// @brief Gets the GLFW window handle
        /// @return Pointer to the GLFWwindow instance
        N_ND GLFWwindow* GetWindowHandle() {
            return mWindow;
        }

        /// @brief Gets the window title
        /// @return String view of the current window title
        N_ND std::string_view GetWindowTitle() const {
            return mTitle;
        }

        /// @brief Gets the render context
        /// @return Reference to the RenderContext managing graphics rendering
        N_ND RenderContext& GetRenderContext() {
            return mRenderContext;
        }

        /// @brief Gets the currently active scene
        /// @return Pointer to the active Scene instance
        N_ND Scene* GetActiveScene() const {
            return mActiveScene.get();
        }

        /// @brief Checks if vertical sync is enabled
        /// @return True if VSync is enabled, false otherwise
        N_ND bool GetVsyncEnabled() const {
            return mVsync;
        }

        /// @brief Gets the script engine
        /// @return Reference to the ScriptEngine for Lua scripting
        N_ND ScriptEngine& GetScriptEngine() {
            return mScriptEngine;
        }

        /// @brief Gets the audio engine
        /// @return Reference to the AudioEngine for audio playback
        N_ND AudioEngine& GetAudioEngine() {
            return mAudioEngine;
        }

    protected:
        // Subclass-accessible window properties. Use provided getters for the others.

        /// @brief Window title text
        string mTitle {};

        /// @brief Whether the game loop is currently running
        bool mRunning {false};

        /// @brief Whether the window is in fullscreen mode
        bool mFullscreen {false};

        /// @brief Current window width in pixels
        u32 mWidth {kDefaultWidth};

        /// @brief Current window height in pixels
        u32 mHeight {kDefaultHeight};

        /// @brief Renders the current frame
        ///
        /// Called internally each frame to render the scene, debug layers,
        /// and swap buffers.
        void Render();

    private:
        /// @brief Initializes all engine systems
        /// @return True if initialization succeeded, false otherwise
        bool Initialize();

        /// @brief Shuts down and cleans up all engine systems
        void Shutdown();

        /// @brief Initializes the Lua script engine and registers bindings
        /// @return True if initialization succeeded, false otherwise
        bool InitializeScriptEngine();

        // GLFW callbacks

        /// @brief GLFW callback for window resize events
        /// @param window GLFW window that was resized
        /// @param width New window width
        /// @param height New window height
        static void GLFWResizeCallback(GLFWwindow* window, i32 width, i32 height);

        /// @brief GLFW callback for keyboard events
        /// @param window GLFW window receiving the event
        /// @param key Key code
        /// @param scancode Platform-specific scan code
        /// @param action Action type (press, release, repeat)
        /// @param mods Modifier key flags
        static void GLFWKeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);

        /// @brief GLFW callback for mouse button events
        /// @param window GLFW window receiving the event
        /// @param button Mouse button code
        /// @param action Action type (press, release)
        /// @param mods Modifier key flags
        static void GLFWMouseButtonCallback(GLFWwindow* window, i32 button, i32 action, i32 mods);

        /// @brief GLFW callback for mouse cursor movement
        /// @param window GLFW window receiving the event
        /// @param xpos Cursor X position
        /// @param ypos Cursor Y position
        static void GLFWMouseCursorCallback(GLFWwindow* window, f64 xpos, f64 ypos);

        /// @brief GLFW callback for mouse scroll events
        /// @param window GLFW window receiving the event
        /// @param xoffset Horizontal scroll offset
        /// @param yoffset Vertical scroll offset
        static void GLFWMouseScrollCallback(GLFWwindow* window, f64 xoffset, f64 yoffset);

        // Window properties

        /// @brief GLFW window handle
        GLFWwindow* mWindow {nullptr};

        /// @brief Whether vertical sync is enabled
        bool mVsync {false};

        // Internal systems

        /// @brief Debug rendering manager
        DebugManager mDebugManager;

        /// @brief High-precision clock for timing and frame rate
        Clock mClock;

        /// @brief Rendering context managing graphics state and operations
        RenderContext mRenderContext;

        /// @brief Lua script execution engine
        ScriptEngine mScriptEngine;

        /// @brief Input handling manager for keyboard and mouse
        InputManager mInputManager;

        /// @brief Audio playback and management engine
        AudioEngine mAudioEngine;

        // Client systems

        /// @brief Currently active game scene
        unique_ptr<Scene> mActiveScene;

        /// @brief ImGui-based debug UI layer (optional)
        unique_ptr<ImGuiDebugLayer> mImGuiDebugLayer;

        /// @brief Physics visualization debug layer (optional)
        unique_ptr<PhysicsDebugLayer> mPhysicsDebugLayer;
    };

#if defined(N_ENGINE_PLATFORM_WINDOWS) && defined(NDEBUG)
    /// @brief Platform-specific entry point macro for Windows release builds
    /// Uses WinMain for GUI applications without console window
    #define N_ENTRYPOINT int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
#else
    /// @brief Standard entry point macro for all other configurations
    /// Uses standard main function with command-line arguments
    #define N_ENTRYPOINT int main(int argc, char* argv[])
#endif
}  // namespace Nth