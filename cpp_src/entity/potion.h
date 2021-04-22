//
// Created by Kirill Golubev on 20.04.2021.
//
#include "entity.hpp"

#ifndef ROGUE_LIKE_POTION_H
#define ROGUE_LIKE_POTION_H
namespace roguelike {
    struct potion {
        entity_id id;
        repr_component repr_cpt = repr_component("p");
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(potion, id, repr_cpt);

    template<>
    struct interacter<potion, player> {
        static void interact(potion &inted, player &inting) {
            inting.lg_cpt.log << "you interacted with potion\n";
        }
    };
} // namespace roguelike
#endif //ROGUE_LIKE_POTION_H
