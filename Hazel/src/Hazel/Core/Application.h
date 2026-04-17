#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Core/LayerStack.h>
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
    std::unique_ptr<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    bool m_Running = true;
    LayerStack m_LayerStack;

    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<VertexArray> m_SquareVA;

    OrthographicCamera m_Camera;

    // 这里的 Shader 和 VertexArray 只是为了测试 Renderer 提前准备的
    // 后续会放到 Layer 中去创建和管理

private:
    static Application* s_Instance;
};
Application* CreateApplication();
} // namespace Hazel