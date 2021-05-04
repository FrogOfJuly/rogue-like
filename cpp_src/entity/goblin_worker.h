//
// Created by Kirill Golubev on 02.05.2021.
//

#include "../strategies/agressive_strategy/agressive_strategy.h"
#include "../strategies/passive_strategy/passive_strategy.h"
#include "../strategies/fearfull_strategy/fearfull_strategy.h"
#include "entity.hpp"
#include "goblin.h"

#ifndef ROGUE_LIKE_GOBLIN_WORKER_H
#define ROGUE_LIKE_GOBLIN_WORKER_H
namespace roguelike {
    struct goblin_worker : public goblin {
        goblin_worker(int id) : goblin(id) {
            h_cpt.health = 2;
            a_cpt.damage = 1;
            dm_cpt.active_strategy = std::make_unique<fearfull_strategy>();
            dm_cpt.idle_strategy = std::make_unique<passive_strategy>();
            nm_cpt.name = "goblin_worker" + std::to_string(id);
        }
    };

    template<>
    inline std::string repr_component::compute_representation<goblin_worker>(const goblin_worker *g) {
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
                return u8"♙";
        }
    }

    template<typename entityType>
    struct interacter<goblin_worker, entityType> {
        static inline void interact(goblin_worker &inted, entityType &inting) {
            if constexpr (has_member_logging_component<entityType>::value) {
                inting.lg_cpt.log << "you interacted with goblin worker" << std::to_string(inted.id.value) << "\n";
            }
            std::string inting_name = "unknown";
            if constexpr (has_member_name_component<entityType>::value) {
                inting_name = inting.nm_cpt.name;
            }
            if constexpr (has_member_atk_component<entityType>::value and not std::is_base_of_v<goblin, entityType>) {
                std::cout << inted.nm_cpt.name << " was damaged by " << inting_name << std::endl;
                auto dmg = inting.a_cpt.damage;
                inted.h_cpt.health -= dmg;
                health_component::receive_damage(&inted, dmg);
                return;
            }
            return;
        }
    };

    template<>
    struct interacter<goblin_worker, goblin_worker> {
        static inline void interact(goblin_worker &inted, goblin_worker &inting) {}
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_GOBLIN_GUARD_H
