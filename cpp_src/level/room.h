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

      public:
        room() = default;

        [[nodiscard]] std::optional<tile> get_tile_if_exists(int x, int y) const noexcept;

        [[nodiscard]] std::array<tile_idx, 4> get_tile_neighbours(tile_idx idx) const noexcept;

        [[nodiscard]] tile_idx get_random_empty_tile() const;

        void remove_resident(tile_idx idx);

        tile &get_tile(int x, int y);

        [[nodiscard]] const tile &get_tile(int x, int y) const;

        static tile_idx idxFromPair(int x, int y);

        static std::pair<int, int> pairFromIdx(tile_idx idx);

        [[nodiscard]] tile_idx get_empty_tile() const;

        void spawn_on_level(entity_type &ent, tile_idx tidx = -1);

        void generate_level(int lvl_num);

        [[nodiscard]] room_view get_area_around_tile(tile_idx idx, int radius) const;

        room(const room &) = delete;

        room &operator=(const room &) = delete;

        room(room &&) = default;

        room &operator=(room &&) = default;

        ~room();
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_LEVEL_H
