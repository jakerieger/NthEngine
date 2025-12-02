/// @author Jake Rieger
/// @created 11/28/25
///
#include "Scene.hpp"

#include "SceneParser.hpp"

namespace Nth {
    Scene::~Scene() {
        mState.Reset();
    }

    void Scene::Awake() {
        N_UNUSED(this);
    }

    void Scene::Update(const Clock& clock) {
        N_UNUSED(clock);
    }

    void Scene::LateUpdate() {
        N_UNUSED(this);
    }

    void Scene::Destroyed() {
        N_UNUSED(this);
    }

    void Scene::Render(RenderContext& context) {
        u32 screenWidth, screenHeight;
        context.GetViewportDimensions(screenWidth, screenHeight);

        const auto iter = mState.View<Transform, SpriteRenderer>().each();
        for (auto [entity, transform, sprite] : iter) {
            context.Submit(DrawSpriteCommand {sprite, transform, {screenWidth, screenHeight}, {1, 1, 1, 1}});
        }
    }

    void Scene::Load(const fs::path& filename) {
        mState.Reset();
        SceneDescriptor descriptor;
        SceneParser::DeserializeDescriptor(filename, descriptor);
        SceneParser::DescriptorToState(descriptor, mState);
    }

    void Scene::Load(const string& source) {}
}  // namespace Nth