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

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(component, id);

    struct move_component : public component {
        int x = -1, y = -1;
        tile_idx residency = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(move_component, x, y, residency, id);

    //--------------end of move_component----------------------------------------

    struct atk_component : public component {
        int damage = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(atk_component, damage, id);

    //--------------end of atk_component-----------------------------------------

    struct health_component : public component {
        int health = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(health_component, health, id);

    //--------------end of health_component--------------------------------------

    struct repr_component : public component {
        explicit repr_component(std::string repr_string) {
            repr = [repr_string]() -> std::string { return repr_string; };
        }

        explicit repr_component(const std::function<std::string()> &repr_func) { repr = repr_func; };

       private:
        std::function<std::string()> repr = []() -> std::string { return "?"; };

        friend void to_json(nlohmann::json &j, const repr_component &p);
    };

    void to_json(nlohmann::json &j, const repr_component &p) {
        j["id"] = p.id;
        j["repr"] = p.repr();
    }

    void from_json(const nlohmann::json &j, repr_component &p) {
        throw std::runtime_error("One CAN NOT reconstruct representation component from its serialization");
    }

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

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(decision_making_component, decision, id);

    //--------------end of decision_making_component-----------------------------

    struct logging_component : public component {
        std::stringstream log{};
    };

    void to_json(nlohmann::json &j, const logging_component &p) {
        j["id"] = p.id;
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
