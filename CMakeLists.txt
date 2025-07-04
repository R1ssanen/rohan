cmake_minimum_required(VERSION 4.0)
project(rohan
	VERSION 2025.2.0
	LANGUAGES CXX
	DESCRIPTION "CPU-only 3D renderer"
)

add_library(rohan SHARED
	"src/plane.cpp"
	"src/barycentric.cpp"
	"src/config.cpp"
	"src/render.cpp"
	"src/state.cpp"
	"src/transform.cpp"
)

set_target_properties(rohan PROPERTIES
	EXPORT_COMPILE_COMMANDS TRUE
	CXX_VISIBILITY_PRESET hidden
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED TRUE
	CXX_EXTENSIONS OFF
)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set (OPTIMIZATIONS "-g")
else()
	set (OPTIMIZATIONS "-O3")
endif()

target_include_directories(rohan PUBLIC "include")
target_compile_options(rohan PRIVATE ${OPTIMIZATIONS} -Wall -Wextra -Wpedantic -march=native)

add_subdirectory("vendor/glm" EXCLUDE_FROM_ALL)
target_link_libraries(rohan PUBLIC glm::glm)

add_subdirectory("testing")
