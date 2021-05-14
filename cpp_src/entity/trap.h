//
// Created by Kirill Golubev on 02.05.2021.
//

#include "entity.hpp"

#ifndef ROGUE_LIKE_TRAP_H
#define ROGUE_LIKE_TRAP_H

namespace roguelike {
    struct trap {
        trap(int id) : id{id} {
            a_cpt.damage = 5;
            nm_cpt.name = "trap";
            h_cpt.health = 1;
        }
        entity_id id;
        repr_component repr_cpt;
        atk_component a_cpt;
        name_component nm_cpt;
        health_component h_cpt;
        bool activated = true;
    };

    template <>
    inline std::string repr_component::compute_representation<trap>(const trap *p) {
        return p->activated ? " " : "☒";
    }

    template <>
    struct interacter<trap, player> {
        static inline interaction_info interact(trap &inted, player &inting) {
            if (inted.activated) {
                inting.lg_cpt.log << "It's a trap!!\n";
                health_component::receive_damage(&inting, inted.a_cpt.damage);
                inted.activated = false;
                return health_component::is_alive(&inting) ? interaction_info::nothing_happened
                                                           : interaction_info::interacting_got_killed;
            } else {
                inting.lg_cpt.log << "you interacted with disarmed trap\n";
                inted.h_cpt.health = 0;
                return interaction_info::interacted_got_destroyed;
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_TRAP_H
