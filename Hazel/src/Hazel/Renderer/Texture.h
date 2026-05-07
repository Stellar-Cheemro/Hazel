#pragma once
#include <Hazel/Core/Core.h>
#include <Hazel/Core/Ref.h>
#include <string>
namespace Hazel
{
class HAZEL_API Texture : public RefCounted
{
public:
    virtual ~Texture() = default;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void SetData(void* data, uint32_t size) = 0;
    virtual void Bind(uint32_t slot = 0) const = 0;
    virtual void UnBind(uint32_t slot = 0) const = 0;
};
class HAZEL_API Texture2D : public Texture
{
public:
    static Ref<Texture2D> Create(uint32_t width, uint32_t height);
    static Ref<Texture2D> Create(const std::string& path);
};
} // namespace Hazel