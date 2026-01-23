# include only those modules that we will link molsim_core against
include(modules/spdlog)
include(modules/yaml-cpp)

# Collect all simulation sources except main
file(GLOB_RECURSE MOLSIM_SRC CONFIGURE_DEPENDS
        *.cpp
        *.h
)

list(REMOVE_ITEM MOLSIM_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/MolSim.cpp
)

# Core simulation library. This makes molsim_core a global target meaning other executables like the test executable can link against the global target
# Crucially the core simulation logic is defined only once and is authoritative
add_library(molsim_core ${MOLSIM_SRC})

# needed so that the compiler can understand include paths in src and knows where to locate file
target_include_directories(molsim_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# include the vtk cmake module which links against molsim_core
include(modules/vtk)

# link spdlog and yaml-cpp against molsim_core library.
target_link_libraries(molsim_core PUBLIC spdlog::spdlog)
target_link_libraries(molsim_core PRIVATE yaml-cpp::yaml-cpp)

# set LOG_LEVEL as globally accessible in molsim_core target
target_compile_definitions(molsim_core PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_${LOG_LEVEL})


# Production executable linked against molsim_core which contains all the logic
add_executable(MolSim MolSim.cpp)

# link molsim_core, compiler warnings to MolSim target
target_link_libraries(MolSim
        PRIVATE molsim_core project_warnings
)