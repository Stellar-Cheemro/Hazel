#include <Hazel.h>

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

        std::shared_ptr<Hazel::VertexBuffer> vertexBuffer(
            Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        // 创建 EBO 索引缓冲对象
        unsigned int indices[3] = {0, 1, 2};
        std::shared_ptr<Hazel::IndexBuffer> indexBuffer(
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
        std::shared_ptr<Hazel::VertexBuffer> squareVB(
            Hazel::VertexBuffer::Create(SQvertices, sizeof(SQvertices)));
        squareVB->SetLayout(layout);
        m_SquareVA->AddVertexBuffer(squareVB);
        // 创建 EBO 索引缓冲对象
        unsigned int squareIndices[6] = {0, 1, 2, 2, 3, 0};
        std::shared_ptr<Hazel::IndexBuffer> squareIB(Hazel::IndexBuffer::Create(
            squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
        m_SquareVA->SetIndexBuffer(squareIB);

        // 着色器源码
        std::string vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec3 a_Position;
        layout (location = 1) in vec4 a_Color;

        uniform mat4 u_ViewProjection;

        out vec4 v_Color;

        void main()
        {
            v_Color = a_Color;
            gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
        }
    )";

        std::string fragmentSrc = R"(
        #version 330 core
        layout(location = 0) out vec4 FragColor;

        in vec4 v_Color;
        void main()
        {
            FragColor = v_Color;
        }
    )";
        m_Shader = std::make_unique<Hazel::Shader>(vertexSrc, fragmentSrc);
    }

    void OnUpdate() override
    {
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_LEFT))
            m_CameraPosition.x -= m_CameraMoveSpeed;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_RIGHT))
            m_CameraPosition.x += m_CameraMoveSpeed;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_UP))
            m_CameraPosition.y += m_CameraMoveSpeed;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_DOWN))
            m_CameraPosition.y -= m_CameraMoveSpeed;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_Q))
            m_CameraRotation += m_CameraRotationSpeed;
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_E))
            m_CameraRotation -= m_CameraRotationSpeed;

        Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        Hazel::RenderCommand::Clear();

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        Hazel::Renderer::BeginScene(m_Camera);

        Hazel::Renderer::Submit(m_Shader, m_SquareVA);
        Hazel::Renderer::Submit(m_Shader, m_VertexArray);

        Hazel::Renderer::EndScene();
    }

    void OnEvent(Hazel::Event& event) override
    {
    }

    void OnImGuiRender() override
    {
    }

private:
    std::shared_ptr<Hazel::Shader> m_Shader;
    std::shared_ptr<Hazel::VertexArray> m_VertexArray;
    std::shared_ptr<Hazel::VertexArray> m_SquareVA;

    Hazel::OrthographicCamera m_Camera{-1.6f, 1.6f, -0.9f, 0.9f};
    glm::vec3 m_CameraPosition{0.0f, 0.0f, 0.0f};
    float m_CameraRotation = 0.0f;
    float m_CameraMoveSpeed = 0.05f;
    float m_CameraRotationSpeed = 1.0f;
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
