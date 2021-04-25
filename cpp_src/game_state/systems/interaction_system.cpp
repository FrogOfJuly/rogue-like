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
    auto& players = game_ptr->players;
    auto& residents = game_ptr->level.residents;
    while (not interactions.empty()) {
        entity_pair idx_pair = interactions.front();
        interactions.pop();
        //                      variational-------v---------------v
        auto perform_interaction = [](auto &interacted, auto &interacting) {
            std::visit(
                [](auto &left_ptr, auto &right_ptr) {
                    auto &left = *left_ptr;
                    auto &right = *right_ptr;
                    interacter<std::remove_reference_t<decltype(left)>, std::remove_reference_t<decltype(right)>>::
                        interact(left, right);
                },
                interacted,
                interacting);
        };

        std::visit(
            overloaded{
                [perform_interaction, &players](player_id left_id, player_id right_id) {
                    entity_type left_player = &players[left_id.value];
                    entity_type right_player = &players[right_id.value];
                    perform_interaction(left_player, right_player);
                },
                [perform_interaction, &players, &residents](entity_id left_id, player_id right_id) {
                    entity_type right_player = &players[right_id.value];
                    perform_interaction(residents[left_id.value], right_player);
                },
                [perform_interaction, &players, &residents](player_id left_id, entity_id right_id) {
                    entity_type left_player = &players[left_id.value];
                    perform_interaction(left_player, residents[right_id.value]);
                },
                [perform_interaction, &residents](entity_id left_id, entity_id right_id) {
                    perform_interaction(residents[left_id.value], residents[right_id.value]);
                },
            },
            idx_pair.first,
            idx_pair.second);
    }
}
