//
// Created by Kirill Golubev on 17.04.2021.
//

#include <optional>
#include <vector>

#include "../common.h"
#include "tile.h"

#ifndef ROGUE_LIKE_LEVEL_H
#define ROGUE_LIKE_LEVEL_H
namespace roguelike {
    constexpr int room_size = 4;

    struct room {
        static constexpr int H = room_size;
        static constexpr int W = room_size;

        std::array<tile, H * W> tiles;
        std::vector<entity_type> residents;

      public:
        room() = default;

        std::optional<tile> get_tile_if_exists(int x, int y) noexcept;

        tile &get_tile(int x, int y);

        [[nodiscard]] const tile &get_tile(int x, int y) const;

        static tile_idx idxFromPair(int x, int y);

        static std::pair<int, int> pairFromIdx(tile_idx idx);

        [[nodiscard]] tile_idx get_empty_tile() const;

        void spawn_on_level(entity_type &ent, tile_idx tidx = -1);

        void generate_level(int lvl_num);

        room(const room &) = delete;

        room &operator=(const room &) = delete;

        room(room &&) = default;

        room &operator=(room &&) = default;

        ~room();
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

    //    using default_room = room<room_size, room_size>;
}  // namespace roguelike

#endif  // ROGUE_LIKE_LEVEL_H
