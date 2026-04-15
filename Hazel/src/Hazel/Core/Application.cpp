// clang-format off
#include "Application.h"

#include <Hazel/Core/Log.h>

#include <Hazel/Core/Core.h>
#include <Hazel/Core/LayerStack.h>
#include <Hazel/Core/Window.h>

#include <Hazel/Events/ApplicationEvent.h>

#include <Hazel/ImGui/ImGuiLayer.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{

Application* Application::s_Instance = nullptr;

Application::Application()
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

    // 创建 VAO 负责与着色器的顶点属性对接
    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    // 创建 VBO 数据本体
    glGenBuffers(1, &m_VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

    // clang-format off
    float vertices[3 * 3] = 
    {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };
    // clang-format on
    // 把顶点数据上传到 GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置顶点属性指针
    glEnableVertexAttribArray(0);

    // 参数说明：
    // 0：顶点属性位置（location = 0）
    // 3：每个顶点属性由 3 个 float 组成
    // GL_FALSE：不需要归一化
    // 3 * sizeof(float)：每个顶点属性占用的字节数（步长）
    // (const void*)0：顶点属性数据在缓冲中的偏移量，这里是从头开始
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

    // 创建 EBO 索引缓冲对象
    glGenBuffers(1, &m_IndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
    unsigned int indices[3] = {0, 1, 2};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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

void Application::Run()
{

    while (m_Running)
    {
        glClearColor(0.1f, 0.1f, 0.1f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(m_VertexArray);
        // 参数说明：
        // GL_TRIANGLES：绘制模式，表示每三个顶点构成一个三
        // 3：索引数量，这里是 3 个顶点
        // GL_UNSIGNED_INT：索引数据类型，这里是 unsigned int
        // nullptr：索引数据在 EBO 中的偏移量，这里是从头开始
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
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