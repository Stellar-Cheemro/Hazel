#include "Buffer.h"

#include <Hazel/Core/Log.h>
#include <Hazel/Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGLBuffer.h>

#include <cstdint>
namespace Hazel
{
VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            HAZEL_CORE_ASSERT(false, "Renderer API not supported!");
            return nullptr;
        case RendererAPI::OpenGl:
            return new OpenGLVertexBuffer(vertices, size);
    }
    HAZEL_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}
IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            HAZEL_CORE_ASSERT(false, "Renderer API not supported!");
            return nullptr;
        case RendererAPI::OpenGl:
            return new OpenGLIndexBuffer(indices, count);
    }
    HAZEL_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}
} // namespace Hazel
