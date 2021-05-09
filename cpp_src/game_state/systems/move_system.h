//
// Created by Kirill Golubev on 24.04.2021.
//
#include "../../level/room.h"
#include "base_system.h"
#ifndef ROGUE_LIKE_MOVE_SYSTEM_H
namespace roguelike {
    class move_system : public base_system {
      public:
        void move_to_tile(general_id item_ent_ptr, tile_idx dest_tile_idx);

        bool more_general_move(entity_type &var_ent);

        static tile_idx desired_tile_idx(entity_type &var_ent);

        move_system() = delete;

        using base_system::base_system;

        move_system(const move_system &) = delete;

        move_system(move_system &&) = default;

        move_system &operator=(const move_system &) = delete;

        move_system &operator=(move_system &&) = default;

        friend gamestate;

    };
}  // namespace roguelike

#define ROGUE_LIKE_MOVE_SYSTEM_H
#endif  // ROGUE_LIKE_MOVE_SYSTEM_H
