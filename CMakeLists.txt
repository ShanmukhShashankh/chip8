set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.12)

project(chip8)

find_package(SDL2 REQUIRED CONFIG)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(chip8_core src/chip8.cc) 

add_executable(chip8 main.cc src/display.cc)

target_link_libraries(chip8 PRIVATE ${SDL2_LIBRARIES} chip8_core) 
