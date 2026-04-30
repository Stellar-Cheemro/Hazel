#pragma once
// clang-format off
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Timestep.h>
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Events/Event.h>
#include <Hazel/Events/ApplicationEvent.h>
#include <Hazel/Events/MouseEvent.h>
// clang-format on
namespace Hazel
{
class HAZEL_API OrthographicCameraController
{
public:
    OrthographicCameraController(float aspectRatio, bool rotation = false);

    void OnUpdate(Timestep ts);
    void OnEvent(Event& e);

    OrthographicCamera& GetCamera()
    {
        return m_Camera;
    }

private:
    bool OnMouseScrolled(MouseScrolledEvent& e);
    bool OnWindowResized(WindowResizeEvent& e);

private:
    float m_AspectRatio;
    float m_ZoomLevel = 1.0f;
    bool m_Rotation;
    OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition{0.0f, 0.0f, 0.0f};
    float m_CameraRotation = 0.0f;
    float m_CameraMoveSpeed = 1.0f, m_CameraRotationSpeed = 180.0f;
};
} // namespace Hazel