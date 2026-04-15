#include <Hazel.h>

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example")
    {
    }

    void OnUpdate() override
    {
        if (Hazel::Input::IsKeyPressed(HAZEL_KEY_TAB))
        {
            HAZEL_CLIENT_TRACE("Tab key is pressed!");
        }
    }

    void OnEvent(Hazel::Event& event) override
    {
        if (event.GetEventType() == Hazel::EventType::KeyPressed)
        {
            Hazel::KeyPressedEvent& e = static_cast<Hazel::KeyPressedEvent&>(event);
            HAZEL_CLIENT_TRACE("{0}", static_cast<char>(e.GetKeyCode()));
        }
    }

    void OnImGuiRender() override
    {
    }
};

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
    }
    ~Sandbox()
    {
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
