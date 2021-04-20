cmake_minimum_required(VERSION 3.4...3.18)
project(roguelike)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps/pybind11)
add_subdirectory(deps/magic_enum)
add_subdirectory(deps/json)
#add_subdirectory(deps/lwlog)

pybind11_add_module(roguelike
        cpp_src/wrapper.cpp
        cpp_src/game_state/gamestate.h
        cpp_src/level/room.hpp
        cpp_src/level/tile.h
        cpp_src/entity/entity.hpp
        cpp_src/entity/player.h
        cpp_src/common.h
        cpp_src/entity/goblin.h
        cpp_src/entity/potion.h
        )

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(roguelike PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
#
add_executable(roguelike_testing
        cpp_src/test.cpp
        )