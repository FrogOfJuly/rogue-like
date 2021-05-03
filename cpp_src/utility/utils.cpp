//
// Created by Kirill Golubev on 01.05.2021.
//

#include "utils.h"

#include <cmath>
#include <unordered_set>

struct hash_pair {
    size_t operator()(const std::pair<int, int>& p) const { return p.first + p.second * 999; }
};

std::vector<std::pair<int, int>> roguelike::utils::get_circle(int radius, int x_start, int y_start) {
    // algorithm is taken from here: https://www.redblobgames.com/grids/circle-drawing/
    std::unordered_set<std::pair<int, int>, hash_pair> results;
    vec2d center = {x_start, y_start};

    for (int r = 0; r <= floor(radius * sqrt(0.5)); r++) {
        int d = int(floor(sqrt(radius * radius - r * r)));
        results.emplace(center.x - d, center.y + r);
        results.emplace(center.x + d, center.y + r);
        results.emplace(center.x - d, center.y - r);
        results.emplace(center.x + d, center.y - r);
        results.emplace(center.x + r, center.y - d);
        results.emplace(center.x + r, center.y + d);
        results.emplace(center.x - r, center.y - d);
        results.emplace(center.x - r, center.y + d);
    }
    return std::vector<std::pair<int, int>>(results.begin(), results.end());
}
roguelike::utils::vec2d roguelike::utils::operator/(const roguelike::utils::vec2d& lhs, int c) {
    return {lhs.x / c, lhs.y / c};
}
int roguelike::utils::operator*(const roguelike::utils::vec2d& rhs, const roguelike::utils::vec2d& lhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}
roguelike::utils::vec2d roguelike::utils::operator*(const roguelike::utils::vec2d& lhs, int c) {
    return {c * lhs.x, c * lhs.y};
}
roguelike::utils::vec2d roguelike::utils::operator*(int c, const roguelike::utils::vec2d& rhs) {
    return {c * rhs.x, c * rhs.y};
}
roguelike::utils::vec2d roguelike::utils::operator-(const roguelike::utils::vec2d& rhs) { return {-rhs.x, -rhs.y}; }
roguelike::utils::vec2d roguelike::utils::operator-(
    const roguelike::utils::vec2d& lhs, const roguelike::utils::vec2d& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
roguelike::utils::vec2d roguelike::utils::operator+(
    const roguelike::utils::vec2d& lhs, const roguelike::utils::vec2d& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}
roguelike::cmd roguelike::utils::get_random_active_action() {
    switch (rand() % 5) {
        case 0:
            return cmd::PASS;
        case 1:
            return cmd::UP;
        case 2:
            return cmd::DOWN;
        case 3:
            return cmd::LEFT;
        case 4:
            return cmd::RIGHT;
    }
    return cmd::PASS;
}
roguelike::cmd roguelike::utils::get_random_action() {
    switch (rand() % 7) {
        case 0:
            return cmd::PASS;
        case 1:
            return cmd::UP;
        case 2:
            return cmd::DOWN;
        case 3:
            return cmd::RIGHT;
        case 4:
            return cmd::LEFT;
        case 5:
            return cmd::ESC;
        case 6:
            return cmd::ENTER;
    }
    return cmd::PASS;
}

double lerp(double start, double end, double t) { return start + t * (end - start); }

int diagonal_distance(roguelike::utils::vec2d p0, roguelike::utils::vec2d p1) {
    int dx = p1.x - p0.x, dy = p1.y - p0.y;
    return std::max(abs(dx), abs(dy));
}

roguelike::utils::vec2d round_point(double x, double y) {
    return roguelike::utils::vec2d{(int)std::round(x), (int)std::round(y)};
}

std::vector<std::pair<int, int>> roguelike::utils::get_los(std::pair<int, int> start, std::pair<int, int> end) {
    vec2d p0 = {start.first, start.second};
    vec2d p1 = {end.first, end.second};
    std::vector<std::pair<int, int>> res;
    auto N = diagonal_distance(p0, p1);
    for (int step = 0; step <= N; step++) {
        double t = N == 0 ? 0.0 : (double)(step) / (double)(N);
        double lerp_x = lerp(p0.x, p1.x, t);
        double lerp_y = lerp(p0.y, p1.y, t);
        auto res_pt = round_point(lerp_x, lerp_y);
        res.emplace_back(res_pt.x, res_pt.y);
    }
    return res;
}
bool roguelike::operator==(const roguelike::entity_id& lhs, const roguelike::entity_id& rhs) {
    return lhs.value == rhs.value;
}
bool roguelike::operator==(const roguelike::player_id& lhs, const roguelike::player_id& rhs) {
    return lhs.value == rhs.value;
}

bool roguelike::operator!=(const roguelike::entity_id& lhs, const roguelike::entity_id& rhs) {
    return lhs.value != rhs.value;
}
bool roguelike::operator!=(const roguelike::player_id& lhs, const roguelike::player_id& rhs) {
    return lhs.value != rhs.value;
}
std::pair<int, int> roguelike::utils::vec2d::to_pair() const { return std::make_pair(x, y); }

#define register_entity(entity_type_name)                                                                             \
    bool roguelike::is_entity_a_##entity_type_name(const roguelike::entity_type& var_ent) {                                    \
        return std::visit(                                                                                            \
            [](auto* ent_ptr) { return std::is_same_v<std::remove_pointer_t<decltype(ent_ptr)>, entity_type_name>; }, \
            var_ent);                                                                                                 \
    }
#include "register_for_entities.h"
#undef register_entity


