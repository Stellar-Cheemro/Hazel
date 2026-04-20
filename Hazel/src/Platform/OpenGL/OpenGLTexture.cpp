#include "OpenGLTexture.h"

#include <Hazel/Core/Log.h>

#include <glad/glad.h>
#include <stb_image.h>
namespace Hazel
{
OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_Path(path)
{
    // 加载图像数据并生成纹理
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1); // 翻转图像
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data)
    {
        GLenum dataFormat = 0;
        GLenum internalFormat = 0;
        switch (channels)
        {
                // clang-format off
            case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
            case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
            case 1: dataFormat = GL_RED; internalFormat = GL_R8; break;
                // clang-format on
        }
        m_Width = width;
        m_Height = height;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE,
                            data);
    }
    else
    {
        HAZEL_CORE_ERROR("Failed to load texture: {0}", path);
    }
    stbi_image_free(data);
}

OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &m_RendererID);
}

void OpenGLTexture2D::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, m_RendererID);
}

} // namespace Hazel