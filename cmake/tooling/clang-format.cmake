# essentially executes a $ which clang-format
find_program(CLANG_FORMAT_EXE NAMES clang-format)

# if a valid path is found proceed
if(CLANG_FORMAT_EXE)
    # gather all files for fomating
    file(GLOB_RECURSE ALL_CODE_FILES
            "${CMAKE_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_SOURCE_DIR}/src/*.h"
            "${CMAKE_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_SOURCE_DIR}/tests/*.h"
    )

    # custom target for checking the format which has can be called manually
    add_custom_target(clang_format_check
            COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_CODE_FILES}
            COMMENT "Checking code formatting with clang-format"
            VERBATIM)

    # custom target for formating the code according to the rules provided by .clang-format
    add_custom_target(clang_format_fix
            COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CODE_FILES}
            COMMENT "Fixing code formatting with clang-format"
            VERBATIM)
endif()
