//
// Created by Kirill Golubev on 02.05.2021.
//

#include "agressive_strategy.h"

#include "../../utility/utils.h"
void roguelike::agressive_strategy::form_decision() {
    if (view == nullptr) {
        decision = cmd::PASS;
        return;
    }
    view->center();
    for (const auto& c : view->view) {
        if (not c.observed_entity.has_value()) {
            continue;
        }
        bool is_player = std::visit(
            overloaded{[](player*) { return true; }, [](auto*) { return false; }}, c.observed_entity.value());
        if (not is_player) {
            continue;
        }
        // no pathfinding yet
        assert(c.x != 0);
        assert(c.y != 0);
        if (abs(c.x) > abs(c.y)) {
            if (c.x < 0) {
                decision = cmd::LEFT;
            } else {
                decision = cmd::RIGHT;
            }
        } else {
            if (c.y < 0) {
                decision = cmd::UP;
            } else {
                decision = cmd::DOWN;
            }
        }
        return;
    }
    decision = utils::get_random_active_action();
}
