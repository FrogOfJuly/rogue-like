//
// Created by Kirill Golubev on 13.05.2021.
//

#include "../base_includes.h"

#ifndef ROGUELIKE_COMMON_DEBUG_H
#define ROGUELIKE_COMMON_DEBUG_H
namespace roguelike {

#define define_has_member_with_concepts(member_name, member_type)                    \
    template <typename T>                                                            \
    concept has_##member_type = std::same_as<decltype(T::member_name), member_type>; \
    template <typename T>                                                            \
    struct has_member_##member_type {                                                \
        template <typename U>                                                        \
        inline constexpr static bool f(U *) {                                        \
            return false;                                                            \
        }                                                                            \
        template <typename U>                                                        \
        inline constexpr static bool f(U *) requires has_##member_type<U> {          \
            return true;                                                             \
        }                                                                            \
                                                                                     \
      public:                                                                        \
        inline static constexpr bool value = f((T *)(nullptr));                      \
    };                                                                               \
    template <typename T>                                                            \
    inline constexpr bool has_##member_type##_v = has_member_##member_type<T>::value;

}  // namespace roguelike
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace roguelike {

    enum cmd { UP = 0, DOWN, LEFT, RIGHT, ENTER, ESC, PASS };

    using tile_idx = int;

    // type aliasing is not enough, thus they deserve their own types
    struct entity_id {
        int value = -1;
    };

    struct player_id {
        int value = -1;
    };

    using general_id = std::variant<entity_id, player_id>;

    //--------------forward declarations---------------------

#define register_entity(entity_type_name) struct entity_type_name;
#include "register_for_entities.h"
#undef register_entity
    struct entity;

    using entity_type = std::variant<
#define register_entity(entity_type_name) entity_type_name *,
#include "register_for_entities.h"
#undef register_entity
        entity *>;

    using const_entity_type = std::variant<
#define register_entity(entity_type_name) const entity_type_name *,
#include "register_for_entities.h"
#undef register_entity
        const entity *>;

#define register_component(cpt_name, component_type) struct component_type;
#include "register_for_components.h"
#undef register_component

    inline const_entity_type repack(const entity_type &var) {
        return std::visit(
            [](const auto *ent_ptr) {
                const_entity_type cnst_var_ent = ent_ptr;
                return cnst_var_ent;
            },
            var);
    }
    //--------------end of forward declarations---------------------

    //--------------concepts----------------------------------------

#define register_component(cmpt_name, component_type) define_has_member_with_concepts(cmpt_name, component_type)
#include "register_for_components.h"
#undef register_component

    //--------------end of concepts----------------------------------------

}  // namespace roguelike

#endif  // ROGUELIKE_COMMON_DEBUG_H
