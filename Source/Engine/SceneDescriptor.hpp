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
        const char* texture;
    };

    struct EntityDescriptor {
        u32 id {};
        const char* name {};
        TransformDescriptor transform {};
        optional<SpriteRendererDescriptor> spriteRenderer {};
    };

    struct SceneDescriptor {
        string name;
        vector<EntityDescriptor> entities;
    };
}  // namespace Nth
