if(BUILD_TESTING)

    include(gtest)

    # -----------------------------
    # Collect test source files
    # -----------------------------
    file(GLOB_RECURSE TEST_SRC
            "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
            "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    )

    # -----------------------------
    # Test executable
    # -----------------------------
    # added MY_SRC so that MolSImTests can link correctly
    add_executable(MolSimTests ${TEST_SRC})

    # link molsim_core and project warnings
    target_link_libraries(MolSimTests
            PRIVATE molsim_core project_warnings
    )

    target_link_libraries(MolSimTests PRIVATE GTest::gtest_main)


    # GoogleTest automatic discovery of every test that begins with TEST, TEST_F, TEST_P
    include(GoogleTest)
    gtest_discover_tests(MolSimTests)


endif()