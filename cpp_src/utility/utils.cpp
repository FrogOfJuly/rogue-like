//
// Created by Kirill Golubev on 01.05.2021.
//

#include "utils.h"
#include <unordered_set>

struct hash_pair {
    size_t operator()(const std::pair<int, int>& p) const
    {
        return p.first + p.second*999;
    }
};

std::vector<std::pair<int, int>> roguelike::utils::get_circle(int radius, int x_start, int y_start) {
    // algorithm is taken from here: https://www.redblobgames.com/grids/circle-drawing/
    std::unordered_set<std::pair<int, int>,hash_pair> results;
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
std::pair<int, int> roguelike::utils::vec2d::to_pair() const { return std::make_pair(x, y); }
