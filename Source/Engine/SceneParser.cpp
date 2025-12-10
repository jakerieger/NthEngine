/// @author Jake Rieger
/// @created 12/2/25
///

#include "SceneParser.hpp"
#include "Content.hpp"
#include "Log.hpp"
#include "SceneDescriptor.hpp"
#include "SceneState.hpp"
#include "TextureManager.hpp"
#include "ScriptEngine.hpp"
#include "Components/Behavior.hpp"
#include "Components/Rigidbody2D.hpp"

#include <pugixml.hpp>

namespace Nth {
    static Rigidbody2DDescriptor ParseRigidbodyComponent(const pugi::xml_node& rigidbodyNode) {
        Rigidbody2DDescriptor rigidbody {};

        if (const auto node = rigidbodyNode.child("BodyType")) { rigidbody.type = node.child_value(); }
        if (const auto node = rigidbodyNode.child("Velocity")) {
            rigidbody.velocity.x = node.attribute("x").as_float();
            rigidbody.velocity.y = node.attribute("y").as_float();
        }
        if (const auto node = rigidbodyNode.child("Acceleration")) {
            rigidbody.acceleration.x = node.attribute("x").as_float();
            rigidbody.acceleration.y = node.attribute("y").as_float();
        }
        if (const auto node = rigidbodyNode.child("Force")) {
            rigidbody.force.x = node.attribute("x").as_float();
            rigidbody.force.y = node.attribute("y").as_float();
        }
        if (const auto node = rigidbodyNode.child("AngularVelocity")) {
            rigidbody.angularVelocity = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("AngularAcceleration")) {
            rigidbody.angularAcceleration = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Torque")) { rigidbody.torque = StringToF32(node.child_value()); }
        if (const auto node = rigidbodyNode.child("Mass")) { rigidbody.mass = StringToF32(node.child_value()); }
        if (const auto node = rigidbodyNode.child("InverseMass")) {
            rigidbody.inverseMass = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Inertia")) { rigidbody.inertia = StringToF32(node.child_value()); }
        if (const auto node = rigidbodyNode.child("InverseInertia")) {
            rigidbody.inverseInertia = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Restitution")) {
            rigidbody.restitution = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Friction")) { rigidbody.friction = StringToF32(node.child_value()); }
        if (const auto node = rigidbodyNode.child("LinearDamping")) {
            rigidbody.linearDamping = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("AngularDamping")) {
            rigidbody.angularDamping = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("GravityScale")) {
            rigidbody.gravityScale = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("LockRotation")) {
            rigidbody.lockRotation = (strcmp(node.child_value(), "true") == 0) ? true : false;
        }

        return rigidbody;
    }

    static TransformDescriptor ParseTransformComponent(const pugi::xml_node& transformNode) {
        TransformDescriptor transform {};

        if (const auto node = transformNode.child("Position")) {
            transform.position.x = node.attribute("x").as_float();
            transform.position.y = node.attribute("y").as_float();
        }

        if (const auto node = transformNode.child("Rotation")) {
            transform.rotation.x = node.attribute("x").as_float();
            transform.rotation.y = node.attribute("y").as_float();
        }

        if (const auto node = transformNode.child("Scale")) {
            transform.scale.x = node.attribute("x").as_float();
            transform.scale.y = node.attribute("y").as_float();
        }

        return transform;
    }

    static SpriteRendererDescriptor ParseSpriteRendererComponent(const pugi::xml_node& spriteNode) {
        SpriteRendererDescriptor renderer {};

        if (const auto node = spriteNode.child("Texture")) { renderer.texture = node.child_value(); }

        return renderer;
    }

    static BehaviorDescriptor ParseBehaviorComponent(const pugi::xml_node& behaviorNode) {
        BehaviorDescriptor behavior {};

        if (const auto scriptNode = behaviorNode.child("Script")) {
            behavior.id     = scriptNode.attribute("id").as_int();
            behavior.script = scriptNode.child_value();
        }

        return behavior;
    }

    static void ParseEntity(const pugi::xml_node& entityNode, EntityDescriptor& entity) {
        entity.id   = entityNode.attribute("id").as_int();
        entity.name = entityNode.attribute("name").as_string();

        const auto componentsNode = entityNode.child("Components");
        if (!componentsNode) { throw std::runtime_error("Entity is missing Transform component."); }

        // Parse Transform component
        if (const auto node = componentsNode.child("Transform")) { entity.transform = ParseTransformComponent(node); }

        // Parse SpriteRenderer component
        if (const auto node = componentsNode.child("SpriteRenderer")) {
            entity.spriteRenderer = ParseSpriteRendererComponent(node);
        }

        if (const auto node = componentsNode.child("Rigidbody2D")) {
            entity.rigidbody2D = ParseRigidbodyComponent(node);
        }

        if (const auto node = componentsNode.child("Behavior")) { entity.behavior = ParseBehaviorComponent(node); }
    }

    void SceneParser::StateToDescriptor(const SceneState& state, SceneDescriptor& outDescriptor) {
        throw N_NOT_IMPLEMENTED;
    }

    void SceneParser::DescriptorToState(const SceneDescriptor& descriptor,
                                        SceneState& outState,
                                        ScriptEngine& scriptEngine) {
        for (const auto& entity : descriptor.entities) {
            const auto newEntity              = outState.CreateEntity(entity.name);
            auto& [position, rotation, scale] = outState.GetTransform(newEntity);
            position                          = entity.transform.position;
            rotation                          = entity.transform.rotation;
            scale                             = entity.transform.scale;

            if (entity.spriteRenderer.has_value()) {
                auto& [textureId, geometry] = outState.AddComponent<SpriteRenderer>(newEntity);
                geometry                    = Geometry::CreateQuad();
                textureId = TextureManager::Load(Content::Get<ContentType::Sprite>(entity.spriteRenderer->texture));
            }

            if (entity.behavior.has_value()) {
                // Load script
                const auto scriptPath   = Content::Get<ContentType::Script>(entity.behavior->script);
                const auto scriptSource = IO::ReadString(scriptPath);
                scriptEngine.LoadScript(scriptSource, entity.behavior->id);

                auto& [id, script] = outState.AddComponent<Behavior>(newEntity);
                id                 = entity.behavior->id;
                script             = entity.behavior->script;
            }

            if (entity.rigidbody2D.has_value()) {
                auto desc = *entity.rigidbody2D;
                // Load rigidbody
                auto& rigidbody = outState.AddComponent<Rigidbody2D>(newEntity);
                if (desc.type == "Static") {
                    rigidbody.type = BodyType::Static;
                } else if (desc.type == "Dynamic") {
                    rigidbody.type = BodyType::Dynamic;
                } else if (desc.type == "Kinematic") {
                    rigidbody.type = BodyType::Kinematic;
                } else {
                    Log::Error("SceneParser", "BodyType incorrect for Rigidbody2D component: {}", desc.type);
                    return;
                }

                rigidbody.velocity            = desc.velocity;
                rigidbody.acceleration        = desc.acceleration;
                rigidbody.force               = desc.force;
                rigidbody.angularVelocity     = desc.angularVelocity;
                rigidbody.angularAcceleration = desc.angularAcceleration;
                rigidbody.torque              = desc.torque;
                rigidbody.mass                = desc.mass;
                rigidbody.inverseMass         = desc.inverseMass;
                rigidbody.inertia             = desc.inertia;
                rigidbody.inverseInertia      = desc.inverseInertia;
                rigidbody.restitution         = desc.restitution;
                rigidbody.friction            = desc.friction;
                rigidbody.linearDamping       = desc.linearDamping;
                rigidbody.angularDamping      = desc.angularDamping;
                rigidbody.gravityScale        = desc.gravityScale;
                rigidbody.lockRotation        = desc.lockRotation;
            }
        }
    }

    void SceneParser::SerializeDescriptor(const SceneDescriptor& descriptor) {
        throw N_NOT_IMPLEMENTED;
    }

    void SceneParser::DeserializeDescriptor(const fs::path& filename, SceneDescriptor& outDescriptor) {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.string().c_str());

        if (!result) { throw std::runtime_error(fmt::format("XML parsing error: {}", result.description())); }

        const auto sceneNode = doc.child("Scene");
        if (!sceneNode) { throw std::runtime_error("No scene node found"); }

        // Parse scene name
        outDescriptor.name = sceneNode.attribute("name").as_string();

        // Parse entities
        if (const auto entitiesNode = sceneNode.child("Entities")) {
            for (auto entityNode : entitiesNode.children("Entity")) {
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

        const auto sceneNode = doc.child("Scene");
        if (!sceneNode) { throw std::runtime_error("No scene node found"); }

        // Parse scene name
        outDescriptor.name = sceneNode.attribute("name").as_string();

        // Parse entities
        if (const auto entitiesNode = sceneNode.child("Entities")) {
            for (pugi::xml_node entityNode : entitiesNode.children("Entity")) {
                EntityDescriptor entity {};
                ParseEntity(entityNode, entity);
                outDescriptor.entities.push_back(entity);
            }
        }
    }
}  // namespace Nth