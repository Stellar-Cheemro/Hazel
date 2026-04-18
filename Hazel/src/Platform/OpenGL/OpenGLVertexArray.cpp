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
void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
    HAZEL_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(),
                      "Vertex Buffer has no layout!");

    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    uint32_t index = 0;
    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout)
    {
        glEnableVertexAttribArray(index);
        // 参数说明：
        // index：顶点属性位置（layout(location = x)中的x）
        // size：每个顶点属性的组件数量（例如，vec3是3，mat4是16）
        // type：数据类型（例如，GL_FLOAT）
        // normalized：是否将整数数据归一化为[0, 1]或[-1, 1]
        // stride：每个顶点的字节偏移量（即顶点属性之间的间隔）
        // pointer：顶点属性在缓冲区中的偏移量（即属性在顶点结构中的位置）
        glVertexAttribPointer(index, element.GetComponentCount(),
                              ShaderDataTypeToOpenGLBaseType(element.GetType()),
                              element.GetNormalized() ? GL_TRUE : GL_FALSE, layout.GetStride(),
                              (const void*)element.Offset);
        index++;
    }
    m_VertexBuffers.push_back(vertexBuffer);
}
void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
}
} // namespace Hazel