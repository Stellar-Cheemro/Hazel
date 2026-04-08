// clang-format off
#include "Application.h"
#include "Hazel/Log.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/Event.h"
// clang-format on

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
    HAZEL_CLIENT_TRACE(e);
    while (true)
    {
    }
}
} // namespace Hazel
