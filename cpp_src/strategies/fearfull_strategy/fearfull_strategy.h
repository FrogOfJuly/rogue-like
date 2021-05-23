//
// Created by Kirill Golubev on 05.05.2021.
//

#ifndef ROGUELIKE_FEARFULL_STRATEGY_H
#define ROGUELIKE_FEARFULL_STRATEGY_H

#include "../abstract_strategy.h"

namespace roguelike {
    class fearfull_strategy : public strategy {
    public:
        void form_decision(decision_making_component &) override;
    };
}  // namespace roguelike


#endif //ROGUELIKE_FEARFULL_STRATEGY_H
