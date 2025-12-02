// Author: Jake Rieger
// Created: 12/2/25.
//

#pragma once

#include "CommonPCH.hpp"

namespace Nth {
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
    };

    struct SceneDescriptor {
        string name;
        vector<EntityDescriptor> entities;
    };
}  // namespace Nth
