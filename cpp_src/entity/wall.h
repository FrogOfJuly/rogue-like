//
// Created by Kirill Golubev on 25.04.2021.
//

#ifndef ROGUE_LIKE_WALL_H
#define ROGUE_LIKE_WALL_H
namespace roguelike {
    struct wall {
        wall(int id) : id{id} {
            nm_cpt.name = "wall";
        }
        entity_id id;
        repr_component repr_cpt;
        name_component nm_cpt;
    };

    template<>
    inline std::string repr_component::compute_representation<wall>(const wall* p){
        return "█";
    }

    template <>
    struct interacter<wall, player> {
        static inline void interact(wall &inted, player &inting) { inting.lg_cpt.log << "you interacted with wall\n"; }
    };
}  // namespace roguelike
#endif  // ROGUE_LIKE_WALL_H
