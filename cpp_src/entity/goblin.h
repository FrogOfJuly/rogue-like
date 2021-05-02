//
// Created by Kirill Golubev on 18.04.2021.
//
#include "../strategies/agressive_strategy/agressive_strategy.h"
#include "../strategies/random_strategy/random_strategy.h"
#include "entity.hpp"

#ifndef ROGUE_LIKE_GOBLIN_H
#define ROGUE_LIKE_GOBLIN_H
namespace roguelike {
    struct goblin : entity {
        goblin(int id) : entity(id) {
            h_cpt.health = 2;
            a_cpt.damage = 4;
            m_cpt.y = -1;
            m_cpt.x = -1;
            dm_cpt.eye_sight = 5;
            dm_cpt.active_strategy = std::make_unique<agressive_strategy>();
            dm_cpt.idle_strategy = std::make_unique<random_strategy>();
            nm_cpt.name = "goblin" + std::to_string(id);
        }

        decision_making_component dm_cpt;
        health_component h_cpt;
        atk_component a_cpt;
        move_component m_cpt;
        repr_component repr_cpt;
        name_component nm_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<goblin>(const goblin *g) {
        switch (g->dm_cpt.decision) {
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
    }

    template <>
    inline bool health_component::is_alive<goblin>(const goblin *g) {
        return g->h_cpt.health > 0;
    }

    template <typename entityType>
    struct interacter<goblin, entityType> {
        static inline void interact(goblin &inted, entityType &inting) {
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

    template <>
    struct interacter<goblin, goblin> {
        static inline void interact(goblin &inted, goblin &inting) {
            // no friendly fire
            return;
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_GOBLIN_H
