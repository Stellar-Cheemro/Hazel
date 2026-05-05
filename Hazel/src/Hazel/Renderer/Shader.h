#pragma once

// clang-format off
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>

#include <cstdint>
#include <string>

#include <glm/glm.hpp>
// clang-format on
namespace Hazel
{
class HAZEL_API Shader : public RefCounted
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
    static Shader* Create(const std::string& filepath);
    // 设置 Uniform 变量接口
    virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) = 0;
    virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& vector) = 0;
    virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& vector) = 0;
    virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& vector) = 0;
    virtual void UploadUniformFloat(const std::string& name, float value) = 0;
    virtual void UploadUniformInt(const std::string& name, int value) = 0;
    virtual void UploadUniformIntArray(const std::string& name, int* values, uint32_t count) = 0;
};
} // namespace Hazel