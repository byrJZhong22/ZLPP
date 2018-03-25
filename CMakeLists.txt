cmake_minimum_required(VERSION 3.5)
project(ZLPP)

set(CMAKE_CXX_STANDARD 11)
include_directories(
        "${PROJECT_SOURCE_DIR}"
        "${PROJECT_BINARY_DIR}"
)

find_package(Boost REQUIRED)

add_subdirectory(src/base)
add_subdirectory(src/containers)
add_subdirectory(src/net)

#set(SOURCE_FILES library.cpp library.h)
#add_library(ZLPP SHARED ${SOURCE_FILES})