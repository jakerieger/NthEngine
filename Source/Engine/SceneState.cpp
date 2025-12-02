/// @author Jake Rieger
/// @created 11/28/25
///
#include "SceneState.hpp"

namespace Nth {
    SceneState::~SceneState() {
        Reset();
    }

    void SceneState::Reset() {
        mRegistry.clear();  // Clear the entity registry
    }
}  // namespace Nth