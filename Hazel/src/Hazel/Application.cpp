#include "Application.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Log.h"

namespace Hazel
{
Application::Application()
{
}
Application::~Application()
{
}

void Application::Run()
{
    WindowResizeEvent e(1280, 720);
    HAZEL_CLIENT_TRACE("{}", e.ToString());
    while (true)
    {
    }
}
} // namespace Hazel
