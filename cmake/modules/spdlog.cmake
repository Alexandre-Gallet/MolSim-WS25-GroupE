include(FetchContent)

# spdlog configuration options
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        spdlog
        URL ${SPDLOG_URL}
)

FetchContent_MakeAvailable(spdlog)
