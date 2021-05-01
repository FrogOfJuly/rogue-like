//
// Created by Kirill Golubev on 01.05.2021.
//

#include "abstract_strategy.h"

#include "../level/room_view.h"

roguelike::cmd roguelike::strategy::get_decision() const { return decision; }
void roguelike::strategy::set_view(const roguelike::room_view& _view) { this->view = std::make_unique<room_view>(_view); }
void roguelike::strategy::discard_view() { this->view.reset(); }
