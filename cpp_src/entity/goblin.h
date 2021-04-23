//
// Created by Kirill Golubev on 18.04.2021.
//
#include "entity.hpp"

#ifndef ROGUE_LIKE_GOBLIN_H
#define ROGUE_LIKE_GOBLIN_H
namespace roguelike {
    struct goblin : entity {
        goblin() : entity() {
            h_cpt.health = 5;
            a_cpt.damage = 1;
            m_cpt.y = 0;
            m_cpt.x = 0;
        }

        goblin(int x, int y) : goblin() {
            h_cpt.health = 5;
            a_cpt.damage = 1;
            m_cpt.y = x;
            m_cpt.x = y;
        }

        decision_making_component dm_cpt;
        health_component h_cpt;
        atk_component a_cpt;
        move_component m_cpt;
        repr_component repr_cpt = repr_component([this]() -> std::string {
            switch (this->dm_cpt.decision) {
                case LEFT:
                    return "<";
                case RIGHT:
                    return ">";
                case DOWN:
                    return "v";
                case UP:
                    return "^";
                default:
                    return "x";
            }
        });
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(goblin, id, dm_cpt, h_cpt, a_cpt, m_cpt, repr_cpt)

    template<typename entityType>
    struct interacter<goblin, entityType> {
        static void interact(goblin &inted, entityType &inting) {
            if constexpr (has_member_logging_component<entityType>::value) {
                inting.lg_cpt.log << "you interacted with goblin" << std::to_string(inted.id.value) << "\n";
            }
            if constexpr (has_member_atk_component<entityType>::value) {
                auto dmg = inting.a_cpt.damage;
                inted.h_cpt.health -= dmg;
                return;
            }
            return;
        }
    };
}

#endif //ROGUE_LIKE_GOBLIN_H
