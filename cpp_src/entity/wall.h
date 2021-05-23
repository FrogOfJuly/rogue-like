//
// Created by Kirill Golubev on 25.04.2021.
//

#ifndef ROGUE_LIKE_WALL_H
#define ROGUE_LIKE_WALL_H
namespace roguelike {
    struct wall {
        wall(int id) : id{id} { nm_cpt.name = "wall"; }
        entity_id id;
        repr_component repr_cpt;
        name_component nm_cpt;
        destruction_component dstr_cpt;
    };

    template <>
    inline std::string repr_component::compute_representation<wall>(const wall *p) {
        return "█";
    }
    template <>
    inline bool destruction_component::destroyed_on_impact<wall, player>(wall *impacted, player *impacting) {
        auto of_spot = simple_inventory_component::inventory_spot::sword;
        bool has_off_tool = impacting->s_inv_cpt.spots.count(of_spot) != 0;
        bool has_shovel = has_off_tool ? holds_alternative<shovel *>(impacting->s_inv_cpt.spots.at(of_spot)) : false;
        return has_shovel;
    }

    template <>
    struct interacter<wall, player> {
        static inline interaction_info interact(wall *inted, player *inting) {
            if (destruction_component::destroyed_on_impact(inted, inting)) {
                inted->dstr_cpt.destroyed = true;
                inting->lg_cpt.log << "you destroyed the wall\n";
                return interaction_info::interacted_got_destroyed;
            }
            inting->lg_cpt.log << "you interacted with a wall\n";
            return interaction_info::nothing_happened;
        }
    };
}  // namespace roguelike
#endif  // ROGUE_LIKE_WALL_H
