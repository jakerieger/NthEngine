/// @author Jake Rieger
/// @created 11/30/25
///

#pragma once

#include "CommonPCH.hpp"
#include "VertexArray.hpp"

namespace Nth {
    /// @brief Vertex structure for sprite/quad rendering (position + texcoord packed)
    struct SpriteVertex {
        f32 x, y;  // Position
        f32 u, v;  // Texture coordinates

        SpriteVertex() = default;
        SpriteVertex(f32 x, f32 y, f32 u, f32 v) : x(x), y(y), u(u), v(v) {}
    };

    using GeometryHandle = shared_ptr<class Geometry>;

    /// @brief High-level geometry abstraction that manages vertex/index data and VAO setup
    class Geometry {
    public:
        Geometry()  = default;
        ~Geometry() = default;

        N_CLASS_PREVENT_MOVES_COPIES(Geometry)

        /// @brief Create a quad geometry for sprite rendering
        /// @param width Width of the quad
        /// @param height Height of the quad
        /// @return Shared pointer to the created geometry
        static GeometryHandle CreateQuad(f32 width = 1.0f, f32 height = 1.0f);

        /// @brief Bind this geometry for rendering
        void Bind() const;

        void Destroy() const;

        /// @brief Unbind the currently bound geometry
        static void Unbind();

        /// @brief Draw this geometry using indices
        void DrawIndexed() const;

        N_ND const shared_ptr<VertexArray>& GetVertexArray() const {
            return mVertexArray;
        }

    private:
        shared_ptr<VertexArray> mVertexArray;
    };
}  // namespace Nth