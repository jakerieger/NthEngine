/// @author Jake Rieger
/// @created 12/3/2025
///

#pragma once

#include "Common/CommonPCH.hpp"

namespace sol {
    class state;
}

namespace Nth {
    /// @brief Manages keyboard and mouse input state for the game
    ///
    /// Provides polling-based input handling for keyboard keys and mouse buttons,
    /// as well as mouse position and delta tracking. Input state is updated
    /// internally by the Game class and can be queried at any time.
    /// Also supports Lua scripting integration.
    class InputManager {
    public:
        N_CLASS_PREVENT_MOVES_COPIES(InputManager)

        /// @brief Default constructor
        InputManager() = default;

        // Getters

        /// @brief Checks if a keyboard key is currently pressed
        /// @param key Key code to check (platform-specific key constant)
        /// @return True if the key is pressed, false otherwise
        bool GetKeyDown(i32 key);

        /// @brief Checks if a keyboard key was just released
        /// @param key Key code to check (platform-specific key constant)
        /// @return True if the key was released this frame, false otherwise
        bool GetKeyUp(i32 key);

        /// @brief Checks if a mouse button is currently pressed
        /// @param button Mouse button code to check (0 = left, 1 = right, 2 = middle)
        /// @return True if the button is pressed, false otherwise
        bool GetMouseButtonDown(i32 button);

        /// @brief Checks if a mouse button was just released
        /// @param button Mouse button code to check (0 = left, 1 = right, 2 = middle)
        /// @return True if the button was released this frame, false otherwise
        bool GetMouseButtonUp(i32 button);

        /// @brief Gets the current mouse X position
        /// @return Mouse X coordinate in screen/window space
        N_ND i32 GetMouseX() const;

        /// @brief Gets the current mouse Y position
        /// @return Mouse Y coordinate in screen/window space
        N_ND i32 GetMouseY() const;

        /// @brief Gets the mouse movement delta on the X axis since last frame
        /// @return Horizontal mouse movement in pixels
        N_ND f32 GetMouseDeltaX() const;

        /// @brief Gets the mouse movement delta on the Y axis since last frame
        /// @return Vertical mouse movement in pixels
        N_ND f32 GetMouseDeltaY() const;

        /// @brief Resets the mouse delta values to zero
        ///
        /// Typically called at the end of each frame after deltas have been processed.
        void ResetMouseDeltas();

    private:
        friend class Game;

        /// @brief Registers input functions as Lua global functions
        /// @param state Lua state to register globals in
        void RegisterLuaGlobals(sol::state& state);

        // Private update methods

        /// @brief Updates the state of a keyboard key
        /// @param key Key code to update
        /// @param pressed True if the key is pressed, false if released
        void UpdateKeyState(u16 key, bool pressed);

        /// @brief Updates the state of a mouse button
        /// @param button Mouse button code to update
        /// @param pressed True if the button is pressed, false if released
        void UpdateMouseButtonState(u16 button, bool pressed);

        /// @brief Updates the current mouse position and calculates deltas
        /// @param x New mouse X position
        /// @param y New mouse Y position
        void UpdateMousePosition(f64 x, f64 y);

        /// @brief Enables or disables input processing
        /// @param enabled True to enable input, false to disable
        void SetEnabled(bool enabled);

        /// @brief Internal structure for tracking key/button state
        struct KeyState {
            /// @brief True if the key/button is currently pressed
            bool pressed = false;

            /// @brief True if the key/button was just released
            bool released = false;
        };

        /// @brief Map of keyboard key codes to their current states
        unordered_map<i32, KeyState> mKeyStates;

        /// @brief Map of mouse button codes to their current states
        unordered_map<i32, KeyState> mMouseStates;

        /// @brief Current mouse X position in screen/window space
        i32 mMouseX = 0;

        /// @brief Current mouse Y position in screen/window space
        i32 mMouseY = 0;

        /// @brief Horizontal mouse movement since last frame
        f32 mMouseDeltaX = 0.f;

        /// @brief Vertical mouse movement since last frame
        f32 mMouseDeltaY = 0.f;

        /// @brief Whether input processing is currently enabled
        bool mEnabled = true;
    };
}  // namespace Nth