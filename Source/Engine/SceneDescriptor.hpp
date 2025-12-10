/// @author Jake Rieger
/// @created 12/2/25
///
#pragma once

#include "Common/CommonPCH.hpp"

namespace Nth {
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
        string texture;
    };

    struct BehaviorDescriptor {
        u64 id;
        string script;
    };

    struct EntityDescriptor {
        u32 id {};
        string name {};
        TransformDescriptor transform {};
        optional<SpriteRendererDescriptor> spriteRenderer {};
        optional<BehaviorDescriptor> behavior {};
        optional<Rigidbody2DDescriptor> rigidbody2D {};
    };

    struct SceneDescriptor {
        string name;
        vector<EntityDescriptor> entities;
    };
}  // namespace Nth
