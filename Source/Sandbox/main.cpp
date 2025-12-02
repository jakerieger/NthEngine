/// @author Jake Rieger
/// @created 11/27/25
///

#include <Game.hpp>
#include <InputCodes.hpp>
#include <ShaderManager.hpp>
#include <TextureManager.hpp>
#include <Rendering/Shader.hpp>
#include <Content.hpp>
#include <Rendering/Geometry.hpp>
#include <Coordinates.hpp>
#include <Log.hpp>

namespace Nth {
    class SandboxGame final : public Game {
    public:
        SandboxGame() : Game("Sandbox", 1280, 720) {}

        void OnKeyDown(u32 keyCode) override {
            if (keyCode == Input::Keys::Escape) { Quit(); }
            if (keyCode == Input::Keys::F11) { ToggleFullscreen(); }
        }

        void OnAwake() override {
            GetActiveScene()->Load(Content::GetContentPath("Scenes/Sandbox.xml"), GetScriptEngine());
            Game::OnAwake();
        }

        void OnUpdate(const Clock& clock) override {
            Game::OnUpdate(clock);
        }

        void OnLateUpdate() override {
            Game::OnLateUpdate();
        }

        void OnDestroyed() override {
            Game::OnDestroyed();
        }

        void OnResize(u32 width, u32 height) override {}
    };
}  // namespace Nth

int main() {
    Nth::SandboxGame game;
    game.Run();
}