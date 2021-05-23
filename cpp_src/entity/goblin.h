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
            h_cpt.max_health = 3;
            h_cpt.health = h_cpt.max_health;
            a_cpt.damage = 4;
            m_cpt.y = -1;
            m_cpt.x = -1;
            dm_cpt.eye_sight = 5;
            dm_cpt.active_strategy = std::make_unique<agressive_strategy>();
            dm_cpt.idle_strategy = std::make_unique<random_strategy>();
            nm_cpt.name = "goblin";
            lvl_cpt.lvl = 3;
            s_inv_cpt = simple_inventory_component::get_locked_invetory();
        }

        decision_making_component dm_cpt;
        health_component h_cpt;
        atk_component a_cpt;
        move_component m_cpt;
        repr_component repr_cpt;
        name_component nm_cpt;
        level_component lvl_cpt;
        simple_inventory_component s_inv_cpt;
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
                return g->dm_cpt.wait_before_strike ? "x" : "☠";
        }
    }

    template <typename entityType>
    struct interacter<goblin, entityType> {
        static inline interaction_info interact(goblin *inted, entityType *inting) {
            if constexpr (has_member_atk_component<entityType>::value and not std::is_base_of_v<goblin, entityType>) {
                return default_interactors::agressive<goblin, entityType>::interact(inted, inting);
            }else {
                return default_interactors::logging<goblin, entityType>::interact(inted, inting);
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_GOBLIN_H
