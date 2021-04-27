//
// Created by Kirill Golubev on 23.04.2021.
//
#include "../common.h"

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
        static inline bool is_alive(const T *) {
            return false;
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
        strategy strat = strategy::passive;

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

}  // namespace roguelike

#endif  // ROGUE_LIKE_COMPONENTS_H
