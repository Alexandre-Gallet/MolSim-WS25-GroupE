include(FetchContent)

FetchContent_Declare(
        yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        GIT_TAG        master           # yaml-cpp 0.8.0 is so old that gcc > 13 prohibits some behaviour.
                                        # This is not ideal but the only way to not break configuration.
)

FetchContent_MakeAvailable(yaml-cpp)