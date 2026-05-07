#pragma once

#include <Hazel.h>

// ----------------------------------------------------------------------------
// Player
// ----------------------------------------------------------------------------
//
// 职责：
// 1. 管理玩家位置、速度和旋转。
// 2. 处理重力、跳跃和边界限制。
// 3. 渲染玩家。
// 4. 提供独立碰撞体数据。
//
// 边界：
// 1. 不处理关卡碰撞。
// 2. 不处理分数。
// 3. 不处理游戏状态。

class Player
{
public:
    Player();

    void LoadAssets();

    void Reset();
    void Jump();

    void OnUpdate(Hazel::Timestep timestep);
    void OnRender();
    void OnDebugRender() const;

    const glm::vec2& GetPosition() const
    {
        return m_Position;
    }

    const glm::vec2& GetSize() const
    {
        return m_Size;
    }

    glm::vec2 GetCollisionPosition() const
    {
        return m_Position + m_CollisionOffset;
    }

    const glm::vec2& GetCollisionSize() const
    {
        return m_CollisionSize;
    }

    float GetRotation() const
    {
        return m_Rotation;
    }

private:
    glm::vec2 m_Position = {0.0f, 0.0f};
    glm::vec2 m_Velocity = {0.0f, 0.0f};

    glm::vec2 m_Size = {1.0f, 1.0f};
    glm::vec2 m_CollisionSize = {1.0f, 1.0f};
    glm::vec2 m_CollisionOffset = {0.0f, 0.0f};

    float m_Rotation = 0.0f;

    Hazel::Ref<Hazel::Texture2D> m_Texture;
};