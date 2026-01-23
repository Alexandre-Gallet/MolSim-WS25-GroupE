# defines which cmake version to use
cmake_minimum_required(VERSION 3.10)

# project declaration (in future this could be adapted to automatically update versions)
project(MolSim_GroupE VERSION 0.1.1 LANGUAGES CXX)

# pass on path to cmake folder so that all modules, tools and configuration logic .cmake files can be found
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(ProjectOptions)
include(CompilerWarnings)

# Tooling
include(tooling/clang-format.cmake)
include(tooling/clang-tidy.cmake)

add_subdirectory(src) # see the src/CMakeLists.txt

include(modules/doxygen)

# include CTest
include(CTest)

# see tests/CMakeLists.txt for testing logic
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

