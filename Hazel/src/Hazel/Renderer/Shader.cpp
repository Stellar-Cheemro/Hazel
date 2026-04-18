#include "Shader.h"

#include <Hazel/Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGLShader.h>

#include <string>
namespace Hazel
{
Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGl:
            return new OpenGLShader(vertexSrc, fragmentSrc);
    }
    HAZEL_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
} // namespace Hazel