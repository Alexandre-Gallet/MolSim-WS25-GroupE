# Interface library which can then be linked onto all targets (MolSim, MolSimTests, Benchmarking targets)
add_library(project_warnings INTERFACE)

target_compile_options(project_warnings INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-Wall>
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
)