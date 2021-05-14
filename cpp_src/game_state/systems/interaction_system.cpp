//
// Created by Kirill Golubev on 24.04.2021.
//

#include "interaction_system.h"

#include "../../utility/entity_info.h"
#include "../gamestate.h"

void roguelike::interaction_system::perform_interaction(entity_type &interacted, entity_type &interacting) {
    std::visit(
        [](auto &left_ptr, auto &right_ptr) {
            lwlog_info("calling interaction method");
            assert(left_ptr);
            assert(right_ptr);
            auto &left = *left_ptr;
            auto &right = *right_ptr;
            interacter<std::remove_reference_t<decltype(left)>, std::remove_reference_t<decltype(right)>>::interact(
                left, right);
        },
        interacted,
        interacting);
}
void roguelike::interaction_system::push_interaction(general_id inted, general_id inting) {
    interactions.emplace(inted, inting);
}
void roguelike::interaction_system::resolve_all_interactions() {
    lwlog_info("there are %lu interctions to perform", interactions.size());
    auto &players = game_ptr->players;
    auto &residents = game_ptr->level.residents;
    while (not interactions.empty()) {
        entity_pair idx_pair = interactions.front();
        interactions.pop();

        auto interacting_pair =
            std::make_pair(game_ptr->get_entity(idx_pair.first), game_ptr->get_entity(idx_pair.second));

        auto interaction_res = std::visit(
            [](auto *left_ptr, auto *right_ptr) {
                using intedT = std::remove_pointer_t<decltype(left_ptr)>;
                using intingT = std::remove_pointer_t<decltype(right_ptr)>;
                return interacter<intedT, intingT>::interact(*left_ptr, *right_ptr);
            },
            interacting_pair.first,
            interacting_pair.second);

        auto maybe_des_idx = move_system::desired_tile_idx(interacting_pair.second);
        auto maybe_cur_idx = move_system::current_tile_idx(interacting_pair.second);

        std::optional<entity_type> item_in_tmp_spot = std::visit(
            [](auto *ent_ptr) {
                using entT = std::remove_pointer_t<decltype(ent_ptr)>;
                if constexpr (has_member_simple_inventory_component<entT>::value) {
                    return ent_ptr->s_inv_cpt.spot;
                }
                return std::optional<entity_type>();
            },
            interacting_pair.first);
        lwlog_crit(
            "interaction resolved! {%d, %d, %d}",
            interaction_res.consolidate,
            interaction_res.remove_interacted,
            interaction_res.remove_interacting);

        if (interaction_res.remove_interacted and maybe_des_idx.has_value()) {
            lwlog_crit("removing the one who has been interacted");
            game_ptr->level.remove_resident(maybe_des_idx.value());
            game_ptr->report_despawn(idx_pair.first);
            if (item_in_tmp_spot.has_value()) {
                game_ptr->level.spawn_on_level(item_in_tmp_spot.value(), maybe_des_idx.value());
            }
        }
        if (interaction_res.consolidate) {
            lwlog_crit("consolidating");
            game_ptr->mv_system.more_general_move(interacting_pair.second);
        }

        if (interaction_res.remove_interacting and maybe_cur_idx.has_value()) {
            lwlog_crit("removing the one who has been interacting");
            game_ptr->level.remove_resident(maybe_cur_idx.value());
            game_ptr->report_despawn(idx_pair.second);
        }
    }
}
