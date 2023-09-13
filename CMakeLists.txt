cmake_minimum_required(VERSION 3.1...3.26)
project(
    gravity
    VERSION 0.1
    LANGUAGES CXX)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(gravity src/main.cpp src/gravity.cpp)
target_link_libraries(gravity ${SDL2_LIBRARIES})
