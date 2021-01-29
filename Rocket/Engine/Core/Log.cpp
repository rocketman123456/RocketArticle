#include "Core/Log.h"

namespace Rocket
{
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;
    std::shared_ptr<spdlog::logger> Log::s_EventLogger;
    std::shared_ptr<spdlog::logger> Log::s_GraphicsLogger;

    void Log::Init(LogLevel level)
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        s_CoreLogger = spdlog::stdout_color_mt("Rocket");
        s_ClientLogger = spdlog::stdout_color_mt("App");
        s_EventLogger = spdlog::stdout_color_mt("Event");
        s_GraphicsLogger = spdlog::stdout_color_mt("Graphics");

        switch(level)
        {
        case LogLevel::TRACE:
            s_CoreLogger->set_level(spdlog::level::trace);
            s_ClientLogger->set_level(spdlog::level::trace);
            s_EventLogger->set_level(spdlog::level::trace);
            s_GraphicsLogger->set_level(spdlog::level::trace);
            break;
        case LogLevel::INFO:
            s_CoreLogger->set_level(spdlog::level::info);
            s_ClientLogger->set_level(spdlog::level::info);
            s_EventLogger->set_level(spdlog::level::info);
            s_GraphicsLogger->set_level(spdlog::level::info);
            break;
        case LogLevel::WARN:
            s_CoreLogger->set_level(spdlog::level::warn);
            s_ClientLogger->set_level(spdlog::level::warn);
            s_EventLogger->set_level(spdlog::level::warn);
            s_GraphicsLogger->set_level(spdlog::level::warn);
            break;
        case LogLevel::ERR:
            s_CoreLogger->set_level(spdlog::level::err);
            s_ClientLogger->set_level(spdlog::level::err);
            s_EventLogger->set_level(spdlog::level::err);
            s_GraphicsLogger->set_level(spdlog::level::err);
            break;
        case LogLevel::CRITICAL:
            s_CoreLogger->set_level(spdlog::level::critical);
            s_ClientLogger->set_level(spdlog::level::critical);
            s_EventLogger->set_level(spdlog::level::critical);
            s_GraphicsLogger->set_level(spdlog::level::critical);
            break;
        }
    }
} // namespace Rocket
