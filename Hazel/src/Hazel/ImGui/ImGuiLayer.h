#pragma once

#include <Hazel/Core/Core.h>
#include <Hazel/Core/Layer.h>

#include <Hazel/Events/ApplicationEvent.h>
#include <Hazel/Events/KeyEvent.h>
#include <Hazel/Events/MouseEvent.h>

namespace Hazel
{

class HAZEL_API ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    virtual ~ImGuiLayer() = default;

    void Begin();
    void End();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override;

private:
    float m_Time = 0.0f;
};

} // namespace Hazel