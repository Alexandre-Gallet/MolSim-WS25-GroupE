include(FetchContent)

# spdlog configuration options
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.zip
)

FetchContent_MakeAvailable(spdlog)
