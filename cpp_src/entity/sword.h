//
// Created by Kirill Golubev on 09.05.2021.
//

#ifndef ROGUELIKE_SWORD_H
#define ROGUELIKE_SWORD_H

namespace roguelike {
    struct sword {
        sword(int id) : id{id} {
            nm_cpt.name = "sword";
            pk_cpt.desired_spot = simple_inventory_component::sword;
            a_cpt.damage = 2;
            lvl_cpt.lvl = 5;
        }
        inline static sword make_sword(int id, int lvl) {
            auto sw = sword(id);
            sw.lvl_cpt.lvl = lvl;
            sw.a_cpt.damage = lvl;
            return sw;
        }
        entity_id id;
        repr_component repr_cpt;
        pickable_component pk_cpt;
        name_component nm_cpt;
        atk_component a_cpt;
        level_component lvl_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<sword>(const sword *p) {
        return "⌿";
    }

    template <typename entityType>
    struct interacter<sword, entityType> {
        static inline interaction_info interact(sword *inted, entityType *inting) {
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                return default_interactors::item_picking<sword, entityType>::interact(inted, inting);
            } else {
                return default_interactors::logging<sword, entityType>::interact(inted, inting);
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUELIKE_SWORD_H
