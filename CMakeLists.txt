cmake_minimum_required(VERSION 3.10)

#----------------------
# Project Configuration
#----------------------

# define project name, version (In the future we will automate versioning with github tags)
project(MolSim_GroupE VERSION 0.1.1 LANGUAGES CXX)

# Enable C++17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# let ccmake and cmake-gui offer the default build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# set Release as the default build type if it is not yet set.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#----------------------
# Collect source files
#----------------------
file(GLOB_RECURSE MY_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

#--------------------------
# Define executable target
#--------------------------
add_executable(MolSim ${MY_SRC})

target_include_directories(MolSim
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

#-------------------
# Compiler Warnings
#-------------------
target_compile_options(MolSim
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        # disable some spam warnings for icpc...
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
)

# ------------------------------
# Custom clang-format targets
# ------------------------------
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_CODE_FILES
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    )

    # Check formatting
    add_custom_target(clang_format_check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_CODE_FILES}
            COMMENT "Checking code formatting with clang-format"
            VERBATIM
    )

    # Fix formatting
    add_custom_target(clang_format_fix
            COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CODE_FILES}
            COMMENT "Fixing code formatting with clang-format"
            VERBATIM
    )
endif()

# ------------------------------
# Custom clang-tidy target
# ------------------------------
find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    # Export compile_commands.json for clang-tidy
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    add_custom_target(run_clang_tidy
            COMMAND ${CLANG_TIDY_EXE} -p ${CMAKE_BINARY_DIR} ${MY_SRC}
            COMMENT "Running clang-tidy static analysis"
            VERBATIM
    )
endif()

# ------------------------------
# Include custom CMake modules
# ------------------------------
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

# Optional modules
include(vtk)
include(clang-format)
include(doxygen)
