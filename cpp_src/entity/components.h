//
// Created by Kirill Golubev on 23.04.2021.
//
#include <optional>
#include <unordered_map>

#include "../common.h"
#include "../strategies/abstract_strategy.h"

#ifndef ROGUE_LIKE_COMPONENTS_H
#define ROGUE_LIKE_COMPONENTS_H

namespace roguelike {
    struct component {
        using component_id = int;
        component_id id = -1;
    };

    //--------------end of component---------------------------------------------

    struct move_component : public component {
        int x = -1, y = -1;
        tile_idx residency = -1;
    };

    //--------------end of move_component----------------------------------------

    struct expirience_components : public component {
        int exp = 0;
        int lvlups = 0;

        template <typename entType>
        inline static std::pair<int, int> get_level(entType *ent) {
            return std::make_pair(-1, -1);
        }

        template <typename entType>
        inline static bool gain_experience(entType *ent, int new_exp) {
            return false;
        }

        template <typename entType>
        inline static void perform_lvlups(entType *ent) {}
    };

    //--------------end of move_component----------------------------------------

    struct atk_component : public component {
        int damage = -1;

        template <typename T>
        static inline int calculate_damage(const T *ent) {
            if constexpr (has_member_simple_inventory_component<T>::value) {
                return ent->a_cpt.damage + ent->s_inv_cpt.get_damage_bonus();
            }
            return ent->a_cpt.damage;
        }
    };

    //--------------end of atk_component-----------------------------------------

    struct dfc_component : public component {
        int damage_reduction = 0;

        template <typename T>
        static inline int calculate_damage_reduction(const T *ent) {
            if constexpr (has_member_simple_inventory_component<T>::value) {
                return ent->dfc_cpt.damage_reduction + ent->s_inv_cpt.get_defence_bonus();
            }
            return ent->dfc_cpt.damage_reduction;
        }
    };

    //--------------end of prot_component-----------------------------------------

    struct prot_component : public component {
        double protection_scale = 1.0;

        template <typename T>
        static inline double calculate_protection_scale(const T *ent) {
            if constexpr (has_member_simple_inventory_component<T>::value) {
                return ent->prt_cpt.protection_scale + ent->s_inv_cpt.get_protection_bonus();
            }
            return ent->prt_cpt.protection_scale;
        }
    };

    //--------------end of dfc_component-----------------------------------------

    struct health_component : public component {
        int max_health = -1;
        int health = -1;

        template <typename T>
        static inline bool is_alive(const T *ent) {
            return ent->h_cpt.health > 0;
        }

        template <typename T>
        static inline int receive_damage(T *ent, int damage) {
            if constexpr (has_member_simple_inventory_component<T>::value) {
                double prot = ent->s_inv_cpt.get_protection_bonus();
                int red = ent->s_inv_cpt.get_defence_bonus();
                damage = std::max(0, (int)((damage - red) * prot));
            }
            ent->h_cpt.health -= damage;
            return damage;
        }
    };

    //--------------end of health_component--------------------------------------

    struct repr_component : public component {
        template <typename T>
        static inline std::string compute_representation(const T *) {
            return "?";
        }
    };

    //--------------end of repr_component----------------------------------------

    struct decision_making_component : public component {
        cmd decision = cmd::PASS;
        int eye_sight = -1;
        bool wait_before_strike = true;
        std::unique_ptr<strategy> idle_strategy;
        std::unique_ptr<strategy> active_strategy;
        std::optional<general_id> charges_into;

        [[nodiscard]] std::pair<int, int> get_velocity() const;
    };

    //--------------end of decision_making_component-----------------------------

    struct logging_component : public component {
        std::stringstream log{};
    };

    //--------------end of logging_component-------------------------------------

    struct name_component : public component {
        std::string name;
    };

    //--------------end of name_component----------------------------------------

    struct simple_inventory_component : public component {
        enum inventory_spot { active = 0, defence, offence, armor };
        std::optional<entity_type> spot;
        std::unordered_map<inventory_spot, entity_type> spots;
        bool locked = false;
        void manage();
        [[nodiscard]] int get_damage_bonus() const;
        [[nodiscard]] int get_defence_bonus() const;
        [[nodiscard]] double get_protection_bonus() const;
        [[nodiscard]] std::optional<entity_type> get_some_item() const;

        static simple_inventory_component get_locked_invetory();
    };

    //--------------end of simple inventory component----------------------------
    struct pickable_component : public component {
        using spot_type = simple_inventory_component::inventory_spot;
        bool picked = false;
        spot_type desired_spot = spot_type::active;
    };

    //--------------end of pickable component------------------------------------

    struct one_time_effect_component : public component {
        std::string msg_on_effect = "?";
        template <typename U, typename T>
        inline bool apply_effect(U *src, T *tgt) {
            return false;
        }
    };

    //--------------end of one time effect component-----------------------------

    struct level_component : public component {
        int lvl = -1;
        [[nodiscard]] int experience_on_kill() const;
    };

}  // namespace roguelike

#endif  // ROGUE_LIKE_COMPONENTS_H
