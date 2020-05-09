cmake_minimum_required(VERSION 3.16)
project(card-game)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
include_directories(/usr/local/Cellar/boost/1.71.0/include/)
link_directories(/usr/local/Cellar/boost/1.71.0/lib/)

file(GLOB protocols
        "${CMAKE_SOURCE_DIR}/include/protocol/*.h"
        "${CMAKE_SOURCE_DIR}/src/protocol/*.cpp")

file(GLOB apps
        "${CMAKE_SOURCE_DIR}/include/app/*.h"
        "${CMAKE_SOURCE_DIR}/src/app/*.cpp")

add_executable(card-game ${apps} src/main.cpp)

target_link_libraries(card-game ${CMAKE_SOURCE_DIR}/lib/libncursesw.6.dylib)