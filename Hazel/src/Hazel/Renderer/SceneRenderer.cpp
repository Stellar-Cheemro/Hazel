#include "SceneRenderer.h"
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Renderer/RenderCommand.h>
#include <Hazel/Renderer/SceneRenderer.h>
#include <Platform/OpenGL/OpenGLShader.h>
namespace Hazel
{
Scope<SceneRenderer::SceneData> SceneRenderer::s_SceneData =
    CreateScope<SceneRenderer::SceneData>();

void SceneRenderer::BeginScene(OrthographicCamera& camera)
{
    s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}
void SceneRenderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray,
                           const glm::mat4& modelMatrix)
{
    shader->Bind();
    switch (SceneRenderer::GetAPI())
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
void SceneRenderer::EndScene()
{
}

void SceneRenderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewport(0, 0, width, height);
}

void SceneRenderer::Init()
{
    RenderCommand::Init();
}

} // namespace Hazel