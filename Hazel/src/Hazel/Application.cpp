// clang-format off
#include "Application.h"
#include "Hazel/Log.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Events/ApplicationEvent.h"
#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{
Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::Create());
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
} // namespace Hazel
