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
            h_cpt.max_health = 2;
            h_cpt.health = h_cpt.max_health;
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

    template <typename entityType>
    struct interacter<goblin, entityType> {
        static inline void interact(goblin &inted, entityType &inting) {
            if constexpr (has_member_logging_component<entityType>::value) {
                inting.lg_cpt.log << "you interacted with goblin" << std::to_string(inted.id.value) << "\n";
            }
            std::string inting_name = "unknown";
            if constexpr (has_member_name_component<entityType>::value) {
                inting_name = inting.nm_cpt.name;
            }
            if constexpr (has_member_atk_component<entityType>::value and not std::is_base_of_v<goblin, entityType>) {
                std::cout << inted.nm_cpt.name << " was damaged by " << inting_name << std::endl;
                auto dmg = atk_component::calculate_damage(&inting);
                health_component::receive_damage(&inted, dmg);
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
