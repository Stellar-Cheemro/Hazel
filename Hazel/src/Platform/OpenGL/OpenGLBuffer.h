#pragma once

#include "Hazel/Renderer/Buffer.h"
#include <Hazel/Core/Log.h>

#include <cstdint>
namespace Hazel
{
class OpenGLVertexBuffer : public Hazel::VertexBuffer
{
public:
    OpenGLVertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer() override;
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void SetLayout(const BufferLayout& layout) override
    {
        m_Layout = layout;
    }
    virtual const BufferLayout& GetLayout() const override
    {
        return m_Layout;
    }

private:
    uint32_t m_RendererID;
    BufferLayout m_Layout;
};
class OpenGLIndexBuffer : public Hazel::IndexBuffer
{
public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t count);

    virtual ~OpenGLIndexBuffer() override;
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual uint32_t GetCount() const override
    {
        return m_Count;
    }

private:
    uint32_t m_RendererID;
    uint32_t m_Count;
};
} // namespace Hazel