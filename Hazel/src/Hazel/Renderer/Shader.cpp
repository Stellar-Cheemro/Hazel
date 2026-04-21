// clang-format off
#include "Shader.h"

#include <string>

#include <Hazel/Renderer/Renderer.h>
#include <Platform/OpenGL/OpenGLShader.h>
// clang-format on
namespace Hazel
{
Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLShader(vertexSrc, fragmentSrc);
    }
    HAZEL_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
Shader* Shader::Create(const std::string& filepath)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLShader(filepath);
    }
    HAZEL_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
} // namespace Hazel