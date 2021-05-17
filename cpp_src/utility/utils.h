//
// Created by Kirill Golubev on 01.05.2021.
//

#include <array>
#include <vector>

#include "../common.h"

#ifndef ROGUE_LIKE_UTILS_H
#define ROGUE_LIKE_UTILS_H

namespace roguelike::utils {

    struct vec2d {
        int x, y;
        [[nodiscard]] std::pair<int, int> to_pair() const;
    };
    vec2d operator+(const vec2d& lhs, const vec2d& rhs);
    vec2d operator-(const vec2d& lhs, const vec2d& rhs);
    vec2d operator-(const vec2d& rhs);
    vec2d operator*(int c, const vec2d& rhs);
    vec2d operator*(const vec2d& lhs, int c);
    vec2d operator/(const vec2d& lhs, int c);
    int operator*(const vec2d& rhs, const vec2d& lhs);

    struct vec2d_hasher {
        std::size_t operator()(const vec2d& v) { return v.x + 12345 * v.y; }
    };

    std::vector<std::pair<int, int>> get_circle(int radius, int x_start, int y_start);

    std::vector<std::pair<int, int>> get_los(std::pair<int, int> start, std::pair<int, int> end);

    cmd get_random_active_action();

    cmd get_random_action();
}  // namespace roguelike::utils

namespace roguelike {
    bool operator==(const entity_id& lhs, const entity_id& rhs);
    bool operator==(const player_id& lhs, const player_id& rhs);

    bool operator!=(const entity_id& lhs, const entity_id& rhs);
    bool operator!=(const player_id& lhs, const player_id& rhs);
}  // namespace roguelike

#endif  // ROGUE_LIKE_UTILS_H
