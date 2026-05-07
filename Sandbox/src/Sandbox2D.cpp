// clang-format off
#include "Sandbox2D.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

// clang-format on
// ----------------------------------------------------------------------------
// 构造/析构函数
// ----------------------------------------------------------------------------
Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
}
// ----------------------------------------------------------------------------
// PUBLIC API
// ----------------------------------------------------------------------------
void Sandbox2D::OnAttach()
{
    Hazel::AssetHandle textureHandle =
        Hazel::UserAssetManager::ImportProjectAsset("textures/Checkerboard.png");
    m_CheckTex = Hazel::UserAssetManager::LoadTexture2D(textureHandle);
}

void Sandbox2D::OnDetach()
{
}
void Sandbox2D::OnUpdate(Hazel::Timestep timestep)
{
    // 更新
    m_CameraController.OnUpdate(timestep);
    // 渲染
    Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.0f});
    Hazel::RenderCommand::Clear();

    Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
    Hazel::Renderer2D::DrawQuad({0.0f, 0.0f, -0.1f}, {10.0f, 10.0f}, m_CheckTex);
    Hazel::Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f});
    Hazel::Renderer2D::DrawQuad({1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f});
    Hazel::Renderer2D::EndScene();
}
void Sandbox2D::OnEvent(Hazel::Event& event)
{
    m_CameraController.OnEvent(event);
}
void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}
