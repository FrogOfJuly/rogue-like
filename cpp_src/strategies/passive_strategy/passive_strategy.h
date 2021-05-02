//
// Created by Kirill Golubev on 02.05.2021.
//

#include "../abstract_strategy.h"
#include "../../level/room_view.h"

#ifndef ROGUE_LIKE_PASSIVE_STRATEGY_H
#define ROGUE_LIKE_PASSIVE_STRATEGY_H

namespace roguelike {
    class passive_strategy : public strategy {
      public:
        void form_decision(decision_making_component& ) override;
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_PASSIVE_STRATEGY_H
