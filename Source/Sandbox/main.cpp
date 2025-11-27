// Author: Jake Rieger
// Created: 11/27/25.
//

#include <Game.hpp>
#include <InputCodes.hpp>
#include <Log.hpp>

namespace N {
    class SandboxGame final : public Game {
    public:
        SandboxGame() : Game("Sandbox", 1280, 720) {}

        void OnKeyDown(u32 keyCode) override {
            if (keyCode == Input::Keys::Escape) { Quit(); }

            if (keyCode == Input::Keys::F11) { ToggleFullscreen(); }
        }

        void OnAwake() override {}

        void OnUpdate(const Clock& clock) override {}

        void OnLateUpdate() override {}

        void OnDestroyed() override {}
    };
}  // namespace N

int main() {
    N::SandboxGame game;
    game.Run();
}