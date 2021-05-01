//
// Created by Kirill Golubev on 25.04.2021.
//
#include "../../common.h"
#include "../../entity/components.h"
#include "../../utility/entity_info.h"
#include "base_system.h"
#ifndef ROGUE_LIKE_DECISION_MAKING_SYSTEM_H
#define ROGUE_LIKE_DECISION_MAKING_SYSTEM_H
namespace roguelike {
    class decision_making_system : public base_system {
      public:
        using base_system::base_system;
        void make_decision(entity_type& var_ent);
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_DECISION_MAKING_SYSTEM_H
