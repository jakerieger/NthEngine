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
#include "Log.hpp"
#include "SceneDescriptor.hpp"
#include "SceneState.hpp"
#include "ScriptEngine.hpp"
#include "EntityBuilder.hpp"
#include "StringConvert.inl"

#include <pugixml.hpp>

namespace Astera {
    static SoundSourceDescriptor ParseSoundSourceComponentXML(const pugi::xml_node& soundSourceNode) {
        SoundSourceDescriptor sound {};

        if (const auto node = soundSourceNode.child("Name")) {
            sound.name = node.child_value();
        }

        if (const auto node = soundSourceNode.child("Source")) {
            sound.sound = StringConvert::StringToU64Or(node.child_value(), kInvalidAssetID);
        }

        if (const auto node = soundSourceNode.child("Volume")) {
            sound.volume = StringConvert::StringToF32Or(node.child_value(), 0.f);
        }

        return sound;
    }

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
            rigidbody.angularVelocity = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("AngularAcceleration")) {
            rigidbody.angularAcceleration = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("Torque")) {
            rigidbody.torque = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("Mass")) {
            rigidbody.mass = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("InverseMass")) {
            rigidbody.inverseMass = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("Inertia")) {
            rigidbody.inertia = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("InverseInertia")) {
            rigidbody.inverseInertia = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("Restitution")) {
            rigidbody.restitution = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("Friction")) {
            rigidbody.friction = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("LinearDamping")) {
            rigidbody.linearDamping = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("AngularDamping")) {
            rigidbody.angularDamping = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("GravityScale")) {
            rigidbody.gravityScale = StringConvert::StringToF32Or(node.child_value(), 0);
        }
        if (const auto node = rigidbodyNode.child("LockRotation")) {
            rigidbody.lockRotation = ASTERA_STREQ(node.child_value(), "true") ? true : false;
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
            renderer.texture = StringConvert::StringToU64Or(node.child_value(), 0);
        }

        return renderer;
    }

    static BehaviorDescriptor ParseBehaviorComponentXML(const pugi::xml_node& behaviorNode) {
        BehaviorDescriptor behavior {};

        if (const auto node = behaviorNode.child("Script")) {
            behavior.script = StringConvert::StringToU64Or(node.child_value(), 0);
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

        if (const auto node = componentsNode.child("Collider2D")) {
            entity.collider2D = {};  // TODO
        }

        if (const auto node = componentsNode.child("Camera")) {
            entity.camera = {};  // TODO
        }

        if (const auto node = componentsNode.child("SoundSource")) {
            entity.soundSource = ParseSoundSourceComponentXML(node);
        }
    }

    void SceneParser::StateToDescriptor(const SceneState& state, SceneDescriptor& outDescriptor) {
        throw ASTERA_NOT_IMPLEMENTED;
    }

    void SceneParser::DescriptorToScene(const SceneDescriptor& descriptor, Scene* scene, ScriptEngine& scriptEngine) {
        for (const auto& entity : descriptor.entities) {
            auto builder = EntityBuilder::Create(scene, entity.name);
            builder.SetTransform(entity.transform);

            if (entity.spriteRenderer.has_value()) {
                builder.AddSpriteRenderer(*entity.spriteRenderer);
            }

            if (entity.behavior.has_value()) {
                builder.AddBehavior(*entity.behavior, scriptEngine);
            }

            if (entity.rigidbody2D.has_value()) {
                builder.AddRigidbody2D(*entity.rigidbody2D);
            }

            if (entity.collider2D.has_value()) {
                builder.AddCollider2D(*entity.collider2D);
            }

            if (entity.camera.has_value()) {
                builder.AddCamera(*entity.camera);
            }

            if (entity.soundSource.has_value()) {
                builder.AddSoundSource(*entity.soundSource);
            }

            const auto newEntity = builder.Build();
            Log::Info("SceneParser", "Loaded entity '{} (ID: {})' to scene state", entity.name, (u32)newEntity);
        }
    }

    void SceneParser::SerializeDescriptorXML(const SceneDescriptor& descriptor, const Path& filename) {
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
            writer.WriteUInt64(entity.spriteRenderer->texture);
        }
        if (entity.rigidbody2D.has_value()) {
            componentCount++;
            writer.WriteUInt32(u32(ComponentType::Rigidbody2D));
            writer.WriteUInt32(8);
        }
        if (entity.behavior.has_value()) {
            componentCount++;
            writer.WriteUInt32(u32(ComponentType::Behavior));
            writer.WriteUInt64(entity.behavior->script);
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
    void SceneParser::SerializeDescriptorBytes(const SceneDescriptor& descriptor, const Path& filename) {
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

    void SceneParser::DeserializeDescriptorXML(const Path& filename, SceneDescriptor& outDescriptor) {
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
        outDescriptor.name   = sceneNode.attribute("name").as_string();
        const auto entryAttr = sceneNode.attribute("entry");
        if (!entryAttr.empty()) {
            outDescriptor.entry = entryAttr.as_bool();
        }

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