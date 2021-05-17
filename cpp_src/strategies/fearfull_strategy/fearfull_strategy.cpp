//
// Created by Kirill Golubev on 05.05.2021.
//

#include "fearfull_strategy.h"

#include "../../level/room.h"
#include "../../utility/entity_info.h"
#include "../../utility/utils.h"

void roguelike::fearfull_strategy::form_decision(roguelike::decision_making_component &dm_cpt) {
    if (view == nullptr) {
        dm_cpt.decision = cmd::PASS;
        return;
    }
    view->center();
    for (const auto &c : view->view) {
        if (not c.observed_entity.has_value()) {
            continue;
        }
        if (not std::holds_alternative<player *>(c.observed_entity.value())) {
            continue;
        }

        bool has_line_of_sight = view->oracle->do_tiles_have_loc(
            {view->point_of_view.x, view->point_of_view.y}, {c.x + view->point_of_view.x, c.y + view->point_of_view.y});

        if (not has_line_of_sight) {
            continue;
        }
        dm_cpt.charges_into = view->oracle->get_tile(c.x + view->point_of_view.x, c.y + view->point_of_view.y).resident;
        // no pathfinding yet
        assert(c.x != 0 or c.y != 0) ;
        if (abs(c.x) > abs(c.y)) {
            if (c.x < 0) {
                dm_cpt.decision = cmd::RIGHT;
            } else {
                dm_cpt.decision = cmd::LEFT;
            }
        } else {
            if (c.y < 0) {
                dm_cpt.decision = cmd::DOWN;
            } else {
                dm_cpt.decision = cmd::UP;
            }
        }
        auto wall_ahead = view->oracle->do_target_tile_have<wall>(
            room::idxFromPair(view->point_of_view.x, view->point_of_view.y), dm_cpt.decision);

        if (wall_ahead) {
            if (dm_cpt.decision == cmd::UP or dm_cpt.decision == cmd::DOWN) {
                dm_cpt.decision = rand() % 2 == 0 ? cmd::LEFT : cmd::RIGHT;
            } else {
                dm_cpt.decision = rand() % 2 == 0 ? cmd::UP : cmd::DOWN;
            }
        }
        return;
    }
    dm_cpt.decision = cmd::PASS;
    dm_cpt.charges_into.reset();
}