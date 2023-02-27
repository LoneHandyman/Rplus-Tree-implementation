cmake_minimum_required(VERSION 3.15)

project(RplusTree)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-g)

file(GLOB SOURCES
    *.cpp
    source/*.cpp
)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCES})