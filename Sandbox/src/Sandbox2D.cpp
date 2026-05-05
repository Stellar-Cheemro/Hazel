// clang-format off
#include "Sandbox2D.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

// clang-format on
// ----------------------------------------------------------------------------
// 构造/析构函数
// ----------------------------------------------------------------------------
Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}
// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------
void Sandbox2D::OnAttach()
{

    // 正方形绘制
    //     float SQvertices[4 * 5] =
    // {
    //      -0.5f,  0.5f,  0.0f,  0.0f,1.0f, // top
    //      0.5f,   0.5f,  0.0f,  1.0f,1.0f, // right
    //      0.5f,  -0.5f,  0.0f,  1.0f,0.0f, // bottom
    //      -0.5f,  -0.5f,  0.0f,  0.0f,0.0f // left
    // };
    // clang-format off
    float SQvertices[4 * 3] = 
    {
         -0.5f,  0.5f,  0.0f, // left_top
         0.5f,   0.5f,  0.0f, // right_top
         0.5f,  -0.5f,  0.0f, // right_bottom
         -0.5f,  -0.5f,  0.0f // left_bottom
    };
    // clang-format on
    // 创建VA
    m_SquareVA = Hazel::VertexArray::Create();
    // 把顶点数据上传到 GPU
    Hazel::Ref<Hazel::VertexBuffer> squareVB(
        Hazel::VertexBuffer::Create(SQvertices, sizeof(SQvertices)));
    Hazel::BufferLayout SQlayout = {{Hazel::ShaderDataType::Float3, "a_Position"}};
    squareVB->SetLayout(SQlayout);
    m_SquareVA->AddVertexBuffer(squareVB);
    // 创建 EBO 索引缓冲对象
    unsigned int squareIndices[6] = {0, 1, 2, 2, 3, 0};
    Hazel::Ref<Hazel::IndexBuffer> squareIB(
        Hazel::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
    m_SquareVA->SetIndexBuffer(squareIB);

    m_FlatShaderHandle = Hazel::UserAssetManager::ImportProjectAsset("Shaders/FlatColor.glsl");
    // m_TextureShaderHandle = Hazel::UserAssetManager::ImportProjectAsset("Shaders/Texture.glsl");

    auto shaderAsset = Hazel::UserAssetManager::GetAsset<Hazel::ShaderAsset>(m_FlatShaderHandle);
    // auto textureShaderAsset =
    //     Hazel::UserAssetManager::GetAsset<Hazel::ShaderAsset>(m_TextureShaderHandle);

    if (shaderAsset)
        m_Shader = shaderAsset->GetShader();
    // if (textureShaderAsset)
    //     m_TextureShader = textureShaderAsset->GetShader();

    // m_CheckTexHandle = Hazel::UserAssetManager::ImportProjectAsset("textures/Checkerboard.png");
    // m_LogoTexHandle = Hazel::UserAssetManager::ImportProjectAsset("textures/ChernoLogo.png");
}

void Sandbox2D::OnDetach()
{
}
void Sandbox2D::OnUpdate(Hazel::Timestep timestep)
{
    // 更新
    m_CameraController.OnUpdate(timestep);
    // 渲染
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
    Hazel::RenderCommand::Clear();

    Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
    Hazel::Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
    Hazel::Renderer2D::EndScene();
}
void Sandbox2D::OnEvent(Hazel::Event& event)
{
    m_CameraController.OnEvent(event);
}
void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}
