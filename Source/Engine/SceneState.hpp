/// @author Jake Rieger
/// @created 11/28/25
///

#pragma once

#include "Common/CommonPCH.hpp"

#pragma region Components
#include "Components/Camera.hpp"
#include "Components/Transform.hpp"
#include "Components/SpriteRenderer.hpp"
#include "Components/Behavior.hpp"
#include "Components/Rigidbody2D.hpp"
#include "Components/Collider2D.hpp"
#pragma endregion

#include <entt/entt.hpp>

namespace Nth {
    /// @brief Type alias for entt::entity. Casts to integer types as id value.
    using Entity = entt::entity;

    /// @brief Ensures only valid components are passed to the ECS
    /// @tparam T Component type
    template<typename T>
    concept ValidComponent =
      std::is_same_v<T, Transform> || std::is_same_v<T, SpriteRenderer> || std::is_same_v<T, Camera> ||
      std::is_same_v<T, Behavior> || std::is_same_v<T, Rigidbody2D> || std::is_same_v<T, Collider2D>;

    /// @brief Holds the current state of the scene such as entities, components, and scene-specific components like
    /// cameras and audio
    class SceneState {
    public:
        SceneState();
        ~SceneState();

        N_CLASS_PREVENT_COPIES(SceneState)

        SceneState(SceneState&& other) noexcept;
        SceneState& operator=(SceneState&& other) noexcept;

        /// @brief Resets the scene to its initial state
        void Reset();

        /// @brief Creates a new entity in the scene tree. All entities are required to have a Transform component
        /// and this method automatically adds it when creating the new entity.
        Entity CreateEntity(const string& name);

        /// @brief Destroys the provided entity from the scene registry
        void DestroyEntity(Entity entity);

        /// @brief Get number of entities currently in scene
        /// @returns Number of entities
        N_ND size_t GetEntityCount() const;

        /// @brief Gets the transform component of the specified entity. All entities are created with a Transform
        /// component attached by default.
        /// @param entity Entity id
        /// @returns Transform component reference
        N_ND Transform& GetTransform(Entity entity);

        /// @brief Returns the name of the given entity if it exists
        /// @param entity Entity id
        /// @return Name of entity
        N_ND const string& GetEntityName(Entity entity) const;

        /// @brief Attaches specified component to specified entity
        /// @tparam Component Component type
        /// @tparam Args Component construction arguments
        /// @param entity The entity to attach the component to
        /// @param args Arguments for component construction
        /// @returns Newly added component reference
        template<typename Component, typename... Args>
            requires ValidComponent<Component>
        Component& AddComponent(Entity entity, Args&&... args) {
            mRegistry.emplace<Component>(entity, std::forward<Args>(args)...);
            return mRegistry.get<Component>(entity);
        }

        /// @brief Fetches the given component if it exists on the provided entity
        /// @tparam Component Component type
        /// @param entity Entity id
        /// @returns Component reference
        template<typename Component>
            requires ValidComponent<Component>
        Component& GetComponent(Entity entity) {
            return mRegistry.get<Component>(entity);
        }

        /// @brief Returns a view of all entities with the provided components. `.each()` can be used to get an
        /// iterator.
        /// @tparam Components Component types to get
        /// @returns EnTT view
        template<typename... Components>
            requires(ValidComponent<Components> && ...)
        auto View() {
            return mRegistry.view<Components...>();
        }

        /// @brief Returns an array of all the entities that contain the provided component
        /// @tparam Component Component type
        /// @returns Vector of entity IDs
        template<typename Component>
            requires ValidComponent<Component>
        vector<Entity> GetAllEntitiesWithComponent() {
            vector<Entity> entities;

            const auto iter = mRegistry.view<Component>().each();
            for (const auto [entity, component] : iter) {
                entities.push_back(entity);
            }

            return entities;
        }

    private:
        entt::registry mRegistry {};
        unordered_map<Entity, string> mEntityNames {};
    };
}  // namespace Nth
