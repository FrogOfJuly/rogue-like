//
// Created by Kirill Golubev on 17.04.2021.
//

#include <variant>
#include <sstream>
#include "../deps/json/single_include/nlohmann/json.hpp"
#include "../deps/lwlog/lwlog.h"

#define LOG_LEVEL -1

#ifndef ROGUE_LIKE_COMMON_H
#define ROGUE_LIKE_COMMON_H

#define define_has_member(member_name)                                         \
    template <typename T>                                                      \
    class has_member_##member_name                                             \
    {                                                                          \
        typedef char yes_type;                                                 \
        typedef long no_type;                                                  \
        template <typename U> static yes_type test(decltype(&U::member_name)); \
        template <typename U> static no_type  test(...);                       \
    public:                                                                    \
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);  \
    };


#define register_component(cmpt_name) define_has_member(cmpt_name)

#include "utility/register_for_component.h"

#undef register_component


template<class... Ts>
struct overloaded : Ts ... {
    using Ts::operator()...;
};

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


namespace roguelike {

    enum cmd {
        UP = 0, DOWN, LEFT, RIGHT, ENTER, ESC, PASS
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(cmd, {
        { UP, "up" },
        { DOWN, "down" },
        { LEFT, "left" },
        { RIGHT, "right" },
        { PASS, "pass" },
        { ESC, "esc" },
        { ENTER, "enter" },
    })

    using tile_idx = int;

    //type aliasing is not enough, thus they deserve their own structs
    struct entity_id {
        int value = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(entity_id, value);

    struct player_id {
        int value = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(player_id, value);

    using general_id = std::variant<entity_id, player_id>;

    void to_json(nlohmann::json &j, const general_id &p) {
        auto res = std::visit([](auto ent) {
            return ent.value;
        }, p);
        j = nlohmann::json{{"general_id", res}};
    }

    void from_json(const nlohmann::json &j, general_id &p) {
        p = j.at("general_id").get_to(p);
    }

}//namespace roguelike

#endif //ROGUE_LIKE_COMMON_H
