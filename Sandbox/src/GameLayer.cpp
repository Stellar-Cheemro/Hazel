#include "GameLayer.h"
#include "GameConfig.h"
#include "ParticleSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

// ----------------------------------------------------------------------------
// 生命周期
// ----------------------------------------------------------------------------

GameLayer::GameLayer() : Hazel::Layer("GameLayer")
{
    auto& window = Hazel::Application::GetApplication().GetWindow();
    CreateCamera(window.GetWidth(), window.GetHeight());
}

void GameLayer::OnAttach()
{
    m_Player.LoadAssets();
    m_Level.LoadAssets();

    ResetGame();
    m_State = GameState::MainMenu;
}

void GameLayer::OnDetach()
{
}

// ----------------------------------------------------------------------------
// 更新
// ----------------------------------------------------------------------------

void GameLayer::OnUpdate(Hazel::Timestep timestep)
{
    m_Time += timestep;

    const bool shouldSimulate = m_State == GameState::Playing && (!m_Paused || m_StepFrame);

    if (shouldSimulate)
    {
        const float scaledDeltaTime = static_cast<float>(timestep) * m_TimeScale;
        Hazel::Timestep simulationTimestep(scaledDeltaTime);

        m_Player.OnUpdate(simulationTimestep);
        m_Level.OnUpdate(simulationTimestep, m_Player);

        if (m_EnableParticles)
            EmitPlayerTrail(simulationTimestep);

        m_ParticleSystem.OnUpdate(simulationTimestep);

        if (m_Level.CheckCollision(m_Player))
        {
            m_State = GameState::GameOver;
            m_Paused = false;
            m_StepFrame = false;

            if (m_Level.GetScore() > m_BestScore)
                m_BestScore = m_Level.GetScore();
        }
    }

    if (m_StepFrame)
        m_StepFrame = false;

    Hazel::RenderCommand::SetClearColor({0.08f, 0.09f, 0.12f, 1.0f});
    Hazel::RenderCommand::Clear();

    Hazel::Renderer2D::BeginScene(*m_Camera);

    m_Level.OnRender();
    m_ParticleSystem.OnRender();
    m_Player.OnRender();

    if (m_ShowCollisionBounds)
    {
        m_Level.OnDebugRender();
        m_Player.OnDebugRender();
    }

    Hazel::Renderer2D::EndScene();
}

// ----------------------------------------------------------------------------
// 事件
// ----------------------------------------------------------------------------

void GameLayer::OnEvent(Hazel::Event& event)
{
    Hazel::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<Hazel::WindowResizeEvent>(HAZEL_BIND_EVENT_FN(GameLayer::OnWindowResize));

    dispatcher.Dispatch<Hazel::MouseButtonPressedEvent>(
        HAZEL_BIND_EVENT_FN(GameLayer::OnMouseButtonPressed));

    dispatcher.Dispatch<Hazel::KeyPressedEvent>(HAZEL_BIND_EVENT_FN(GameLayer::OnKeyPressed));
}

bool GameLayer::OnWindowResize(Hazel::WindowResizeEvent& event)
{
    CreateCamera(event.GetWidth(), event.GetHeight());
    return false;
}

bool GameLayer::OnMouseButtonPressed(Hazel::MouseButtonPressedEvent& event)
{
    if (event.GetMouseButton() == HAZEL_MOUSE_BUTTON_LEFT)
        HandleActionInput();

    return false;
}

bool GameLayer::OnKeyPressed(Hazel::KeyPressedEvent& event)
{
    if (event.GetRepeatCount() > 0)
        return false;

    if (event.GetKeyCode() == HAZEL_KEY_SPACE)
        HandleActionInput();

    if (event.GetKeyCode() == HAZEL_KEY_P && m_State == GameState::Playing)
        m_Paused = !m_Paused;

    if (event.GetKeyCode() == HAZEL_KEY_O && m_State == GameState::Playing && m_Paused)
        m_StepFrame = true;

    return false;
}

// ----------------------------------------------------------------------------
// 流程
// ----------------------------------------------------------------------------

void GameLayer::CreateCamera(uint32_t width, uint32_t height)
{
    if (height == 0)
        height = 1;

    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    const float cameraHeight = GameConfig::Camera::Height;
    const float bottom = -cameraHeight;
    const float top = cameraHeight;
    const float left = bottom * aspectRatio;
    const float right = top * aspectRatio;

    m_Camera = Hazel::CreateScope<Hazel::OrthographicCamera>(left, right, bottom, top);
}

