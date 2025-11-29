set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)

project(chip8)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(chip8_core src/chip8.cc) 

add_executable(chip8 main.cc)

target_link_libraries(chip8 PRIVATE chip8_core) 
