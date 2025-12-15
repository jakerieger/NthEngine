#include "AssetManager.hpp"

#include <AsteraCore.hpp>

using namespace Astera;

class SpaceInvadersDemo final : public Game {
public:
    SpaceInvadersDemo() : Game("Space Invaders [Demo]", 1280, 720) {
        AssetManager::SetWorkingDirectory(fs::current_path() / "Content");
    }

    void OnAwake() override {
        auto& debug = GetDebugManager();
        debug.SetOverlayEnabled("ImGuiDebugLayer", true);
        debug.SetOverlayEnabled("PhysicsDebugLayer", false);

        GetActiveScene()->Load(Content::Get<ContentType::Scene>("Main.xml"), GetScriptEngine());

        Game::OnAwake();
    }

    void OnKeyDown(const u32 keyCode) override {
        using namespace Input;
        if (keyCode == Keys::Escape) {
            Quit();
        }
    }
};

ASTERA_RUN_GAME(SpaceInvadersDemo)
