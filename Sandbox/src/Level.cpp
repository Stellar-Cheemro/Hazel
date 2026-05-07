#include "Level.h"

#include "GameConfig.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace
{
float RandomFloat(float min, float max)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(rng);
}

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

Level::Level()
{
    Reset();
}

void Level::LoadAssets()
{
    Hazel::AssetHandle checkerboardHandle =
        Hazel::UserAssetManager::ImportProjectAsset("Textures/Checkerboard.png");

    m_BackgroundTexture = Hazel::UserAssetManager::LoadTexture2D(checkerboardHandle);
    m_GroundTexture = Hazel::UserAssetManager::LoadTexture2D(checkerboardHandle);
    m_ObstacleTexture = Hazel::UserAssetManager::LoadTexture2D(checkerboardHandle);
}

void Level::Reset()
{
    m_Score = 0;
    m_BackgroundOffset = 0.0f;
    m_GroundOffset = 0.0f;

    for (size_t i = 0; i < m_Obstacles.size(); i++)
    {
        const float x = GameConfig::Level::FirstObstacleX +
                        static_cast<float>(i) * GameConfig::Level::ObstacleSpacing;

        ResetObstacle(m_Obstacles[i], x);
    }
}

// ----------------------------------------------------------------------------
// 更新
// ----------------------------------------------------------------------------

void Level::OnUpdate(Hazel::Timestep timestep, const Player& player)
{
    const float dt = timestep;
    const float playerX = player.GetPosition().x;
    const float currentScrollSpeed = GetCurrentScrollSpeed();

    m_BackgroundOffset -= GameConfig::Level::BackgroundScrollSpeed * dt;
    if (m_BackgroundOffset <= -GameConfig::Level::BackgroundTileWidth)
        m_BackgroundOffset += GameConfig::Level::BackgroundTileWidth;

    m_GroundOffset -= currentScrollSpeed * dt;
    if (m_GroundOffset <= -GameConfig::Level::GroundTileWidth)
        m_GroundOffset += GameConfig::Level::GroundTileWidth;

    for (auto& obstacle : m_Obstacles)
    {
        obstacle.X -= currentScrollSpeed * dt;

        if (!obstacle.Passed && obstacle.X < playerX)
        {
            obstacle.Passed = true;
            m_Score++;
        }

        if (obstacle.X < GameConfig::Level::DespawnX)
        {
            const float newX = GetNextObstacleSpawnX();
            ResetObstacle(obstacle, newX);
        }
    }
}

// ----------------------------------------------------------------------------
// 渲染
// ----------------------------------------------------------------------------

void Level::OnRender()
{
    RenderBackground();
    RenderObstacles();
    RenderGround();
}

void Level::OnDebugRender() const
{
    RenderObstacleDebugBounds();
    RenderGroundDebugBounds();
}

void Level::RenderBackground()
{
    for (int i = -1; i <= 2; i++)
    {
        Hazel::QuadDrawParams backgroundQuad;
        backgroundQuad.Position = {m_BackgroundOffset + static_cast<float>(i) *
                                                            GameConfig::Level::BackgroundTileWidth,
                                   0.0f, -0.2f};
        backgroundQuad.Size = {GameConfig::Level::BackgroundTileWidth, 30.0f};
        backgroundQuad.Color = {0.10f, 0.12f, 0.18f, 1.0f};
        backgroundQuad.Texture = m_BackgroundTexture;
        backgroundQuad.TextureScale = 6.0f;

        Hazel::Renderer2D::DrawQuad(backgroundQuad);
    }
}

void Level::RenderObstacles()
{
    const glm::vec2 obstacleSize = GetObstacleSize();

    for (const auto& obstacle : m_Obstacles)
    {
        const glm::vec2 upperPosition = GetUpperObstaclePosition(obstacle);
        const glm::vec2 lowerPosition = GetLowerObstaclePosition(obstacle);

        Hazel::QuadDrawParams upperObstacleQuad;
        upperObstacleQuad.Position = {upperPosition.x, upperPosition.y, 0.0f};
        upperObstacleQuad.Size = obstacleSize;
        upperObstacleQuad.Color = {0.20f, 0.75f, 0.35f, 1.0f};
        upperObstacleQuad.Texture = m_ObstacleTexture;
        upperObstacleQuad.TextureScale = 2.0f;

        Hazel::Renderer2D::DrawQuad(upperObstacleQuad);

        Hazel::QuadDrawParams lowerObstacleQuad;
        lowerObstacleQuad.Position = {lowerPosition.x, lowerPosition.y, 0.0f};
        lowerObstacleQuad.Size = obstacleSize;
        lowerObstacleQuad.Color = {0.20f, 0.75f, 0.35f, 1.0f};
        lowerObstacleQuad.Texture = m_ObstacleTexture;
        lowerObstacleQuad.TextureScale = 2.0f;

        Hazel::Renderer2D::DrawQuad(lowerObstacleQuad);
    }
}

void Level::RenderGround()
{
    for (int i = -2; i <= 3; i++)
    {
        Hazel::QuadDrawParams groundQuad;
        groundQuad.Position = {m_GroundOffset +
                                   static_cast<float>(i) * GameConfig::Level::GroundTileWidth,
                               GameConfig::Level::GroundY, 0.1f};
        groundQuad.Size = {GameConfig::Level::GroundTileWidth, GameConfig::Level::GroundHeight};
        groundQuad.Color = {0.25f, 0.55f, 0.28f, 1.0f};
        groundQuad.Texture = m_GroundTexture;
        groundQuad.TextureScale = 3.0f;

        Hazel::Renderer2D::DrawQuad(groundQuad);
    }
}

