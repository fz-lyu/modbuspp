cmake_minimum_required(VERSION 3.1)
project(modbuspp VERSION 0.3.1)
set(CMAKE_CXX_STANDARD 11)


if(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()

# BUILD INTERFACE LIB
add_library(modbuspp_lib INTERFACE)
target_include_directories(modbuspp_lib INTERFACE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/modbus.h>
		$<INSTALL_INTERFACE:include>
		)

include(CMakePackageConfigHelpers)
install(TARGETS modbuspp_lib
		EXPORT modbusppTargets
		ARCHIVE DESTINATION lib
		LIBRARY DESTINATION lib
		RUNTIME DESTINATION bin
		PUBLIC_HEADER DESTINATION include )

write_basic_package_version_file("modbusppConfigVersion.cmake"
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY SameMajorVersion)

configure_package_config_file(
		"${PROJECT_SOURCE_DIR}/cmake/modbusppConfig.cmake.in"
		"${PROJECT_BINARY_DIR}/modbusppConfig.cmake"
		INSTALL_DESTINATION lib/cmake/modbuspp
)
install(EXPORT modbusppTargets DESTINATION lib/cmake/modbuspp)
install(FILES "${PROJECT_BINARY_DIR}/modbusppConfigVersion.cmake"
		"${PROJECT_BINARY_DIR}/modbusppConfig.cmake"
		DESTINATION lib/cmake/modbuspp)
install(FILES ${PROJECT_SOURCE_DIR}/modbus.h DESTINATION include)

# BUILD EXAMPLE
IF(BUILD_EXAMPLE)
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
ENDIF()
