#include "Hazel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

glm::mat4 camera(float Translate, glm::vec2 const& Rotate)
{
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, Translate));
    View = glm::rotate(View, Rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
    View = glm::rotate(View, Rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 Model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    glm::mat4 mvp = Projection * View * Model;
    return mvp;
}

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example")
    {
        auto cam = camera(5.0f, {0.5f, 0.5f});
        HAZEL_CLIENT_INFO(cam);
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
};

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        PushLayer(new ExampleLayer());
        PushOverlay(new Hazel::ImGuiLayer());
    }
    ~Sandbox()
    {
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
