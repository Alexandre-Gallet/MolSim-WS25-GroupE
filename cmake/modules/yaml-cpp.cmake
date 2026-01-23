include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG        0.8.0            # Pinned to stable release to avoid unexpecting breaking because of new commits to master branch
)

FetchContent_MakeAvailable(yaml-cpp)