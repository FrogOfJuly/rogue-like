//
// Created by Kirill Golubev on 17.04.2021.
//

#include <iostream>
#include "entity/entity.hpp"
#include "entity/player.h"
#include "level/room.hpp"
#include "game_state/gamestate.h"


int main() {
    auto game = roguelike::gamestate();
    game.initialize(1);
}