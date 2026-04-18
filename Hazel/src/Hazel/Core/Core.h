#pragma once

#include <memory>

#ifdef HAZEL_PLATFORM_WINDOWS
#    ifdef HAZEL_BUILD_DLL
#        define HAZEL_API __declspec(dllexport)
#    else
#        define HAZEL_API __declspec(dllimport)
#    endif
#else
#    error Hazel only supports Windows!
#endif

#define BIT(x) (1 << x)

#define HAZEL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Hazel
{
template <typename T> using Scope = std::unique_ptr<T>;

template <typename T> using Ref = std::shared_ptr<T>;
} // namespace Hazel