void Level::RenderObstacleDebugBounds() const
{
    const glm::vec2 obstacleSize = GetObstacleSize();

    for (const auto& obstacle : m_Obstacles)
    {
        DrawDebugRect(GetUpperObstaclePosition(obstacle), obstacleSize, {0.1f, 1.0f, 0.2f, 1.0f},
                      0.55f);

        DrawDebugRect(GetLowerObstaclePosition(obstacle), obstacleSize, {0.1f, 1.0f, 0.2f, 1.0f},
                      0.55f);
    }
}

void Level::RenderGroundDebugBounds() const
{
    DrawDebugRect({0.0f, GameConfig::Level::GroundY}, {50.0f, GameConfig::Level::GroundHeight},
                  {1.0f, 0.1f, 0.1f, 1.0f}, 0.55f);
}

// ----------------------------------------------------------------------------
// 碰撞
// ----------------------------------------------------------------------------

bool Level::CheckCollision(const Player& player) const
{
    const glm::vec2 playerPosition = player.GetCollisionPosition();
    const glm::vec2 playerSize = player.GetCollisionSize();

    const float playerBottom = playerPosition.y - playerSize.y * 0.5f;
    const float groundTop = GameConfig::Level::GroundY + GameConfig::Level::GroundHeight * 0.5f;

    if (playerBottom <= groundTop)
        return true;

    const glm::vec2 obstacleSize = GetObstacleSize();

    for (const auto& obstacle : m_Obstacles)
    {
        if (CheckAABB(playerPosition, playerSize, GetUpperObstaclePosition(obstacle), obstacleSize))
            return true;

        if (CheckAABB(playerPosition, playerSize, GetLowerObstaclePosition(obstacle), obstacleSize))
            return true;
    }

    return false;
}

bool Level::CheckAABB(const glm::vec2& aPosition, const glm::vec2& aSize,
                      const glm::vec2& bPosition, const glm::vec2& bSize) const
{
    const float aLeft = aPosition.x - aSize.x * 0.5f;
    const float aRight = aPosition.x + aSize.x * 0.5f;
    const float aBottom = aPosition.y - aSize.y * 0.5f;
    const float aTop = aPosition.y + aSize.y * 0.5f;

    const float bLeft = bPosition.x - bSize.x * 0.5f;
    const float bRight = bPosition.x + bSize.x * 0.5f;
    const float bBottom = bPosition.y - bSize.y * 0.5f;
    const float bTop = bPosition.y + bSize.y * 0.5f;

    return aLeft < bRight && aRight > bLeft && aBottom < bTop && aTop > bBottom;
}

// ----------------------------------------------------------------------------
// 障碍物计算
// ----------------------------------------------------------------------------

glm::vec2 Level::GetUpperObstaclePosition(const Obstacle& obstacle) const
{
    const float upperY =
        obstacle.GapY + GetCurrentGapSize() * 0.5f + GameConfig::Level::ObstacleHeight * 0.5f;

    return {obstacle.X, upperY};
}

glm::vec2 Level::GetLowerObstaclePosition(const Obstacle& obstacle) const
{
    const float lowerY =
        obstacle.GapY - GetCurrentGapSize() * 0.5f - GameConfig::Level::ObstacleHeight * 0.5f;

    return {obstacle.X, lowerY};
}

glm::vec2 Level::GetObstacleSize() const
{
    return {GameConfig::Level::ObstacleWidth, GameConfig::Level::ObstacleHeight};
}

// ----------------------------------------------------------------------------
// 关卡状态
// ----------------------------------------------------------------------------

void Level::ResetObstacle(Obstacle& obstacle, float x)
{
    obstacle.X = x;
    obstacle.GapY = RandomFloat(GameConfig::Level::MinGapY, GameConfig::Level::MaxGapY);
    obstacle.Passed = false;
}

float Level::GetFarthestObstacleX() const
{
    float farthestX = m_Obstacles[0].X;

    for (const auto& obstacle : m_Obstacles)
        farthestX = std::max(farthestX, obstacle.X);

    return farthestX;
}

float Level::GetNextObstacleSpawnX() const
{
    const float spacedSpawnX = GetFarthestObstacleX() + GameConfig::Level::ObstacleSpacing;
    return std::max(spacedSpawnX, GameConfig::Level::MinSpawnX);
}

// ----------------------------------------------------------------------------
// 难度
// ----------------------------------------------------------------------------

float Level::GetDifficultyStep() const
{
    if (!GameConfig::Level::EnableDifficultyScaling)
        return 0.0f;

    if (GameConfig::Level::DifficultyScoreStep <= 0.0f)
        return 0.0f;

    return std::floor(static_cast<float>(m_Score) / GameConfig::Level::DifficultyScoreStep);
}

float Level::GetCurrentScrollSpeed() const
{
    const float speed = GameConfig::Level::ScrollSpeed +
                        GetDifficultyStep() * GameConfig::Level::SpeedIncreasePerStep;

    return std::min(speed, GameConfig::Level::MaxScrollSpeed);
}

float Level::GetCurrentGapSize() const
{
    const float gapSize =
        GameConfig::Level::GapSize - GetDifficultyStep() * GameConfig::Level::GapShrinkPerStep;

    return std::max(gapSize, GameConfig::Level::MinGapSize);
}