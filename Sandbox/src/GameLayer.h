#pragma once

#include <Hazel.h>
#include <Hazel/Core/Scope.h>

#include "Level.h"
#include "ParticleSystem.h"
#include "Player.h"

// ----------------------------------------------------------------------------
// GameLayer
// ----------------------------------------------------------------------------
//
// 职责：
// 1. 管理游戏状态机。
// 2. 组织 Player / Level / ParticleSystem 的更新和渲染。
// 3. 处理输入。
// 4. 提供临时 ImGui 游戏 UI 和调试 UI。
//
// 边界：
// 1. 不直接实现玩家物理。
// 2. 不直接实现障碍物逻辑。
// 3. 不直接处理底层渲染资源。

class GameLayer : public Hazel::Layer
{
public:
    GameLayer();
    virtual ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(Hazel::Timestep timestep) override;
    virtual void OnEvent(Hazel::Event& event) override;
    virtual void OnImGuiRender() override;

private:
    enum class GameState
    {
        MainMenu = 0,
        Playing,
        GameOver
    };

private:
    // ------------------------------------------------------------------------
    // 事件
    // ------------------------------------------------------------------------

    bool OnWindowResize(Hazel::WindowResizeEvent& event);
    bool OnMouseButtonPressed(Hazel::MouseButtonPressedEvent& event);
    bool OnKeyPressed(Hazel::KeyPressedEvent& event);

private:
    // ------------------------------------------------------------------------
    // 流程
    // ------------------------------------------------------------------------

    void CreateCamera(uint32_t width, uint32_t height);

    void ResetGame();
    void StartGame();
    void HandleActionInput();

private:
    // ------------------------------------------------------------------------
    // 粒子
    // ------------------------------------------------------------------------

    void ResetParticles();
    void EmitPlayerTrail(Hazel::Timestep timestep);

private:
    // ------------------------------------------------------------------------
    // UI
    // ------------------------------------------------------------------------

    void DrawGameUI();
    void DrawDebugUI();

    const char* GetStateName() const;

private:
    Hazel::Scope<Hazel::OrthographicCamera> m_Camera;

    Player m_Player;
    Level m_Level;
    ParticleSystem m_ParticleSystem;

    ParticleProps m_PlayerTrailParticle;

    GameState m_State = GameState::MainMenu;

    uint32_t m_BestScore = 0;

    float m_Time = 0.0f;
    float m_ParticleSpawnTimer = 0.0f;

    bool m_Paused = false;
    bool m_StepFrame = false;
    float m_TimeScale = 1.0f;

    bool m_ShowCollisionBounds = false;
    bool m_EnableParticles = true;
};