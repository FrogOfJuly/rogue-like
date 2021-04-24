//
// Created by Kirill Golubev on 24.04.2021.
//
#include <iostream>
#ifndef ROGUE_LIKE_BASE_SYSTEM_H
#define ROGUE_LIKE_BASE_SYSTEM_H
namespace roguelike {
    struct gamestate;
    class base_system {
      protected:
        gamestate* game_ptr;

      public:
        explicit base_system(gamestate* game_ptr) : game_ptr(game_ptr) {}
        void reset_owner(gamestate* ptr) { game_ptr = ptr; }
    };
}  // namespace roguelike
#endif  // ROGUE_LIKE_BASE_SYSTEM_H
