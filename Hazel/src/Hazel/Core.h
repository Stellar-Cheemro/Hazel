#pragma once

#ifdef HAZEL_PLATFORM_WINDOWS
#    ifdef HAZEL_BUILD_DLL
#        define HAZEL_API __declspec(dllexport)
#    else
#        define HAZEL_API __declspec(dllimport)
#    endif
#else
#    error Hazel only supports Windows!
#endif