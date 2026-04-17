#pragma once

#include <Hazel/Renderer/RenderCommand.h>
#include <Hazel/Renderer/RendererAPI.h>
#include <Hazel/Renderer/Shader.h>

namespace Hazel
{
// clang-format off

// clang-format on
class Renderer
{
public:
    inline static void BeginScene();
    static void Submit(const std::shared_ptr<VertexArray>& vertexArray);
    inline static void EndScene();

    inline static RendererAPI::API GetAPI()
    {
        return RendererAPI::GetAPI();
    }

private:
};

} // namespace Hazel