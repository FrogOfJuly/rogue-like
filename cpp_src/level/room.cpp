//
// Created by Kirill Golubev on 24.04.2021.
//
#include "room.h"

#include <iostream>
#include <queue>
#include <unordered_set>
#include <variant>

#include "../utility/entity_info.h"
#include "../utility/utils.h"
#include "room_view.h"

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

roguelike::tile_idx roguelike::room::idxFromPair(int x, int y) {
    if (x >= W or y >= W or x < 0 or y < 0) {
        return -1;
    }
    return x + y * W;
}

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
            using entT = std::remove_pointer_t<decltype(ent)>;
            if constexpr (has_member_name_component<entT>::value) {
                lwlog_info("spawning %s on tile %d with id %d", ent->nm_cpt.name.c_str(), tidx, ent->id.value);
            }

            tiles[tidx].resident = ent->id;
            if constexpr (has_member_move_component<entT>::value) {
                ent->m_cpt.residency = tidx;
                auto coords = pairFromIdx(tidx);
                ent->m_cpt.x = coords.first;
                ent->m_cpt.y = coords.second;
            }
        },
        var_ent);
}

void roguelike::room::generate_terrain(int lvl_num) {
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

roguelike::tile_idx roguelike::room::get_random_empty_tile() const {
    tile_idx tile_num = rand() % (W * H);
    for (; tile_num < 2 * W * H; ++tile_num) {
        if (tiles[tile_num % (W * H)].empty()) {
            return tile_num % (W * H);
        }
    }
    return -1;
}

void roguelike::room::remove_resident(roguelike::tile_idx idx) { tiles[idx].resident.reset(); }

roguelike::room_view roguelike::room::get_area_around_tile(roguelike::tile_idx idx, int radius) const {
    auto p = pairFromIdx(idx);
    std::vector<room_view::cell> view;
    room_view::cell point_of_view{p.first, p.second, room_view::observation()};
    for (int i = 0; i < radius + 1; ++i) {
        auto idxes = utils::get_circle(i, p.first, p.second);
        for (const auto& idx_pair : idxes) {
            if (idx_pair.first == p.first and idx_pair.second == p.second) {
                continue;
            }
            auto maybe_tl = get_tile_if_exists(idx_pair.first, idx_pair.second);
            if (not maybe_tl.has_value()) {
                continue;
            }
            auto tl = maybe_tl.value();
            if (tl.empty()) {
                view.push_back(room_view::make_cell(room_view::observation(), idx_pair.first, idx_pair.second));
            } else {
                auto cell = std::visit(
                    overloaded{
                        [this, idx_pair](entity_id id) {
                            return room_view::make_cell(residents[id.value], idx_pair.first, idx_pair.second);
                        },
                        [idx_pair](player_id id) {
                            entity_type player_proxy = (player*)(nullptr);
                            return room_view::make_cell(player_proxy, idx_pair.first, idx_pair.second);
                        }},
                    tl.resident.value());
                view.push_back(cell);
            }
        }
    }
    return roguelike::room_view(view, point_of_view, this);
}

std::optional<roguelike::tile> roguelike::room::get_target_tile(
    roguelike::tile_idx idx, roguelike::cmd direction) const {
    auto p = pairFromIdx(idx);
    auto x = p.first, y = p.second;
    switch (direction) {
        case UP:
            return get_tile_if_exists(x, y - 1);
        case DOWN:
            return get_tile_if_exists(x, y + 1);
        case LEFT:
            return get_tile_if_exists(x - 1, y);
        case RIGHT:
            return get_tile_if_exists(x + 1, y);
        default:
            return get_tile_if_exists(x, y);
    }
}

bool roguelike::room::do_tiles_have_loc(std::pair<int, int> p0, std::pair<int, int> p1) const {
    utils::vec2d point_of_view{p0.first, p0.second};
    utils::vec2d tgt_point{p1.first, p1.second};
    tgt_point = tgt_point - point_of_view;
    auto loc = utils::get_los({0, 0}, {tgt_point.x, tgt_point.y});
    bool has_line_of_sight = true;
    for (const auto& p : loc) {
        auto maybe_tile = get_tile_if_exists(p.first + point_of_view.x, p.second + point_of_view.y);
        if (not maybe_tile.has_value()) {
            has_line_of_sight = false;
            break;
        }
        if (maybe_tile.value().empty()) {
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
        const auto& var_ent = residents[maybe_ent_idx.value()];
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
    return has_line_of_sight;
}

roguelike::entity_type roguelike::room::get_resident(roguelike::entity_id id) {
    auto res = residents.at(id.value);
    std::visit(
        [](auto* ent_ptr) {
            using entT = std::remove_pointer_t<decltype(ent_ptr)>;
            if constexpr (has_member_name_component<entT>::value) {
                lwlog_debug("getting entity with name %s", ent_ptr->nm_cpt.name.c_str());
            }
        },
        res);
    return res;
}

roguelike::const_entity_type roguelike::room::get_resident(roguelike::entity_id id) const {
    return std::visit(
        [](auto* ent_ptr) {
            using entT = std::remove_pointer_t<decltype(ent_ptr)>;
            if constexpr (has_member_name_component<entT>::value) {
                lwlog_debug("getting entity with name %s", ent_ptr->nm_cpt.name.c_str());
            }
            const_entity_type var_ent = ent_ptr;
            return var_ent;
        },
        residents.at(id.value));
}

void roguelike::room::generate_enemies(int lvl_num) {
    for (int i = 0; i < 3; ++i) {
        lwlog_info("placing goblin");
        auto new_id = residents.size();
        auto g = new goblin(new_id);
        g->dm_cpt.decision = DOWN;
        entity_type var_ent = g;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(g);
    }
    for (int i = 3; i < 5; ++i) {
        lwlog_info("placing goblin guard");
        auto new_id = residents.size();
        auto gg = new goblin_guard(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(gg);
    }
    for (int i = 5; i < 7; ++i) {
        lwlog_info("placing trap");
        auto new_id = residents.size();
        auto gg = new trap(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(gg);
    }
    for (int i = 7; i < 10; ++i) {
        lwlog_info("placing goblin worker");
        auto new_id = residents.size();
        auto gg = new goblin_worker(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(gg);
    }
    for (int i = 10; i < 12; ++i) {
        lwlog_info("placing potion");
        auto new_id = residents.size();
        auto gg = new potion(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(gg);
    }
    for (int i = 12; i < 14; ++i) {
        lwlog_info("placing sword");
        auto new_id = residents.size();
        auto gg = new sword(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(gg);
    }
    for (int i = 14; i < 16; ++i) {
        lwlog_info("placing shield");
        auto new_id = residents.size();
        auto gg = new shield(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(gg);
    }
    {
        int new_id = residents.size();
        lwlog_info("creating armor with id %d", new_id);
        auto gg = new armor(new_id);
        entity_type var_ent = gg;
        residents.emplace_back(gg);
        new_id = residents.size();
        lwlog_info("creating chest with id %d,  with armor in it", new_id);
        auto ch = new chest(new_id, gg);
        var_ent = ch;
        residents.emplace_back(ch);
        auto rnd_tile = get_random_empty_tile();
        lwlog_info("spawned chest on tile %d", rnd_tile);
        spawn_on_level(var_ent, rnd_tile);
    }
    {
        lwlog_info("placing shovel");
        auto new_id = residents.size();
        auto gg = new shovel(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = get_random_empty_tile();
        spawn_on_level(var_ent, rnd_tile);
        residents.emplace_back(gg);
    }
}
bool roguelike::room::is_tile_empty(roguelike::tile_idx idx) const {
    auto p = pairFromIdx(idx);
    auto maybe_tile = get_tile_if_exists(p.first, p.second);
    if (not maybe_tile.has_value()) {
        return false;
    }
    return maybe_tile.value().empty();
}
