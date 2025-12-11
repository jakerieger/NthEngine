#include <AsteraCore.hpp>

using namespace Astera;

class SpaceInvadersDemo final : public Game {
public:
    SpaceInvadersDemo() : Game("Space Invaders [Astera Demo]", 1280, 720) {}

    void OnAwake() override {
        auto& debug = GetDebugManager();
        debug.SetOverlayEnabled("ImGuiDebugLayer", false);
        debug.SetOverlayEnabled("PhysicsDebugLayer", false);

        GetActiveScene()->Load(Content::Get<ContentType::Scene>("Main.xml"), GetScriptEngine());
        GetAudioEngine().LoadSound(Content::Get<ContentType::Audio>("shoot.wav"));

        Game::OnAwake();
    }

    void OnKeyDown(u32 keyCode) override {
        using namespace Input;

        if (keyCode == Keys::Escape) { Quit(); }
    }
};

ASTERA_RUN_GAME(SpaceInvadersDemo)