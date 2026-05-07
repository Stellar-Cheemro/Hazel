#include "ParticleSystem.h"

#include <algorithm>
#include <random>

namespace
{
static std::mt19937 s_RandomEngine(std::random_device{}());
}

// ----------------------------------------------------------------------------
// 生命周期
// ----------------------------------------------------------------------------

ParticleSystem::ParticleSystem()
{
    Resize(500);
}

void ParticleSystem::Resize(uint32_t maxParticles)
{
    m_ParticlePool.clear();
    m_ParticlePool.resize(maxParticles);
    m_PoolIndex = maxParticles > 0 ? maxParticles - 1 : 0;
}

void ParticleSystem::Clear()
{
    for (auto& particle : m_ParticlePool)
        particle.Active = false;
}

// ----------------------------------------------------------------------------
// 发射
// ----------------------------------------------------------------------------

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
    if (m_ParticlePool.empty())
        return;

    Particle& particle = m_ParticlePool[m_PoolIndex];

    particle.Active = true;
    particle.Position = particleProps.Position;

    particle.Velocity = particleProps.Velocity;
    particle.Velocity.x +=
        RandomFloat(-particleProps.VelocityVariation.x, particleProps.VelocityVariation.x);
    particle.Velocity.y +=
        RandomFloat(-particleProps.VelocityVariation.y, particleProps.VelocityVariation.y);

    particle.ColorBegin = particleProps.ColorBegin;
    particle.ColorEnd = particleProps.ColorEnd;

    particle.SizeBegin = particleProps.SizeBegin +
                         RandomFloat(-particleProps.SizeVariation, particleProps.SizeVariation);
    particle.SizeEnd = particleProps.SizeEnd;

    particle.LifeTime = particleProps.LifeTime;
    particle.LifeRemaining = particleProps.LifeTime;

    if (m_PoolIndex == 0)
        m_PoolIndex = static_cast<uint32_t>(m_ParticlePool.size()) - 1;
    else
        m_PoolIndex--;
}

// ----------------------------------------------------------------------------
// 更新
// ----------------------------------------------------------------------------

void ParticleSystem::OnUpdate(Hazel::Timestep timestep)
{
    const float dt = timestep;

    for (auto& particle : m_ParticlePool)
    {
        if (!particle.Active)
            continue;

        if (particle.LifeRemaining <= 0.0f)
        {
            particle.Active = false;
            continue;
        }

        particle.LifeRemaining -= dt;
        particle.Position += particle.Velocity * dt;
    }
}

// ----------------------------------------------------------------------------
// 渲染
// ----------------------------------------------------------------------------

void ParticleSystem::OnRender()
{
    for (const auto& particle : m_ParticlePool)
    {
        if (!particle.Active)
            continue;

        const float life = particle.LifeRemaining / particle.LifeTime;
        const glm::vec4 color = glm::mix(particle.ColorEnd, particle.ColorBegin, life);
        const float size = glm::mix(particle.SizeEnd, particle.SizeBegin, life);

        Hazel::Renderer2D::DrawQuad({particle.Position.x, particle.Position.y, 0.2f}, {size, size},
                                    color);
    }
}

// ----------------------------------------------------------------------------
// 工具
// ----------------------------------------------------------------------------

float ParticleSystem::RandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(s_RandomEngine);
}