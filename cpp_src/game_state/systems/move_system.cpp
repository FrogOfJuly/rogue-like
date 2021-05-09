//
// Created by Kirill Golubev on 24.04.2021.
//
#include "move_system.h"

#include "../../utility/entity_info.h"
#include "../gamestate.h"

void roguelike::move_system::move_to_tile(general_id id, tile_idx dest_tile_idx) {
    assert(game_ptr);

    auto var_ent = game_ptr->get_entity(id);
    auto pr = room ::pairFromIdx(dest_tile_idx);

    assert(game_ptr->level.get_tile_if_exists(pr.first, pr.second).has_value());
    game_ptr->level.tiles[dest_tile_idx].resident = id;
    std::visit(
        [this, dest_tile_idx, pr](auto *ent_ptr) -> void {
            using entT = std::remove_pointer_t<decltype(ent_ptr)>;
            std::optional<entity_id> dropped_item_id;
            if constexpr (has_member_simple_inventory_component<entT>::value) {
                if (ent_ptr->s_inv_cpt.spot.has_value()) {
                    dropped_item_id = std::visit(
                        [](auto *item_ent_ptr) -> entity_id {
                            using item_entT = std::remove_pointer_t<decltype(item_ent_ptr)>;

                            if constexpr (has_member_pickable_component<item_entT>::value) {
                                item_ent_ptr->pk_cpt.picked = false;  // now it is not picked
                            }
                            if constexpr (std::is_same_v<item_entT, player>) {
                                assert(false);  // player must never be in inventory spot :)
                                return entity_id{-1};
                            } else {
                                return item_ent_ptr->id;
                            }
                        },
                        ent_ptr->s_inv_cpt.spot.value());
                    ent_ptr->s_inv_cpt.spot.reset();
                }
            }
            if constexpr (has_member_move_component<entT>::value) {
                auto &m_cpt = ent_ptr->m_cpt;
                auto &old_tile = game_ptr->level.tiles.at(m_cpt.residency);
                old_tile.resident = dropped_item_id;
                m_cpt.residency = dest_tile_idx;
                m_cpt.x = pr.first;
                m_cpt.y = pr.second;
            }
        },
        var_ent);
}

roguelike::tile_idx roguelike::move_system::desired_tile_idx(entity_type &var_ent) {
    return std::visit(
        [](const auto *ent_ptr) {
            constexpr bool able_to_dm = has_member_decision_making_component<
                std::remove_pointer_t<std::remove_const_t<decltype(ent_ptr)>>>::value;
            constexpr bool able_to_move =
                has_member_move_component<std::remove_pointer_t<std::remove_const_t<decltype(ent_ptr)>>>::value;
            if constexpr (able_to_move and able_to_dm) {
                auto v = ent_ptr->dm_cpt.get_velocity();
                if (v.first == 0 and v.second == 0) {
                    return -1;
                }
                auto des_x = ent_ptr->m_cpt.x + v.first;
                auto des_y = ent_ptr->m_cpt.y + v.second;
                return room::idxFromPair(des_x, des_y);
            } else {
                return -1;
            }
        },
        var_ent);
}

bool roguelike::move_system::more_general_move(entity_type &var_ent) {
    auto des_idx = desired_tile_idx(var_ent);
    return std::visit(
        [this, des_idx](auto *ent_ptr) {
            bool moved = false;
            using entT = std::remove_pointer_t<decltype(ent_ptr)>;
            constexpr bool able_to_dm = has_member_decision_making_component<entT>::value;
            constexpr bool able_to_move = has_member_move_component<entT>::value;
            constexpr bool has_inventory = has_member_simple_inventory_component<entT>::value;

            if constexpr (able_to_dm and has_inventory) {
                auto active_slot = simple_inventory_component::inventory_spot::active;
                lwlog_info("Somebody has dm and inventory");
                bool has_active_slot = ent_ptr->s_inv_cpt.spots.count(active_slot) != 0;
                if (ent_ptr->dm_cpt.decision == cmd::ENTER and has_active_slot) {
                    lwlog_info("received enter command with nonempty spot");
                    auto var_spot_cnt = ent_ptr->s_inv_cpt.spots.at(active_slot);
                    bool used = std::visit(
                        [ent_ptr](auto *spot_cnt_ptr) {
                            using cntT = std::remove_pointer_t<decltype(spot_cnt_ptr)>;
                            if constexpr (has_member_one_time_effect_component<cntT>::value) {
                                lwlog_info("applying effect");
                                return spot_cnt_ptr->ot_eff_cpt.apply_effect(spot_cnt_ptr, ent_ptr);
                            }
                            return false;
                        },
                        var_spot_cnt);
                    if (used) {
                        ent_ptr->s_inv_cpt.spots.erase(active_slot);
                    }
                    return moved;
                }
            }

            if (des_idx < 0) {
                return false;
            }
            if constexpr (able_to_dm and able_to_move) {
                auto des_xy = room::pairFromIdx(des_idx);
                auto des_x = des_xy.first;
                auto des_y = des_xy.second;
                auto opt_tile = game_ptr->level.get_tile_if_exists(des_x, des_y);
                if (not opt_tile.has_value()) {
                    return moved;
                }
                auto tle = opt_tile.value();
                if (not tle.resident.has_value()) {
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
