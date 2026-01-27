# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build types exposed to cmake /cmake-gui
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# Set log_level to default "INFO" and make it user configurable via cmake
set(LOG_LEVEL "INFO" CACHE STRING "Minimum log level: TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF")
set_property(CACHE LOG_LEVEL PROPERTY STRINGS TRACE DEBUG INFO WARN ERROR CRITICAL OFF)

# Default build type (just in case the user doesnt define this during building/configuration)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel."
            FORCE)
endif()

# vtk options for output writer
option(ENABLE_VTK_OUTPUT "Enable VTK output" ON)
#option(ENABLE_VTK_BINARY_OUTPUT "Enable binary VTK output" ON)
