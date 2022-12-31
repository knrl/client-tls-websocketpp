cmake_minimum_required(VERSION 3.14)
project(App)

file(GLOB_RECURSE SOURCES "src/*.cpp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC
        libs::adapters)

target_compile_options(${PROJECT_NAME} PRIVATE 
        -std=c++2a)
target_link_options(${PROJECT_NAME} PRIVATE 
        -pthread -lboost_system -lboost_random -lboost_thread -lssl -lcrypto -ldl)
