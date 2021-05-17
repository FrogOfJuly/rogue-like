//
// Created by Kirill Golubev on 09.05.2021.
//

#ifndef ROGUELIKE_SHOVEL_H
#define ROGUELIKE_SHOVEL_H
namespace roguelike {
    struct shovel {
        shovel(int id) : id{id} {
            nm_cpt.name = "shovel";
            pk_cpt.desired_spot = simple_inventory_component::offence;
            lvl_cpt.lvl = 0;
        }
        entity_id id;
        repr_component repr_cpt;
        pickable_component pk_cpt;
        name_component nm_cpt;
        level_component lvl_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<shovel>(const shovel *p) {
        return "♩";
    }

    template <typename entityType>
    struct interacter<shovel, entityType> {
        static inline interaction_info interact(shovel *inted, entityType *inting) {
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                return default_interactors::item_picking<shovel, entityType>::interact(inted, inting);
            } else {
                return default_interactors::logging<shovel, entityType>::interact(inted, inting);
            }
        }
    };
}  // namespace roguelike
#endif  // ROGUELIKE_SHOVEL_H
