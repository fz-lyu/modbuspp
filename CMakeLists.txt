cmake_minimum_required(VERSION 3.1)
project(modbuspp)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES example.cpp)
include_directories(./)

add_executable(modbuspp_example ${SOURCE_FILES})