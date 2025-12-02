// Author: Jake Rieger
// Created: 12/2/25.
//

#include "SceneParser.hpp"

#include "Content.hpp"
#include "SceneDescriptor.hpp"
#include "SceneState.hpp"
#include "TextureManager.hpp"

#include <pugixml.hpp>

namespace Nth {
    static TransformDescriptor ParseTransformComponent(const pugi::xml_node& transformNode) {
        TransformDescriptor transform {};

        const pugi::xml_node posNode = transformNode.child("Position");
        if (posNode) {
            transform.position.x = posNode.attribute("x").as_float();
            transform.position.y = posNode.attribute("y").as_float();
        }

        const pugi::xml_node rotNode = transformNode.child("Rotation");
        if (rotNode) {
            transform.rotation.x = rotNode.attribute("x").as_float();
            transform.rotation.y = rotNode.attribute("y").as_float();
        }

        const pugi::xml_node scaleNode = transformNode.child("Scale");
        if (scaleNode) {
            transform.scale.x = scaleNode.attribute("x").as_float();
            transform.scale.y = scaleNode.attribute("y").as_float();
        }

        return transform;
    }

    static SpriteRendererDescriptor ParseSpriteRendererComponent(const pugi::xml_node& spriteNode) {
        SpriteRendererDescriptor renderer {};
        const pugi::xml_node textureNode = spriteNode.child("Texture");
        if (textureNode) { renderer.texture = textureNode.child_value(); }
        return renderer;
    }

    static void ParseEntity(const pugi::xml_node& entityNode, EntityDescriptor& entity) {
        entity.id   = entityNode.attribute("id").as_int();
        entity.name = entityNode.attribute("name").as_string();

        const pugi::xml_node componentsNode = entityNode.child("Components");
        if (!componentsNode) { throw std::runtime_error("Entity is missing Transform component."); }

        // Parse Transform component
        const pugi::xml_node transformNode = componentsNode.child("Transform");
        if (transformNode) { entity.transform = ParseTransformComponent(transformNode); }

        // Parse SpriteRenderer component
        const pugi::xml_node spriteNode = componentsNode.child("SpriteRenderer");
        if (spriteNode) { entity.spriteRenderer = ParseSpriteRendererComponent(spriteNode); }
    }

    void SceneParser::StateToDescriptor(const SceneState& state, SceneDescriptor& outDescriptor) {
        throw N_NOT_IMPLEMENTED;
    }

    void SceneParser::DescriptorToState(const SceneDescriptor& descriptor, SceneState& outState) {
        auto entities = descriptor.entities;
        std::ranges::sort(entities, {}, &EntityDescriptor::id);  // Sort by id to ensure correct insertion order
        for (const auto& entity : entities) {
            const auto newEntity = outState.CreateEntity();
            auto& transform      = outState.GetTransform(newEntity);
            transform.position   = entity.transform.position;
            transform.rotation   = entity.transform.rotation;
            transform.scale      = entity.transform.scale;

            if (entity.spriteRenderer.has_value()) {
                auto& spriteRenderer    = outState.AddComponent<SpriteRenderer>(newEntity);
                spriteRenderer.geometry = Geometry::CreateQuad();
                spriteRenderer.textureId =
                  TextureManager::Load(Content::GetContentPath(entity.spriteRenderer->texture));
            }
        }
    }

    void SceneParser::SerializeDescriptor(const SceneDescriptor& descriptor) {
        throw N_NOT_IMPLEMENTED;
    }

    void SceneParser::DeserializeDescriptor(const fs::path& filename, SceneDescriptor& outDescriptor) {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.c_str());

        if (!result) { throw std::runtime_error(fmt::format("XML parsing error: {}", result.description())); }

        const pugi::xml_node sceneNode = doc.child("Scene");
        if (!sceneNode) { throw std::runtime_error("No scene node found"); }

        // Parse scene name
        outDescriptor.name = sceneNode.attribute("name").as_string();

        // Parse entities
        const pugi::xml_node entitiesNode = sceneNode.child("Entities");
        if (entitiesNode) {
            for (pugi::xml_node entityNode : entitiesNode.children("Entity")) {
                EntityDescriptor entity {};
                ParseEntity(entityNode, entity);
                outDescriptor.entities.push_back(entity);
            }
        }
    }

    void SceneParser::DeserializeDescriptor(const string& source, SceneDescriptor& outDescriptor) {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_string(source.c_str());

        if (!result) { throw std::runtime_error(fmt::format("XML parsing error: {}", result.description())); }

        const pugi::xml_node sceneNode = doc.child("Scene");
        if (!sceneNode) { throw std::runtime_error("No scene node found"); }

        // Parse scene name
        outDescriptor.name = sceneNode.attribute("name").as_string();

        // Parse entities
        const pugi::xml_node entitiesNode = sceneNode.child("Entities");
        if (entitiesNode) {
            for (pugi::xml_node entityNode : entitiesNode.children("Entity")) {
                EntityDescriptor entity {};
                ParseEntity(entityNode, entity);
                outDescriptor.entities.push_back(entity);
            }
        }
    }
}  // namespace Nth