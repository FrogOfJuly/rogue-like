//
// Created by Kirill Golubev on 17.04.2021.
//
#pragma once

#include <iostream>
#include <sstream>
#include <variant>
#include <memory>
#include <cassert>

#define LOG_LEVEL -1

#include "../deps/lwlog/lwlog.h"

#ifndef ROGUE_LIKE_COMMON_H
#define ROGUE_LIKE_COMMON_H

#define define_has_member(member_name, member_type)                                                             \
    template <typename T>                                                                                       \
    class has_member_##member_type {                                                                            \
        typedef char yes_type;                                                                                  \
        typedef long no_type;                                                                                   \
        template <typename U>                                                                                   \
        static typename std::enable_if_t<std::is_same_v<member_type, decltype(U::member_name)>, yes_type> test( \
            decltype(&U::member_name));                                                                         \
        template <typename U>                                                                                   \
        static no_type test(...);                                                                               \
                                                                                                                \
      public:                                                                                                   \
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);                                   \
    };

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace roguelike {

    enum cmd { UP = 0, DOWN, LEFT, RIGHT, ENTER, ESC, PASS };

    using tile_idx = int;

    // type aliasing is not enough, thus they deserve their own structs
    struct entity_id {
        int value = -1;
    };

    struct player_id {
        int value = -1;
    };

    using general_id = std::variant<entity_id, player_id>;

    //--------------enumerations----------------------

    enum entity_names {
#define register_entity(entity_type_name) entity_type_name##_entity,
#include "utility/register_for_entities.h"
#undef register_entity
    };

    //--------------end of enumerations----------------------
    //--------------forward declarations---------------------

#define register_entity(entity_type_name) struct entity_type_name;
#include "utility/register_for_entities.h"
#undef register_entity
    struct entity;

    using entity_type = std::variant<
#define register_entity(entity_type_name) entity_type_name *,
#include "utility/register_for_entities.h"
#undef register_entity
        entity *>;

#define register_component(cpt_name, component_type) struct component_type;
#include "utility/register_for_components.h"
#undef register_component
    //--------------end of forward declarations---------------------

#define register_component(cmpt_name, component_type) define_has_member(cmpt_name, component_type);
#include "utility/register_for_components.h"
#undef register_component

    define_has_member(id, entity_id);

}  // namespace roguelike

#endif  // ROGUE_LIKE_COMMON_H
