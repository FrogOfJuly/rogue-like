//
// Created by Kirill Golubev on 02.05.2021.
//

#include "agressive_strategy.h"

#include "../../level/room.h"
#include "../../utility/entity_info.h"
#include "../../utility/utils.h"

void roguelike::agressive_strategy::form_decision(roguelike::decision_making_component& dm_cpt) {
    if (view == nullptr) {
        dm_cpt.decision = cmd::PASS;
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

        bool has_line_of_sight = view->oracle->do_tiles_have_loc(
            {view->point_of_view.x, view->point_of_view.y}, {c.x + view->point_of_view.x, c.y + view->point_of_view.y});

        if (not has_line_of_sight) {
            continue;
        }
        auto charges_into = view->oracle->get_tile(c.x + view->point_of_view.x, c.y + view->point_of_view.y).resident;

        if (not dm_cpt.charges_into.has_value() or dm_cpt.charges_into != charges_into) {
            std::visit(
                overloaded{
                    [this](player_id id) {
                        view->oracle->common_log << "player" << id.value << " is charged by an enemy" << std::endl;
                    },
                    [](entity_id id) {}},
                charges_into.value());
        }
        dm_cpt.charges_into = charges_into;
        // no pathfinding yet
        assert(c.x != 0);
        assert(c.y != 0);
        if (abs(c.x) > abs(c.y)) {
            if (c.x < 0) {
                dm_cpt.decision = cmd::LEFT;
            } else {
                dm_cpt.decision = cmd::RIGHT;
            }
        } else {
            if (c.y < 0) {
                dm_cpt.decision = cmd::UP;
            } else {
                dm_cpt.decision = cmd::DOWN;
            }
        }
        auto wall_ahead = view->oracle->do_target_tile_have_wall(
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
