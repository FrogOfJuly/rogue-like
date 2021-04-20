//
// Created by Kirill Golubev on 17.04.2021.
//

#include "entity.hpp"


#ifndef ROGUE_LIKE_PLAYER_H
#define ROGUE_LIKE_PLAYER_H

namespace roguelike {
    struct player {
        player(int pl_id) {
            id = player_id{pl_id};
            h_cpt.health = 10;
            a_cpt.damage = 1;
            m_cpt.x = 0;
            m_cpt.y = 0;
            dm_cpt.decision = PASS;
        }

        player(int id, int x, int y) : player(id) {
            m_cpt.x = x;
            m_cpt.y = y;
        }

        player_id id = player_id{-1};
        int lvl = 0;
        logging_component lg_cpt;
        health_component h_cpt;
        move_component m_cpt;
        atk_component a_cpt;
        decision_making_component dm_cpt;
        repr_component repr_cpt = repr_component("p");
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(player, id, lvl, h_cpt, m_cpt, a_cpt, dm_cpt, lg_cpt, repr_cpt);

    template<typename entityType>
    struct interacter<player, entityType> {
        static void interact(player &inted, entityType &inting) {
            if constexpr(has_member_a_cpt<entityType>::value) {
                auto dmg = inting.a_cpt.damage;
                inted.h_cpt.health -= dmg;
                inted.lg_cpt.log += "you was attacked by the unknown\n";
                return;
            }
            inted.lg_cpt.log += "you was interacted by the unknown\n";
            return;
        }
    };

    template<>
    struct interacter<player, player> {
        static void interact(player &inted, player &inting) {
            auto dmg = inting.a_cpt.damage;
            inted.h_cpt.health -= dmg;
            inted.lg_cpt.log += "you was interacted by the player " + std::to_string(inting.id.value) + "\n";
            inting.lg_cpt.log += "you interacted with player" + std::to_string(inted.id.value) + "\n";
            return;
        }
    };
} //namespace roguelike

#endif //ROGUE_LIKE_PLAYER_H