void GameLayer::ResetGame()
{
    m_Player.Reset();
    m_Level.Reset();
    ResetParticles();
}

void GameLayer::StartGame()
{
    ResetGame();

    m_State = GameState::Playing;
    m_Paused = false;
    m_StepFrame = false;

    m_Player.Jump();
}

void GameLayer::HandleActionInput()
{
    switch (m_State)
    {
        case GameState::MainMenu:
        {
            StartGame();
            break;
        }

        case GameState::Playing:
        {
            if (!m_Paused)
                m_Player.Jump();

            break;
        }

        case GameState::GameOver:
        {
            StartGame();
            break;
        }
    }
}

// ----------------------------------------------------------------------------
// 粒子
// ----------------------------------------------------------------------------

void GameLayer::ResetParticles()
{
    m_ParticleSystem.Resize(GameConfig::Particle::PoolSize);
    m_ParticleSystem.Clear();

    m_PlayerTrailParticle.Velocity = GameConfig::Particle::Velocity;
    m_PlayerTrailParticle.VelocityVariation = GameConfig::Particle::VelocityVariation;

    m_PlayerTrailParticle.ColorBegin = GameConfig::Particle::ColorBegin;
    m_PlayerTrailParticle.ColorEnd = GameConfig::Particle::ColorEnd;

    m_PlayerTrailParticle.SizeBegin = GameConfig::Particle::SizeBegin;
    m_PlayerTrailParticle.SizeEnd = GameConfig::Particle::SizeEnd;
    m_PlayerTrailParticle.SizeVariation = GameConfig::Particle::SizeVariation;

    m_PlayerTrailParticle.LifeTime = GameConfig::Particle::LifeTime;

    m_ParticleSpawnTimer = 0.0f;
}

void GameLayer::EmitPlayerTrail(Hazel::Timestep timestep)
{
    m_ParticleSpawnTimer += timestep;

    if (m_ParticleSpawnTimer < GameConfig::Particle::SpawnInterval)
        return;

    m_ParticleSpawnTimer = 0.0f;

    m_PlayerTrailParticle.Position = m_Player.GetPosition();
    m_PlayerTrailParticle.Position.x -= m_Player.GetSize().x * 0.35f;

    m_ParticleSystem.Emit(m_PlayerTrailParticle);
}

// ----------------------------------------------------------------------------
// UI
// ----------------------------------------------------------------------------

void GameLayer::OnImGuiRender()
{
    DrawGameUI();
    DrawDebugUI();
}

void GameLayer::DrawGameUI()
{
    ImGuiIO& io = ImGui::GetIO();

    const ImGuiWindowFlags overlayFlags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 24.0f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    ImGui::Begin("##GameScoreOverlay", nullptr, overlayFlags | ImGuiWindowFlags_NoInputs);
    ImGui::Text("Score: %u", m_Level.GetScore());

    if (m_State == GameState::Playing && m_Paused)
        ImGui::TextUnformatted("Paused");

    ImGui::End();

    if (m_State == GameState::MainMenu)
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.75f);

        ImGui::Begin("##MainMenuOverlay", nullptr, overlayFlags | ImGuiWindowFlags_NoInputs);
        ImGui::TextUnformatted("Hazel Bird");
        ImGui::Separator();
        ImGui::TextUnformatted("Press Space or Left Mouse Button");
        ImGui::TextUnformatted("to start");
        ImGui::End();
    }

    if (m_State == GameState::GameOver)
    {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                                ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.75f);

        ImGui::Begin("##GameOverOverlay", nullptr, overlayFlags | ImGuiWindowFlags_NoInputs);
        ImGui::TextUnformatted("Game Over");
        ImGui::Separator();
        ImGui::Text("Score: %u", m_Level.GetScore());
        ImGui::Text("Best: %u", m_BestScore);
        ImGui::Spacing();
        ImGui::TextUnformatted("Press Space or Left Mouse Button");
        ImGui::TextUnformatted("to restart");
        ImGui::End();
    }
}

