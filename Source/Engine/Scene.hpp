/// @author Jake Rieger
/// @created 11/28/25
///

#pragma once

#include "Clock.hpp"
#include "CommonPCH.hpp"
#include "SceneState.hpp"
#include "ScriptEngine.hpp"
#include "Rendering/RenderContext.hpp"

namespace Nth {
    class Scene {
        friend class Game;

    public:
        Scene() = default;
        ~Scene();

        N_CLASS_PREVENT_MOVES_COPIES(Scene)

        /// Lifecycle hooks
        void Awake(ScriptEngine& scriptEngine);
        void Update(ScriptEngine& scriptEngine, const Clock& clock);
        void LateUpdate(ScriptEngine& scriptEngine);
        void Destroyed(ScriptEngine& scriptEngine);

        void Render(RenderContext& context);

        void Load(const fs::path& filename, ScriptEngine& scriptEngine);
        void Load(const string& source, ScriptEngine& scriptEngine);

        N_ND SceneState& GetState() {
            return mState;
        }

    private:
        SceneState mState;
    };
}  // namespace Nth
