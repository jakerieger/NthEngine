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
            const auto spriteTex = TextureManager::Load(Content::GetContentPath("Sprites/ball.png"));

            auto& state      = GetActiveScene()->GetState();
            mBallEntity      = state.CreateEntity();
            auto& sprite     = state.AddComponent<SpriteRenderer>(mBallEntity);
            sprite.textureId = spriteTex;
            sprite.geometry  = Geometry::CreateQuad(1.f, 1.f);

            auto& transform    = state.GetTransform(mBallEntity);
            transform.position = {640, 360};  // Center of screen
            transform.scale    = {64, 64};
        }

        void OnUpdate(const Clock& clock) override {
            auto& state     = GetActiveScene()->GetState();
            auto& transform = state.GetTransform(mBallEntity);
        }

        void OnLateUpdate() override {}

        void OnDestroyed() override {}

        void OnResize(u32 width, u32 height) override {
            auto& state     = GetActiveScene()->GetState();
            auto& transform = state.GetTransform(mBallEntity);

            transform.position.x = (f32)width / 2;
            transform.position.y = (f32)height / 2;
        }

    private:
        Entity mBallEntity {};
    };
}  // namespace Nth

int main() {
    Nth::SandboxGame game;
    game.Run();
}