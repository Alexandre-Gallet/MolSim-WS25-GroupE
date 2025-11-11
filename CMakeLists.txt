cmake_minimum_required(VERSION 3.14)

# Enable C++17. You may raise the standard if you want
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# define project name, version
project(MolSim_GroupE VERSION 0.0.1 LANGUAGES CXX)

# let ccmake and cmake-gui offer the default build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# set Release as the default build type if it is not yet set.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

# collect all cpp files
file(GLOB_RECURSE MY_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        # header don't need to be included but this might be necessary for some IDEs
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# create make target
add_executable(MolSim ${MY_SRC}
        src/ParticleContainer.cpp
        src/ParticleContainer.h
        src/ParticleGenerator.cpp
        src/ParticleGenerator.h
        src/ParticleGenerator.h
)

target_include_directories(MolSim
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
# Just have to add #include "utils/MaxwellBoltzmannDistribution.h" to make use of the file
)

# activate all compiler warnings. Clean up your code :P
# depending on the compiler different flags are used
target_compile_options(MolSim
        PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        # disable some spam warnings for icpc...
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
)

# add additional modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(vtk)
include(clang-format)
include(doxygen)  # This now loads your complete doxygen module
include(CTest)

if (BUILD_TESTING)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(ParticleContainerTest
        tests/Temporary/Unit/ParticleContainerTest.cpp
)
target_link_libraries(ParticleContainerTest PRIVATE GTest::gtest_main)
target_include_directories(ParticleContainerTest PRIVATE ${PROJECT_SOURCE_DIR}/src)
include(GoogleTest)
gtest_discover_tests(ParticleContainerTest)
endif ()
