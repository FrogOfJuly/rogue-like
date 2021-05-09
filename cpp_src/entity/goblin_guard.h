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
                return u8"←";
            case RIGHT:
                return u8"→";
            case DOWN:
                return u8"↓";
            case UP:
                return u8"↑";
            default:
                return g->dm_cpt.wait_before_strike ?  u8"♠" : u8"☠";
        }
    }

    template <typename entityType>
    struct interacter<goblin_guard, entityType> {
        static inline void interact(goblin_guard &inted, entityType &inting) {
            if constexpr (has_member_atk_component<entityType>::value and not std::is_base_of_v<goblin, entityType>) {
                auto dmg = atk_component::calculate_damage(&inting);
                auto rec_dmg = health_component::receive_damage(&inted, dmg);
                if constexpr (has_member_logging_component<entityType>::value) {
                    inting.lg_cpt.log << "you damaged goblin guard by " << rec_dmg << "\n";
                }
                if (not health_component::is_alive(&inted)) {
                    if constexpr (has_member_expirience_components<entityType>::value) {
                        int new_exp = inted.lvl_cpt.experience_on_kill();
                        bool lvlup = expirience_components::gain_experience(&inting, new_exp);
                        if (lvlup) {
                            expirience_components::perform_lvlups(&inting);
                        }
                    }
                }
                return;
            }
            if constexpr (has_member_logging_component<entityType>::value) {
                inting.lg_cpt.log << "you interacted with goblin guard" << std::to_string(inted.id.value) << "\n";
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
