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
            m_cpt.x = -1;
            m_cpt.y = -1;
            dm_cpt.decision = PASS;
            dm_cpt.strat = simple_strategy::external;
        }

        player_id id = player_id{-1};
        int lvl = 0;
        logging_component lg_cpt;
        health_component h_cpt;
        move_component m_cpt;
        atk_component a_cpt;
        decision_making_component dm_cpt;
        repr_component repr_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<player>(const player *p) {
        return std::to_string(p->id.value);
    }

    template <>
    inline bool health_component::is_alive<player>(const player *p) {
        return p->h_cpt.health > 0;
    }

    template <typename entityType>
    struct interacter<player, entityType> {
        static inline void interact(player &inted, entityType &inting) {
            std::cout << "type of interacting: " << typeid(entityType).name() << std::endl;
            if constexpr (has_member_atk_component<entityType>::value) {
                auto dmg = inting.a_cpt.damage;
                inted.h_cpt.health -= dmg;
                inted.lg_cpt.log << "you received " << dmg << " damage\n";
                return;
            }
            inted.lg_cpt.log << "you was interacted by the unknown\n";
            return;
        }
    };

    template <>
    struct interacter<player, player> {
        static inline void interact(player &inted, player &inting) {
            auto dmg = inting.a_cpt.damage;
            inted.h_cpt.health -= dmg;
            inted.lg_cpt.log << "you was interacted by the player " << std::to_string(inting.id.value) << "\n";
            inting.lg_cpt.log << "you interacted with player" << std::to_string(inted.id.value) << "\n";
            return;
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_PLAYER_H
