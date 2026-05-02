#include "Texture.h"
#include <Hazel/Core/Log.h>
#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/SceneRenderer.h>
#include <Platform/OpenGL/OpenGLTexture.h>
namespace Hazel
{
Ref<Texture2D> Texture2D::Create(const std::string& path)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return Ref<Texture2D>(new OpenGLTexture2D(path.c_str()));
    }

    HAZEL_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
} // namespace Hazel