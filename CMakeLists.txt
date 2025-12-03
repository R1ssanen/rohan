cmake_minimum_required(VERSION 3.10)
project(rohan LANGUAGES C DESCRIPTION "SIMD rasterize_triangler")

add_executable(rohan)
target_sources(rohan PRIVATE
	"src/main.c"
    "src/frame.c"
    "src/render.c"
    "src/rmath.c"
    "src/simd.c"
)

target_include_directories(rohan PRIVATE "include")

set_target_properties(rohan PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED TRUE
    C_EXTENSIONS FALSE
)

target_compile_options(rohan PRIVATE
    -Wall -Wextra -Wpedantic -march=native
    $<$<CONFIG:Debug>: -g -O0>
    $<$<CONFIG:Release>: -O3 -mavx2>
)

find_package(SDL3 REQUIRED)
target_link_libraries(rohan PRIVATE SDL3::SDL3)
