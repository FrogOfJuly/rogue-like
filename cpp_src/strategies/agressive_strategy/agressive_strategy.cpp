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

        bool has_line_of_sight = true;
        auto loc = utils::get_los({0, 0}, {c.x, c.y});
        for (const auto& p : loc) {
            auto maybe_tile =
                view->oracle->get_tile_if_exists(p.first + view->point_of_view.x, p.second + view->point_of_view.y);
            if (not maybe_tile.has_value()) {
                has_line_of_sight = false;
                break;
            }
            if (not maybe_tile.value().resident.has_value()) {
                continue;
            }
            auto maybe_ent_idx = std::visit(
                overloaded{
                    [](player_id id) { return std::optional<int>(); },
                    [](entity_id id) { return std::optional<int>(id.value); }},
                maybe_tile->resident.value());
            if (not maybe_ent_idx.has_value()) {
                continue;
            }
            const auto& var_ent = view->oracle->residents[maybe_ent_idx.value()];
            bool is_wall = std::visit(
                [](auto* entity_ptr) {
                    if constexpr (std::is_same_v<decltype(entity_ptr), wall*>) {
                        return true;
                    }
                    return false;
                },
                var_ent);
            if (is_wall) {
                has_line_of_sight = false;
                break;
            }
        }
        if (not has_line_of_sight) {
            continue;
        }
        auto charges_into = view->oracle->get_tile(c.x + view->point_of_view.x, c.y + view->point_of_view.y).resident;

        if (not dm_cpt.charges_into.has_value() or dm_cpt.charges_into != charges_into) {
            std::cout << "Kill the enemy!!" << std::endl;
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
        return;
    }
    dm_cpt.decision = cmd::PASS;
}
