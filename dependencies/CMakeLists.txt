include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.10.0
)

FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 6.1.2
)

FetchContent_Declare(benchmark
    GIT_REPOSITORY https://github.com/google/benchmark
)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG v1.6.0
)

# Otherwise we get big long LNK2038 errors on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

set(BENCHMARK_ENABLE_TESTING OFF)

FetchContent_MakeAvailable(
    googletest
    benchmark
    fmt
    spdlog
)
