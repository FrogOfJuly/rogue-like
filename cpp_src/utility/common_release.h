
#include "base_includes.h"

#ifndef ROGUELIKE_COMMON_RELEASE_H
#define ROGUELIKE_COMMON_RELEASE_H

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

    struct entity;

    using entity_type = std::variant<

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

        entity *>;

    using const_entity_type = std::variant<

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

        const entity *>;

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

    template <typename T>
    class has_member_move_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<move_component, decltype(U::m_cpt)>, yes_type> test(
            decltype(&U::m_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_atk_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<atk_component, decltype(U::a_cpt)>, yes_type> test(
            decltype(&U::a_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_dfc_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<dfc_component, decltype(U::dfc_cpt)>, yes_type> test(
            decltype(&U::dfc_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_health_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<health_component, decltype(U::h_cpt)>, yes_type> test(
            decltype(&U::h_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_repr_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<repr_component, decltype(U::repr_cpt)>, yes_type> test(
            decltype(&U::repr_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_decision_making_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<decision_making_component, decltype(U::dm_cpt)>, yes_type> test(
            decltype(&U::dm_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_logging_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<logging_component, decltype(U::lg_cpt)>, yes_type> test(
            decltype(&U::lg_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_name_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<name_component, decltype(U::nm_cpt)>, yes_type> test(
            decltype(&U::nm_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_simple_inventory_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<simple_inventory_component, decltype(U::s_inv_cpt)>, yes_type>
        test(decltype(&U::s_inv_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_pickable_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<pickable_component, decltype(U::pk_cpt)>, yes_type> test(
            decltype(&U::pk_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_one_time_effect_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<one_time_effect_component, decltype(U::ot_eff_cpt)>, yes_type>
        test(decltype(&U::ot_eff_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_expirience_components {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<expirience_components, decltype(U::exp_cpt)>, yes_type> test(
            decltype(&U::exp_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_level_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<level_component, decltype(U::lvl_cpt)>, yes_type> test(
            decltype(&U::lvl_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;
    template <typename T>
    class has_member_prot_component {
        typedef char yes_type;
        typedef long no_type;
        template <typename U>
        static typename std::enable_if_t<std::is_same_v<prot_component, decltype(U::prt_cpt)>, yes_type> test(
            decltype(&U::prt_cpt));
        template <typename U>
        static no_type test(...);

      public:
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(yes_type);
    };
    ;

    template <typename T>
    concept has_move_component = has_member_move_component<T>::value;
    template <typename T>
    concept has_atk_component = has_member_atk_component<T>::value;
    template <typename T>
    concept has_dfc_component = has_member_dfc_component<T>::value;
    template <typename T>
    concept has_health_component = has_member_health_component<T>::value;
    template <typename T>
    concept has_repr_component = has_member_repr_component<T>::value;
    template <typename T>
    concept has_decision_making_component = has_member_decision_making_component<T>::value;
    template <typename T>
    concept has_logging_component = has_member_logging_component<T>::value;
    template <typename T>
    concept has_name_component = has_member_name_component<T>::value;
    template <typename T>
    concept has_simple_inventory_component = has_member_simple_inventory_component<T>::value;
    template <typename T>
    concept has_pickable_component = has_member_pickable_component<T>::value;
    template <typename T>
    concept has_one_time_effect_component = has_member_one_time_effect_component<T>::value;
    template <typename T>
    concept has_expirience_components = has_member_expirience_components<T>::value;
    template <typename T>
    concept has_level_component = has_member_level_component<T>::value;
    template <typename T>
    concept has_prot_component = has_member_prot_component<T>::value;

}  // namespace roguelike

#endif
