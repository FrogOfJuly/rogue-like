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
        cmd decision = cmd::PASS;
        std::unique_ptr<room_view> view;

      public:
        void set_view(const room_view& _view);
        void discard_view();
        [[nodiscard]] cmd get_decision() const;

        virtual void form_decision() = 0;
        virtual ~strategy() = default;
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_ABSTRACT_STRATEGY_H
