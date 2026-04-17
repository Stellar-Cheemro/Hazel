#pragma once

#include <Hazel/Renderer/VertexArray.h>

#include <glm/glm.hpp>

#include <memory>

namespace Hazel
{
class HAZEL_API RendererAPI
{
public:
    enum class API
    { // clang-format off
        None,
        OpenGl,DirectX,Metal,Vulkan
        // clang-format on
    };

public:
    virtual void Clear() = 0;
    virtual void SetClearColor(const glm::vec4& color) = 0;

    virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray) = 0;

    inline static API GetAPI()
    {
        return s_API;
    }

private:
    static API s_API;
};
} // namespace Hazel