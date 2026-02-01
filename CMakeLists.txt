cmake_minimum_required(VERSION 3.10)
project(rohan LANGUAGES C DESCRIPTION "AVX2 rasterizer")

add_library(rohan SHARED "rohan.c" "rohan.h")

target_include_directories(rohan PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(rohan PRIVATE ROHAN_BUILD)

set_target_properties(rohan PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED TRUE
    C_EXTENSIONS FALSE
)

target_compile_options(rohan PUBLIC
    -Wall -Wextra -Wpedantic -march=native -mavx2 -mfma
    $<$<CONFIG:Debug>: -g -O0 -fno-inline -fno-omit-frame-pointer>
    $<$<CONFIG:Release>: -O3>
)

find_package(SDL3 REQUIRED)
target_link_libraries(rohan PUBLIC m SDL3::SDL3)
