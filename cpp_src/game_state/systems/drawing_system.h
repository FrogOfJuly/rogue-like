//
// Created by Kirill Golubev on 24.04.2021.
//

#include "../../level/room.h"

#ifndef ROGUE_LIKE_DRAWING_SYSTEM_H
#define ROGUE_LIKE_DRAWING_SYSTEM_H
namespace roguelike {
    class drawing_system {
      public:
        static void general_draw(entity_type &var_ent);
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_DRAWING_SYSTEM_H
