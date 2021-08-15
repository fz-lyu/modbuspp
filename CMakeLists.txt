cmake_minimum_required(VERSION 3.1)
project(modbuspp)
set(CMAKE_CXX_STANDARD 11)


if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()

# BUILD INTERFACE LIB
install(FILES
    "modbus.h"
    DESTINATION
    include/modbuspp
)
# BUILD EXAMPLE
set(EXAMPLE_FILES example/example.cpp)
add_executable(modbuspp_example ${EXAMPLE_FILES})
target_compile_definitions(modbuspp_example
PUBLIC
    ENABLE_MODBUSPP_LOGGING
)
target_include_directories(modbuspp_example
PUBLIC
    .
)
