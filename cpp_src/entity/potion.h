//
// Created by Kirill Golubev on 20.04.2021.
//
#include "entity.hpp"

#ifndef ROGUE_LIKE_ITEMS_H
#define ROGUE_LIKE_ITEMS_H
namespace roguelike {
    struct potion {
        potion(int id) : id{id} {
            nm_cpt.name = "health potion";
            ot_eff_cpt.msg_on_effect = "you have drank a potion of health";
        }
        entity_id id;
        repr_component repr_cpt;
        pickable_component pk_cpt;
        one_time_effect_component ot_eff_cpt;
        name_component nm_cpt;
    };

    template <>
    inline bool one_time_effect_component::apply_effect(potion *src, player *tgt) {
        tgt->lg_cpt.log << msg_on_effect << std::endl;
        tgt->h_cpt.health += 5;
        return true;
    }

    template <>
    inline std::string repr_component::compute_representation<potion>(const potion *p) {
        return u8"☩";
    }

    template <typename entityType>
    struct interacter<potion, entityType> {
        static inline void interact(potion &inted, entityType &inting) {
            constexpr bool has_log = has_member_logging_component<entityType>::value;
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                lwlog_info("somebody with inventory interacted with potion");
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
                std::string lg_entry = inted.pk_cpt.picked ? "you picked potion" : "you interacted with potion";
                inting.lg_cpt.log << lg_entry << std::endl;
            }
        }
    };
}  // namespace roguelike
#endif  // ROGUE_LIKE_ITEMS_H
