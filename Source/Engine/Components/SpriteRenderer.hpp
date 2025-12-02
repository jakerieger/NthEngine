/// @author Jake Rieger
/// @created 11/28/25
///

#pragma once

#include "CommonPCH.hpp"
#include "Rendering/Geometry.hpp"

namespace Nth {
    struct SpriteRenderer {
        u32 textureId;
        GeometryHandle geometry;

        ~SpriteRenderer() {
            geometry.reset();
        }
    };
}  // namespace Nth
