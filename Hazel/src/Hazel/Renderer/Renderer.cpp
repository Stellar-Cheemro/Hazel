#include "Renderer.h"
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Renderer/Renderer.h>

#include <Platform/OpenGL/OpenGLShader.h>
namespace Hazel
{
Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;
void Renderer::BeginScene(OrthographicCamera& camera)
{
    s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}
void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,
                      const glm::mat4& modelMatrix)
{
    shader->Bind();
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            HAZEL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            return;
        case RendererAPI::API::OpenGL:
            shader.As<OpenGLShader>()->UploadUniformMat4("u_ViewProjection",
                                                         s_SceneData->ViewProjectionMatrix);
            shader.As<OpenGLShader>()->UploadUniformMat4("u_Model", modelMatrix);
            break;
    }
    vertexArray->Bind();
    RenderCommand ::DrawIndexed(vertexArray);
}
void Renderer::EndScene()
{
}

} // namespace Hazel