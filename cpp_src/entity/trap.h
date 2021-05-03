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
            nm_cpt.name = "trap" + std::to_string(id);
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
        return p->activated ? " " : u8"☒";
    }
    template <>
    inline bool health_component::is_alive<trap>(const trap *ent) {
        return ent->h_cpt.health > 0;
    }

    template <>
    struct interacter<trap, player> {
        static inline void interact(trap &inted, player &inting) {
            if (inted.activated) {
                inting.lg_cpt.log << "It's a trap!!\n";
                inting.h_cpt.health -= inted.a_cpt.damage;
                inted.activated = false;
            } else {
                inting.lg_cpt.log << "you interacted with disactivated trap\n";
                inted.h_cpt.health = 0;
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_TRAP_H
