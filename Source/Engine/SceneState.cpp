/// @author Jake Rieger
/// @created 11/28/25
///
#include "SceneState.hpp"
#include "Log.hpp"

namespace Nth {
    SceneState::SceneState() = default;
    SceneState::SceneState(SceneState&& other) noexcept : mRegistry(std::exchange(other.mRegistry, {})) {}

    SceneState& SceneState::operator=(SceneState&& other) noexcept {
        if (this != &other) { mRegistry = std::exchange(other.mRegistry, {}); }
        return *this;
    }

    Entity SceneState::CreateEntity(const string& name) {
        N_ASSERT(!name.empty());
        const auto entity = mRegistry.create();
        mRegistry.emplace<Transform>(entity);

        mEntityNames[entity] = name;

        return entity;
    }

    void SceneState::DestroyEntity(Entity entity) {
        mRegistry.destroy(entity);
        mEntityNames.erase(entity);
    }

    size_t SceneState::GetEntityCount() const {
        const auto iter = mRegistry.view<Transform>().each();
        return CAST<size_t>(Nth::Distance(iter));
    }

    Transform& SceneState::GetTransform(Entity entity) {
        return GetComponent<Transform>(entity);
    }

    const string& SceneState::GetEntityName(Entity entity) const {
        return mEntityNames.at(entity);
    }

    SceneState::~SceneState() {
        Reset();
    }

    void SceneState::Reset() {
        mRegistry.clear();
    }
}  // namespace Nth