//
// Created by Kirill Golubev on 17.04.2021.
//

#ifndef ROGUE_LIKE_TILE_H
#define ROGUE_LIKE_TILE_H

#include <optional>

#include "../common.h"

namespace roguelike {
    struct tile {
        tile() = default;
        std::optional<general_id> resident;
        [[nodiscard]] bool empty() const noexcept;
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_TILE_H
