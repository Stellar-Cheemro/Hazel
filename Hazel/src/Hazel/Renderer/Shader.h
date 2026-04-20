#pragma once

#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <cstdint>
#include <string>
namespace Hazel
{
class HAZEL_API Shader : public RefCounted
{
public:
    virtual ~Shader() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
};
} // namespace Hazel