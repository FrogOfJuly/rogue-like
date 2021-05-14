//
// Created by Kirill Golubev on 14.05.2021.
//
#include "entity.hpp"

namespace roguelike {
    const interaction_info interaction_info::nothing_happened = interaction_info{false, false, false};
    const interaction_info interaction_info::item_picked = interaction_info{true, true, false};
    const interaction_info interaction_info::interacted_got_killed = interaction_info{true, true, false};
    const interaction_info interaction_info::interacted_got_destroyed = interaction_info{false, true, false};
    const interaction_info interaction_info::interacting_got_killed = interaction_info{false, false, true};
}  // namespace roguelike