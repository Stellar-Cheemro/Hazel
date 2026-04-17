#include "VertexArray.h"

#include <Hazel/Core/Log.h>
#include <Hazel/Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGLVertexArray.h>
namespace Hazel
{
VertexArray* VertexArray::Create()
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "Renderer API not supported!");
            return nullptr;
        case RendererAPI::API::OpenGl:
            return new OpenGLVertexArray();
    }
    HAZEL_CORE_ASSERT(false, "Unknown Renderer API!");
    return nullptr;
}
}