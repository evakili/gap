cmake_minimum_required(VERSION 3.10)

project(gap LANGUAGES CXX VERSION 0.1)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_subdirectory(server)
add_subdirectory(client)
