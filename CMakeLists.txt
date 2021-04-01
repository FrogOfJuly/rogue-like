cmake_minimum_required(VERSION 3.4...3.18)
project(rogue_like)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps/pybind11)
add_subdirectory(deps/magic_enum)
pybind11_add_module(rogue_like
        cpp_src/wrapper.cpp
        cpp_src/controller/controller.cpp
        cpp_src/controller/controller.h
        )

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(rogue_like PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
