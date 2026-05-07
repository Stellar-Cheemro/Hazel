#include <Hazel.h>

#include "GameLayer.h"

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

        PushLayer<GameLayer>();
    }

    ~Sandbox() = default;
};

Hazel::Application* Hazel::CreateApplication()
{
    return new Sandbox();
}

#include <Hazel/Core/EntryPoint.h>