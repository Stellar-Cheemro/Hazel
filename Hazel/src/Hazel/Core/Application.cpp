// clang-format off
#include "Application.h"

#include <Hazel/Core/Log.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/LayerStack.h>
#include <Hazel/Core/Window.h>

#include <Hazel/Renderer/Renderer.h>
#include <Hazel/Renderer/RenderCommand.h>

#include <Hazel/Events/ApplicationEvent.h>

#include <Hazel/ImGui/ImGuiLayer.h>

#include <Platform/OpenGL/OpenGLBuffer.h>

#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{

Application* Application::s_Instance = nullptr;

void Application::Run()
{

    while (m_Running)
    {
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        Renderer::BeginScene(m_Camera);

        Renderer::Submit(m_Shader, m_SquareVA);
        Renderer::Submit(m_Shader, m_VertexArray);

        Renderer::EndScene();

        // 先更新普通 Layer
        for (Layer* layer : m_LayerStack)
        {
            layer->OnUpdate();
        }

        // 开始一帧 ImGui
        m_ImGuiLayer->Begin();
        // 让每个 Layer 绘制自己的 ImGui 内容
        for (Layer* layer : m_LayerStack)
        {
            layer->OnImGuiRender();
        }
        // 结束 ImGui，并执行真正的 ImGui 渲染
        m_ImGuiLayer->End();

        // OnUpdate 连接主循环和平台窗口系统，目前负责：
        // 1. glfwPollEvents()：轮询系统消息
        // 2. glfwSwapBuffers()：交换前后缓冲
        m_Window->OnUpdate();
        m_Shader->Unbind();
    }
}

Application::Application() : m_Camera(-1.6f, 1.6f, -0.9f, 0.9f) // 根据窗口宽高比设置正交投影范围
{
    HAZEL_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    // 创建平台窗口对象
    // 这里返回的是 Window 抽象接口，实际类型通常是 WindowsWindow
    // 这样 Application 不需要直接依赖 GLFW，而是只依赖 Hazel 的窗口抽象层
    m_Window = std::unique_ptr<Window>(Window::Create());

    // 将 Application::OnEvent 绑定为窗口事件回调
    // 后续 GLFW 原生回调会先进入 WindowsWindow
    // 再由 WindowsWindow 转成 Hazel 事件对象并转发到这里
    m_Window->SetEventCallback(HAZEL_BIND_EVENT_FN(Application::OnEvent));

    // ImGuiLayer 由 LayerStack 统一管理生命周期
    // 这里不再使用 unique_ptr 或者shared_ptr 持有 ImGuiLayer避免双重释放
    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
    // 顶点数据布局
    BufferLayout layout = {{ShaderDataType::Float3, "a_Position"},
                           {ShaderDataType::Float4, "a_Color"}};
    // clang-format off
    // 一个简单的三角形顶点数据，包含位置和颜色属性
    float vertices[3 * 7] = 
    {
        -0.5f, -0.5f, 0.0f,  0.5f,0.5f, 0.0f,1.0f, // left
         0.5f, -0.5f, 0.0f,  0.0f,0.5f, 0.5f,1.0f,// right
         0.0f,  0.5f, 0.0f,  0.5f,0.0f, 0.5f,1.0f,// top
    };
    // clang-format on
    m_VertexArray.reset(VertexArray::Create());
    // 把顶点数据上传到 GPU

    std::shared_ptr<VertexBuffer> vertexBuffer(VertexBuffer::Create(vertices, sizeof(vertices)));
    vertexBuffer->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(vertexBuffer);
    // 创建 EBO 索引缓冲对象
    unsigned int indices[3] = {0, 1, 2};
    std::shared_ptr<IndexBuffer> indexBuffer(
        IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
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
    m_SquareVA.reset(VertexArray::Create());
    // 把顶点数据上传到 GPU
    std::shared_ptr<VertexBuffer> squareVB(VertexBuffer::Create(SQvertices, sizeof(SQvertices)));
    squareVB->SetLayout(layout);
    m_SquareVA->AddVertexBuffer(squareVB);
    // 创建 EBO 索引缓冲对象
    unsigned int squareIndices[6] = {0, 1, 2, 2, 3, 0};
    std::shared_ptr<IndexBuffer> squareIB(
        IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(unsigned int)));
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
    m_Shader = std::make_unique<Shader>(vertexSrc, fragmentSrc);
}
Application::~Application()
{
    // 注意：
    // 当前 Layer 的生命周期由 LayerStack 统一管理
    // 因此这里不手动 delete m_ImGuiLayer
    // 如果这里再 delete，会与 LayerStack 析构重复释放
}

void Application::PushLayer(Layer* layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}
void Application::PushOverlay(Layer* layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(HAZEL_BIND_EVENT_FN(Application::OnWindowClose));

    HAZEL_CORE_TRACE(e);

    // 事件从栈顶往下传播，上层 UI/Overlay 优先拦截
    // 一旦某层将事件标记为 Handled，则停止继续向下传播
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.IsHandled())
            break;
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    // 收到主窗口关闭事件后，不在这里立即销毁窗口对象
    // 而只是把主循环退出标记置为 false
    // 目的：
    // 1. 当前帧逻辑仍能完整跑完
    // 2. 退出流程统一交给 Application 析构阶段处理
    // 3. 不会在回调链中途打断对象生命周期
    m_Running = false;
    return true;
}

} // namespace Hazel