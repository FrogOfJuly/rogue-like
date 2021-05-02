//
// Created by Kirill Golubev on 01.05.2021.
//
#include "../common.h"

#ifndef ROGUE_LIKE_ABSTRACT_STRATEGY_H
#define ROGUE_LIKE_ABSTRACT_STRATEGY_H

namespace roguelike {
    struct room_view;

    class strategy {
      protected:
        std::unique_ptr<room_view> view;

      public:
        void set_view(const room_view& _view);
        void discard_view();

        virtual void form_decision(decision_making_component&) = 0;
        virtual ~strategy() = default;
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_ABSTRACT_STRATEGY_H
