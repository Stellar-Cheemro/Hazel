#pragma once

#include <Hazel/Core/Core.h>
namespace Hazel
{

class HAZEL_API Timestep
{
public:
    Timestep(float time = 0.0f) : m_Time(time)
    {
    }

    operator float() const
    {
        return m_Time;
    }

    float GetSeconds() const
    {
        return m_Time;
    }
    float GetMilliseconds() const
    {
        return m_Time * 1000.0f;
    }
    float GetMicroseconds() const
    {
        return m_Time * 1000000.0f;
    }

private:
    float m_Time;
};

} // namespace Hazel