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
    static void Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,
                       const glm::mat4& modelMatrix = glm::mat4(1.0f));
    static void EndScene();
    static void Init();
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