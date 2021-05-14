//
// Created by Kirill Golubev on 09.05.2021.
//

#ifndef ROGUELIKE_SHIELD_H
#define ROGUELIKE_SHIELD_H

namespace roguelike {
    struct shield {
        shield(int id) : id{id} {
            nm_cpt.name = "shield";
            pk_cpt.desired_spot = simple_inventory_component::defence;
            lvl_cpt.lvl = 5;
            dfc_cpt.damage_reduction = 2;
        }
        inline static shield make_shield(int id, int lvl) {
            auto sw = shield(id);
            sw.lvl_cpt.lvl = lvl;
            sw.dfc_cpt.damage_reduction = std::max(1, lvl / 3);
            return sw;
        }
        entity_id id;
        repr_component repr_cpt;
        pickable_component pk_cpt;
        name_component nm_cpt;
        dfc_component dfc_cpt;
        level_component lvl_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<shield>(const shield *p) {
        return "☉";
    }

    template <typename entityType>
    struct interacter<shield, entityType> {
        static inline interaction_info interact(shield &inted, entityType &inting) {
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                return default_interactors::item_picking<shield, entityType>::interact(inted, inting);
            } else {
                return default_interactors::logging<shield, entityType>::interact(inted, inting);
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUELIKE_SHIELD_H
