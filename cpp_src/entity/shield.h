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
        return u8"☉";
    }

    template <typename entityType>
    struct interacter<shield, entityType> {
        static inline void interact(shield &inted, entityType &inting) {
            constexpr bool has_log = has_member_logging_component<entityType>::value;
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                lwlog_info("somebody with inventory interacted with shield");
                inted.pk_cpt.picked = false;
                if (inting.s_inv_cpt.spot.has_value()) {
                    lwlog_info("something is in temporal spot");
                    if constexpr (has_log) {
                        inting.lg_cpt.log << "can't pick item" << std::endl;
                    }
                    return;
                }
                inting.s_inv_cpt.spot = &inted;
                inting.s_inv_cpt.manage();
                if (inting.s_inv_cpt.spot.has_value()) {
                    lwlog_info("managing left item on temporary spot");
                    if constexpr (has_log) {
                        inting.lg_cpt.log << "you can't pick this item" << std::endl;
                    }
                    inting.s_inv_cpt.spot.reset();
                    return;
                }
                inted.pk_cpt.picked = true;
            }
            if constexpr (has_log) {
                std::string lg_entry = inted.pk_cpt.picked ? "you picked shield" : "you interacted with shield";
                inting.lg_cpt.log << lg_entry << std::endl;
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUELIKE_SHIELD_H
