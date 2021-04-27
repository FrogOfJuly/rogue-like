cmake_minimum_required(VERSION 3.4...3.18)
project(roguelike)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(deps/pybind11)
add_subdirectory(deps/magic_enum)
add_subdirectory(deps/json)
#add_subdirectory(deps/lwlog)

set(LIB_SOURCES
        cpp_src/game_state/gamestate.h
        cpp_src/level/room.h
        cpp_src/level/tile.h
        cpp_src/entity/entity.hpp
        cpp_src/entity/player.h
        cpp_src/common.h
        cpp_src/entity/goblin.h
        cpp_src/entity/potion.h
        cpp_src/entity/wall.h
        cpp_src/game_state/systems/base_system.h
        cpp_src/game_state/systems/drawing_system.h
        cpp_src/game_state/systems/interaction_system.h
        cpp_src/game_state/systems/move_system.cpp
        cpp_src/game_state/systems/move_system.h
        cpp_src/game_state/gamestate.cpp
        cpp_src/level/room.cpp
        cpp_src/game_state/systems/drawing_system.cpp
        cpp_src/game_state/systems/interaction_system.cpp
        cpp_src/game_state/systems/decision_making_system.hpp
        cpp_src/serializers/non_default_serialize.h
        cpp_src/serializers/non_default_serialize.cpp
        cpp_src/serializers/default_serializers.h
        )

pybind11_add_module(roguelike
        cpp_src/wrapper.cpp
        ${LIB_SOURCES}
        )

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(roguelike PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
#
add_executable(roguelike_testing
        ${LIB_SOURCES}
        cpp_src/test.cpp
        )