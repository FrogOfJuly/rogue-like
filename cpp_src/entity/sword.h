//
// Created by Kirill Golubev on 09.05.2021.
//

#ifndef ROGUELIKE_SWORD_H
#define ROGUELIKE_SWORD_H

namespace roguelike {
    struct sword {
        sword(int id) : id{id} {
            nm_cpt.name = "sword";
            pk_cpt.desired_spot = simple_inventory_component::offence;
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
        return u8"⌿";
    }

    template <typename entityType>
    struct interacter<sword, entityType> {
        static inline void interact(sword &inted, entityType &inting) {
            constexpr bool has_log = has_member_logging_component<entityType>::value;
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                lwlog_info("somebody with inventory interacted with sword");
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
                std::string lg_entry = inted.pk_cpt.picked ? "you picked sword" : "you interacted with sword";
                inting.lg_cpt.log << lg_entry << std::endl;
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUELIKE_SWORD_H
