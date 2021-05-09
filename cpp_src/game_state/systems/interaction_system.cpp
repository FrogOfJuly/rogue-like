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

        std::visit(
            [](auto *left_ptr, auto *right_ptr) {
                using intedT = std::remove_pointer_t<decltype(left_ptr)>;
                using intingT = std::remove_pointer_t<decltype(right_ptr)>;
                interacter<intedT, intingT>::interact(*left_ptr, *right_ptr);
            },
            interacting_pair.first,
            interacting_pair.second);

        auto des_idx = move_system::desired_tile_idx(interacting_pair.second);
        bool is_dead = std::visit(
            [](auto *inted_ent_ptr) {
                if constexpr (has_member_health_component<std::remove_pointer_t<decltype(inted_ent_ptr)>>::value) {
                    return not health_component::is_alive(inted_ent_ptr);
                }
                return false;
            },
            interacting_pair.first);
        std::optional<entity_type> item_in_tmp_spot = std::visit(
            [](auto *ent_ptr) {
                using entT = std::remove_pointer_t<decltype(ent_ptr)>;
                if constexpr (has_member_simple_inventory_component<entT>::value) {
                    return ent_ptr->s_inv_cpt.spot;
                }
                return std::optional<entity_type>();
            },
            interacting_pair.first);
        if (is_dead) {
            game_ptr->level.remove_resident(des_idx);
            game_ptr->report_murder(idx_pair.first, idx_pair.second);
            auto &inting_ent = interacting_pair.second;
            if (item_in_tmp_spot.has_value()) {
                game_ptr->level.spawn_on_level(item_in_tmp_spot.value(), des_idx);
            } else {
                game_ptr->mv_system.more_general_move(inting_ent);
            }
        }
        bool is_picked = std::visit(
            [](auto *inted_ent_ptr) {
                if constexpr (has_member_pickable_component<std::remove_pointer_t<decltype(inted_ent_ptr)>>::value) {
                    return inted_ent_ptr->pk_cpt.picked;
                }
                return false;
            },
            interacting_pair.first);
        if (is_picked) {
            game_ptr->level.remove_resident(des_idx);
            auto &inting_ent = interacting_pair.second;
            game_ptr->mv_system.more_general_move(inting_ent);
        }

        bool is_wall = std::holds_alternative<wall *>(interacting_pair.first);
        bool has_shovel = std::visit(
            [](auto *ent_ptr) {
                using entT = std::remove_pointer_t<decltype(ent_ptr)>;
                if constexpr (has_member_simple_inventory_component<entT>::value) {
                    bool has_of_item =
                        ent_ptr->s_inv_cpt.spots.count(simple_inventory_component::inventory_spot::offence) != 0;
                    if (not has_of_item) {
                        return false;
                    }
                    auto of_item = ent_ptr->s_inv_cpt.spots.at(simple_inventory_component::inventory_spot::offence);
                    bool is_shovel = std::holds_alternative<shovel *>(of_item);
                    return is_shovel;
                }
                return false;
            },
            interacting_pair.second);
        if (is_wall and has_shovel) {
            game_ptr->level.remove_resident(des_idx);
        }
    }
}
