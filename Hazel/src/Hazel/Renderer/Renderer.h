#pragma once

#include <Hazel/Core/Core.h>

#include <Hazel/Renderer/RenderCommand.h>
#include <Hazel/Renderer/RendererAPI.h>
#include <Hazel/Renderer/Shader.h>

namespace Hazel
{
// clang-format off

// clang-format on
class HAZEL_API Renderer
{
public:
    static void BeginScene(OrthographicCamera& camera);
    static void Submit(const std::shared_ptr<Shader>& shader,
                       const std::shared_ptr<VertexArray>& vertexArray);
    static void EndScene();

    inline static RendererAPI::API GetAPI()
    {
        return RendererAPI::GetAPI();
    }

private:
    struct SceneData
    {
        glm::mat4 ViewProjectionMatrix;
    };
    static SceneData* s_SceneData;
};

} // namespace Hazel