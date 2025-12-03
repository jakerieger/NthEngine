/// @author Jake Rieger
/// @created 11/28/25
///

#include "Scene.hpp"
#include "SceneParser.hpp"
#include "ScriptTypeRegistry.hpp"
#include "Log.hpp"

namespace Nth {
    Scene::~Scene() {
        mState.Reset();
    }

    void Scene::Awake(ScriptEngine& scriptEngine) {
        const auto iter = mState.View<Transform, Behavior>().each();
        for (auto [entity, transform, behavior] : iter) {
            BehaviorEntity behaviorEntity(std::to_string(CAST<u32>(entity)), &transform);
            scriptEngine.CallAwakeBehavior(behavior.id, behaviorEntity);
        }
    }

    void Scene::Update(ScriptEngine& scriptEngine, const Clock& clock) {
        const auto iter = mState.View<Transform, Behavior>().each();
        for (auto [entity, transform, behavior] : iter) {
            BehaviorEntity behaviorEntity(std::to_string(CAST<u32>(entity)), &transform);
            scriptEngine.CallUpdateBehavior(behavior.id, behaviorEntity, clock);
        }
    }

    void Scene::LateUpdate(ScriptEngine& scriptEngine) {
        const auto iter = mState.View<Transform, Behavior>().each();
        for (auto [entity, transform, behavior] : iter) {
            BehaviorEntity behaviorEntity(std::to_string(CAST<u32>(entity)), &transform);
            scriptEngine.CallLateUpdateBehavior(behavior.id, behaviorEntity);
        }
    }

    void Scene::Destroyed(ScriptEngine& scriptEngine) {
        const auto iter = mState.View<Transform, Behavior>().each();
        for (auto [entity, transform, behavior] : iter) {
            BehaviorEntity behaviorEntity(std::to_string(CAST<u32>(entity)), &transform);
            scriptEngine.CallDestroyedBehavior(behavior.id, behaviorEntity);
        }
    }

    void Scene::Render(RenderContext& context) {
        u32 screenWidth = 0, screenHeight = 0;
        context.GetViewportDimensions(screenWidth, screenHeight);

        const auto iter = mState.View<Transform, SpriteRenderer>().each();
        for (auto [entity, transform, sprite] : iter) {
            context.Submit(DrawSpriteCommand {sprite, transform, {screenWidth, screenHeight}, {1, 1, 1, 1}});
        }
    }

    void Scene::Load(const fs::path& filename, ScriptEngine& scriptEngine) {
        mState.Reset();
        SceneDescriptor descriptor;
        SceneParser::DeserializeDescriptor(filename, descriptor);
        SceneParser::DescriptorToState(descriptor, mState, scriptEngine);

        Log::Debug("Scene", "Loaded scene: `{}`", descriptor.name);
    }

    void Scene::Load(const string& source, ScriptEngine& scriptEngine) {
        mState.Reset();
        SceneDescriptor descriptor;
        SceneParser::DeserializeDescriptor(source, descriptor);
        SceneParser::DescriptorToState(descriptor, mState, scriptEngine);

        Log::Debug("Scene", "Loaded scene: `{}`", descriptor.name);
    }
}  // namespace Nth