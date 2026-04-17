#include "Renderer.h"
#include <Hazel/Renderer/OrthographicCamera.h>
namespace Hazel
{
Renderer::SceneData* Renderer::s_SceneData = new Renderer::SceneData;
void Renderer::BeginScene(OrthographicCamera& camera)
{
    s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}
void Renderer::Submit(const std::shared_ptr<Shader>& shader,
                      const std::shared_ptr<VertexArray>& vertexArray)
{
    shader->Bind();
    shader->UploadUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
    vertexArray->Bind();
    RenderCommand ::DrawIndexed(vertexArray);
}
void Renderer::EndScene()
{
}

} // namespace Hazel