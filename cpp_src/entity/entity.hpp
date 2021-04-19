//
// Created by Kirill Golubev on 17.04.2021.
//

#include <iostream>
#include "../common.h"

#ifndef ROGUE_LIKE_ENTITY_HPP
#define ROGUE_LIKE_ENTITY_HPP
namespace roguelike {
    struct component {
        using component_id = int;
        component_id id = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(component, id);

    struct move_component : public component {
        int x = -1, y = -1;
        tile_idx residency = -1;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(move_component, x, y, residency, id);

    define_has_member(m_cpt);

    struct atk_component : public component {
        int damage = -1;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(atk_component, damage, id);

    define_has_member(a_cpt);

    struct health_component : public component {
        int health = -1;

    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(health_component, health, id);

    define_has_member(h_cpt);

    struct repr_component : public component {

        explicit repr_component(std::string repr_string) {
            repr = [repr_string]() -> std::string {
                return repr_string;
            };
        }

        explicit repr_component(const std::function<std::string()> &repr_func) {
            repr = repr_func;
        };

    private:
        std::function<std::string()> repr = []() -> std::string {
            return "?";
        };

        friend void to_json(nlohmann::json &j, const repr_component &p);
    };

    void to_json(nlohmann::json &j, const repr_component &p) {
        j["id"] = p.id;
        j["repr"] = p.repr();
    }

    void from_json(const nlohmann::json &j, repr_component &p) {
        throw std::runtime_error("One CAN NOT reconstruct representation component from its serialization");
    }

    define_has_member(repr_cpt);


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

    define_has_member(dm_cpt);

    struct entity {
        entity_id id;
        int level = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(entity, id, level);

    template<typename interacted, typename interacting>
    struct interacter {//bc C++ does not permit partial function specialization
    public:
        static void interact(interacted &inted, interacting &inting) {
            std::cout << "unknown interacted by unknown" << std::endl;
            return;
        }
    };
}

#endif //ROGUE_LIKE_ENTITY_HPP
