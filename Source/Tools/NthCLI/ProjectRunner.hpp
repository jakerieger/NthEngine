/// @author: Jake Rieger
/// @created: 12/3/2025.
///

#pragma once

#include <Game.hpp>
#include <Content.hpp>
#include <InputCodes.hpp>

namespace Nth {
    class ProjectRunner final : public Game {
    public:
        explicit ProjectRunner(const string& name) : Game(name, 1280, 720) {}

        void OnKeyDown(u32 keyCode) override {
            Game::OnKeyDown(keyCode);
            if (keyCode == Input::Keys::Escape) { Quit(); }
        }

        void OnAwake() override {
            if (!mStartupScene.empty())
                GetActiveScene()->Load(Content::Get<ContentType::Scene>(mStartupScene), GetScriptEngine());
            Game::OnAwake();
        }

        void SetStartupScene(const string& name) {
            mStartupScene = name + ".xml";  // TODO: Cheap hack, replace with actual scene caching
        }

    private:
        string mStartupScene;
    };
}  // namespace Nth
