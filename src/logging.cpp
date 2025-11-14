#include "logging.hpp"

#include <memory>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace logging {

    void init_logging() {
        // Console sink (colored output on terminal)
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // File sink: append to "simulation.log" in the working directory
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("simulation.log", true);

        std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

        auto logger = std::make_shared<spdlog::logger>("MolSimLogger", sinks.begin(), sinks.end());

        // Timestamp + level + message
        logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%^%l%$] %v");

        // Set runtime log level based on compile-time SPDLOG_ACTIVE_LEVEL
#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
        logger->set_level(spdlog::level::trace);
#elif SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
        logger->set_level(spdlog::level::debug);
#elif SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO
        logger->set_level(spdlog::level::info);
#elif SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN
        logger->set_level(spdlog::level::warn);
#elif SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR
        logger->set_level(spdlog::level::err);
#elif SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_CRITICAL
        logger->set_level(spdlog::level::critical);
#else
        logger->set_level(spdlog::level::off);
#endif

        // Make this the default logger for SPDLOG_* macros and spdlog::info(), etc.
        spdlog::set_default_logger(logger);

        // Flush logs automatically for INFO and above (so simulation.log doesn't lag)
        spdlog::flush_on(spdlog::level::info);
    }

}  // namespace logging
