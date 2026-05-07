#include "Player.h"

#include "GameConfig.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace
{
void DrawDebugRect(const glm::vec2& center, const glm::vec2& size, const glm::vec4& color,
                   float z = 0.5f)
{
    const float thickness = 0.04f;

    const float halfWidth = size.x * 0.5f;
    const float halfHeight = size.y * 0.5f;

    Hazel::Renderer2D::DrawQuad({center.x, center.y + halfHeight, z}, {size.x, thickness}, color);
    Hazel::Renderer2D::DrawQuad({center.x, center.y - halfHeight, z}, {size.x, thickness}, color);
    Hazel::Renderer2D::DrawQuad({center.x - halfWidth, center.y, z}, {thickness, size.y}, color);
    Hazel::Renderer2D::DrawQuad({center.x + halfWidth, center.y, z}, {thickness, size.y}, color);
}
} // namespace

// ----------------------------------------------------------------------------
// 生命周期
// ----------------------------------------------------------------------------

Player::Player()
{
    Reset();
}

void Player::LoadAssets()
{
    Hazel::AssetHandle playerTextureHandle =
        Hazel::UserAssetManager::ImportProjectAsset("Textures/ChernoLogo.png");

    m_Texture = Hazel::UserAssetManager::LoadTexture2D(playerTextureHandle);
}

// ----------------------------------------------------------------------------
// 状态
// ----------------------------------------------------------------------------

void Player::Reset()
{
    m_Position = GameConfig::Player::InitialPosition;
    m_Velocity = {0.0f, 0.0f};

    m_Size = GameConfig::Player::Size;
    m_CollisionSize = GameConfig::Player::CollisionSize;
    m_CollisionOffset = GameConfig::Player::CollisionOffset;

    m_Rotation = 0.0f;
}

void Player::Jump()
{
    m_Velocity.y = GameConfig::Player::JumpForce;
}

// ----------------------------------------------------------------------------
// 更新
// ----------------------------------------------------------------------------

void Player::OnUpdate(Hazel::Timestep timestep)
{
    const float dt = timestep;

    m_Velocity.y += GameConfig::Player::Gravity * dt;
    m_Position += m_Velocity * dt;

    m_Rotation = glm::clamp(m_Velocity.y * GameConfig::Player::RotationVelocityScale,
                            GameConfig::Player::MinRotation, GameConfig::Player::MaxRotation);

    if (m_Position.y < GameConfig::Player::MinY)
    {
        m_Position.y = GameConfig::Player::MinY;
        m_Velocity.y = 0.0f;
        m_Rotation = 0.0f;
    }

    if (m_Position.y > GameConfig::Player::MaxY)
    {
        m_Position.y = GameConfig::Player::MaxY;
        m_Velocity.y = 0.0f;
    }
}

// ----------------------------------------------------------------------------
// 渲染
// ----------------------------------------------------------------------------

void Player::OnRender()
{
    Hazel::QuadDrawParams playerQuad;
    playerQuad.Position = {m_Position.x, m_Position.y, 0.1f};
    playerQuad.Size = m_Size;
    playerQuad.Rotation = m_Rotation;
    playerQuad.Color = {1.0f, 1.0f, 1.0f, 1.0f};
    playerQuad.Texture = m_Texture;
    playerQuad.TextureScale = 1.0f;

    Hazel::Renderer2D::DrawQuad(playerQuad);
}

void Player::OnDebugRender() const
{
    DrawDebugRect(GetCollisionPosition(), GetCollisionSize(), {1.0f, 0.9f, 0.1f, 1.0f}, 0.6f);
}