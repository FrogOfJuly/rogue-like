//
// Created by Kirill Golubev on 09.05.2021.
//

#ifndef ROGUELIKE_ARMOR_H
#define ROGUELIKE_ARMOR_H
namespace roguelike {
    struct armor {
        armor(int id) : id{id} {
            nm_cpt.name = "armor";
            pk_cpt.desired_spot = simple_inventory_component::armor;
            lvl_cpt.lvl = 2;
            prt_cpt.protection_scale = 0.95;
        }
        inline static armor make_armor(int id, int lvl) {
            auto sw = armor(id);
            sw.lvl_cpt.lvl = lvl;
            sw.prt_cpt.protection_scale = std::max(0.5, 1.0 - 0.1 * lvl);
            return sw;
        }
        entity_id id;
        repr_component repr_cpt;
        pickable_component pk_cpt;
        name_component nm_cpt;
        prot_component prt_cpt;
        level_component lvl_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<armor>(const armor *p) {
        return "θ";
    }

    template <typename entityType>
    struct interacter<armor, entityType> {
        static inline void interact(armor &inted, entityType &inting) {
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                default_interactors::item_picking<armor, entityType>::interact(inted, inting);
            } else {
                default_interactors::logging<armor, entityType>::interact(inted, inting);
            }
        }
    };
}  // namespace roguelike
#endif  // ROGUELIKE_ARMOR_H
