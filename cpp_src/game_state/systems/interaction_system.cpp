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

        auto interacting_pair = std::visit(
            overloaded{
                [&players](player_id left_id, player_id right_id) {
                    entity_type left_player = &players[left_id.value];
                    entity_type right_player = &players[right_id.value];
                    return std::make_pair(left_player, right_player);
                },
                [&players, &residents](entity_id left_id, player_id right_id) {
                    entity_type right_player = &players[right_id.value];
                    return std::make_pair(residents[left_id.value], right_player);
                },
                [&players, &residents](player_id left_id, entity_id right_id) {
                    entity_type left_player = &players[left_id.value];
                    return std::make_pair(left_player, residents[right_id.value]);
                },
                [&residents](entity_id left_id, entity_id right_id) {
                    return std::make_pair(residents[left_id.value], residents[right_id.value]);
                },
            },
            idx_pair.first,
            idx_pair.second);

        std::visit(
            [](auto* left_ptr, auto* right_ptr) {
                interacter<std::remove_pointer_t<decltype(left_ptr)>, std::remove_pointer_t<decltype(right_ptr)>>::
                    interact(*left_ptr, *right_ptr);
            },
            interacting_pair.first,
            interacting_pair.second);

        auto des_idx = move_system::desired_tile_idx(interacting_pair.second);
        std::visit(
            [this, &interacting_pair, des_idx](auto *inted_ent_ptr) {
                if constexpr (has_member_health_component<std::remove_pointer_t<decltype(inted_ent_ptr)>>::value) {
                    if (not health_component::is_alive(inted_ent_ptr)) {
                        game_ptr->level.remove_resident(des_idx);
                        general_id inted_id = inted_ent_ptr->id;
                        std::visit(
                            overloaded{
                                [this](player_id id) {
                                    game_ptr->dead_players.insert(id.value);
                                    game_ptr->players[id.value].lg_cpt.log << "you are dead!\n";
                                },
                                [this](entity_id id) {
                                    game_ptr->level.dead.insert(id.value);
                                }},
                            inted_id);

                        game_ptr->mv_system.more_general_move(interacting_pair.second);
                    }
                }
            },
            interacting_pair.first);
        // game_ptr->mv_system.more_general_move()
    }
}
