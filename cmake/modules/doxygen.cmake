# Doxygen Documentation Module
# This module creates an optional doc_doxygen target

# Option to enable/disable Doxygen documentation generation
option(BUILD_DOC "Build documentation" ON)

# Check if Doxygen is installed and BUILD_DOC is enabled
if(BUILD_DOC)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        # Set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        # Configure the Doxyfile
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        # Create custom target for generating documentation
        add_custom_target(doc_doxygen
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM
        )

        # Exclude from ALL target
        set_target_properties(doc_doxygen PROPERTIES EXCLUDE_FROM_ALL TRUE)

        message(STATUS "Doxygen found. Target 'doc_doxygen' is available.")
    else()
        message(WARNING "Doxygen not found. Documentation cannot be generated.")
    endif()
else()
    message(STATUS "BUILD_DOC is OFF. Doxygen target will not be created.")
endif()