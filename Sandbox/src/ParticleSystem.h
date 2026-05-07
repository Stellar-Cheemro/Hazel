#pragma once

#include <Hazel.h>

#include <cstdint>
#include <vector>

// ----------------------------------------------------------------------------
// ParticleSystem
// ----------------------------------------------------------------------------
//
// 职责：
// 1. 管理简单 2D 粒子池。
// 2. 负责粒子更新和渲染。
// 3. 为玩家飞行提供临时尾迹效果。
//
// 边界：
// 1. 不负责游戏状态。
// 2. 不负责碰撞。
// 3. 不负责资源加载。

struct ParticleProps
{
    glm::vec2 Position = {0.0f, 0.0f};

    glm::vec2 Velocity = {0.0f, 0.0f};
    glm::vec2 VelocityVariation = {0.0f, 0.0f};

    glm::vec4 ColorBegin = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 ColorEnd = {1.0f, 1.0f, 1.0f, 0.0f};

    float SizeBegin = 1.0f;
    float SizeEnd = 0.0f;
    float SizeVariation = 0.0f;

    float LifeTime = 1.0f;
};

class ParticleSystem
{
public:
    ParticleSystem();

    void Resize(uint32_t maxParticles);
    void Clear();

    void Emit(const ParticleProps& particleProps);

    void OnUpdate(Hazel::Timestep timestep);
    void OnRender();

private:
    struct Particle
    {
        glm::vec2 Position = {0.0f, 0.0f};
        glm::vec2 Velocity = {0.0f, 0.0f};

        glm::vec4 ColorBegin = {1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec4 ColorEnd = {1.0f, 1.0f, 1.0f, 0.0f};

        float SizeBegin = 1.0f;
        float SizeEnd = 0.0f;

        float LifeTime = 1.0f;
        float LifeRemaining = 0.0f;

        bool Active = false;
    };

private:
    float RandomFloat(float min, float max);

private:
    std::vector<Particle> m_ParticlePool;
    uint32_t m_PoolIndex = 0;
};