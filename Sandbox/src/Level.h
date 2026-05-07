#pragma once

#include <Hazel.h>

#include "Player.h"

#include <array>

// ----------------------------------------------------------------------------
// Level
// ----------------------------------------------------------------------------
//
// 职责：
// 1. 管理背景、地面和障碍物。
// 2. 管理滚动、分数和难度递增。
// 3. 处理玩家与关卡的碰撞检测。
// 4. 渲染关卡和 Debug 碰撞框。
//
// 边界：
// 1. 不处理玩家跳跃。
// 2. 不处理游戏状态。
// 3. 不处理输入。

class Level
{
public:
    Level();

    void LoadAssets();
    void Reset();

    void OnUpdate(Hazel::Timestep timestep, const Player& player);
    void OnRender();
    void OnDebugRender() const;

    bool CheckCollision(const Player& player) const;

    uint32_t GetScore() const
    {
        return m_Score;
    }

    float GetCurrentScrollSpeed() const;
    float GetCurrentGapSize() const;

private:
    struct Obstacle
    {
        float X = 0.0f;
        float GapY = 0.0f;
        bool Passed = false;
    };

private:
    // ------------------------------------------------------------------------
    // 渲染
    // ------------------------------------------------------------------------

    void RenderBackground();
    void RenderObstacles();
    void RenderGround();

    void RenderObstacleDebugBounds() const;
    void RenderGroundDebugBounds() const;

private:
    // ------------------------------------------------------------------------
    // 障碍物计算
    // ------------------------------------------------------------------------

    glm::vec2 GetUpperObstaclePosition(const Obstacle& obstacle) const;
    glm::vec2 GetLowerObstaclePosition(const Obstacle& obstacle) const;
    glm::vec2 GetObstacleSize() const;

private:
    // ------------------------------------------------------------------------
    // 关卡状态
    // ------------------------------------------------------------------------

    void ResetObstacle(Obstacle& obstacle, float x);
    float GetFarthestObstacleX() const;
    float GetNextObstacleSpawnX() const;
    float GetDifficultyStep() const;

private:
    // ------------------------------------------------------------------------
    // 碰撞
    // ------------------------------------------------------------------------

    bool CheckAABB(const glm::vec2& aPosition, const glm::vec2& aSize, const glm::vec2& bPosition,
                   const glm::vec2& bSize) const;

private:
    static constexpr size_t s_ObstacleCount = 5;

    std::array<Obstacle, s_ObstacleCount> m_Obstacles;

    uint32_t m_Score = 0;

    float m_BackgroundOffset = 0.0f;
    float m_GroundOffset = 0.0f;

    Hazel::Ref<Hazel::Texture2D> m_BackgroundTexture;
    Hazel::Ref<Hazel::Texture2D> m_GroundTexture;
    Hazel::Ref<Hazel::Texture2D> m_ObstacleTexture;
};