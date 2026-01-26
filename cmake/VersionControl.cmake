# Centralized third-party dependency versions and sources
# yaml-cpp
set(YAML_CPP_GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git)
set(YAML_CPP_GIT_TAG 89ff142b991af432b5d7a7cee55282f082a7e629) # yaml-cpp <= 0.8.0 is incompatible with GCC >= 13 due to outdated language usage. We are using the latest commit hash.

# spdlog
set(SPDLOG_VERSION 1.14.1)
set(SPDLOG_URL
        https://github.com/gabime/spdlog/archive/refs/tags/v${SPDLOG_VERSION}.zip
)

# gtest
set(GTEST_VERSION 1.15.2)
set(GTEST_URL
        https://github.com/google/googletest/archive/refs/tags/v${GTEST_VERSION}.zip
)

# vtk
set(VTK_MIN_VERSION 8.9)