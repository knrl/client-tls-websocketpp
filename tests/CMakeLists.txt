cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(tests
    ringbuffer_test.cpp
    adapters_test.cpp)

target_compile_options(tests PRIVATE -std=c++2a)

target_link_libraries(tests PRIVATE
    libs::adapters
    GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
