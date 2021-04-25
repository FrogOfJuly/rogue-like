//
// Created by Kirill Golubev on 24.04.2021.
//
#include "room.h"

#include <iostream>

#include "../utility/entity_info.h"

std::optional<roguelike::tile> roguelike::room::get_tile_if_exists(int x, int y) noexcept {
    if (x < 0 or y < 0) {
        return std::optional<tile>();
    }
    if (x >= W or y >= H) {
        return std::optional<tile>();
    }
    auto des_tile_idx = x + y * W;
    if (des_tile_idx < H * W) {
        return std::optional<tile>(tiles[des_tile_idx]);
    }
    return std::optional<tile>();
}
roguelike::tile& roguelike::room::get_tile(int x, int y) {
    auto des_tile_idx = x + y * W;
    return tiles[des_tile_idx];
}
const roguelike::tile& roguelike::room::get_tile(int x, int y) const {
    auto des_tile_idx = x + y * W;
    return tiles[des_tile_idx];
}
roguelike::tile_idx roguelike::room::idxFromPair(int x, int y) { return x + y * W; }
std::pair<int, int> roguelike::room::pairFromIdx(roguelike::tile_idx idx) { return std::make_pair(idx % W, idx / W); }
roguelike::tile_idx roguelike::room::get_empty_tile() const {
    for (int i = 0; i < W * H; ++i) {
        if (not tiles[i].resident.has_value()) {
            return i;
        }
    }
    return -1;
}
void roguelike::room::spawn_on_level(entity_type& var_ent, roguelike::tile_idx tidx) {
    if (tidx == -1) {
        tidx = get_empty_tile();
    }
    if (tidx == -1) {
        throw std::runtime_error("Can't spawn. No empty tiles");
    }
    if (not tiles[tidx].empty()) {
        throw std::runtime_error("Can't spawn. Tile" + std::to_string(tidx) + " occupied");
    }
    std::visit(
        [this, tidx](auto* ent) {
            tiles[tidx].resident = ent->id;
            if constexpr (has_member_move_component<std::remove_pointer_t<decltype(ent)>>::value) {
                ent->m_cpt.residency = tidx;
                auto coords = pairFromIdx(tidx);
                ent->m_cpt.x = coords.first;
                ent->m_cpt.y = coords.second;
            }
        },
        var_ent);
}
void roguelike::room::generate_level(int lvl_num) {}
roguelike::room::~room() {
    for (auto& var_ent : residents) {
        std::visit(
            [](auto* ent_ptr) {
                if constexpr (std::is_same_v<player, std::decay_t<decltype(ent_ptr)>>) {
                    assert(false);
                }
                delete ent_ptr;
            },
            var_ent);
    }
}
