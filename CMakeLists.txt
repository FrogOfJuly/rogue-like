cmake_minimum_required(VERSION 3.14)
project(roguelike)

include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent.git
        GIT_TAG v3.9.1)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11
        GIT_TAG        v2.6.2
)

FetchContent_GetProperties(pybind11)
if(NOT pybind11_POPULATED)
    FetchContent_Populate(pybind11)
    add_subdirectory(${pybind11_SOURCE_DIR} ${pybind11_BINARY_DIR})
endif()

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS "-fconcepts-diagnostics-depth=3")

add_subdirectory(deps/magic_enum)
#add_subdirectory(deps/lwlog)

set(LIB_SOURCES
        cpp_src/game_state/gamestate.h
        cpp_src/level/room.h
        cpp_src/level/tile.h
        cpp_src/level/tile.cpp
        cpp_src/common.h
        cpp_src/game_state/gamestate.cpp
        cpp_src/level/room.cpp
        cpp_src/level/room_view.cpp
        cpp_src/level/room_view.h

        cpp_src/entity/components.cpp
        cpp_src/entity/components.h

        cpp_src/entity/entity.hpp
        cpp_src/entity/entity.cpp
        cpp_src/entity/player.h
        cpp_src/entity/goblin.h
        cpp_src/entity/goblin_guard.h
        cpp_src/entity/potion.h
        cpp_src/entity/wall.h
        cpp_src/entity/trap.h
        cpp_src/entity/chest.h
        cpp_src/entity/sword.h
        cpp_src/entity/shield.h
        cpp_src/entity/armor.h
        cpp_src/entity/shovel.h

        cpp_src/game_state/systems/base_system.h
        cpp_src/game_state/systems/interaction_system.h
        cpp_src/game_state/systems/move_system.cpp
        cpp_src/game_state/systems/move_system.h
        cpp_src/game_state/systems/interaction_system.cpp
        cpp_src/game_state/systems/decision_making_system.h
        cpp_src/game_state/systems/decision_making_system.cpp

        cpp_src/serializers/non_default_serialize.h
        cpp_src/serializers/non_default_serialize.cpp
        cpp_src/serializers/default_serializers.h

        cpp_src/utility/utils.cpp
        cpp_src/utility/utils.h

        cpp_src/strategies/abstract_strategy.h
        cpp_src/strategies/abstract_strategy.cpp
        cpp_src/strategies/random_strategy/random_strategy.cpp
        cpp_src/strategies/random_strategy/random_strategy.h
        cpp_src/strategies/passive_strategy/passive_strategy.cpp
        cpp_src/strategies/passive_strategy/passive_strategy.h
        cpp_src/strategies/agressive_strategy/agressive_strategy.cpp
        cpp_src/strategies/agressive_strategy/agressive_strategy.h
        cpp_src/strategies/fearfull_strategy/fearfull_strategy.cpp
        cpp_src/strategies/fearfull_strategy/fearfull_strategy.h
        )

pybind11_add_module(roguelike
        cpp_src/wrapper.cpp
        ${LIB_SOURCES}
        )

target_link_libraries(roguelike PRIVATE nlohmann_json::nlohmann_json)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(roguelike PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
#
add_executable(roguelike_testing
        ${LIB_SOURCES}
        cpp_src/test.cpp
        )
target_link_libraries(roguelike_testing PRIVATE nlohmann_json::nlohmann_json)