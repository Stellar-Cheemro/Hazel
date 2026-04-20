#pragma once
#include <Hazel/Core/Core.h>
#include <atomic>
#include <cstdint>
#include <utility>
namespace Hazel
{
class HAZEL_API RefCounted
{
public:
    virtual ~RefCounted() = default;
    void IncrementRefcount() const
    {
        m_RefCount.fetch_add(1, std::memory_order_relaxed);
    }
    void DecrementRefcount()
    {
        if (m_RefCount.fetch_sub(1, std::memory_order_relaxed) == 1)
        {
            delete this;
        }
    }
    uint32_t GetRefcount() const
    {
        return m_RefCount.load(std::memory_order_relaxed);
    }

private:
    mutable std::atomic<uint32_t> m_RefCount = 0;
};

template <typename T> class Ref
{
public:
    Ref() = default;
    ~Ref()
    {
        DecrementRef();
    }

    Ref(T* instance) : m_Instance(instance)
    {
        IncrementRef();
    }
    Ref(const Ref& other) : m_Instance(other.m_Instance)
    {
        IncrementRef();
    }
    Ref& operator=(const Ref& other)
    {
        if (this != &other)
        {
            other.IncrementRef();
            DecrementRef();
            m_Instance = other.m_Instance;
        }
        return *this;
    }
    Ref& operator=(Ref&& other) noexcept
    {
        if (this != &other)
        {
            DecrementRef();
            m_Instance = other.m_Instance;
            other.m_Instance = nullptr;
        }
        return *this;
    }

    // 使用示例 ref->Function() 或 (*ref).Function()
    T* operator->()
    {
        return m_Instance;
    }
    const T* operator->() const
    {
        return m_Instance;
    }
    T& operator*()
    {
        return *m_Instance;
    }
    const T& operator*() const
    {
        return *m_Instance;
    }
    T* Raw() const
    {
        return m_Instance;
    }

    void reset(T* newInstance = nullptr)
    {
        if (m_Instance)
        {
            DecrementRef(); // 释放当前对象
        }

        m_Instance = newInstance; // 将智能指针指向新的对象或 nullptr

        if (m_Instance)
        {
            IncrementRef(); // 如果新的对象非空，增加引用计数
        }
    }
    operator bool() const
    {
        return m_Instance != nullptr;
    }

    template <typename U> Ref<U> As() const
    {
        return Ref<U>(static_cast<U*>(m_Instance));
    }

    template <typename... Args> static Ref<T> Create(Args&&... args)
    {
        return Ref<T>(new T(std::forward<Args>(args)...));
    }

private:
    void IncrementRef() const
    {
        if (m_Instance)
        {
            m_Instance->IncrementRefcount();
        }
    }
    void DecrementRef()
    {
        if (m_Instance)
        {
            m_Instance->DecrementRefcount();
        }
    }

private:
    template <typename U> friend class Ref;
    T* m_Instance = nullptr;
};

} // namespace Hazel