cmake_minimum_required(VERSION 3.10)
project(rohan LANGUAGES C DESCRIPTION "SIMD rasterizer")

add_executable(rohan)
target_sources(rohan PRIVATE
	"main.c"
    "src/render.c"
    "src/barycentric.c"
)

target_include_directories(rohan PRIVATE "src")

set_target_properties(rohan PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED TRUE
    C_EXTENSIONS FALSE
)

#set(instuctions -msse2 -mno-avx2)
set(instruction_sets -mavx2 -mfma)

target_compile_options(rohan PRIVATE
    -Wall -Wextra -Wpedantic -march=native ${instruction_sets}
    $<$<CONFIG:Debug>: -g -O0>
    $<$<CONFIG:Release>: -O3>
)

find_package(SDL3 REQUIRED)
target_link_libraries(rohan PRIVATE m SDL3::SDL3)

add_subdirectory(test_shader)
