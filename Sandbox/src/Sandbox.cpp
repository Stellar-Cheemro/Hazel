// clang-format off
#include <Hazel.h>
#include "Sandbox2D.h"
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
// clang-format on

class Sandbox : public Hazel::Application
{
public:
    Sandbox()
    {
        Hazel::ProjectConfig config;
        config.Name = "Sandbox";
        config.ProjectDirectory = SANDBOX_PROJECT_DIR;
        config.AssetDirectory = "assets";
        Hazel::Project::SetActive(Hazel::CreateRef<Hazel::Project>(config));
        PushLayer<Sandbox2D>();
    }
    ~Sandbox()
    {
    }
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}
#include <Hazel/Core/EntryPoint.h>