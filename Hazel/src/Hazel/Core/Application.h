#pragma once
// clang-format off
#include <Hazel/Core/Core.h>
#include <Hazel/Core/LayerStack.h>
#include <Hazel/Core/Timestep.h>
#include <Hazel/Core/Window.h>
#include <Hazel/Core/Scope.h>
#include <Hazel/Events/ApplicationEvent.h>

#include <Hazel/ImGui/ImGuiLayer.h>

#include <Hazel/Renderer/Buffer.h>
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Renderer/Shader.h>
#include <Hazel/Renderer/VertexArray.h>

#include <memory>
// clang-format on
namespace Hazel
{
class HAZEL_API Application
{
public:
    Application();

    virtual ~Application();

    template <LayerType T, typename... Args> T& PushLayer(Args&&... args)
    {
        return m_LayerStack.PushLayer<T>(std::forward<Args>(args)...);
    }
    template <LayerType T, typename... Args> T& PushOverlay(Args&&... args)
    {
        return m_LayerStack.PushOverlay<T>(std::forward<Args>(args)...);
    }

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

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

private:
    bool OnWindowClose(WindowCloseEvent& e);

private:
    LayerStack m_LayerStack;
    Scope<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    Timestep m_Timestep;
    float m_LastFrameTime = 0.0f;
    bool m_Running = true;

private:
    static Application* s_Instance;
};
Application* CreateApplication();
} // namespace Hazel