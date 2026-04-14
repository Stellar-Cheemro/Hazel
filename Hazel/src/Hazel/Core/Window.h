#pragma once
#include <Hazel/Core/Core.h>

#include <Hazel/Events/Event.h>

#include <functional>
#include <string>
namespace Hazel
{

struct WindowProps
{
    std::string Title;
    unsigned int Width;
    unsigned int Height;

    WindowProps(const std::string& title = "Hazel Engine", unsigned int width = 1280,
                unsigned int height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};

// Windows系统窗口接口
class HAZEL_API Window
{
public:
    using EventCallbackFn = std::function<void(Event&)>;

    virtual ~Window()
    {
    }
    virtual void OnUpdate() = 0;
    virtual unsigned int GetWidth() const = 0;
    virtual unsigned int GetHeight() const = 0;

    // 窗口属性
    virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
    virtual void SetVSync(bool enabled) = 0;
    virtual bool IsVSync() const = 0;

    inline virtual void* GetNativeWindow() const = 0;

    static Window* Create(const WindowProps& porps = WindowProps());
};
} // namespace Hazel