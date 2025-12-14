/*
 *  Filename: SceneParser.cpp
 *  This code is part of the Astera core library
 *  Copyright 2025 Jake Rieger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include "SceneParser.hpp"

#include "BinaryWriter.hpp"
#include "Content.hpp"
#include "Log.hpp"
#include "SceneDescriptor.hpp"
#include "SceneState.hpp"
#include "TextureManager.hpp"
#include "ScriptEngine.hpp"
#include "IO.hpp"
#include "Components/Behavior.hpp"
#include "Components/Rigidbody2D.hpp"

#include <pugixml.hpp>

namespace Astera {
    static Rigidbody2DDescriptor ParseRigidbodyComponentXML(const pugi::xml_node& rigidbodyNode) {
        Rigidbody2DDescriptor rigidbody {};

        if (const auto node = rigidbodyNode.child("BodyType")) {
            rigidbody.type = node.child_value();
        }
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
        if (const auto node = rigidbodyNode.child("Torque")) {
            rigidbody.torque = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Mass")) {
            rigidbody.mass = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("InverseMass")) {
            rigidbody.inverseMass = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Inertia")) {
            rigidbody.inertia = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("InverseInertia")) {
            rigidbody.inverseInertia = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Restitution")) {
            rigidbody.restitution = StringToF32(node.child_value());
        }
        if (const auto node = rigidbodyNode.child("Friction")) {
            rigidbody.friction = StringToF32(node.child_value());
        }
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

    static TransformDescriptor ParseTransformComponentXML(const pugi::xml_node& transformNode) {
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

    static SpriteRendererDescriptor ParseSpriteRendererComponentXML(const pugi::xml_node& spriteNode) {
        SpriteRendererDescriptor renderer {};

        if (const auto node = spriteNode.child("Texture")) {
            renderer.texture = node.child_value();
        }

        return renderer;
    }

    static BehaviorDescriptor ParseBehaviorComponentXML(const pugi::xml_node& behaviorNode) {
        BehaviorDescriptor behavior {};

        if (const auto scriptNode = behaviorNode.child("Script")) {
            behavior.id     = scriptNode.attribute("id").as_int();
            behavior.script = scriptNode.child_value();
        }

        return behavior;
    }

    static void ParseEntityXML(const pugi::xml_node& entityNode, EntityDescriptor& entity) {
        entity.id   = entityNode.attribute("id").as_int();
        entity.name = entityNode.attribute("name").as_string();

        const auto componentsNode = entityNode.child("Components");
        if (!componentsNode) {
            throw std::runtime_error("Entity is missing Transform component.");
        }

        // Parse Transform component
        if (const auto node = componentsNode.child("Transform")) {
            entity.transform = ParseTransformComponentXML(node);
        }

        // Parse SpriteRenderer component
        if (const auto node = componentsNode.child("SpriteRenderer")) {
            entity.spriteRenderer = ParseSpriteRendererComponentXML(node);
        }

        if (const auto node = componentsNode.child("Rigidbody2D")) {
            entity.rigidbody2D = ParseRigidbodyComponentXML(node);
        }

        if (const auto node = componentsNode.child("Behavior")) {
            entity.behavior = ParseBehaviorComponentXML(node);
        }
    }

    void SceneParser::StateToDescriptor(const SceneState& state, SceneDescriptor& outDescriptor) {
        throw ASTERA_NOT_IMPLEMENTED;
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
                const auto scriptPath       = Content::Get<ContentType::Script>(entity.behavior->script);
                const auto readScriptResult = IO::ReadText(scriptPath);
                if (!readScriptResult.has_value()) {
                    Log::Error("SceneParser", "Failed to read script file: {}", readScriptResult.error());
                }
                scriptEngine.LoadScript(*readScriptResult, entity.behavior->id);

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

    void SceneParser::SerializeDescriptorXML(const SceneDescriptor& descriptor, const fs::path& filename) {
        throw ASTERA_NOT_IMPLEMENTED;
    }

    void SceneParser::WriteEntityBytes(BinaryWriter& writer, const EntityDescriptor& entity) {
        writer.WriteUInt32(entity.id);
        writer.WriteString(entity.name);

        u32 componentCount           = 1;  // transform is always present
        const auto componentCountPos = writer.Tell();
        writer.WriteUInt32(componentCount);  // write initial count

        if (entity.spriteRenderer.has_value()) {
            componentCount++;
            writer.WriteUInt32(u32(ComponentType::SpriteRenderer));
            writer.WriteString(entity.spriteRenderer->texture);
        }
        if (entity.rigidbody2D.has_value()) {
            componentCount++;
            writer.WriteUInt32(u32(ComponentType::Rigidbody2D));
            writer.WriteUInt32(8);
        }
        if (entity.behavior.has_value()) {
            componentCount++;
            writer.WriteUInt32(u32(ComponentType::Behavior));
            writer.WriteUInt32(8 + CAST<u32>(entity.behavior->script.size()));
            writer.WriteUInt32(entity.behavior->id);
            writer.WriteString(entity.behavior->script);
        }

        // update count
        writer.UpdateAt(componentCountPos, componentCount);
    }

    // [Header]
    // - Magic Number (4 bytes): "SCNE" - for file validation
    // - Version (4 bytes): Format version for compatibility
    // - Scene Name Length (4 bytes)
    // - Scene Name (variable)
    //
    // [Entity Count] (4 bytes)
    //
    // [For each Entity]
    //   - Entity ID (4 bytes)
    //   - Name Length (4 bytes)
    //   - Name (variable)
    //   - Component Count (4 bytes)
    //
    //   [For each Component]
    //     - Component Type ID (4 bytes) - enum/hash identifying component type
    //     - Component Data Size (4 bytes) - size of following data block
    //     - Component Data (variable)
    void SceneParser::SerializeDescriptorBytes(const SceneDescriptor& descriptor, const fs::path& filename) {
        const Header header {.sceneNameLength = CAST<u32>(descriptor.name.length()),
                             .sceneName       = descriptor.name.c_str()};

        BinaryWriter writer(2_KB);
        writer.WriteBytes(header.magic, sizeof(header.magic));
        writer.WriteUInt32(header.version);
        writer.WriteString(
          descriptor.name);  // WriteString writes the size prefix for us so we can omit writing it here
        writer.WriteUInt32(descriptor.entities.size());

        for (const auto& entity : descriptor.entities) {
            WriteEntityBytes(writer, entity);
        }

        if (!writer.SaveToFile(filename)) {
            throw std::runtime_error("Failed to save scene descriptor to file");
        }
    }

    void SceneParser::DeserializeDescriptorXML(const fs::path& filename, SceneDescriptor& outDescriptor) {
        pugi::xml_document doc;
        const pugi::xml_parse_result result = doc.load_file(filename.string().c_str());

        if (!result) {
            throw std::runtime_error(fmt::format("XML parsing error: {}", result.description()));
        }

        const auto sceneNode = doc.child("Scene");
        if (!sceneNode) {
            throw std::runtime_error("No scene node found");
        }

        // Parse scene name
        outDescriptor.name = sceneNode.attribute("name").as_string();

        // Parse entities
        if (const auto entitiesNode = sceneNode.child("Entities")) {
            for (auto entityNode : entitiesNode.children("Entity")) {
                EntityDescriptor entity {};
                entity.id = entityNode.attribute("id").as_int();
                ParseEntityXML(entityNode, entity);
                outDescriptor.entities.push_back(entity);
            }
        }
    }

    void SceneParser::DeserializeDescriptorBytes(const vector<u8>& bytes, SceneDescriptor& outDescriptor) {
        throw ASTERA_NOT_IMPLEMENTED;
    }
}  // namespace Astera