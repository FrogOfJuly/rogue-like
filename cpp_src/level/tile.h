//
// Created by Kirill Golubev on 17.04.2021.
//

#ifndef ROGUE_LIKE_TILE_H
#define ROGUE_LIKE_TILE_H

#include <optional>

#include "../common.h"

namespace roguelike {
    struct tile {
        std::optional<general_id> resident;
        bool empty() const { return not resident.has_value(); }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_TILE_H
