//
// Created by Kirill Golubev on 23.04.2021.
//
#include <optional>

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

    struct level_component : public component {
        int exp = 0;
        template <typename entType>
        inline std::pair<int, int> get_level(entType *ent) {
            int cur_level = exp / 10;
            int next_level = (cur_level + 1) * 10;
            return std::make_pair(cur_level, next_level);
        }
    };

    //--------------end of move_component----------------------------------------

    struct atk_component : public component {
        int damage = -1;

        template <typename T>
        static inline int calculate_damage(const T *ent) {
            return ent->a_cpt.damage;
        }
    };

    //--------------end of atk_component-----------------------------------------

    struct health_component : public component {
        int max_health = -1;
        int health = -1;

        template <typename T>
        static inline bool is_alive(const T *ent) {
            return ent->h_cpt.health > 0;
        }

        template <typename T>
        static inline void receive_damage(T *ent, int damage) {
            ent->h_cpt.health -= damage;
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
        std::unique_ptr<strategy> idle_strategy;
        std::unique_ptr<strategy> active_strategy;
        std::optional<general_id> charges_into;

        [[nodiscard]] std::pair<int, int> get_velocity() const {
            switch (decision) {
                case UP:
                    return std::make_pair(0, -1);
                case DOWN:
                    return std::make_pair(0, 1);
                case LEFT:
                    return std::make_pair(-1, 0);
                case RIGHT:
                    return std::make_pair(1, 0);
                case ENTER:
                case ESC:
                case PASS:
                    return std::make_pair(0, 0);
            }
        }
    };

    //--------------end of decision_making_component-----------------------------

    struct logging_component : public component {
        std::stringstream log{};
    };

    //--------------end of logging_component-------------------------------------

    struct name_component : public component {
        std::string name = "";
    };

    //--------------end of name_component----------------------------------------

    struct simple_inventory_component : public component {
        std::optional<entity_type> spot;
    };

    //--------------end of simple inventory component----------------------------
    struct pickable_component : public component {
        bool picked = false;
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

}  // namespace roguelike

#endif  // ROGUE_LIKE_COMPONENTS_H
