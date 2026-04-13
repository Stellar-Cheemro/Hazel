#pragma once
#include "Core.h"
#include "Events/ApplicationEvent.h"
#include "LayerStack.h"
#include "Window.h"
#include "input.h"
#include <memory>
namespace Hazel
{
class HAZEL_API Application
{
public:
    Application();

    virtual ~Application();
    void Run();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    inline static Application& GetApplication()
    {
        return *s_Instance;
    }

    inline Window& GetWindow()
    {
        return *m_Window;
    }

private:
    bool OnWindowClose(WindowCloseEvent& e);
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
    LayerStack m_LayerStack;

private:
    static Application* s_Instance;
};
Application* CreateApplication();
} // namespace Hazel