set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)

project(chip8)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(chip8 main.cc)
