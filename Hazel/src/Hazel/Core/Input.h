#pragma once
#include <Hazel/Core/Core.h>

#include <utility>

namespace Hazel
{
// Input 是一个“平台无关的输入查询接口”
//
// 当前设计不是把所有输入状态缓存到一个独立系统里
// 而是采用更简单的“平台层即时查询”轮询式输入方案
//
//   Input::IsKeyPressed(...)
//   -> 调用平台实现
//   -> 平台实现再向 GLFW 查询当前状态
class HAZEL_API Input
{
public:
    // 键盘
    inline static bool IsKeyPressed(int keycode)
    {
        return s_Instance->IsKeyPressedImpl(keycode);
    }
    // 鼠标
    inline static bool IsMouseButtonPressed(int buttoncode)
    {
        return s_Instance->IsMouseButtonPressedImpl(buttoncode);
    }
    inline static std::pair<float, float> GetMousePosition()
    {
        return s_Instance->GetMousePositionImpl();
    }
    inline static float GetMouseX()
    {
        return s_Instance->GetMouseXImpl();
    }
    inline static float GetMouseY()
    {
        return s_Instance->GetMouseYImpl();
    }

protected:
    // 由具体平台实现继承并实现这些接口
    virtual bool IsKeyPressedImpl(int keycode) = 0;
    virtual bool IsMouseButtonPressedImpl(int buttoncode) = 0;
    virtual std::pair<float, float> GetMousePositionImpl() = 0;
    virtual float GetMouseXImpl() = 0;
    virtual float GetMouseYImpl() = 0;

private:
    // 平台实现只保留一个入口对象，供静态 API 转发到底层
    static Input* s_Instance;
};
} // namespace Hazel