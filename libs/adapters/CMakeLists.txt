cmake_minimum_required(VERSION 3.14)
project(adapters)

find_package(Boost REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp" "inc/*.hpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

find_package(Threads)
target_link_libraries(${PROJECT_NAME} PUBLIC
        Boost::boost 
        Threads::Threads)

find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )
        include_directories(${OPENSSL_INCLUDE_DIRS})
        message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
        # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC 
        ${OPENSSL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PUBLIC
        libs::utils)

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:includes>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/includes>
        ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/libs/adapters/inc)
