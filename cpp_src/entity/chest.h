//
// Created by Kirill Golubev on 09.05.2021.
//

#include "components.h"

#ifndef ROGUELIKE_CHEST_H
#define ROGUELIKE_CHEST_H

namespace roguelike {
    struct chest : public entity {
        chest(int id) : entity(id) {
            nm_cpt.name = "chest";
            h_cpt.health = 1;
            s_inv_cpt = simple_inventory_component::get_locked_invetory();
        }

        chest(int id, entity_type item) : entity(id) {
            nm_cpt.name = "chest";
            h_cpt.health = 1;
            s_inv_cpt = simple_inventory_component::get_locked_invetory();
            s_inv_cpt.spots[simple_inventory_component::inventory_spot::active] = item;
        }
        repr_component repr_cpt;
        name_component nm_cpt;
        health_component h_cpt;
        simple_inventory_component s_inv_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<chest>(const chest *p) {
        return "▣";
    }

    template <>
    struct interacter<chest, player> {
        static inline void interact(chest &inted, player &inting) {
            inting.lg_cpt.log << "It's a chest!!\n";
            inted.h_cpt.health = 0;
            inted.s_inv_cpt.spot = inted.s_inv_cpt.get_some_item();
        }
    };
}  // namespace roguelike

#endif  // ROGUELIKE_CHEST_H
