#pragma once

#include <glm/glm.hpp>

#include <string>

namespace Hazel
{
class Shader
{
public:
    Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
    virtual ~Shader();

    virtual void Bind() const;
    virtual void Unbind() const;

    // 设置 Uniform 变量接口
    virtual void SetUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
    uint32_t m_RendererID;
};
} // namespace Hazel