void GameLayer::DrawDebugUI()
{
    ImGui::Begin("Game Debug");

    ImGui::Text("State: %s", GetStateName());
    ImGui::Text("Time: %.2f", m_Time);
    ImGui::Text("Paused: %s", m_Paused ? "true" : "false");
    ImGui::Text("Time Scale: %.2f", m_TimeScale);
    ImGui::Separator();

    ImGui::Text("Score: %u", m_Level.GetScore());
    ImGui::Text("Best Score: %u", m_BestScore);
    ImGui::Text("Current Scroll Speed: %.2f", m_Level.GetCurrentScrollSpeed());
    ImGui::Text("Current Gap Size: %.2f", m_Level.GetCurrentGapSize());
    ImGui::Separator();

    ImGui::Text("Player Position: %.2f, %.2f", m_Player.GetPosition().x, m_Player.GetPosition().y);
    ImGui::Text("Player Rotation: %.2f", m_Player.GetRotation());

    ImGui::Checkbox("Show Collision Bounds", &m_ShowCollisionBounds);
    ImGui::Checkbox("Enable Particles", &m_EnableParticles);
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Simulation Controls", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button(m_Paused ? "Resume" : "Pause"))
        {
            if (m_State == GameState::Playing)
                m_Paused = !m_Paused;
        }

        ImGui::SameLine();

        if (ImGui::Button("Step Frame"))
        {
            if (m_State == GameState::Playing && m_Paused)
                m_StepFrame = true;
        }

        ImGui::DragFloat("Time Scale", &m_TimeScale, 0.05f, 0.05f, 3.0f);

        if (ImGui::Button("Normal Speed"))
            m_TimeScale = 1.0f;

        ImGui::SameLine();

        if (ImGui::Button("Slow Motion"))
            m_TimeScale = 0.25f;

        ImGui::TextUnformatted("P = Pause / Resume");
        ImGui::TextUnformatted("O = Step one frame when paused");
    }

    ImGui::Separator();

    if (ImGui::CollapsingHeader("Runtime Tuning", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Reset Config To Defaults"))
        {
            GameConfig::ResetToDefaults();
            ResetGame();

            auto& window = Hazel::Application::GetApplication().GetWindow();
            CreateCamera(window.GetWidth(), window.GetHeight());
        }

        ImGui::Spacing();

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::DragFloat("Camera Height", &GameConfig::Camera::Height, 0.1f, 2.0f, 20.0f))
            {
                auto& window = Hazel::Application::GetApplication().GetWindow();
                CreateCamera(window.GetWidth(), window.GetHeight());
            }
        }

        if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat2("Initial Position", &GameConfig::Player::InitialPosition.x, 0.05f,
                              -20.0f, 20.0f);
            ImGui::DragFloat2("Size", &GameConfig::Player::Size.x, 0.05f, 0.1f, 5.0f);
            ImGui::DragFloat2("Collision Size", &GameConfig::Player::CollisionSize.x, 0.05f, 0.1f,
                              5.0f);
            ImGui::DragFloat2("Collision Offset", &GameConfig::Player::CollisionOffset.x, 0.05f,
                              -2.0f, 2.0f);

            ImGui::DragFloat("Gravity", &GameConfig::Player::Gravity, 0.1f, -60.0f, 0.0f);
            ImGui::DragFloat("Jump Force", &GameConfig::Player::JumpForce, 0.1f, 0.0f, 30.0f);
            ImGui::DragFloat("Rotation Velocity Scale", &GameConfig::Player::RotationVelocityScale,
                             0.005f, 0.0f, 1.0f);

            float minRotationDegrees = glm::degrees(GameConfig::Player::MinRotation);
            float maxRotationDegrees = glm::degrees(GameConfig::Player::MaxRotation);

            if (ImGui::DragFloat("Min Rotation Degrees", &minRotationDegrees, 1.0f, -180.0f, 0.0f))
                GameConfig::Player::MinRotation = glm::radians(minRotationDegrees);

            if (ImGui::DragFloat("Max Rotation Degrees", &maxRotationDegrees, 1.0f, 0.0f, 180.0f))
                GameConfig::Player::MaxRotation = glm::radians(maxRotationDegrees);

            ImGui::DragFloat("Min Y", &GameConfig::Player::MinY, 0.1f, -20.0f, 0.0f);
            ImGui::DragFloat("Max Y", &GameConfig::Player::MaxY, 0.1f, 0.0f, 20.0f);

            if (ImGui::Button("Apply Player Reset"))
                m_Player.Reset();
        }

        if (ImGui::CollapsingHeader("Level", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("Scroll Speed", &GameConfig::Level::ScrollSpeed, 0.1f, 0.0f, 20.0f);

            ImGui::DragFloat("Obstacle Spacing", &GameConfig::Level::ObstacleSpacing, 0.1f, 2.0f,
                             20.0f);
            ImGui::DragFloat("First Obstacle X", &GameConfig::Level::FirstObstacleX, 0.1f, -10.0f,
                             30.0f);
            ImGui::DragFloat("Despawn X", &GameConfig::Level::DespawnX, 0.1f, -40.0f, 0.0f);
            ImGui::DragFloat("Min Spawn X", &GameConfig::Level::MinSpawnX, 0.1f, 0.0f, 40.0f);

            ImGui::DragFloat("Obstacle Width", &GameConfig::Level::ObstacleWidth, 0.05f, 0.1f,
                             10.0f);
            ImGui::DragFloat("Obstacle Height", &GameConfig::Level::ObstacleHeight, 0.1f, 1.0f,
                             30.0f);
            ImGui::DragFloat("Gap Size", &GameConfig::Level::GapSize, 0.1f, 1.0f, 12.0f);

            ImGui::DragFloat("Min Gap Y", &GameConfig::Level::MinGapY, 0.1f, -10.0f, 10.0f);
            ImGui::DragFloat("Max Gap Y", &GameConfig::Level::MaxGapY, 0.1f, -10.0f, 10.0f);

            ImGui::DragFloat("Ground Y", &GameConfig::Level::GroundY, 0.1f, -20.0f, 0.0f);
            ImGui::DragFloat("Ground Height", &GameConfig::Level::GroundHeight, 0.05f, 0.1f, 5.0f);

            ImGui::DragFloat("Background Scroll Speed", &GameConfig::Level::BackgroundScrollSpeed,
                             0.05f, 0.0f, 10.0f);
            ImGui::DragFloat("Background Tile Width", &GameConfig::Level::BackgroundTileWidth, 0.5f,
                             5.0f, 100.0f);
            ImGui::DragFloat("Ground Tile Width", &GameConfig::Level::GroundTileWidth, 0.5f, 1.0f,
                             50.0f);

            ImGui::Separator();
            ImGui::TextUnformatted("Difficulty");

            ImGui::Checkbox("Enable Difficulty Scaling",
                            &GameConfig::Level::EnableDifficultyScaling);
            ImGui::DragFloat("Difficulty Score Step", &GameConfig::Level::DifficultyScoreStep, 1.0f,
                             1.0f, 50.0f);
            ImGui::DragFloat("Speed Increase Per Step", &GameConfig::Level::SpeedIncreasePerStep,
                             0.05f, 0.0f, 5.0f);
            ImGui::DragFloat("Max Scroll Speed", &GameConfig::Level::MaxScrollSpeed, 0.1f, 1.0f,
                             20.0f);
            ImGui::DragFloat("Gap Shrink Per Step", &GameConfig::Level::GapShrinkPerStep, 0.05f,
                             0.0f, 2.0f);
            ImGui::DragFloat("Min Gap Size", &GameConfig::Level::MinGapSize, 0.1f, 1.0f, 10.0f);

            if (ImGui::Button("Apply Level Reset"))
                m_Level.Reset();
        }

        if (ImGui::CollapsingHeader("Particles", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("Spawn Interval", &GameConfig::Particle::SpawnInterval, 0.005f, 0.005f,
                             0.5f);
            ImGui::DragFloat2("Velocity", &GameConfig::Particle::Velocity.x, 0.05f, -10.0f, 10.0f);
            ImGui::DragFloat2("Velocity Variation", &GameConfig::Particle::VelocityVariation.x,
                              0.05f, 0.0f, 10.0f);

            ImGui::ColorEdit4("Color Begin", &GameConfig::Particle::ColorBegin.x);
            ImGui::ColorEdit4("Color End", &GameConfig::Particle::ColorEnd.x);

            ImGui::DragFloat("Size Begin", &GameConfig::Particle::SizeBegin, 0.01f, 0.01f, 2.0f);
            ImGui::DragFloat("Size End", &GameConfig::Particle::SizeEnd, 0.01f, 0.0f, 2.0f);
            ImGui::DragFloat("Size Variation", &GameConfig::Particle::SizeVariation, 0.01f, 0.0f,
                             2.0f);

            ImGui::DragFloat("Life Time", &GameConfig::Particle::LifeTime, 0.05f, 0.05f, 5.0f);

            if (ImGui::Button("Apply Particle Reset"))
                ResetParticles();
        }
    }

    ImGui::End();
}

const char* GameLayer::GetStateName() const
{
    switch (m_State)
    {
        case GameState::MainMenu:
            return "MainMenu";

        case GameState::Playing:
            return "Playing";

        case GameState::GameOver:
            return "GameOver";
    }

    return "Unknown";
}