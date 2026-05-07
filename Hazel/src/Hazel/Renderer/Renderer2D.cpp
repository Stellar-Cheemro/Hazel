// clang-format off
#include "Renderer2D.h"
#include <Hazel/Core/Ref.h>
#include <Hazel/Core/Scope.h>
#include <Hazel/Renderer/VertexArray.h>
#include <Hazel/Renderer/Shader.h>
#include <Hazel/Asset/AssetManager.h>
#include <Hazel/Asset/EngineAssets.h>
#include <Hazel/Asset/Runtime/ShaderAsset.h>
#include <Hazel/Renderer/RenderCommand.h>
#include <glm/gtc/matrix_transform.hpp>
// clang-format on
namespace Hazel
{
namespace
{
struct Renderer2DStorage
{
    Ref<VertexArray> VertexArray;
    Ref<Texture2D> WhiteTexture;
    Ref<Shader> QuadShader;
};
} // namespace
static Scope<Renderer2DStorage> s_Data;

void Renderer2D::Init()
{
    if (!s_Data)
    {
        s_Data = CreateScope<Renderer2DStorage>();
    }
    s_Data->VertexArray = VertexArray::Create();

    float quadVertices[4 * 5] = {
        // position              // texcoord
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, // left_top
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // right_top
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // right_bottom
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f  // left_bottom
    };
    // 创建VA
    // 把顶点数据上传到 GPU
    Ref<VertexBuffer> QuadVB(VertexBuffer::Create(quadVertices, sizeof(quadVertices)));
    BufferLayout Quadlayout = {{ShaderDataType::Float3, "a_Position"},
                               {ShaderDataType::Float2, "a_TexCoord"}};
    QuadVB->SetLayout(Quadlayout);
    s_Data->VertexArray->AddVertexBuffer(QuadVB);
    // 创建 EBO 索引缓冲对象
    unsigned int QuadIndices[6] = {0, 1, 2, 2, 3, 0};
    Ref<IndexBuffer> QuadIB(
        IndexBuffer::Create(QuadIndices, sizeof(QuadIndices) / sizeof(unsigned int)));
    s_Data->VertexArray->SetIndexBuffer(QuadIB);

    s_Data->WhiteTexture = Texture2D::Create(1, 1);
    uint32_t whiteTextureData = 0xffffffff;
    s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

    s_Data->QuadShader = EngineAssets::GetShader(EngineShader::Quad);
    if (!s_Data->QuadShader)
    {
        HAZEL_CORE_ERROR("Fail:init renderer2d. Failed to get Quad shader.");
        return;
    }

    s_Data->QuadShader->Bind();
    s_Data->QuadShader->SetInt("u_Texture", 0);
}
void Renderer2D::Shutdown()
{
    s_Data.reset();
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
    if (!s_Data || !s_Data->QuadShader)
        return;
    s_Data->QuadShader->Bind();
    s_Data->QuadShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
}

void Renderer2D::EndScene()
{
}
void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    DrawQuad({position.x, position.y, 0.0f}, size, color);
}
void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
{
    if (!s_Data || !s_Data->VertexArray || !s_Data->QuadShader)
        return;

    s_Data->QuadShader->Bind();

    const glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
                            glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
    s_Data->QuadShader->SetMat4("u_Model", model);
    s_Data->QuadShader->SetFloat4("u_Color", color);

    s_Data->VertexArray->Bind();
    s_Data->WhiteTexture->Bind();
    RenderCommand::DrawIndexed(s_Data->VertexArray);
    s_Data->WhiteTexture->UnBind();
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size,
                          const Ref<Texture2D>& texture)
{
    DrawQuad({position.x, position.y, 0.0f}, size, texture);
}
void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size,
                          const Ref<Texture2D>& texture)
{
    if (!s_Data || !s_Data->VertexArray || !s_Data->QuadShader)
        return;

    s_Data->QuadShader->Bind();
    texture->Bind();

    const glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
                            glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    s_Data->QuadShader->SetMat4("u_Model", model);
    s_Data->QuadShader->SetFloat("u_TexScale", 10.0f);
    s_Data->QuadShader->SetFloat4("u_Color", glm::vec4(1.0f));
    s_Data->VertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data->VertexArray);
    texture->UnBind();
}
} // namespace Hazel