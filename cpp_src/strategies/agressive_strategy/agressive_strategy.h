//
// Created by Kirill Golubev on 02.05.2021.
//
#include "../../level/room_view.h"
#include "../abstract_strategy.h"

#ifndef ROGUE_LIKE_AGRESSIVE_STRATEGY_H
#define ROGUE_LIKE_AGRESSIVE_STRATEGY_H

namespace roguelike {
    class agressive_strategy : public strategy {
      public:
        void form_decision(decision_making_component& ) override;
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_AGRESSIVE_STRATEGY_H
