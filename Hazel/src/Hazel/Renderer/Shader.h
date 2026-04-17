#pragma once

#include <Hazel/Core/Core.h>

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
namespace Hazel
{
class HAZEL_API Shader
{
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    virtual ~Shader();

    virtual void Bind() const;
    virtual void Unbind() const;

    // 设置 Uniform 变量接口
    virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
    uint32_t m_RendererID;
};
} // namespace Hazel