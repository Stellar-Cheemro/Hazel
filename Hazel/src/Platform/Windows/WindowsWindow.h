#pragma once

#include "Hazel/Window.h"

struct GLFWwindow;

namespace Hazel
{
class WindowsWindow : public Window
{
public:
    WindowsWindow(const WindowProps& props);
    virtual ~WindowsWindow();
    void OnUpdate() override;

    // 窗口属性
    inline void SetEventCallback(const EventCallbackFn& callback) override
    {
        m_Data.EventCallback = callback;
    }

    void SetVSync(bool enabled) override;
    bool IsVSync() const override;

    inline unsigned int GetWidth() const override
    {
        return m_Data.Width;
    }
    inline unsigned int GetHeight() const override
    {
        return m_Data.Height;
    }

    inline void* GetNativeWindow() const override
    {
        return m_Window;
    }

private:
    virtual void Init(const WindowProps& props);
    virtual void Shutdown();
    void RegisterGLFWCallbacks();

private:
    GLFWwindow* m_Window;
    struct WindowData
    {
        EventCallbackFn EventCallback;
        std::string Title;
        unsigned int Width, Height;
        bool VSync;
    };
    WindowData m_Data;
};
} // namespace Hazel