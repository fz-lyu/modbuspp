cmake_minimum_required(VERSION 3.1)
project(modbuspp)
set(CMAKE_CXX_STANDARD 11)

# Create interface library
add_library(${PROJECT_NAME} INTERFACE modbus.h)

# INSTALL INTERFACE LIB
install(FILES
    "modbus.h"
    DESTINATION
    include/modbuspp
)

# BUILD EXAMPLE
set(EXAMPLE_FILES example/example.cpp)
add_executable(modbuspp_example ${EXAMPLE_FILES})

target_compile_options(modbuspp_example PRIVATE -Wall)

if(NOT WIN32)
    # Append "-Werror" if it is not WIN32
    target_compile_options(modbuspp_example PRIVATE -Werror)
endif()

target_compile_definitions(modbuspp_example PUBLIC ENABLE_MODBUSPP_LOGGING)
target_include_directories(modbuspp_example PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
