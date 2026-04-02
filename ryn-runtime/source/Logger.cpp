module;
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

module Ryntime.Logger;
import std;

namespace {
    std::shared_ptr<spdlog::logger> s_Logger;

    constexpr spdlog::level::level_enum s_LevelLUT[] = {
        spdlog::level::trace,   // Trace = 0
        spdlog::level::info,    // Info = 1
        spdlog::level::warn,    // Warn = 2
        spdlog::level::err,     // Error = 3
        spdlog::level::critical // Critical = 4
    };
}

namespace ryn {
    static spdlog::logger* getLogger() {
        if (s_Logger) [[likely]]
            return s_Logger.get();
        return spdlog::default_logger().get();
    }

    void Logger::init() {
        auto color_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        color_sink->set_pattern("%^[%T.%e][%L] [%s(%#) -> %!] %v%$");
        s_Logger = std::make_shared<spdlog::logger>("RYN", color_sink);
        s_Logger->flush_on(spdlog::level::err);

#if RYN_CONFIG_DEBUG
        s_Logger->set_level(spdlog::level::trace);
#else
        s_Logger->set_level(spdlog::level::info);
#endif
    }

    void Logger::shutdown() {
        s_Logger.reset();
        spdlog::shutdown();
    }

    void Logger::dispatch(const LogMeta& meta, const char* message) {
        spdlog::logger* logger = getLogger();

        const auto level    = s_LevelLUT[static_cast<int>(meta.Level)];
        const auto location = spdlog::source_loc{meta.File, meta.Line, meta.Function};
        logger->log(location, level, message);
    }
}
