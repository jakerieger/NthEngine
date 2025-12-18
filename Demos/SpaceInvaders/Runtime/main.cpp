#include <AsteraCore.hpp>
#include <ScriptCompiler.hpp>
#include <IO.hpp>

using namespace Astera;

class SpaceInvadersDemo final : public Game {
public:
    SpaceInvadersDemo() : Game("Space Invaders [Demo]", 1280, 720) {}

    void LoadContent() override {
        auto* dbgLayer = GetDebugManager().GetOverlay<ImGuiDebugLayer>("ImGuiDebugLayer");
        vector<string> lines;
        lines.emplace_back("A / Left - Move player left");
        lines.emplace_back("D / Right - Move player right");
        lines.emplace_back("Space - Reset level");
        lines.emplace_back("Esc - Quit");
        dbgLayer->SetCustomText("Controls", lines);
    }

    void OnKeyDown(const u32 keyCode) override {
        using namespace Input;
        if (keyCode == Keys::Escape) {
            Quit();
        }
    }
};

ASTERA_RUN_GAME(SpaceInvadersDemo)
