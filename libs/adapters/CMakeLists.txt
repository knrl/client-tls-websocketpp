cmake_minimum_required(VERSION 3.14)
project(adapters)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCES})
add_library(libs::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC
        $<INSTALL_INTERFACE:inc>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost REQUIRED)
find_package(Threads)
find_package(PkgConfig REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC
        Boost::boost 
        Threads::Threads)

pkg_search_module(OPENSSL REQUIRED openssl)

if(OPENSSL_FOUND)
        include_directories(${OPENSSL_INCLUDE_DIRS})
        message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
        # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC 
        ${OPENSSL_LIBRARIES}
        libs::utils)
