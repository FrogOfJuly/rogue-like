//
// Created by Kirill Golubev on 17.04.2021.
//

#include <cmath>

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
        return std::to_string(p->id.value + 1);
    }

    template <>
    inline std::pair<int, int> expirience_components::get_level<player>(player *ent) {
        auto &exp = ent->exp_cpt.exp;
        int exp_offset = 16;
        int lvl_offset = 4;
        int cur_level = (int)(std::log(exp + exp_offset) / std::log(2.0)) - lvl_offset;
        int next_level = (int)std::pow(2.0, (cur_level + lvl_offset + 1)) - exp_offset;
        return std::make_pair(cur_level, next_level);
    }

    template <>
    inline bool expirience_components::gain_experience<player>(player *ent, int new_exp) {
        auto cur_lvl = get_level(ent).first;
        ent->exp_cpt.exp += new_exp;
        auto new_lvl = get_level(ent).first;
        ent->exp_cpt.lvlups += new_lvl - cur_lvl;
        return cur_lvl != new_lvl;
    }

    template <>
    inline void expirience_components::perform_lvlups<player>(player *ent) {
        for (int i = 0; i < ent->exp_cpt.lvlups; ++i) {
            ent->h_cpt.health += 1;
        }
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

    struct player_hasher {
        inline size_t operator()(const player &plr) const { return plr.id.value; }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_PLAYER_H
