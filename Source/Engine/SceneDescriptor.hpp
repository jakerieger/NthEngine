/*
 *  Filename: SceneDescriptor.hpp
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

#pragma once

#include "Asset.hpp"
#include "EngineCommon.hpp"

namespace Astera {
    struct SoundSourceDescriptor {
        string name;
        AssetID sound;
        f32 volume;
    };

    struct CameraDescriptor {};

    struct Collider2DDescriptor {};

    struct Rigidbody2DDescriptor {
        string type;  // "Static", "Dynamic", or "Kinematic"
        Vec2 velocity;
        Vec2 acceleration;
        Vec2 force;
        f32 angularVelocity;
        f32 angularAcceleration;
        f32 torque;
        f32 mass;
        f32 inverseMass;
        f32 inertia;
        f32 inverseInertia;
        f32 restitution;
        f32 friction;
        f32 linearDamping;
        f32 angularDamping;
        f32 gravityScale;
        bool lockRotation;
    };

    struct TransformDescriptor {
        Vec2 position;
        Vec2 rotation;
        Vec2 scale;
    };

    struct SpriteRendererDescriptor {
        AssetID texture;
    };

    struct BehaviorDescriptor {
        AssetID script;
    };

    struct EntityDescriptor {
        u32 id {};
        string name {};
        TransformDescriptor transform {};
        optional<SpriteRendererDescriptor> spriteRenderer {};
        optional<BehaviorDescriptor> behavior {};
        optional<Rigidbody2DDescriptor> rigidbody2D {};
        optional<Collider2DDescriptor> collider2D {};
        optional<CameraDescriptor> camera {};
        optional<SoundSourceDescriptor> soundSource {};
    };

    struct SceneDescriptor {
        string name;
        bool entry {false};
        vector<EntityDescriptor> entities;
    };
}  // namespace Astera
