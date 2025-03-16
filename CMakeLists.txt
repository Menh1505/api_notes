cmake_minimum_required(VERSION 3.10)
project(api_notes)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(api_notes src/main.cpp)
