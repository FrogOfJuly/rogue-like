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
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(goblin, dm_cpt, h_cpt, a_cpt, m_cpt)

    template<typename entityType>
    struct interacter<goblin, entityType>{
        static void interact(goblin& inted, entityType& inting){
            if constexpr (has_member_a_cpt<entityType>::value){
                auto dmg = inting.a_cpt.damage;
                inted.h_cpt.health -= dmg;
                std::cout << "goblin was attacked by the unknown" << std::endl;
                return;
            }
            std::cout << "goblin interacted by unknown" << std::endl;
            return;
        }
    };
}

#endif //ROGUE_LIKE_GOBLIN_H
