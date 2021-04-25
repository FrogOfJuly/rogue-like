//
// Created by Kirill Golubev on 24.04.2021.
//
#include "move_system.h"

#include "../../utility/entity_info.h"
#include "../gamestate.h"

void roguelike::move_system::move_to_tile(general_id id, tile_idx dest_tile) {
    assert(game_ptr);
    auto pr = room ::pairFromIdx(dest_tile);
    auto &tile = game_ptr->level.get_tile(pr.first, pr.second);
    tile.resident = id;
    // pattern match for id
    // general_id@(Left entity_id)  <- take from player array
    // general_id@(Right player_id) <- take from level.residents array
    std::visit(
        overloaded{
            [this, pr, dest_tile](entity_id id) {
                // if taken from residents array then check for
                // all we need to check is existence of move component;
                // entry_ptr : std::variant<player*, goblin*, ... etc>
                std::visit(
                    [this, pr, dest_tile](auto &entity_ptr) {
                        auto &ent = *entity_ptr;
                        if constexpr (has_member_move_component<std::remove_reference_t<decltype(ent)>>::value) {
                            auto &ent_m_cpt = ent.m_cpt;
                            auto &old_tile = game_ptr->level.tiles[ent_m_cpt.residency];
                            old_tile.resident.reset();
                            ent_m_cpt.residency = dest_tile;
                            ent_m_cpt.x = pr.first;
                            ent_m_cpt.y = pr.second;
                        }
                    },
                    game_ptr->level.residents[id.value]);
            },
            [this, dest_tile, pr](player_id id) {
                auto &pl_m_cpt = game_ptr->players[id.value].m_cpt;
                auto &old_tile = game_ptr->level.tiles[pl_m_cpt.residency];
                old_tile.resident.reset();
                pl_m_cpt.residency = dest_tile;
                pl_m_cpt.x = pr.first;
                pl_m_cpt.y = pr.second;
            }},
        id);
}

bool roguelike::move_system::more_general_move(entity_type &var_ent) {
    return std::visit(
        [this](auto *ent_ptr) {
            bool moved = false;
            constexpr bool able_to_dm =
                has_member_decision_making_component<std::remove_pointer_t<decltype(ent_ptr)>>::value;
            constexpr bool able_to_move = has_member_move_component<std::remove_pointer_t<decltype(ent_ptr)>>::value;

            if constexpr (able_to_dm) {
                auto v = ent_ptr->dm_cpt.get_velocity();
                if (v.first == 0 and v.second == 0) {
                    return moved;
                }
                auto des_x = ent_ptr->m_cpt.x + v.first;
                auto des_y = ent_ptr->m_cpt.y + v.second;
                auto opt_tile = game_ptr->level.get_tile_if_exists(des_x, des_y);
                if (not opt_tile.has_value()) {
                    return moved;
                }
                auto tle = opt_tile.value();
                if (not tle.resident.has_value() and able_to_move) {
                    move_to_tile(ent_ptr->id, room::idxFromPair(des_x, des_y));
                    moved = true;
                } else {
                    general_id interacted_id = tle.resident.value();
                    general_id interacting_id = ent_ptr->id;
                    game_ptr->inter_system.push_interaction(interacted_id, interacting_id);
                }
                return moved;
            }
            return false;
        },
        var_ent);
}
