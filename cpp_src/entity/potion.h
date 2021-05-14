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
        return "☩";
    }

    template <typename entityType>
    struct interacter<potion, entityType> {
        static inline interaction_info interact(potion &inted, entityType &inting) {
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                return default_interactors::item_picking<potion, entityType>::interact(inted, inting);
            } else {
                return default_interactors::logging<potion, entityType>::interact(inted, inting);
            }
        }
    };
}  // namespace roguelike
#endif  // ROGUE_LIKE_ITEMS_H
