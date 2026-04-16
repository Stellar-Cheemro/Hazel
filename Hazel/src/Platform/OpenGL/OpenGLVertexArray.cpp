#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace Hazel
{
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) noexcept
{
    switch (type)
    {       // clang-format off
        case ShaderDataType::Float:   return GL_FLOAT;
        case ShaderDataType::Float2:  return GL_FLOAT;
        case ShaderDataType::Float3:  return GL_FLOAT;
        case ShaderDataType::Float4:  return GL_FLOAT;
        case ShaderDataType::Mat3:    return GL_FLOAT;
        case ShaderDataType::Mat4:    return GL_FLOAT;

        case ShaderDataType::Int:     return GL_INT;
        case ShaderDataType::Int2:    return GL_INT;
        case ShaderDataType::Int3:    return GL_INT;
        case ShaderDataType::Int4:    return GL_INT;

        case ShaderDataType::Bool:    return GL_BOOL;
            // clang-format on
    }
    HAZEL_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_RendererID);
}
OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}
void OpenGLVertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}
void OpenGLVertexArray::Unbind() const
{
    glBindVertexArray(0);
}
void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    HAZEL_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(),
                      "Vertex Buffer has no layout!");

    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    uint32_t index = 0;
    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout)
    {
        // 设置顶点属性指针
        glEnableVertexAttribArray(index);
        // 参数说明：
        // 0：顶点属性位置（location = 0）
        // 3：每个顶点属性由 3 个 float 组成
        // GL_FALSE：不需要归一化
        // 3 * sizeof(float)：每个顶点属性占用的字节数（步长）
        // (const void*)0：顶点属性数据在缓冲中的偏移量，这里是从头开始
        glVertexAttribPointer(index, element.GetComponentCount(),
                              ShaderDataTypeToOpenGLBaseType(element.GetType()),
                              element.GetNormalized() ? GL_TRUE : GL_FALSE, layout.GetStride(),
                              (const void*)element.Offset);
        index++;
    }
    m_VertexBuffers.push_back(vertexBuffer);
}
void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
}
} // namespace Hazel