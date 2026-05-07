#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <Hazel/Renderer/OrthographicCamera.h>
#include <Hazel/Renderer/Texture.h>
namespace Hazel
{
struct HAZEL_API QuadDrawParams
{
    glm::vec3 Position = {0.0f, 0.0f, 0.0f};
    glm::vec2 Size = {1.0f, 1.0f};

    // 弧度制，不是角度。调用方可以使用 glm::radians(45.0f)
    float Rotation = 0.0f;

    glm::vec4 Color = {1.0f, 1.0f, 1.0f, 1.0f};

    // 为空时自动使用白纹理，因此 Color Quad 和 Texture Quad 可以走同一套渲染路径
    Ref<Texture2D> Texture = nullptr;

    float TextureScale = 1.0f;
};
class HAZEL_API Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();
    // clang-format off
    static void DrawQuad(const QuadDrawParams& params);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
    static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::f32& texScale = 1.0f);
    static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::f32& texScale = 1.0f);
    // clang-format on
};

} // namespace Hazel