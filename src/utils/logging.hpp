#pragma once

#include <spdlog/spdlog.h>

/**
 * @brief Logging initialization utilities.
 *
 * Call logging::init_logging() once at program startup (before doing any work).
 * This sets up:
 *  - console logger
 *  - file logger ("simulation.log")
 *  - timestamps
 *  - log level derived from SPDLOG_ACTIVE_LEVEL (i.e., from CMake LOG_LEVEL)
 */
namespace logging {

    /**
     * @brief Initialize the global logger (console + file).
     *
     * Safe to call exactly once at program start (e.g. in main()).
     */
    void init_logging();

}  // namespace logging
