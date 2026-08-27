cmake_minimum_required(VERSION 3.16)
project(rtiow CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings and optimizations
if (MSVC)
    add_compile_options(/W4 /O2)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3 -pthread)
endif()

include_directories(src)
include_directories(SYSTEM third_party)

# Main ray tracer executable
add_executable(raytracer src/main.cc src/image_writer.cc)
if (UNIX)
    target_link_libraries(raytracer PRIVATE pthread)
endif()

# Unit tests executable
add_executable(unit_tests
    tests/test_main.cc
    tests/test_vec3.cc
    tests/test_sphere.cc
    tests/test_material.cc
    tests/test_aabb.cc
)
if (UNIX)
    target_link_libraries(unit_tests PRIVATE pthread)
endif()

# Pixel diff verification utility
add_executable(pixel_diff tools/pixel_diff.cc)

# CTest integration
enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
