# defines which cmake version to use
cmake_minimum_required(VERSION 3.10)

# because of requirement for older version of yaml-cpp
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# project declaration (in future this could be adapted to automatically update versions)
project(MolSim_GroupE VERSION 0.1.1 LANGUAGES CXX)

# pass on path to cmake folder so that all modules, tools and configuration logic .cmake files can be found
list(APPEND CMAKE_MODULE_PATH
        "${CMAKE_SOURCE_DIR}/cmake"
        "${CMAKE_SOURCE_DIR}/cmake/modules"
)

# include all cmake files needed
include(cmake/VersionControl.cmake)
include(ProjectOptions)
include(CompilerWarnings)

# Tooling
include(cmake/tooling/clang-format.cmake)
include(cmake/tooling/clang-tidy.cmake)
include(cmake/tooling/doxygen.cmake)

# see the src/CMakeLists.txt
add_subdirectory(src)

# include CTest
include(CTest)

# see tests/CMakeLists.txt for testing logic
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
