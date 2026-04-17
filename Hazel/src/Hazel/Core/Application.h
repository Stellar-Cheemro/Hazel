#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Core/LayerStack.h>
#include <Hazel/Core/Timestep.h>
#include <Hazel/Core/Window.h>

#include <Hazel/Events/ApplicationEvent.h>

#include <Hazel/ImGui/ImGuiLayer.h>

#include <Hazel/Renderer/Buffer.h>
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Renderer/Shader.h>
#include <Hazel/Renderer/VertexArray.h>

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
    LayerStack m_LayerStack;
    std::unique_ptr<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    Timestep m_Timestep;
    float m_LastFrameTime = 0.0f;
    bool m_Running = true;

private:
    static Application* s_Instance;
};
Application* CreateApplication();
} // namespace Hazel