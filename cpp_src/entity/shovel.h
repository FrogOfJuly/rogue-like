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
        return u8"♩";
    }

    template <typename entityType>
    struct interacter<shovel, entityType> {
        static inline void interact(shovel &inted, entityType &inting) {
            constexpr bool has_log = has_member_logging_component<entityType>::value;
            if constexpr (has_member_simple_inventory_component<entityType>::value) {
                lwlog_info("somebody with inventory interacted with shovel");
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
                        inting.lg_cpt.log << "you can't pick shovel" << std::endl;
                    }
//                    inting.s_inv_cpt.spot.reset();
                    return;
                }
            }
            if constexpr (has_log) {
                std::string lg_entry = inted.pk_cpt.picked ? "you picked shovel" : "you interacted with shovel";
                inting.lg_cpt.log << lg_entry << std::endl;
            }
        }
    };
}  // namespace roguelike
#endif  // ROGUELIKE_SHOVEL_H
