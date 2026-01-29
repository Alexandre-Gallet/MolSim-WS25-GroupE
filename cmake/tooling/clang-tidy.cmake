find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)

    # Collect only CPP files from src and tests for clang-tidy
    file(GLOB_RECURSE ALL_CPP
            "${CMAKE_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_SOURCE_DIR}/tests/*.cpp"
    )

    # this together with the limited scope to cpp files avoids the "noise explosion" when invoking clang-tidy
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Run clang-tidy per translation unit with header/STL noise suppressed
    add_custom_target(run_clang_tidy
            COMMAND ${CMAKE_COMMAND}
            -DALL_CPP=${ALL_CPP}
            -DCLANG_TIDY_EXE=${CLANG_TIDY_EXE}
            -DCMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}
            -DCMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}
            -P ${CMAKE_SOURCE_DIR}/cmake/tooling/quiet-clang-tidy-run.cmake
            COMMENT "Running clang-tidy on all source files"
            VERBATIM)
endif()