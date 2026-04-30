#pragma once

#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Renderer/RendererAPI.h>
namespace Hazel
{
class HAZEL_API RenderCommand
{
public:
    inline static void Init()
    {
        s_RendererAPI->Init();
    }
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

    static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        s_RendererAPI->SetViewport(x, y, width, height);
    }

private:
    static RendererAPI* s_RendererAPI;
};
} // namespace Hazel