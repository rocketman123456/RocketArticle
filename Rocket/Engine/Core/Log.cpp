#include "Core/Log.h"

namespace Rocket
{
    std::shared_ptr<spdlog::logger> Log::s_empty_logger_;
    std::shared_ptr<spdlog::logger> Log::s_core_logger_;
    std::shared_ptr<spdlog::logger> Log::s_client_logger_;
    std::shared_ptr<spdlog::logger> Log::s_event_logger_;
    std::shared_ptr<spdlog::logger> Log::s_graphics_logger_;

    void Log::Init(LogLevel level)
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        s_empty_logger_ = spdlog::stdout_color_mt("Empty");
        s_core_logger_ = spdlog::stdout_color_mt("Rocket");
        s_client_logger_ = spdlog::stdout_color_mt("App");
        s_event_logger_ = spdlog::stdout_color_mt("Event");
        s_graphics_logger_ = spdlog::stdout_color_mt("Graphics");

        switch(level)
        {
        case LogLevel::TRACE:
            s_core_logger_->set_level(spdlog::level::trace);
            s_client_logger_->set_level(spdlog::level::trace);
            s_event_logger_->set_level(spdlog::level::trace);
            s_graphics_logger_->set_level(spdlog::level::trace);
            break;
        case LogLevel::INFO:
            s_core_logger_->set_level(spdlog::level::info);
            s_client_logger_->set_level(spdlog::level::info);
            s_event_logger_->set_level(spdlog::level::info);
            s_graphics_logger_->set_level(spdlog::level::info);
            break;
        case LogLevel::WARN:
            s_core_logger_->set_level(spdlog::level::warn);
            s_client_logger_->set_level(spdlog::level::warn);
            s_event_logger_->set_level(spdlog::level::warn);
            s_graphics_logger_->set_level(spdlog::level::warn);
            break;
        case LogLevel::ERR:
            s_core_logger_->set_level(spdlog::level::err);
            s_client_logger_->set_level(spdlog::level::err);
            s_event_logger_->set_level(spdlog::level::err);
            s_graphics_logger_->set_level(spdlog::level::err);
            break;
        case LogLevel::CRITICAL:
            s_core_logger_->set_level(spdlog::level::critical);
            s_client_logger_->set_level(spdlog::level::critical);
            s_event_logger_->set_level(spdlog::level::critical);
            s_graphics_logger_->set_level(spdlog::level::critical);
            break;
        }
    }
} // namespace Rocket
