// clang-format off
#include "Application.h"
#include "Hazel/Log.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
}
Application::~Application()
{
}

void Application::Run()
{
    while (m_Running)
    {
        WindowResizeEvent event(1280, 720);
        glClearColor(0.0f, 1.0f, 1.0f, 1);
        glClear(GL_COLOR_BUFFER_BIT);
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
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

    HAZEL_CORE_TRACE("{}", e.ToString());
}
} // namespace Hazel