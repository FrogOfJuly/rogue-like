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
            constexpr bool has_log = has_member_logging_component<entityType>::value;
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                lwlog_info("somebody with inventory interacted with armor");
                inted.pk_cpt.picked = false;
                if (inting.s_inv_cpt.spot.has_value()) {
                    lwlog_info("something is in temporal spot");
                    if constexpr (has_log) {
                        inting.lg_cpt.log << "can't pick item" << std::endl;
                    }
                    return;
                }
                inted.pk_cpt.picked = true;
                inting.s_inv_cpt.spot = &inted;
                inting.s_inv_cpt.manage();
                if (inting.s_inv_cpt.spot.has_value()) {
                    lwlog_info("managing left item on temporary spot");
                    if constexpr (has_log) {
                        inting.lg_cpt.log << "you can't pick armor" << std::endl;
                    }
                    // inting.s_inv_cpt.spot.reset();
                    return;
                }
            }
            if constexpr (has_log) {
                std::string lg_entry = inted.pk_cpt.picked ? "you picked armor" : "you interacted with armor";
                inting.lg_cpt.log << lg_entry << std::endl;
            }
        }
    };
}  // namespace roguelike
#endif  // ROGUELIKE_ARMOR_H
