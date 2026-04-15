#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Core/LayerStack.h>
#include <Hazel/Core/Window.h>

#include <Hazel/Events/ApplicationEvent.h>

#include <Hazel/ImGui/ImGuiLayer.h>
#include <memory>
namespace Hazel
{
class HAZEL_API Application
{
public:
    Application();

    virtual ~Application();

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    void OnEvent(Event& e);

    void Run();

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

private:
    std::unique_ptr<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    bool m_Running = true;
    LayerStack m_LayerStack;

    unsigned int m_VertexArray, m_VertexBuffer, m_IndexBuffer;

private:
    static Application* s_Instance;
};
Application* CreateApplication();
} // namespace Hazel