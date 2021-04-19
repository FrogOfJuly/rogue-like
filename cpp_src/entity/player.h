//
// Created by Kirill Golubev on 17.04.2021.
//

#include "entity.hpp"


#ifndef ROGUE_LIKE_PLAYER_H
#define ROGUE_LIKE_PLAYER_H

namespace roguelike {
    struct player {
        player() {
            h_cpt.health = 10;
            a_cpt.damage = 1;
            m_cpt.x = 0;
            m_cpt.y = 0;
            dm_cpt.decision = PASS;
        }

        player(int x, int y) : player() {
            m_cpt.x = x;
            m_cpt.y = y;
        }

        player_id id;
        int lvl = 0;
        std::string log;
        health_component h_cpt;
        move_component m_cpt;
        atk_component a_cpt;
        decision_making_component dm_cpt;
        repr_component repr_cpt = repr_component("p");
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(player, id, lvl, h_cpt, m_cpt, a_cpt, dm_cpt, log, repr_cpt);

    template<typename entityType>
    struct interacter<player, entityType> {
        static void interact(player &inted, entityType &inting) {
            if constexpr(has_member_a_cpt<entityType>::value) {
                auto dmg = inting.a_cpt.damage;
                inted.h_cpt.health -= dmg;
                std::cout << "player was attacked by the unknown" << std::endl;
                inted.log += "player was attacked by the unknown";
                return;
            }
            std::cout << "player interacted by the unknown" << std::endl;
            inted.log += "player interacted by the unknown";
            return;
        }
    };

    template<>
    struct interacter<player, player> {
        static void interact(player &inted, player &inting) {
            auto dmg = inting.a_cpt.damage;
            inted.h_cpt.health -= dmg;
            std::cout << "player interacted by player" << std::endl;
            return;
        }
    };
} //namespace roguelike

#endif //ROGUE_LIKE_PLAYER_H
