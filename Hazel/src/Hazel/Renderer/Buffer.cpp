#include "Buffer.h"

#include <Hazel/Core/Log.h>
#include <Hazel/Renderer/SceneRenderer.h>
#include <Platform/OpenGL/OpenGLBuffer.h>

#include <cstdint>
namespace Hazel
{
VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
{
    switch (SceneRenderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "SceneRenderer API not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLVertexBuffer(vertices, size);
    }
    HAZEL_CORE_ASSERT(false, "Unknown SceneRenderer API!");
    return nullptr;
}
IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
    switch (SceneRenderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "SceneRenderer API not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLIndexBuffer(indices, count);
    }
    HAZEL_CORE_ASSERT(false, "Unknown SceneRenderer API!");
    return nullptr;
}
} // namespace Hazel
