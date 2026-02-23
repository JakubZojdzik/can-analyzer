cmake_minimum_required(VERSION 3.16)
project(can_analyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SOURCES src/*.cpp)

add_executable(can_analyzer ${SOURCES})

target_include_directories(can_analyzer PRIVATE include)
target_compile_options(can_analyzer PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(can_analyzer PRIVATE ncurses)
