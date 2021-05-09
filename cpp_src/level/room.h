//
// Created by Kirill Golubev on 17.04.2021.
//

#include <optional>
#include <unordered_set>
#include <vector>

#include "../common.h"
#include "tile.h"

#ifndef ROGUE_LIKE_LEVEL_H
#define ROGUE_LIKE_LEVEL_H
namespace roguelike {
    constexpr int room_size = 15;
    struct room_view;
    struct room {
        static constexpr int H = room_size;
        static constexpr int W = room_size;

        std::array<tile, H * W> tiles;
        std::vector<entity_type> residents;
        std::unordered_set<int> dead;

        mutable std::stringstream common_log;

      public:
        room() = default;

        [[nodiscard]] std::optional<tile> get_tile_if_exists(int x, int y) const noexcept;

        [[nodiscard]] std::array<tile_idx, 4> get_tile_neighbours(tile_idx idx) const noexcept;

        [[nodiscard]] std::optional<tile> get_target_tile(tile_idx idx, cmd direction) const;

        template <typename entityT>
        [[nodiscard]] inline bool do_target_tile_have(tile_idx idx, cmd direction) const {
            auto maybe_tile = get_target_tile(idx, direction);
            if (not maybe_tile.has_value()) {
                // no such tile
                return false;
            }
            if (not maybe_tile.value().resident.has_value()) {
                // nobody on the tile
                return false;
            }
            if (std::is_same_v<entityT, player> and
                std::holds_alternative<player_id>(maybe_tile.value().resident.value())) {
                // you are checking for the player and resident has player id?
                return true;
            }
            auto maybe_ent_idx = std::visit(
                overloaded{
                    [](player_id) { return std::optional<entity_id>(); },
                    [](entity_id id) { return std::optional<entity_id>(id); }},
                maybe_tile.value().resident.value());

            if (not maybe_ent_idx.has_value()) {
                // there are either player or nobody on the tile, but player was checked earlier
                return false;
            }

            auto var_ent = get_resident(maybe_ent_idx.value());
            // is it holding somebody you are looking for?
            bool is_ent = std::holds_alternative<entityT *>(var_ent);
            return is_ent;
        }

        [[nodiscard]] bool do_tiles_have_loc(std::pair<int, int> p0, std::pair<int, int> p1) const;

        [[nodiscard]] tile_idx get_random_empty_tile() const;

        [[nodiscard]] entity_type get_resident(entity_id id) const;

        void remove_resident(tile_idx idx);

        tile &get_tile(int x, int y);

        [[nodiscard]] const tile &get_tile(int x, int y) const;

        static tile_idx idxFromPair(int x, int y);

        static std::pair<int, int> pairFromIdx(tile_idx idx);

        [[nodiscard]] tile_idx get_empty_tile() const;

        void spawn_on_level(entity_type &ent, tile_idx tidx = -1);

        void generate_terrain(int lvl_num);

        void generate_enemies(int lvl_num);

        [[nodiscard]] room_view get_area_around_tile(tile_idx idx, int radius) const;

        room(const room &) = delete;

        room &operator=(const room &) = delete;

        room(room &&) = default;

        room &operator=(room &&) = default;

        ~room();
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_LEVEL_H
