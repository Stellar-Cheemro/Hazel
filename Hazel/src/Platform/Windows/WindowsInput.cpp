// clang-format off
#include "WindowsInput.h"

#include <Hazel/Core/Application.h>
#include <Hazel/Core/KeyCodes.h>
#include <Hazel/Core/MouseCodes.h>

#include <GLFW/glfw3.h>

#include <utility>
// clang-format on
namespace Hazel
{
// 把目前 Input 的平台实例绑定到 WindowsInput。
Input* Input::s_Instance = new WindowsInput();

// 通过 Application 拿到当前主窗口
// 再取出平台原生窗口句柄
//
// 注意：
// 这里依赖的是 Hazel 当前“主窗口唯一”的运行模型
// 如果未来支持多窗口，这里可能要重新设计成：
// - 查询当前活跃窗口
// - 或由调用方显式指定窗口上下文
bool WindowsInput::IsKeyPressedImpl(int keycode)
{
    auto window =
        static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
    auto state = glfwGetKey(window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool WindowsInput::IsMouseButtonPressedImpl(int buttoncode)
{
    auto window =
        static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
    auto state = glfwGetMouseButton(window, buttoncode);
    return state == GLFW_PRESS;
}
std::pair<float, float> WindowsInput::GetMousePositionImpl()
{
    auto window =
        static_cast<GLFWwindow*>(Application::GetApplication().GetWindow().GetNativeWindow());
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return {static_cast<float>(xpos), static_cast<float>(ypos)};
}
float WindowsInput::GetMouseXImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return x;
}
float WindowsInput::GetMouseYImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return y;
}
} // namespace Hazel