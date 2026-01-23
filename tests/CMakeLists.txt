if(BUILD_TESTING)

    include(gtest)

    # -----------------------------
    # Collect test source files
    # -----------------------------
    file(GLOB_RECURSE TEST_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
    )

    # -----------------------------
    # Test executable
    # -----------------------------
    # added MY_SRC so that MolSImTests can link correctly
    add_executable(MolSimTests ${TEST_SRC} ${MY_SRC})

    # Link spdlog to test binary after MolSimTests was created
    target_link_libraries(MolSimTests PRIVATE spdlog::spdlog)
    target_compile_definitions(MolSimTests PRIVATE SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_${LOG_LEVEL})

    target_include_directories(MolSimTests PRIVATE ${PROJECT_SOURCE_DIR}/src)

    target_link_libraries(MolSimTests PRIVATE GTest::gtest_main)

    # Link yaml-cpp to testing executable
    target_link_libraries(MolSimTests PRIVATE yaml-cpp)

    # GoogleTest automatic discovery of every test that begins with TEST, TEST_F, TEST_P
    include(GoogleTest)
    gtest_discover_tests(MolSimTests)


endif()