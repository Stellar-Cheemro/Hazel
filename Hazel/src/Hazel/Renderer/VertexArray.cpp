#include "VertexArray.h"

#include <Hazel/Core/Log.h>
#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/SceneRenderer.h>
#include <Platform/OpenGL/OpenGLVertexArray.h>
namespace Hazel
{
Ref<VertexArray> VertexArray::Create()
{
    switch (SceneRenderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "SceneRenderer API not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<VertexArray>(new OpenGLVertexArray());
    }
    HAZEL_CORE_ASSERT(false, "Unknown SceneRenderer API!");
    return nullptr;
}
} // namespace Hazel