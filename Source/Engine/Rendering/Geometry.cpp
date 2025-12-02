/// @author Jake Rieger
/// @created 11/30/25
///
#include "Geometry.hpp"
#include "GLUtils.hpp"
#include "Log.hpp"

namespace Nth {
    GeometryHandle Geometry::CreateQuad(f32 width, f32 height) {
        auto geometry = make_shared<Geometry>();

        // Calculate half-extents for centered quad
        const f32 halfWidth  = width * 0.5f;
        const f32 halfHeight = height * 0.5f;

        // Define quad vertices (position + texcoord packed as vec4)
        // Triangle strip ordering: bottom-left, bottom-right, top-left, top-right
        const SpriteVertex vertices[] = {
          {-halfWidth, -halfHeight, 0.0f, 0.0f},  // Bottom-left
          {halfWidth, -halfHeight, 1.0f, 0.0f},   // Bottom-right
          {-halfWidth, halfHeight, 0.0f, 1.0f},   // Top-left
          {halfWidth, halfHeight, 1.0f, 1.0f},    // Top-right
        };

        // Define indices for two triangles
        const u32 indices[] = {
          0,
          1,
          2,  // First triangle
          2,
          1,
          3  // Second triangle
        };

        // Create vertex buffer and upload data
        auto vertexBuffer = make_shared<VertexBuffer>();
        vertexBuffer->SetData(vertices, sizeof(vertices), BufferUsage::Static);

        // Create index buffer and upload data
        auto indexBuffer = make_shared<IndexBuffer>();
        indexBuffer->SetIndices(indices, 6, BufferUsage::Static);

        // Define vertex layout matching the shader's input
        // layout (location = 0) in vec4 aVertex;
        VertexLayout layout;
        layout.AddAttribute(VertexAttribute("aVertex", AttributeType::Float4));

        // Create and configure VAO
        geometry->mVertexArray = make_shared<VertexArray>();
        geometry->mVertexArray->AddVertexBuffer(vertexBuffer, layout);
        geometry->mVertexArray->SetIndexBuffer(indexBuffer);

        Log::Debug("Geometry", "Created quad geometry ({}x{})", width, height);

        return geometry;
    }

    void Geometry::Bind() const {
        if (mVertexArray) { mVertexArray->Bind(); }
    }

    void Geometry::Destroy() const {
        mVertexArray->Destroy();
    }

    void Geometry::Unbind() {
        VertexArray::Unbind();
    }

    void Geometry::DrawIndexed() const {
        N_ASSERT(mVertexArray != nullptr);
        N_ASSERT(mVertexArray->GetIndexBuffer() != nullptr);

        Bind();

        const auto indexCount = mVertexArray->GetIndexBuffer()->GetCount();
        GLCall(glDrawElements, GL_TRIANGLES, CAST<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
    }
}  // namespace Nth