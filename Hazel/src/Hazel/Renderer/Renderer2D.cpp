// clang-format off
#include "Renderer2D.h"
#include <Hazel/Core/Ref.h>
#include <Hazel/Core/Scope.h>
#include <Hazel/Renderer/VertexArray.h>
#include <Hazel/Renderer/Shader.h>
#include <Hazel/Asset/AssetManager.h>
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
    Ref<Shader> FlatColorShader;

    AssetHandle FlatColorShaderHandle;
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
    // 正方形绘制
    //     float SQvertices[4 * 5] =
    // {
    //      -0.5f,  0.5f,  0.0f,  0.0f,1.0f, // top
    //      0.5f,   0.5f,  0.0f,  1.0f,1.0f, // right
    //      0.5f,  -0.5f,  0.0f,  1.0f,0.0f, // bottom
    //      -0.5f,  -0.5f,  0.0f,  0.0f,0.0f // left
    // };
    // clang-format off
    float quadVertices[4 * 3] = 
    {
         -0.5f,  0.5f,  0.0f, // left_top
         0.5f,   0.5f,  0.0f, // right_top
         0.5f,  -0.5f,  0.0f, // right_bottom
         -0.5f,  -0.5f,  0.0f // left_bottom
    };
    // clang-format on
    // 创建VA
    // 把顶点数据上传到 GPU
    Hazel::Ref<Hazel::VertexBuffer> QuadVB(
        Hazel::VertexBuffer::Create(quadVertices, sizeof(quadVertices)));
    Hazel::BufferLayout SQlayout = {{Hazel::ShaderDataType::Float3, "a_Position"}};
    QuadVB->SetLayout(SQlayout);
    s_Data->VertexArray->AddVertexBuffer(QuadVB);
    // 创建 EBO 索引缓冲对象
    unsigned int squareIndices[6] = {0, 1, 2, 2, 3, 0};
    Hazel::Ref<Hazel::IndexBuffer> squareIB(
        Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
    s_Data->VertexArray->SetIndexBuffer(squareIB);

    s_Data->FlatColorShaderHandle =
        Hazel::AssetManager::ImportEngineAsset("Shaders/FlatColor.glsl");
    // m_TextureShaderHandle = Hazel::AssetManager::ImportAsset("Shaders/Texture.glsl");
    if (s_Data->FlatColorShaderHandle == 0)
    {
        HAZEL_CORE_ERROR("Fail:init renderer2d. Failed to import FlatColor shader.");
        return;
    }

    Ref<ShaderAsset> ShaderAsset =
        Hazel::AssetManager::GetAsset<Hazel::ShaderAsset>(s_Data->FlatColorShaderHandle);
    if (ShaderAsset)
    {
        s_Data->FlatColorShader = ShaderAsset->GetShader();
    }
    else
    {
        HAZEL_CORE_ERROR("Fail:init renderer2d. Failed to load FlatColor shader. Handle: {0}",
                         s_Data->FlatColorShaderHandle);
        return;
    }
    // auto textureShaderAsset =
    //     Hazel::AssetManager::GetAsset<Hazel::ShaderAsset>(m_TextureShaderHandle);
    // if (textureShaderAsset)
    //     m_TextureShader = textureShaderAsset->GetShader();

    // m_CheckTexHandle = Hazel::AssetManager::ImportAsset("textures/Checkerboard.png");
    // m_LogoTexHandle = Hazel::AssetManager::ImportAsset("textures/ChernoLogo.png");
}
void Renderer2D::Shutdown()
{
}

void Renderer2D::BeginScene(const OrthographicCamera& camera)
{
    if (!s_Data || !s_Data->FlatColorShader)
        return;

    s_Data->FlatColorShader->Bind();
    s_Data->FlatColorShader->UploadUniformMat4("u_ViewProjection",
                                               camera.GetViewProjectionMatrix());
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
    if (!s_Data || !s_Data->VertexArray || !s_Data->FlatColorShader)
        return;

    s_Data->FlatColorShader->Bind();

    const glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
                            glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});

    s_Data->FlatColorShader->UploadUniformMat4("u_Model", model);
    s_Data->FlatColorShader->UploadUniformFloat4("u_Color", color);

    s_Data->VertexArray->Bind();
    RenderCommand::DrawIndexed(s_Data->VertexArray);
}
} // namespace Hazel