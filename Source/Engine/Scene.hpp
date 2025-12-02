/// @author Jake Rieger
/// @created 11/28/25
///

#pragma once

#include "Clock.hpp"
#include "CommonPCH.hpp"
#include "SceneState.hpp"
#include "Rendering/RenderContext.hpp"

namespace Nth {
    class Scene {
    public:
        Scene() = default;
        ~Scene();

        N_CLASS_PREVENT_MOVES_COPIES(Scene)

        /// Lifecycle hooks
        void Awake();
        void Update(const Clock& clock);
        void LateUpdate();
        void Destroyed();

        void Render(RenderContext& context);

        void Load(const fs::path& filename);
        void Load(const string& source);

        N_ND SceneState& GetState() {
            return mState;
        }

    private:
        SceneState mState;
    };
}  // namespace Nth
