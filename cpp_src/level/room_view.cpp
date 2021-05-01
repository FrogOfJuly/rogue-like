//
// Created by Kirill Golubev on 01.05.2021.
//

#include "room_view.h"
void roguelike::room_view::center() {
    if (centered) {
        return;
    }
    assert(point_of_view.x >= 0 and point_of_view.y >= 0);
    for (auto& c : view) {
        c.x -= point_of_view.x;
        c.y -= point_of_view.y;
    }
    centered = true;
}
void roguelike::room_view::decenter() {
    if (not centered) {
        return;
    }
    assert(point_of_view.x >= 0 and point_of_view.y >= 0);
    for (auto& c : view) {
        c.x += point_of_view.x;
        c.y += point_of_view.y;
    }
    centered = false;
}
roguelike::room_view::room_view(std::vector<cell> view, const roguelike::room_view::cell& point_of_view)
    : view(std::move(view)), point_of_view(point_of_view) {}
roguelike::room_view::cell roguelike::room_view::make_cell(const roguelike::room_view::observation& obs, int x, int y) {
    if (not obs.has_value()) {
        return cell{x, y, observation()};
    }
    auto proxy = std::visit(
        [](auto* resident_ptr) {
            // the idea is to preserve type information, but discard everything else
            entity_type proxy = (decltype(resident_ptr))(nullptr);
            return proxy;
        },
        obs.value());
    return cell{x, y, observation(proxy)};
}
