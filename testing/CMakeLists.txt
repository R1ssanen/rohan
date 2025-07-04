cmake_minimum_required(VERSION 4.0)
project(rohan_test)

add_executable(rohan_test "main.cpp")

set_target_properties(rohan_test PROPERTIES
    EXPORT_COMPILE_COMMANDS TRUE
	CXX_VISIBILITY_PRESET hidden
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED TRUE
	CXX_EXTENSIONS OFF
)

target_compile_options(rohan_test PRIVATE ${OPTIMIZATIONS} -Wall -Wextra -Wpedantic -march=native)
add_subdirectory ("vendor/SDL" EXCLUDE_FROM_ALL)
target_link_libraries(rohan_test PRIVATE rohan SDL3::SDL3)

add_custom_command (TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL3::SDL3> ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rohan> ${CMAKE_CURRENT_BINARY_DIR}
)
