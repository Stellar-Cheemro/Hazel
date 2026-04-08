// clang-format off
#include "Application.h"
#include "Hazel/Log.h"
#include "Hazel/Events/ApplicationEvent.h"
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
    if (e.IsInCategory(EventCategoryApplication))
    {
        HAZEL_CLIENT_TRACE(e);
    }
    else
    {
        HAZEL_CLIENT_TRACE("It is not an application event");
    }

    while (true)
    {
    }
}
} // namespace Hazel
