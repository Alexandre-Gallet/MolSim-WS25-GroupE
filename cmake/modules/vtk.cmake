# Use an interface so that we can link vtk individually against targets.
# Remove the legacy code that is no longer needed in vtk version > 7
if(NOT ENABLE_VTK_OUTPUT)
    return()
endif()

message(STATUS "VTK output enabled")

find_package(VTK REQUIRED COMPONENTS
        CommonCore
        CommonDataModel
        IOXML
)

if(VTK_VERSION VERSION_LESS ${VTK_MIN_VERSION})
    message(FATAL_ERROR
            "VTK ${VTK_VERSION} found, but MolSim requires at least ${VTK_MIN_VERSION}"
    )
endif()

message(STATUS "Found VTK Version: ${VTK_VERSION}")

add_library(molsim_vtk INTERFACE)

target_link_libraries(molsim_vtk INTERFACE ${VTK_LIBRARIES})
target_include_directories(molsim_vtk INTERFACE ${VTK_INCLUDE_DIRS})
target_compile_definitions(molsim_vtk INTERFACE ENABLE_VTK_OUTPUT)
