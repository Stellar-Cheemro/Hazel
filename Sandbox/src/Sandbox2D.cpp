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

    m_FlatShaderHandle = Hazel::AssetManager::ImportAsset("Shaders/FlatColor.glsl");
    // m_TextureShaderHandle = Hazel::AssetManager::ImportAsset("Shaders/Texture.glsl");

    auto shaderAsset = Hazel::AssetManager::GetAsset<Hazel::ShaderAsset>(m_FlatShaderHandle);
    // auto textureShaderAsset =
    //     Hazel::AssetManager::GetAsset<Hazel::ShaderAsset>(m_TextureShaderHandle);

    if (shaderAsset)
        m_Shader = shaderAsset->GetShader();
    // if (textureShaderAsset)
    //     m_TextureShader = textureShaderAsset->GetShader();

    // m_CheckTexHandle = Hazel::AssetManager::ImportAsset("textures/Checkerboard.png");
    // m_LogoTexHandle = Hazel::AssetManager::ImportAsset("textures/ChernoLogo.png");
}

void Sandbox2D::OnDetach()
{
}
void Sandbox2D::OnUpdate(Hazel::Timestep timestep)
{
    // 更新
    m_CameraController.OnUpdate(timestep);
    // 渲染
    // 获取纹理资产并上传
    // Hazel::Ref<Hazel::TextureAsset> textureAsset =
    //     Hazel::AssetManager::GetAsset<Hazel::TextureAsset>(m_CheckTexHandle);
    // if (textureAsset)
    //     m_CheckTex = textureAsset->GetTexture();
    // m_TextureShader.As<Hazel::OpenGLShader>()->Bind();
    // m_TextureShader.As<Hazel::OpenGLShader>()->UploadUniformInt("u_Texture", 0);

    // textureAsset = Hazel::AssetManager::GetAsset<Hazel::TextureAsset>(m_LogoTexHandle);
    // if (textureAsset)
    //     m_LogoTex = textureAsset->GetTexture();
    // m_TextureShader.As<Hazel::OpenGLShader>()->UploadUniformInt("u_Texture", 0);

    m_Shader.As<Hazel::OpenGLShader>()->Bind();
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    Hazel::RenderCommand::Clear();
    Hazel::SceneRenderer::BeginScene(m_CameraController.GetCamera());
    m_Shader.As<Hazel::OpenGLShader>()->UploadUniformFloat4("u_Color", m_SquareColor);
    // // 正方形网格
    // glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    //
    // for (int x = 0; x < 20; x++)
    // {
    //     for (int y = 0; y < 20; y++)
    //     {
    //         glm::vec3 translate(x * 0.11f, y * 0.11f, 0.0f);
    //         glm::mat4 model = glm::translate(glm::mat4(1.0f), translate) * scale;
    //         Hazel::SceneRenderer::Submit(m_Shader, m_SquareVA, model);
    //     }
    // }
    glm::vec3 CSQtranslate(0.0f, 0.0f, 0.0f);
    glm::mat4 CSQscale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
    glm::mat4 CSQmodel = glm::translate(glm::mat4(1.0f), CSQtranslate) * CSQscale;
    Hazel::SceneRenderer::Submit(m_Shader, m_SquareVA, CSQmodel);
    // // 纹理绘制
    // m_CheckTex->Bind();
    // Hazel::SceneRenderer::Submit(m_TextureShader, m_SquareVA, CSQmodel);
    // m_LogoTex->Bind();
    // Hazel::SceneRenderer::Submit(m_TextureShader, m_SquareVA, CSQmodel);

    Hazel::SceneRenderer::EndScene();
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
