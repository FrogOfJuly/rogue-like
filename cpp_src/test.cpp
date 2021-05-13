//
// Created by Kirill Golubev on 17.04.2021.
//

#include <iostream>

#include "game_state/gamestate.h"

int main() {
    roguelike::gamestate game;
    game.initialize(1);
    game.receive_player_command(0, roguelike::cmd::UP);

    game.move_players();
    game.resolve_all_interactions();
    std::cout << game.get_serialization() << std::endl;
}