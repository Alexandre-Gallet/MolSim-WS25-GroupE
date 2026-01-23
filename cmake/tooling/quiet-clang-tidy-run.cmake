# loops over each file individually. This is the correct choice! It avoids the explosion in warnings and therefore keeps the signal to noise ratio
# of clang-tidy output reasonable. This snippet actually calls the clang-tidy
foreach(f ${ALL_CPP})
    execute_process(
            COMMAND ${CLANG_TIDY_EXE}
            --quiet
            -p ${CMAKE_BINARY_DIR}
            --header-filter=^${CMAKE_SOURCE_DIR}/src
            --system-headers=false
            --extra-arg=-Wno-everything
            ${f}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND_ECHO STDOUT
    )
endforeach()
