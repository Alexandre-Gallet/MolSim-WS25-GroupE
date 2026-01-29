include(FetchContent)

FetchContent_Declare(
        googletest
        URL ${GTEST_URL}
)

FetchContent_MakeAvailable(googletest)
