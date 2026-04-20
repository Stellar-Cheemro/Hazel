#pragma once

#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Renderer/RendererAPI.h>
namespace Hazel
{
class HAZEL_API RenderCommand
{
public:
    inline static void Clear()
    {
        s_RendererAPI->Clear();
    }
    inline static void SetClearColor(const glm::vec4& color)
    {
        s_RendererAPI->SetClearColor(color);
    }
    static void DrawIndexed(const Ref<VertexArray>& vertexArray)
    {
        s_RendererAPI->DrawIndexed(vertexArray);
    }

private:
    static RendererAPI* s_RendererAPI;
};
} // namespace Hazel