//
// Created by Kirill Golubev on 01.05.2021.
//

#include <array>
#include <vector>

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

    std::vector<std::pair<int, int>> get_circle(int radius, int x_start, int y_start);
}  // namespace roguelike::utils

#endif  // ROGUE_LIKE_UTILS_H
