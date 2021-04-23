//
// Created by Kirill Golubev on 17.04.2021.
//

#include <optional>
#include <variant>
#include <vector>

#include "../entity/entity.hpp"
#include "../utility/all_entities.h"
#include "tile.h"

#ifndef ROGUE_LIKE_LEVEL_H
#define ROGUE_LIKE_LEVEL_H
namespace roguelike {
    using entity_type = std::variant<
#define register_entity(entity_type_name, repr_function) entity_type_name *,
    // hehehehhehehehehe!! It works!!
#include "../utility/register_for_entities.h"

#undef register_entity
        entity *>;

    void to_json(nlohmann::json &j, const entity_type &p) {
        j = std::visit(
            [](auto &ent_ptr) {
                auto j_local = nlohmann::json();
                to_json(j_local, *ent_ptr);
                return j_local;
            },
            p);
    }

    void from_json(const nlohmann::json &j, entity_type &p) {
        throw std::runtime_error("One CAN NOT construct entity type from it's serialization");
    }

    template <int H, int W>
    struct room {
        std::array<tile, H * W> tiles;

        std::vector<entity_type> residents;

       public:
        room() = default;

        std::optional<tile> get_tile_if_exists(int x, int y) noexcept {
            if (x < 0 or y < 0) {
                return std::optional<tile>();
            }
            auto des_tile_idx = x + y * W;
            if (des_tile_idx < H * W) {
                return std::optional<tile>(tiles[des_tile_idx]);
            }
            return std::optional<tile>();
        }

        tile &get_tile(int x, int y) {
            auto des_tile_idx = x + y * W;
            return tiles[des_tile_idx];
        }

        const tile &get_tile(int x, int y) const {
            auto des_tile_idx = x + y * W;
            return tiles[des_tile_idx];
        }

        static tile_idx idxFromPair(int x, int y) { return x + y * W; }

        static std::pair<int, int> pairFromIdx(tile_idx idx) { return std::make_pair(idx % W, idx / W); }

        tile_idx get_empty_tile() const {
            for (int i = 0; i < W * H; ++i) {
                if (not tiles[i].resident.has_value()) {
                    return i;
                }
            }
            return -1;
        }

        template <typename SpawnType>
        void spawn_on_level(SpawnType &ent, tile_idx tidx = -1) {
            if (tidx == -1) {
                tidx = get_empty_tile();
            }
            if (tidx == -1) {
                throw std::runtime_error("Can't spawn player. No empty tiles");
            }
            if (not tiles[tidx].empty()) {
                throw std::runtime_error("Can't spawn player. Tile" + std::to_string(tidx) + " occupied");
            }
            tiles[tidx].resident = ent.id;
            if constexpr (has_member_move_component<SpawnType>::value) {
                ent.m_cpt.residency = tidx;
                auto coords = pairFromIdx(tidx);
                ent.m_cpt.x = coords.first;
                ent.m_cpt.y = coords.second;
            }
        }

        void generate_level(int lvl_num) {}

        room(const room &) = delete;

        room &operator=(const room &) = delete;

        room(room &&) = default;

        room &operator=(room &&) = default;

        ~room() {
            for (const auto &var_ent : residents) {
                std::visit(
                    [](auto *ent_ptr) {
                        if constexpr (std::is_same_v<player, std::decay_t<decltype(ent_ptr)>>) {
                            assert(false);
                        }
                        delete ent_ptr;
                    },
                    var_ent);
            }
        }
        /*
        void spawn_enemy(int tile_idx) {
            if (not tiles[tile_idx].empty()) {
                throw std::runtime_error("Can't spawn enemy here: " + std::to_string(tile_idx));
            }
        }*/
        /*
         * generate_enemies(int lvl_num)
         * generate_loot(int lvl_num)
         * generate_walls(int lvl_num)
         * generate_traps(int lvl_num)
         * generate_doors_and_keys(int lvl_num)
         */
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_LEVEL_H
