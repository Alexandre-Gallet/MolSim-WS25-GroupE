# cmake/modules/vtk.cmake

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
