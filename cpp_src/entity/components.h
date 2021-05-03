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

    struct atk_component : public component {
        int damage = -1;
    };

    //--------------end of atk_component-----------------------------------------

    struct health_component : public component {
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
        std::string repr = "?";

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

}  // namespace roguelike

#endif  // ROGUE_LIKE_COMPONENTS_H
