//
// Created by Kirill Golubev on 17.04.2021.
//

#include <iostream>
#include "game_state/gamestate.h"

int main() {
    auto game = roguelike::gamestate();
    game.initialize(5);
    game.receive_player_command(1, roguelike::cmd::UP);
//    std::cout << game.get_serialization() << std::endl;
    game.move_players();
    game.interact();
//    std::cout << game.get_serialization() << std::endl;
}