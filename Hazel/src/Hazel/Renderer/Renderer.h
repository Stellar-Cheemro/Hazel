#pragma once

namespace Hazel
{
// clang-format off
enum class RendererAPI
{
    None,
    OpenGl,DirectX,Metal,Vulkan
};
// clang-format on
class Renderer
{
public:
    inline static RendererAPI GetAPI()
    {
        return s_RendererAPI;
    }

private:
    static RendererAPI s_RendererAPI;
};

} // namespace Hazel