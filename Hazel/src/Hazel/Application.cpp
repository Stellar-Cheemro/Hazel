// clang-format off
#include "Application.h"
#include "Hazel/Log.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
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

    HAZEL_CORE_TRACE("{}", e.ToString());

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
    // 这里保留一个简单的 GLM 输出测试
    // 用于验证 spdlog formatter 是否已经支持 glm::mat4
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 2.0f, 3.0f));
    HAZEL_CLIENT_INFO(transform);

    while (m_Running)
    {
        // 这里的 glClearColor / glClear 是当前阶段最基础的 OpenGL 调用
        // 主要用于验证：
        // 1. GLFW 成功创建了窗口
        // 2. OpenGL context 已成功建立
        // 3. GLAD 已成功加载 OpenGL 函数指针
        glClearColor(0.0f, 1.0f, 1.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);

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