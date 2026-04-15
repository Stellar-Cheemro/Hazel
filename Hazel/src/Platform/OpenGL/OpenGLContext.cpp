// clang-format off
#include "OpenGLContext.h"

#include <Hazel/Core/Log.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{
OpenGLContext::OpenGLContext(void* windowHandle)
    : m_WindowHandle(static_cast<GLFWwindow*>(windowHandle))
{
    HAZEL_CORE_ASSERT(windowHandle, "Window handle is null!");
}

void OpenGLContext::Init()
{
    // 让这个窗口的 OpenGL context 成为当前线程的激活上下文
    glfwMakeContextCurrent(m_WindowHandle);
    // 使用 GLAD 加载当前平台/驱动实际提供的 OpenGL 函数指针
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    HAZEL_CORE_ASSERT(status, "Failed to initialize Glad!");
}
void OpenGLContext::SwapBuffers()
{
    // glfwSwapBuffers():
    // 交换前台/后台缓冲，把当前帧渲染结果显示到屏幕上
    //
    // 如果开启了 VSync，那么交换缓冲通常会和显示器刷新同步
    glfwSwapBuffers(m_WindowHandle);
}
} // namespace Hazel