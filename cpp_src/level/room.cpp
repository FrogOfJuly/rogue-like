//
// Created by Kirill Golubev on 24.04.2021.
//
#include "room.h"

#include <iostream>
#include <queue>
#include <unordered_set>

#include "../utility/entity_info.h"

std::optional<roguelike::tile> roguelike::room::get_tile_if_exists(int x, int y) const noexcept {
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
void roguelike::room::generate_level(int lvl_num) {
    using chunkType = std::array<int, 25>;
    const static int chunk_size = 5;

    const static chunkType chunk1 = {1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1};
    const static chunkType chunk2 = {0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1};
    const static chunkType chunk3 = {0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1};
    const static chunkType chunk4 = {1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1};
    const static chunkType chunk5 = {1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1};
    const static chunkType chunk6 = {1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 1};
    const static chunkType chunk7 = {1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1};
    const static chunkType cent_chunk = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    auto choose_random_chunk = [&]() -> chunkType {
        switch (abs(rand()) % 7) {
            case 0:
                return chunk1;
            case 1:
                return chunk2;
            case 2:
                return chunk3;
            case 3:
                return chunk4;
            case 4:
                return chunk5;
            case 5:
                return chunk6;
            default:
                return chunk7;
        }
    };

    auto fill_level = [this](tile_idx start, chunkType chunk) -> void {
        auto p = pairFromIdx(start);
        auto x = p.first, y = p.second;
        for (int i = 0; i < chunk_size; ++i) {
            for (int j = 0; j < chunk_size; ++j) {
                if (chunk[i + chunk_size * j] == 1) {
                    int id = residents.size();
                    auto* w = new wall(id);
                    entity_type var_ent = w;
                    residents.push_back(var_ent);
                    tiles[x + i + W * (y + j)].resident = w->id;
                }
            }
        }
    };

    for (int i = 0; i < W / chunk_size; ++i) {
        for (int j = 0; j < H / chunk_size; ++j) {
            chunkType cur_chunk;
            if (i == W / chunk_size / 2 and j == H / chunk_size / 2) {
                cur_chunk = cent_chunk;
            } else {
                cur_chunk = choose_random_chunk();
            }
            fill_level(i * chunk_size + W * (j * chunk_size), cur_chunk);
        }
    }
    for (int i = 0; i < W; ++i) {
        for (int j = 0; j < H; ++j) {
            if (i == 0 or j == 0 or i == W - 1 or j == H - 1) {
                int id = residents.size();
                auto* w = new wall(id);
                entity_type var_ent = w;
                residents.push_back(var_ent);
                tiles[i + W * j].resident = w->id;
            }
        }
    }
}
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
std::array<roguelike::tile_idx, 4> roguelike::room::get_tile_neighbours(roguelike::tile_idx idx) const noexcept {
    std::array<tile_idx, 4> neighbours{-1, -1, -1, -1};
    auto p = pairFromIdx(idx);
    auto x = p.first, y = p.second;
    {
        auto opt_tile = get_tile_if_exists(x, y - 1);
        if (opt_tile.has_value()) {
            neighbours[0] = idxFromPair(x, y - 1);
        }
    }
    {
        auto opt_tile = get_tile_if_exists(x + 1, y);
        if (opt_tile.has_value()) {
            neighbours[1] = idxFromPair(x + 1, y);
        }
    }
    {
        auto opt_tile = get_tile_if_exists(x + 1, y + 1);
        if (opt_tile.has_value()) {
            neighbours[2] = idxFromPair(x + 1, y + 1);
        }
    }
    {
        auto opt_tile = get_tile_if_exists(x - 1, y);
        if (opt_tile.has_value()) {
            neighbours[3] = idxFromPair(x - 1, y);
        }
    }
    return neighbours;
}
std::optional<roguelike::tile_idx> roguelike::room::get_random_empty_tile() const {
    tile_idx tile_num = rand() % (W * H);
    for (; tile_num < 2 * W * H; ++tile_num) {
        if (tiles[tile_num%(W*H)].empty()) {
            return tile_num;
        }
    }
    return std::optional<roguelike::tile_idx>();
}
/*std::vector<roguelike::tile_idx> roguelike::room::get_area_around_tile(roguelike::tile_idx idx, int radius) {
    std::vector<tile_idx> area;
    assert(radius > 0);
    if (idx >= tiles.size()) {
        return area;
    }
    std::unordered_set<tile_idx> visited;
    visited.insert(idx);
    std::queue<tile_idx> to_visit;
    auto neigh = get_tile_neighbours(idx);
    for (const auto& n_idx : neigh) {
        to_visit.push(n_idx);
    }
    while (not to_visit.empty()) {
        auto cur_idx = to_visit.front();
        if (visited.count(cur_idx) != 0) {
            continue;
        }
        area.push_back(cur_idx);
    }
}*/
