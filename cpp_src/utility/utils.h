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

#define register_entity(entity_type_name) bool is_entity_a_##entity_type_name(const entity_type& var_ent);
#include "register_for_entities.h"
#undef register_entity
}  // namespace roguelike

#endif  // ROGUE_LIKE_UTILS_H
