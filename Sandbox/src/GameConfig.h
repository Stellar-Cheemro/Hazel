#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ----------------------------------------------------------------------------
// 游戏参数
// ----------------------------------------------------------------------------
//
// 当前阶段使用 inline static 变量，方便 ImGui 运行时调参。
// 后续如果需要保存配置，可以再迁移为 Project Settings 或 JSON。

struct GameConfig
{
    struct Camera
    {
        inline static float Height = 8.0f;
    };

    struct Player
    {
        inline static glm::vec2 InitialPosition = {-4.0f, 0.0f};
        inline static glm::vec2 Size = {0.8f, 0.8f};

        inline static glm::vec2 CollisionSize = {0.55f, 0.55f};
        inline static glm::vec2 CollisionOffset = {0.0f, 0.0f};

        inline static float Gravity = -18.0f;
        inline static float JumpForce = 7.5f;

        inline static float RotationVelocityScale = 0.08f;
        inline static float MinRotation = glm::radians(-45.0f);
        inline static float MaxRotation = glm::radians(25.0f);

        inline static float MinY = -5.8f;
        inline static float MaxY = 6.8f;
    };

    struct Level
    {
        inline static float ScrollSpeed = 3.0f;

        inline static float ObstacleSpacing = 6.5f;
        inline static float FirstObstacleX = 6.0f;

        // 回收和生成边界。
        // 注意：这两个值不是碰撞边界，而是障碍物生命周期边界。
        inline static float DespawnX = -16.0f;
        inline static float MinSpawnX = 16.0f;

        inline static float ObstacleWidth = 1.2f;
        inline static float ObstacleHeight = 12.0f;
        inline static float GapSize = 4.2f;

        inline static float MinGapY = -2.5f;
        inline static float MaxGapY = 2.5f;

        inline static float GroundY = -6.5f;
        inline static float GroundHeight = 1.0f;

        inline static float BackgroundScrollSpeed = 0.35f;
        inline static float BackgroundTileWidth = 30.0f;
        inline static float GroundTileWidth = 10.0f;

        inline static bool EnableDifficultyScaling = true;

        inline static float DifficultyScoreStep = 5.0f;
        inline static float SpeedIncreasePerStep = 0.35f;
        inline static float MaxScrollSpeed = 6.0f;

        inline static float GapShrinkPerStep = 0.20f;
        inline static float MinGapSize = 2.8f;
    };

    struct Particle
    {
        inline static uint32_t PoolSize = 500;

        inline static float SpawnInterval = 0.035f;

        inline static glm::vec2 Velocity = {-0.8f, 0.0f};
        inline static glm::vec2 VelocityVariation = {0.6f, 1.2f};

        inline static glm::vec4 ColorBegin = {1.0f, 0.45f, 0.25f, 1.0f};
        inline static glm::vec4 ColorEnd = {1.0f, 0.15f, 0.05f, 0.0f};

        inline static float SizeBegin = 0.18f;
        inline static float SizeEnd = 0.02f;
        inline static float SizeVariation = 0.08f;

        inline static float LifeTime = 0.65f;
    };

    static void ResetToDefaults()
    {
        Camera::Height = 8.0f;

        Player::InitialPosition = {-4.0f, 0.0f};
        Player::Size = {0.8f, 0.8f};

        Player::CollisionSize = {0.55f, 0.55f};
        Player::CollisionOffset = {0.0f, 0.0f};

        Player::Gravity = -18.0f;
        Player::JumpForce = 7.5f;

        Player::RotationVelocityScale = 0.08f;
        Player::MinRotation = glm::radians(-45.0f);
        Player::MaxRotation = glm::radians(25.0f);

        Player::MinY = -5.8f;
        Player::MaxY = 6.8f;

        Level::ScrollSpeed = 3.0f;

        Level::ObstacleSpacing = 6.5f;
        Level::FirstObstacleX = 6.0f;

        Level::DespawnX = -16.0f;
        Level::MinSpawnX = 16.0f;

        Level::ObstacleWidth = 1.2f;
        Level::ObstacleHeight = 12.0f;
        Level::GapSize = 4.2f;

        Level::MinGapY = -2.5f;
        Level::MaxGapY = 2.5f;

        Level::GroundY = -6.5f;
        Level::GroundHeight = 1.0f;

        Level::BackgroundScrollSpeed = 0.35f;
        Level::BackgroundTileWidth = 30.0f;
        Level::GroundTileWidth = 10.0f;

        Level::EnableDifficultyScaling = true;

        Level::DifficultyScoreStep = 5.0f;
        Level::SpeedIncreasePerStep = 0.35f;
        Level::MaxScrollSpeed = 6.0f;

        Level::GapShrinkPerStep = 0.20f;
        Level::MinGapSize = 2.8f;

        Particle::PoolSize = 500;

        Particle::SpawnInterval = 0.035f;

        Particle::Velocity = {-0.8f, 0.0f};
        Particle::VelocityVariation = {0.6f, 1.2f};

        Particle::ColorBegin = {1.0f, 0.45f, 0.25f, 1.0f};
        Particle::ColorEnd = {1.0f, 0.15f, 0.05f, 0.0f};

        Particle::SizeBegin = 0.18f;
        Particle::SizeEnd = 0.02f;
        Particle::SizeVariation = 0.08f;

        Particle::LifeTime = 0.65f;
    }
};