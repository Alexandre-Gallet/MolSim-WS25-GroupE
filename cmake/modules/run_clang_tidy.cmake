# Convert semicolon-separated list to space-separated
string(REPLACE ";" " " FILES_SPACED "${ALL_CPP}")

foreach(f ${ALL_CPP})
    execute_process(
            COMMAND ${CLANG_TIDY_EXE} --quiet -p ${CMAKE_BINARY_DIR} --header-filter=src/ ${f}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND_ECHO STDOUT
    )
endforeach()
