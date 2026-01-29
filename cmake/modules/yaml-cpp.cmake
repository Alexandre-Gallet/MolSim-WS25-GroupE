include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY ${YAML_CPP_GIT_REPOSITORY}
        GIT_TAG        ${YAMK_CPP_GIT_TAG}
)

FetchContent_MakeAvailable(yaml-cpp)