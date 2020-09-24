cmake_minimum_required(VERSION 3.16)
project(ntf)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_executable(ntf main.cpp Screen.cpp Screen.h colors.h Lines.cpp Lines.h)
