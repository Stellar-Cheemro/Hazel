#pragma once
#pragma once

#include <Hazel/Core/Core.h>
#include <Hazel/Renderer/Shader.h>
#include <glm/glm.hpp>

#include <cstdint>
#include <string>
namespace Hazel
{
class HAZEL_API OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
    virtual ~OpenGLShader();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    // 设置 Uniform 变量接口
    void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

    void UploadUniformFloat4(const std::string& name, const glm::vec4& vector);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& vector);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& vector);
    void UploadUniformFloat(const std::string& name, float value);

    void UploadUniformInt(const std::string& name, int value);
    void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

private:
    uint32_t m_RendererID;
};
} // namespace Hazel