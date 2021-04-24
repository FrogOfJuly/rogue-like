//
// Created by Kirill Golubev on 23.04.2021.
//

#ifndef ROGUE_LIKE_COMPONENTS_H
#define ROGUE_LIKE_COMPONENTS_H

namespace roguelike {
    struct component {
        using component_id = int;
        component_id id = -1;
    };

    //--------------end of component---------------------------------------------

    //    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(component);

    struct move_component : public component {
        int x = -1, y = -1;
        tile_idx residency = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(move_component, x, y, residency);

    //--------------end of move_component----------------------------------------

    struct atk_component : public component {
        int damage = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(atk_component, damage);

    //--------------end of atk_component-----------------------------------------

    struct health_component : public component {
        int health = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(health_component, health);

    //--------------end of health_component--------------------------------------

    struct repr_component : public component {
        std::string repr = "?";

        template <typename T>
        static std::string compute_representation(const T *) {
            return "?";
        }
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(repr_component, repr);

    //--------------end of repr_component----------------------------------------

    struct decision_making_component : public component {
        cmd decision = cmd::PASS;

        std::pair<int, int> get_velocity() {
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

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(decision_making_component, decision);

    //--------------end of decision_making_component-----------------------------

    struct logging_component : public component {
        std::stringstream log{};
    };

    void to_json(nlohmann::json &j, const logging_component &p) {
        //        j["id"] = p.id;
        j["log"] = p.log.str();
    }

    void from_json(const nlohmann::json &j, logging_component &p) {
        p.id = j["id"];
        p.log << j["log"];
    }

    //--------------end of logging_component-------------------------------------

#define register_component(cmpt_name, component_type) define_has_member(cmpt_name, component_type)

#include "../utility/register_for_components.h"

#undef register_component
}  // namespace roguelike

#endif  // ROGUE_LIKE_COMPONENTS_H
