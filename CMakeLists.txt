cmake_minimum_required(VERSION 3.10)

# ----------------------
# Project Configuration
# ----------------------
project(MolSim_GroupE VERSION 0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Let ccmake and cmake-gui show build types
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
endif()

# -----------------------------
# Collect production source
# -----------------------------
file(GLOB_RECURSE MY_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# ----------------------------------------------------------------------------------------------
# Remove MolSim.cpp from MY_SRC and add it manually so that testing executable only has one main
# ----------------------------------------------------------------------------------------------
list(REMOVE_ITEM MY_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/MolSim.cpp")

# -----------------------------
# Main executable (production)
# -----------------------------
add_executable(MolSim ${MY_SRC}
                        src/MolSim.cpp)

target_include_directories(MolSim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# -------------------
# Compiler warnings
# -------------------
target_compile_options(MolSim
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
)

# --------------------------------
# Custom clang-format
# --------------------------------
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_CODE_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    )

    add_custom_target(clang_format_check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_CODE_FILES}
            COMMENT "Checking code formatting with clang-format"
            VERBATIM)

    add_custom_target(clang_format_fix
            COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CODE_FILES}
            COMMENT "Fixing code formatting with clang-format"
            VERBATIM)
endif()

# --------------------------------
# Custom clang-tidy
# --------------------------------

# Collect only CPP files for clang-tidy
file(GLOB_RECURSE ALL_CPP
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    add_custom_target(run_clang_tidy
            COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} --header-filter=src/ ${ALL_CPP}
            COMMENT "Running clang-tidy static analysis on CPP files only"
            VERBATIM)
endif()

# -----------------------------------
# Load custom modules
# -----------------------------------
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)
include(vtk)
include(clang-format)
include(doxygen)

# -----------------------------
# Logging: spdlog via FetchContent
# -----------------------------
include(FetchContent)

FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/refs/tags/v1.14.1.zip
)

FetchContent_MakeAvailable(spdlog)

# Set log_level to default "INFO" and make it user configurable via cmake
set(LOG_LEVEL "INFO" CACHE STRING "Minimum log level: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF")
set_property(CACHE LOG_LEVEL PROPERTY STRINGS TRACE DEBUG INFO WARN ERROR CRITICAL OFF)

# Link spdlog to main binary
target_link_libraries(MolSim PRIVATE spdlog::spdlog)
target_compile_definitions(MolSim PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_${LOG_LEVEL})


# -----------------------------------
# Testing Support
# -----------------------------------
include(CTest)   # Enables BUILD_TESTING option

if(BUILD_TESTING)

    # -------- GoogleTest (via FetchContent, assignment compliant) ------
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    FetchContent_MakeAvailable(googletest)

    # -----------------------------
    # Collect test source files
    # -----------------------------
    file(GLOB_RECURSE TEST_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
    )

    # -----------------------------
    # Test executable
    # -----------------------------
    # added MY_SRC so that MolSImTests can link correctly
    add_executable(MolSimTests ${TEST_SRC} ${MY_SRC})

    # Link spdlog to test binary after MolSimTests was created
    target_link_libraries(MolSimTests PRIVATE spdlog::spdlog)
    target_compile_definitions(MolSimTests PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_${LOG_LEVEL})

    target_include_directories(MolSimTests PRIVATE ${PROJECT_SOURCE_DIR}/src)

    target_link_libraries(MolSimTests PRIVATE GTest::gtest_main)

    # GoogleTest automatic discovery of every test that begins with TEST, TEST_F, TEST_P
    include(GoogleTest)
    gtest_discover_tests(MolSimTests)

endif()

