#pragma once

#include <string>
#include <type_traits>

#include <spdlog/fmt/fmt.h>

#include "Hazel/Events/Event.h"

// 为 Hazel::Event 及其所有派生类提供统一 formatter
template <typename T>
    requires std::derived_from<T, Hazel::Event>
struct fmt::formatter<T, char> : fmt::formatter<std::string>
{
    auto format(const T& e, format_context& ctx) const -> format_context::iterator
    {
        return fmt::formatter<std::string>::format(e.ToString(), ctx);
    }
};