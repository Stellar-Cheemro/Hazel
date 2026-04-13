#pragma once

#include <spdlog/fmt/fmt.h>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

template <> struct fmt::formatter<glm::vec2>
{
    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(const glm::vec2& v, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "vec2({:.3f}, {:.3f})", v.x, v.y);
    }
};

template <> struct fmt::formatter<glm::vec3>
{
    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(const glm::vec3& v, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "vec3({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
    }
};

template <> struct fmt::formatter<glm::vec4>
{
    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(const glm::vec4& v, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "vec4({:.3f}, {:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z,
                              v.w);
    }
};

template <> struct fmt::formatter<glm::mat4>
{
    constexpr auto parse(fmt::format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(const glm::mat4& m, FormatContext& ctx) const
    {
        // GLM 默认按列主序索引：m[col][row]
        return fmt::format_to(ctx.out(),
                              "mat4([{: .3f}, {: .3f}, {: .3f}, {: .3f}], "
                              "[{: .3f}, {: .3f}, {: .3f}, {: .3f}], "
                              "[{: .3f}, {: .3f}, {: .3f}, {: .3f}], "
                              "[{: .3f}, {: .3f}, {: .3f}, {: .3f}])",
                              m[0][0], m[1][0], m[2][0], m[3][0], m[0][1], m[1][1], m[2][1],
                              m[3][1], m[0][2], m[1][2], m[2][2], m[3][2], m[0][3], m[1][3],
                              m[2][3], m[3][3]);
    }
};