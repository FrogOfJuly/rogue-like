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
            h_cpt.max_health = 50;
            h_cpt.health = h_cpt.max_health;
            a_cpt.damage = 2;
            m_cpt.x = -1;
            m_cpt.y = -1;
            dm_cpt.decision = PASS;
            nm_cpt.name = "player" + std::to_string(pl_id);
        }

        player_id id = player_id{-1};
        logging_component lg_cpt;
        health_component h_cpt;
        move_component m_cpt;
        atk_component a_cpt;
        decision_making_component dm_cpt;
        repr_component repr_cpt;
        name_component nm_cpt;
        simple_inventory_component s_inv_cpt;
        expirience_components exp_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<player>(const player *p) {
        return std::to_string(p->id.value);
    }

    template <typename entityType>
    struct interacter<player, entityType> {
        static inline void interact(player &inted, entityType &inting) {
            if constexpr (has_member_atk_component<entityType>::value) {
                int dmg = atk_component::calculate_damage(&inting);
                int health = inted.h_cpt.health;
                int rec_dmg = health_component::receive_damage(&inted, dmg);
                inted.lg_cpt.log << "you received " << rec_dmg << " damage\n";
                return;
            }
            if constexpr (has_member_name_component<entityType>::value) {
                inted.lg_cpt.log << "you was interacted by " << inting.nm_cpt.name << std::endl;
            } else {
                inted.lg_cpt.log << "you was interacted by the unknown\n";
            }

            return;
        }
    };

    template <>
    struct interacter<player, player> {
        static inline void interact(player &inted, player &inting) {
            auto dmg = atk_component::calculate_damage(&inting);
            health_component::receive_damage(&inted, dmg);
            inted.lg_cpt.log << "you was interacted by the player " << std::to_string(inting.id.value) << "\n";
            inting.lg_cpt.log << "you interacted with player" << std::to_string(inted.id.value) << "\n";
            return;
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_PLAYER_H
