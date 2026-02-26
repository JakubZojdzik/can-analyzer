cmake_minimum_required(VERSION 3.16)
project(can_tools LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB COMMON_SOURCES src/common/*.cpp)

add_library(can_common STATIC ${COMMON_SOURCES})
target_include_directories(can_common PUBLIC include)
target_compile_options(can_common PRIVATE -Wall -Wextra -Wpedantic)

file(GLOB ANALYZER_SOURCES src/analyzer/*.cpp)

add_executable(can_analyzer ${ANALYZER_SOURCES})
target_compile_options(can_analyzer PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(can_analyzer PRIVATE can_common ncurses)

file(GLOB SENDER_SOURCES src/sender/*.cpp)

add_executable(can_sender ${SENDER_SOURCES})
target_compile_options(can_sender PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(can_sender PRIVATE can_common)
