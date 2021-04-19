//
// Created by Kirill Golubev on 20.04.2021.
//
#include "entity.hpp"

#ifndef ROGUE_LIKE_POTION_H
#define ROGUE_LIKE_POTION_H
namespace roguelike {
    struct potion : entity {
        repr_component repr_cpt = repr_component("p");
    };

    template<>
    struct interacter<potion, player> {
        static void interact(potion &inted, player &inting) {
            std::cout << "potion is interacted by player" << std::endl;
        }
    };
} // namespace roguelike
#endif //ROGUE_LIKE_POTION_H
