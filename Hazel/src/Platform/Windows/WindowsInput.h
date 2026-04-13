#pragma once

#include "Hazel/input.h"

namespace Hazel
{
class WindowsInput : public Input
{
protected:
    inline virtual bool IsKeyPressedImpl(int keycode) override;
    inline virtual bool IsMouseButtonPressedImpl(int buttoncode) override;
    inline virtual std::pair<float, float> GetMousePositionImpl() override;
    inline virtual float GetMouseXImpl() override;
    inline virtual float GetMouseYImpl() override;
};
} // namespace Hazel