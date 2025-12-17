#include <AsteraCore.hpp>
#include <ScriptCompiler.hpp>
#include <IO.hpp>

using namespace Astera;

class SpaceInvadersDemo final : public Game {
public:
    SpaceInvadersDemo() : Game("Space Invaders [Demo]", 1280, 720) {}

    void LoadContent() override {
        const Path sceneFile = Content::Get<ContentType::Scene>("Main.xml");
        auto* scene          = GetActiveScene();
        scene->LoadXML(sceneFile, GetScriptEngine());
    }

    void OnKeyDown(const u32 keyCode) override {
        using namespace Input;
        if (keyCode == Keys::Escape) {
            Quit();
        }
    }
};

ASTERA_RUN_GAME(SpaceInvadersDemo)
