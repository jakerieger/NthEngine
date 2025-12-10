/// @author Jake Rieger
/// @created 12/3/2025
///

#include "Input.hpp"
#include "InputCodes.hpp"

#include <sol/sol.hpp>

namespace Nth {
    bool InputManager::GetKeyDown(i32 key) {
        return mKeyStates[key].pressed;
    }

    bool InputManager::GetKeyUp(i32 key) {
        return mKeyStates[key].released;
    }

    bool InputManager::GetMouseButtonDown(i32 button) {
        return mMouseStates[button].pressed;
    }

    bool InputManager::GetMouseButtonUp(i32 button) {
        return mMouseStates[button].released;
    }

    i32 InputManager::GetMouseX() const {
        return mMouseX;
    }

    i32 InputManager::GetMouseY() const {
        return mMouseY;
    }

    f32 InputManager::GetMouseDeltaX() const {
        return mMouseDeltaX;
    }

    f32 InputManager::GetMouseDeltaY() const {
        return mMouseDeltaY;
    }

    void InputManager::ResetMouseDeltas() {
        mMouseDeltaX = 0.0f;
        mMouseDeltaY = 0.0f;
    }

    void InputManager::RegisterLuaGlobals(sol::state& state) {
        using namespace Input;

        state["KeyCode"]     = state.create_table();
        auto keyCode         = state["KeyCode"];
        keyCode["A"]         = Keys::A;
        keyCode["B"]         = Keys::B;
        keyCode["C"]         = Keys::C;
        keyCode["D"]         = Keys::D;
        keyCode["E"]         = Keys::E;
        keyCode["F"]         = Keys::F;
        keyCode["G"]         = Keys::G;
        keyCode["H"]         = Keys::H;
        keyCode["I"]         = Keys::I;
        keyCode["J"]         = Keys::J;
        keyCode["K"]         = Keys::K;
        keyCode["L"]         = Keys::L;
        keyCode["M"]         = Keys::M;
        keyCode["N"]         = Keys::N;
        keyCode["O"]         = Keys::O;
        keyCode["P"]         = Keys::P;
        keyCode["Q"]         = Keys::Q;
        keyCode["R"]         = Keys::R;
        keyCode["S"]         = Keys::S;
        keyCode["T"]         = Keys::T;
        keyCode["U"]         = Keys::U;
        keyCode["V"]         = Keys::V;
        keyCode["W"]         = Keys::W;
        keyCode["X"]         = Keys::X;
        keyCode["Y"]         = Keys::Y;
        keyCode["Z"]         = Keys::Z;
        keyCode["1"]         = Keys::Num1;
        keyCode["2"]         = Keys::Num2;
        keyCode["3"]         = Keys::Num3;
        keyCode["4"]         = Keys::Num4;
        keyCode["5"]         = Keys::Num5;
        keyCode["6"]         = Keys::Num6;
        keyCode["7"]         = Keys::Num7;
        keyCode["8"]         = Keys::Num8;
        keyCode["9"]         = Keys::Num9;
        keyCode["0"]         = Keys::Num0;
        keyCode["Minus"]     = Keys::Minus;
        keyCode["Equal"]     = Keys::Equal;
        keyCode["Backspace"] = Keys::Backspace;
        keyCode["Tab"]       = Keys::Tab;
        keyCode["Home"]      = Keys::Home;
        keyCode["Left"]      = Keys::Left;
        keyCode["Up"]        = Keys::Up;
        keyCode["Right"]     = Keys::Right;
        keyCode["Down"]      = Keys::Down;
        keyCode["Escape"]    = Keys::Escape;
        keyCode["Enter"]     = Keys::Enter;
        keyCode["Space"]     = Keys::Space;
        keyCode["LeftCtrl"]  = Keys::LeftControl;
        keyCode["RightCtrl"] = Keys::RightControl;
        keyCode["F1"]        = Keys::F1;
        keyCode["F2"]        = Keys::F2;
        keyCode["F3"]        = Keys::F3;
        keyCode["F4"]        = Keys::F4;
        keyCode["F5"]        = Keys::F5;
        keyCode["F6"]        = Keys::F6;
        keyCode["F7"]        = Keys::F7;
        keyCode["F8"]        = Keys::F8;
        keyCode["F9"]        = Keys::F9;
        keyCode["F10"]       = Keys::F10;
        keyCode["F11"]       = Keys::F11;
        keyCode["F12"]       = Keys::F12;

        state["MouseButton"]  = state.create_table();
        auto mouseButton      = state["MouseButton"];
        mouseButton["Left"]   = MouseButtons::Left;
        mouseButton["Right"]  = MouseButtons::Right;
        mouseButton["Middle"] = MouseButtons::Middle;

        state.new_usertype<InputManager>("InputManager",
                                         "GetKeyDown",
                                         &InputManager::GetKeyDown,
                                         "GetKeyUp",
                                         &InputManager::GetKeyUp,
                                         "GetMouseButtonDown",
                                         &InputManager::GetMouseButtonDown,
                                         "GetMouseButtonUp",
                                         &InputManager::GetMouseButtonUp,
                                         "GetMouseX",
                                         &InputManager::GetMouseX,
                                         "GetMouseY",
                                         &InputManager::GetMouseY,
                                         "GetMouseDeltaX",
                                         &InputManager::GetMouseDeltaX,
                                         "GetMouseDeltaY",
                                         &InputManager::GetMouseDeltaY);
        state["Input"] = this;
    }

    void InputManager::UpdateKeyState(const u16 key, const bool pressed) {
        if (!mEnabled) return;

        mKeyStates[key].pressed  = pressed;
        mKeyStates[key].released = !pressed;
    }

    void InputManager::UpdateMouseButtonState(const u16 button, const bool pressed) {
        if (!mEnabled) return;

        mMouseStates[button].pressed  = pressed;
        mMouseStates[button].released = !pressed;
    }

    void InputManager::UpdateMousePosition(const f64 x, const f64 y) {
        if (!mEnabled) return;

        mMouseDeltaX = CAST<f32>(x);
        mMouseDeltaY = CAST<f32>(y);

        constexpr f32 deadZone = 2.5f;  // Might need to tweak this, also frame-rate dependent :(
        if (std::abs(mMouseDeltaX) < deadZone) mMouseDeltaX = 0.0f;
        if (std::abs(mMouseDeltaY) < deadZone) mMouseDeltaY = 0.0f;

        mMouseX += x;
        mMouseY += y;
    }

    void InputManager::SetEnabled(const bool enabled) {
        mEnabled = enabled;
    }
}  // namespace Nth