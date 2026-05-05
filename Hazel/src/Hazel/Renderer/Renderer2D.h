#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Renderer/OrthographicCamera.h>
namespace Hazel
{
class HAZEL_API Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();

    // clang-format off
    static void DrawQuad(
        const glm::vec2& position, 
        const glm::vec2& size, 
        const glm::vec4& color);
    static void DrawQuad(
        const glm::vec3& position, 
        const glm::vec2& size, 
        const glm::vec4& color);
    // clang-format on
};
} // namespace Hazel