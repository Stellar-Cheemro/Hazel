// clang-format off
#include "Application.h"

#include <Hazel/Core/Log.h>
#include <Hazel/Core/Core.h>
#include <Hazel/Core/LayerStack.h>
#include <Hazel/Core/Window.h>
#include <Hazel/Core/Timestep.h>

#include <Hazel/Renderer/SceneRenderer.h>
#include <Hazel/Renderer/RenderCommand.h>

#include <Hazel/Events/ApplicationEvent.h>

#include <Hazel/ImGui/ImGuiLayer.h>

#include <Platform/OpenGL/OpenGLBuffer.h>

#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{

Application* Application::s_Instance = nullptr;
// ----------------------------------------------------------------------------
// 构造/析构函数
// ----------------------------------------------------------------------------
Application::Application()
{
    HAZEL_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    // 创建平台窗口对象
    // 这里返回的是 Window 抽象接口，实际类型通常是 WindowsWindow
    // 这样 Application 不需要直接依赖 GLFW，而是只依赖 Hazel 的窗口抽象层
    m_Window = Window::Create();

    // 将 Application::OnEvent 绑定为窗口事件回调
    // 后续 GLFW 原生回调会先进入 WindowsWindow
    // 再由 WindowsWindow 转成 Hazel 事件对象并转发到这里
    m_Window->SetEventCallback(HAZEL_BIND_EVENT_FN(Application::OnEvent));

    SceneRenderer::Init();

    // ImGuiLayer 由 LayerStack 统一管理生命周期
    m_ImGuiLayer = &PushOverlay<ImGuiLayer>();
}

Application::~Application()
{
    // 注意：
    // 当前 Layer 的生命周期由 LayerStack 统一管理
    // 因此这里不手动 delete m_ImGuiLayer
    // 如果这里再 delete，会与 LayerStack 析构重复释放
}
// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------
void Application::Run()
{

    while (m_Running)
    {
        float time = (float)glfwGetTime();          // 获取当前时间
        Timestep timestep = time - m_LastFrameTime; // 计算两帧之间的时间差
        m_LastFrameTime = time;                     // 更新上一帧的时间

        if (!m_Minimized)
        {
            // 先更新普通 Layer
            for (Scope<Layer>& layer : m_LayerStack)
            {
                layer->OnUpdate(timestep);
            }
        }

        // 开始一帧 ImGui
        m_ImGuiLayer->Begin();
        // 让每个 Layer 绘制自己的 ImGui 内容
        for (Scope<Layer>& layer : m_LayerStack)
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

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(HAZEL_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(HAZEL_BIND_EVENT_FN(Application::OnWindowResize));
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
// ----------------------------------------------------------------------------
// 内部实现接口
// ----------------------------------------------------------------------------
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
bool Application::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        m_Minimized = true;
        return false;
    }
    m_Minimized = false;
    SceneRenderer::OnWindowResize(e.GetWidth(), e.GetHeight());
    return false;
}
} // namespace Hazel