cmake_minimum_required(VERSION 3.14)
project(utils)

file(GLOB_RECURSE SOURCES "src/*.cpp" "inc/*.hpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<INSTALL_INTERFACE:includes>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/utils/inc)
