#pragma once
#include <Hazel.h>
class Sandbox2D : public Hazel::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(Hazel::Timestep timestep) override;
    virtual void OnEvent(Hazel::Event& event) override;
    virtual void OnImGuiRender() override;

private:
    Hazel::Ref<Hazel::Shader> m_Shader;
    Hazel::Ref<Hazel::Shader> m_TextureShader;
    Hazel::Ref<Hazel::VertexArray> m_SquareVA;
    Hazel::Ref<Hazel::Texture2D> m_CheckTex;
    Hazel::Ref<Hazel::Texture2D> m_LogoTex;
    glm::vec4 m_SquareColor{0.2f, 0.3f, 0.8f, 1.0f};

    Hazel::OrthographicCameraController m_CameraController;

    Hazel::AssetHandle m_CheckTexHandle = 0;
    Hazel::AssetHandle m_LogoTexHandle = 0;
    Hazel::AssetHandle m_FlatShaderHandle = 0;
    Hazel::AssetHandle m_TextureShaderHandle = 0;
};