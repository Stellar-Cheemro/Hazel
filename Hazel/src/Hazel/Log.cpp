#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Hazel
{
std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
void Log::Init()
{
    // 设置日志格式
    // %^ 开始颜色，%T 输出时间，%n 输出日志器名称，%v 输出日志内容，%$ 结束颜色
    spdlog::set_pattern("%^[%T] %n: %v%$");
    // 设置日志级别为 trace，所有级别的日志都会被输出
    s_CoreLogger = spdlog::stdout_color_mt("HAZEL");
    s_CoreLogger->set_level(spdlog::level::trace);

    s_ClientLogger = spdlog::stdout_color_mt("APP");
    s_ClientLogger->set_level(spdlog::level::trace);
}
} // namespace Hazel