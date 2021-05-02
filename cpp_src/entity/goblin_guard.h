//
// Created by Kirill Golubev on 02.05.2021.
//

#include "../strategies/agressive_strategy/agressive_strategy.h"
#include "../strategies/passive_strategy/passive_strategy.h"
#include "entity.hpp"

#ifndef ROGUE_LIKE_GOBLIN_GUARD_H
#define ROGUE_LIKE_GOBLIN_GUARD_H
namespace roguelike {
    struct goblin_guard : entity {
        goblin_guard(int id) : entity(id) {
            h_cpt.health = 12;
            a_cpt.damage = 1;
            m_cpt.y = -1;
            m_cpt.x = -1;
            dm_cpt.eye_sight = 5;
            dm_cpt.active_strategy = std::make_unique<agressive_strategy>();
            dm_cpt.idle_strategy = std::make_unique<passive_strategy>();
            nm_cpt.name = "goblin_guard" + std::to_string(id);
        }

        decision_making_component dm_cpt;
        health_component h_cpt;
        atk_component a_cpt;
        move_component m_cpt;
        repr_component repr_cpt;
        name_component nm_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<goblin_guard>(const goblin_guard *g) {
        switch (g->dm_cpt.decision) {
            case LEFT:
                return u8"←";
            case RIGHT:
                return u8"→";
            case DOWN:
                return u8"↓";
            case UP:
                return u8"↑";
            default:
                return u8"♠";
        }
    }

    template <>
    inline bool health_component::is_alive<goblin_guard>(const goblin_guard *g) {
        return g->h_cpt.health > 0;
    }

    template <typename entityType>
    struct interacter<goblin_guard, entityType> {
        static inline void interact(goblin_guard &inted, entityType &inting) {
            if constexpr (has_member_logging_component<entityType>::value) {
                inting.lg_cpt.log << "you interacted with goblin guard" << std::to_string(inted.id.value) << "\n";
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
    struct interacter<goblin_guard, goblin_guard> {
        static inline void interact(goblin_guard &inted, goblin_guard &inting) {}
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_GOBLIN_GUARD_H
