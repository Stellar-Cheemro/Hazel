#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Renderer/Texture.h>

namespace Hazel
{
class HAZEL_API OpenGLTexture2D : public Texture2D
{
public:
    OpenGLTexture2D(const std::string& path);
    virtual ~OpenGLTexture2D();
    virtual uint32_t GetWidth() const override
    {
        return m_Width;
    };
    virtual uint32_t GetHeight() const override
    {
        return m_Height;
    };

    virtual void Bind(uint32_t slot = 0) const override;

private:
    std::string m_Path;
    uint32_t m_RendererID;
    uint32_t m_Width, m_Height;
};
} // namespace Hazel