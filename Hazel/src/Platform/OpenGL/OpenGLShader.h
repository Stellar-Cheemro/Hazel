#pragma once
// clang-format off
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Containers.h>
#include <Hazel/Renderer/Shader.h>

#include <cstdint>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>
// clang-format on
namespace Hazel
{
class HAZEL_API OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
    OpenGLShader(const std::string& filepath);
    virtual ~OpenGLShader();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetMat4(const std::string& name, const glm::mat4& matrix) override;
    virtual void SetFloat4(const std::string& name, const glm::vec4& vector) override;
    virtual void SetFloat3(const std::string& name, const glm::vec3& vector) override;
    virtual void SetFloat2(const std::string& name, const glm::vec2& vector) override;
    virtual void SetFloat(const std::string& name, float value) override;
    virtual void SetInt(const std::string& name, int value) override;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;

    // 设置 Uniform 变量接口
    void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
    void UploadUniformFloat4(const std::string& name, const glm::vec4& vector);
    void UploadUniformFloat3(const std::string& name, const glm::vec3& vector);
    void UploadUniformFloat2(const std::string& name, const glm::vec2& vector);
    void UploadUniformFloat(const std::string& name, float value);
    void UploadUniformInt(const std::string& name, int value);
    void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

private:
    std::string ReadFile(const std::string& filepath);
    HashMap<GLenum, std::string> PreProcess(const std::string& source);
    void Compile(const HashMap<GLenum, std::string>& shaderSources);

private:
    uint32_t m_RendererID;
};
} // namespace Hazel