// clang-format off
#include "Application.h"
#include "Hazel/Log.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
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
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(HAZEL_BIND_EVENT_FN(Application::OnEvent));
}
Application::~Application()
{
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

void Application::Run()
{
    while (m_Running)
    {
        glClearColor(0.0f, 1.0f, 1.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        for (Layer* layer : m_LayerStack)
        {
            layer->OnUpdate();
        }

        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(HAZEL_BIND_EVENT_FN(Application::OnWindowClose));

    HAZEL_CORE_TRACE("{}", e.ToString());
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.IsHandled())
            break;
    }
}
} // namespace Hazel