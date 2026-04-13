#pragma once
#ifdef HAZEL_PLATFORM_WINDOWS
extern Hazel::Application* Hazel::CreateApplication();
int main(int argc, char** argv)
{
    Hazel::Log::Init();
    HAZEL_CORE_WARN("Initialized Core Log!");

    auto app = Hazel::CreateApplication();
    app->Run();
    delete app;
}
#endif