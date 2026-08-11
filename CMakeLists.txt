cmake_minimum_required(VERSION 3.16)
project(itch_parser CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(itch_parser src/main.cpp src/parser.cpp)
add_executable(extract_fixture tools/extract_fixture.cpp)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(parser_test test/parser_test.cpp src/parser.cpp)
target_include_directories(parser_test PRIVATE src)
target_link_libraries(parser_test GTest::gtest_main)
add_test(NAME parser_test COMMAND parser_test WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.8.3
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(benchmark)

add_executable(parser_bench bench/parser_bench.cpp src/parser.cpp)
target_include_directories(parser_bench PRIVATE src)
target_link_libraries(parser_bench benchmark::benchmark benchmark::benchmark_main)
