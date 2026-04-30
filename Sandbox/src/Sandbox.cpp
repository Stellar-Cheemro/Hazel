#include <Hazel.h>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer()
        : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) // 根据窗口宽高比设置正交投影范围
    {
        // 正方形绘制
        // clang-format off
        float SQvertices[4 * 5] = 
        {
             -0.5f,  0.5f,  0.0f,  0.0f,1.0f, // top
             0.5f,   0.5f,  0.0f,  1.0f,1.0f, // right
             0.5f,  -0.5f,  0.0f,  1.0f,0.0f, // bottom
             -0.5f,  -0.5f,  0.0f,  0.0f,0.0f // left
        };
        // clang-format on
        // 创建VA
        m_SquareVA.reset(Hazel::VertexArray::Create());
        // 把顶点数据上传到 GPU
        Hazel::Ref<Hazel::VertexBuffer> squareVB(
            Hazel::VertexBuffer::Create(SQvertices, sizeof(SQvertices)));
        Hazel::BufferLayout SQlayout = {{Hazel::ShaderDataType::Float3, "a_Position"},
                                        {Hazel::ShaderDataType::Float2, "a_TexCoord"}};
        squareVB->SetLayout(SQlayout);
        m_SquareVA->AddVertexBuffer(squareVB);
        // 创建 EBO 索引缓冲对象
        unsigned int squareIndices[6] = {0, 1, 2, 2, 3, 0};
        Hazel::Ref<Hazel::IndexBuffer> squareIB(Hazel::IndexBuffer::Create(
            squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
        m_SquareVA->SetIndexBuffer(squareIB);

        m_FlatShaderHandle = Hazel::AssetManager::ImportAsset("Shaders/FlatColor.glsl");
        m_TextureShaderHandle = Hazel::AssetManager::ImportAsset("Shaders/Texture.glsl");

        auto shaderAsset = Hazel::AssetManager::GetAsset<Hazel::ShaderAsset>(m_FlatShaderHandle);
        auto textureShaderAsset =
            Hazel::AssetManager::GetAsset<Hazel::ShaderAsset>(m_TextureShaderHandle);

        if (shaderAsset)
            m_Shader = shaderAsset->GetShader();
        if (textureShaderAsset)
            m_TextureShader = textureShaderAsset->GetShader();

        m_CheckTexHandle = Hazel::AssetManager::ImportAsset("textures/Checkerboard.png");
        m_LogoTexHandle = Hazel::AssetManager::ImportAsset("textures/ChernoLogo.png");
    }

    void OnUpdate(Hazel::Timestep timestep) override
    {
        float time = timestep;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_LEFT))
            m_CameraPosition.x -= m_CameraMoveSpeed * time;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_RIGHT))
            m_CameraPosition.x += m_CameraMoveSpeed * time;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_UP))
            m_CameraPosition.y += m_CameraMoveSpeed * time;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_DOWN))
            m_CameraPosition.y -= m_CameraMoveSpeed * time;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_Q))
            m_CameraRotation += m_CameraRotationSpeed * time;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_E))
            m_CameraRotation -= m_CameraRotationSpeed * time;
        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Hazel::Ref<Hazel::TextureAsset> textureAsset =
            Hazel::AssetManager::GetAsset<Hazel::TextureAsset>(m_CheckTexHandle);
        if (textureAsset)
            m_CheckTex = textureAsset->GetTexture();
        m_TextureShader.As<Hazel::OpenGLShader>()->Bind();
        m_TextureShader.As<Hazel::OpenGLShader>()->UploadUniformInt("u_Texture", 0);

        textureAsset = Hazel::AssetManager::GetAsset<Hazel::TextureAsset>(m_LogoTexHandle);
        if (textureAsset)
            m_LogoTex = textureAsset->GetTexture();
        m_TextureShader.As<Hazel::OpenGLShader>()->UploadUniformInt("u_Texture", 0);

        m_Shader.As<Hazel::OpenGLShader>()->Bind();
        Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Hazel::RenderCommand::Clear();
        Hazel::SceneRenderer::BeginScene(m_Camera);
        m_Shader.As<Hazel::OpenGLShader>()->UploadUniformFloat4("u_Color", m_SquareColor);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        for (int x = 0; x < 20; x++)
        {
            for (int y = 0; y < 20; y++)
            {
                glm::vec3 translate(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translate) * scale;
                Hazel::SceneRenderer::Submit(m_Shader, m_SquareVA, model);
            }
        }

        glm::vec3 CSQtranslate(0.0f, 0.0f, 0.0f);
        glm::mat4 CSQscale = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f));
        glm::mat4 CSQmodel = glm::translate(glm::mat4(1.0f), CSQtranslate) * CSQscale;

        m_CheckTex->Bind();
        Hazel::SceneRenderer::Submit(m_TextureShader, m_SquareVA, CSQmodel);
        m_LogoTex->Bind();
        Hazel::SceneRenderer::Submit(m_TextureShader, m_SquareVA, CSQmodel);

        Hazel::SceneRenderer::EndScene();
    }

    void OnEvent(Hazel::Event& event) override
    {
    }

    void OnImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

private:
    Hazel::Ref<Hazel::Shader> m_Shader;
    Hazel::Ref<Hazel::Shader> m_TextureShader;
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;
    Hazel::Ref<Hazel::Texture2D> m_CheckTex;
    Hazel::Ref<Hazel::Texture2D> m_LogoTex;
    Hazel::OrthographicCamera m_Camera{-1.6f, 1.6f, -0.9f, 0.9f};
    glm::vec3 m_CameraPosition{0.0f, 0.0f, 0.0f};
    glm::vec4 m_SquareColor{0.2f, 0.3f, 0.8f, 1.0f};
    float m_CameraRotation = 0.0f;
    float m_CameraMoveSpeed = 1.0f;
    float m_CameraRotationSpeed = 10.0f;
    Hazel::AssetHandle m_CheckTexHandle = 0;
    Hazel::AssetHandle m_LogoTexHandle = 0;
    Hazel::AssetHandle m_FlatShaderHandle = 0;
    Hazel::AssetHandle m_TextureShaderHandle = 0;
};

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        Hazel::ProjectConfig config;
        config.Name = "Sandbox";
        config.ProjectDirectory = SANDBOX_PROJECT_DIR;
        config.AssetDirectory = "assets";
        Hazel::Project::SetActive(Hazel::Ref<Hazel::Project>::Create(config));
        Hazel::AssetManager::Init();
        PushLayer<ExampleLayer>();
    }
    ~Sandbox()
    {
        Hazel::AssetManager::Shutdown();
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
