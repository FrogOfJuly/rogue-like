//
// Created by Kirill Golubev on 01.05.2021.
//
#include <utility>
#include <vector>

#include "tile.h"

#ifndef ROGUE_LIKE_ROOM_VIEW_H
#define ROGUE_LIKE_ROOM_VIEW_H
namespace roguelike {
    struct room_view {
        room_view() = delete;
        room_view(const room_view&) = default;
        room_view(room_view&&) noexcept = default;
        room_view& operator=(const room_view&) = default;
        room_view& operator=(room_view&&) noexcept = default;
        ~room_view() = default;
        using observation = std::optional<entity_type>;

        struct cell {
            int x = -1;
            int y = -1;
            std::optional<entity_type> observed_entity;
        };

        static cell make_cell(const observation& obs, int x, int y);
        room_view(std::vector<cell> view, const cell& point_of_view);

        cell point_of_view;
        std::vector<cell> view;
        bool centered = false;

        void center();
        void decenter();
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_ROOM_VIEW_H
