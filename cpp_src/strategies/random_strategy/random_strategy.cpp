//
// Created by Kirill Golubev on 02.05.2021.
//

#include "random_strategy.h"
void roguelike::random_strategy::form_decision() {
    switch (rand() % 5) {
        case 0:
            decision = cmd::PASS;
            break;
        case 1:
            decision = cmd::UP;
            break;
        case 2:
            decision = cmd::DOWN;
            break;
        case 3:
            decision = cmd::LEFT;
            break;
        case 4:
            decision = cmd::RIGHT;
            break;
    }
}
