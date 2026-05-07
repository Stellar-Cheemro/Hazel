#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Renderer/Texture.h>
#include <glad/glad.h>
namespace Hazel
{
class HAZEL_API OpenGLTexture2D : public Texture2D
{

public:
    OpenGLTexture2D(const std::string& path);
    OpenGLTexture2D(uint32_t width, uint32_t height);
    virtual ~OpenGLTexture2D();
    virtual uint32_t GetWidth() const override
    {
        return m_Width;
    };
    virtual uint32_t GetHeight() const override
    {
        return m_Height;
    };

    void SetData(void* data, uint32_t size);
    virtual void Bind(uint32_t slot = 0) const override;
    virtual void UnBind(uint32_t slot = 0) const override;

private:
    std::string m_Path;
    uint32_t m_RendererID;
    uint32_t m_Width, m_Height;
    GLenum m_DataFormat, m_InternalFormat;
};
} // namespace Hazel