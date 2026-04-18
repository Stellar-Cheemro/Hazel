#include <Hazel.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer()
        : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) // 根据窗口宽高比设置正交投影范围
    {                                                          // 顶点数据布局
        Hazel::BufferLayout layout = {{Hazel::ShaderDataType::Float3, "a_Position"},
                                      {Hazel::ShaderDataType::Float4, "a_Color"}};

        // 一个简单的三角形顶点数据，包含位置和颜色属性
        float vertices[3 * 7] = {
            // clang-format off
            -0.5f, -0.5f, 0.0f,  0.5f,0.5f, 0.0f,1.0f, // left
             0.5f, -0.5f, 0.0f,  0.0f,0.5f, 0.5f,1.0f,// right
             0.0f,  0.5f, 0.0f,  0.5f,0.0f, 0.5f,1.0f,// top
            // clang-format on
        };

        m_VertexArray.reset(Hazel::VertexArray::Create());
        // 把顶点数据上传到 GPU

        Hazel::Ref<Hazel::VertexBuffer> vertexBuffer(
            Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        // 创建 EBO 索引缓冲对象
        unsigned int indices[3] = {0, 1, 2};
        Hazel::Ref<Hazel::IndexBuffer> indexBuffer(
            Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
        m_VertexArray->SetIndexBuffer(indexBuffer);

        // 正方形绘制
        // clang-format off
    float SQvertices[4 * 7] = 
    {
         0.0f,  0.5f, 0.0f,  0.5f,0.5f, 0.0f,1.0f, // top
         0.5f,  0.0f, 0.0f,  0.0f,0.5f, 0.5f,1.0f,// right
         0.0f, -0.5f, 0.0f,  0.5f,0.0f, 0.5f,1.0f,// bottom
        -0.5f,  0.0f, 0.0f,  0.5f,0.5f, 0.5f,1.0f,// left
    };
        // clang-format on
        // 创建VA
        m_SquareVA.reset(Hazel::VertexArray::Create());
        // 把顶点数据上传到 GPU
        Hazel::Ref<Hazel::VertexBuffer> squareVB(
            Hazel::VertexBuffer::Create(SQvertices, sizeof(SQvertices)));
        squareVB->SetLayout(layout);
        m_SquareVA->AddVertexBuffer(squareVB);
        // 创建 EBO 索引缓冲对象
        unsigned int squareIndices[6] = {0, 1, 2, 2, 3, 0};
        Hazel::Ref<Hazel::IndexBuffer> squareIB(Hazel::IndexBuffer::Create(
            squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
        m_SquareVA->SetIndexBuffer(squareIB);

        // 着色器源码
        std::string vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec3 a_Position;

        uniform mat4 u_ViewProjection;
        uniform mat4 u_Model;

        void main()
        {
            gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
        }
    )";

        std::string fragmentSrc = R"(
        #version 330 core
        layout(location = 0) out vec4 FragColor;

        uniform vec4 v_Color;
        void main()
        {
            FragColor = v_Color;
        }
    )";
        m_Shader.reset(Hazel::Shader::Create(vertexSrc, fragmentSrc));
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

        Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Hazel::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Hazel::Renderer::BeginScene(m_Camera);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        for (int x = 0; x < 20; x++)
        {
            for (int y = 0; y < 20; y++)
            {
                glm::vec3 translate(x * 0.1f, y * 0.1f, 0.0f);
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translate) * scale;
                std::dynamic_pointer_cast<Hazel::OpenGLShader>(m_Shader)->UploadUniformFloat4(
                    "v_Color", m_SquareColor);
                Hazel::Renderer::Submit(m_Shader, m_SquareVA, model);
            }
        }
        Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
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
    Hazel::Ref<Hazel::VertexArray> m_VertexArray;
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;

    Hazel::OrthographicCamera m_Camera{-1.6f, 1.6f, -0.9f, 0.9f};
    glm::vec3 m_CameraPosition{0.0f, 0.0f, 0.0f};
    glm::vec4 m_SquareColor{0.2f, 0.3f, 0.8f, 1.0f};
    float m_CameraRotation = 0.0f;
    float m_CameraMoveSpeed = 1.0f;
    float m_CameraRotationSpeed = 10.0f;
};

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
    }
    ~Sandbox()
    {
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
