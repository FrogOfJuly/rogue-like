//
// Created by Kirill Golubev on 02.05.2021.
//

#include "../strategies/agressive_strategy/agressive_strategy.h"
#include "../strategies/passive_strategy/passive_strategy.h"
#include "entity.hpp"
#include "goblin.h"

#ifndef ROGUE_LIKE_GOBLIN_GUARD_H
#define ROGUE_LIKE_GOBLIN_GUARD_H
namespace roguelike {
    struct goblin_guard : public goblin {
        goblin_guard(int id) : goblin(id) {
            h_cpt.max_health = 12;
            h_cpt.health = h_cpt.max_health;
            a_cpt.damage = 1;
            dm_cpt.idle_strategy = std::make_unique<passive_strategy>();
            nm_cpt.name = "goblin guard";
            lvl_cpt.lvl = 5;
        }
    };

    template <>
    inline std::string repr_component::compute_representation<goblin_guard>(const goblin_guard *g) {
        switch (g->dm_cpt.decision) {
            case LEFT:
                return "←";
            case RIGHT:
                return "→";
            case DOWN:
                return "↓";
            case UP:
                return "↑";
            default:
                return g->dm_cpt.wait_before_strike ? "♠" : "☠";
        }
    }

    template <typename entityType>
    struct interacter<goblin_guard, entityType> {
        static inline interaction_info interact(goblin_guard &inted, entityType &inting) {
            if constexpr (has_member_atk_component<entityType>::value and not std::is_base_of_v<goblin, entityType>) {
                return default_interactors::agressive<goblin_guard, entityType>::interact(inted, inting);
            } else {
                return default_interactors::logging<goblin_guard, entityType>::interact(inted, inting);
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_GOBLIN_GUARD_H
