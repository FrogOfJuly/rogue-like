#include "base_includes.h"

#ifndef ROGUELIKE_COMMON_RELEASE_H
#define ROGUELIKE_COMMON_RELEASE_H

namespace roguelike {
# 31 "cpp_src/utility/debug_only/common_debug.h"
}
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

namespace roguelike {

    enum cmd { UP = 0, DOWN, LEFT, RIGHT, ENTER, ESC, PASS };

    using tile_idx = int;


    struct entity_id {
        int value = -1;
    };

    struct player_id {
        int value = -1;
    };

    using general_id = std::variant<entity_id, player_id>;





# 1 "cpp_src/utility/debug_only/register_for_entities.h" 1




struct goblin;
struct goblin_guard;
struct goblin_worker;
struct player;
struct potion;
struct wall;
struct trap;
struct sword;
struct shield;
struct armor;
struct shovel;
struct chest;
# 61 "cpp_src/utility/debug_only/common_debug.h" 2

    struct entity;

    using entity_type = std::variant<


# 1 "cpp_src/utility/debug_only/register_for_entities.h" 1




goblin *,
goblin_guard *,
goblin_worker *,
player *,
potion *,
wall *,
trap *,
sword *,
shield *,
armor *,
shovel *,
chest *,
# 67 "cpp_src/utility/debug_only/common_debug.h" 2

        entity *>;

    using const_entity_type = std::variant<


# 1 "cpp_src/utility/debug_only/register_for_entities.h" 1




const goblin *,
const goblin_guard *,
const goblin_worker *,
const player *,
const potion *,
const wall *,
const trap *,
const sword *,
const shield *,
const armor *,
const shovel *,
const chest *,
# 73 "cpp_src/utility/debug_only/common_debug.h" 2

        const entity *>;



# 1 "cpp_src/utility/debug_only/register_for_components.h" 1




struct move_component;
struct atk_component;
struct dfc_component;
struct health_component;
struct repr_component;
struct decision_making_component;
struct logging_component;
struct name_component;
struct simple_inventory_component;
struct pickable_component;
struct one_time_effect_component;
struct expirience_components;
struct level_component;
struct prot_component;
struct destruction_component;
# 78 "cpp_src/utility/debug_only/common_debug.h" 2


    inline const_entity_type repack(const entity_type &var) {
        return std::visit(
            [](const auto *ent_ptr) {
                const_entity_type cnst_var_ent = ent_ptr;
                return cnst_var_ent;
            },
            var);
    }






# 1 "cpp_src/utility/debug_only/register_for_components.h" 1




template <typename T> concept has_move_component = std::same_as<decltype(T::m_cpt), move_component>; template <typename T> struct has_member_move_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_move_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_move_component_v = has_member_move_component<T>::value;
template <typename T> concept has_atk_component = std::same_as<decltype(T::a_cpt), atk_component>; template <typename T> struct has_member_atk_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_atk_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_atk_component_v = has_member_atk_component<T>::value;
template <typename T> concept has_dfc_component = std::same_as<decltype(T::dfc_cpt), dfc_component>; template <typename T> struct has_member_dfc_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_dfc_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_dfc_component_v = has_member_dfc_component<T>::value;
template <typename T> concept has_health_component = std::same_as<decltype(T::h_cpt), health_component>; template <typename T> struct has_member_health_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_health_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_health_component_v = has_member_health_component<T>::value;
template <typename T> concept has_repr_component = std::same_as<decltype(T::repr_cpt), repr_component>; template <typename T> struct has_member_repr_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_repr_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_repr_component_v = has_member_repr_component<T>::value;
template <typename T> concept has_decision_making_component = std::same_as<decltype(T::dm_cpt), decision_making_component>; template <typename T> struct has_member_decision_making_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_decision_making_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_decision_making_component_v = has_member_decision_making_component<T>::value;
template <typename T> concept has_logging_component = std::same_as<decltype(T::lg_cpt), logging_component>; template <typename T> struct has_member_logging_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_logging_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_logging_component_v = has_member_logging_component<T>::value;
template <typename T> concept has_name_component = std::same_as<decltype(T::nm_cpt), name_component>; template <typename T> struct has_member_name_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_name_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_name_component_v = has_member_name_component<T>::value;
template <typename T> concept has_simple_inventory_component = std::same_as<decltype(T::s_inv_cpt), simple_inventory_component>; template <typename T> struct has_member_simple_inventory_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_simple_inventory_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_simple_inventory_component_v = has_member_simple_inventory_component<T>::value;
template <typename T> concept has_pickable_component = std::same_as<decltype(T::pk_cpt), pickable_component>; template <typename T> struct has_member_pickable_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_pickable_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_pickable_component_v = has_member_pickable_component<T>::value;
template <typename T> concept has_one_time_effect_component = std::same_as<decltype(T::ot_eff_cpt), one_time_effect_component>; template <typename T> struct has_member_one_time_effect_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_one_time_effect_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_one_time_effect_component_v = has_member_one_time_effect_component<T>::value;
template <typename T> concept has_expirience_components = std::same_as<decltype(T::exp_cpt), expirience_components>; template <typename T> struct has_member_expirience_components { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_expirience_components<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_expirience_components_v = has_member_expirience_components<T>::value;
template <typename T> concept has_level_component = std::same_as<decltype(T::lvl_cpt), level_component>; template <typename T> struct has_member_level_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_level_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_level_component_v = has_member_level_component<T>::value;
template <typename T> concept has_prot_component = std::same_as<decltype(T::prt_cpt), prot_component>; template <typename T> struct has_member_prot_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_prot_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_prot_component_v = has_member_prot_component<T>::value;
template <typename T> concept has_destruction_component = std::same_as<decltype(T::dstr_cpt), destruction_component>; template <typename T> struct has_member_destruction_component { template <typename U> inline constexpr static bool f(U *) { return false; } template <typename U> inline constexpr static bool f(U *) requires has_destruction_component<U> { return true; } public: inline static constexpr bool value = f((T *)(nullptr)); }; template <typename T> inline constexpr bool has_destruction_component_v = has_member_destruction_component<T>::value;
# 94 "cpp_src/utility/debug_only/common_debug.h" 2




}

#endif
