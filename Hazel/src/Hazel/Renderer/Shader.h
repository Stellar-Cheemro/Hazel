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
    // 设置
    virtual void SetMat4(const std::string& name, const glm::mat4& matrix) = 0;
    virtual void SetFloat4(const std::string& name, const glm::vec4& vector) = 0;
    virtual void SetFloat3(const std::string& name, const glm::vec3& vector) = 0;
    virtual void SetFloat2(const std::string& name, const glm::vec2& vector) = 0;
    virtual void SetFloat(const std::string& name, float value) = 0;
    virtual void SetInt(const std::string& name, int value) = 0;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;
};
} // namespace Hazel