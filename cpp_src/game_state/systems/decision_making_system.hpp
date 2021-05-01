//
// Created by Kirill Golubev on 25.04.2021.
//
#include "../../common.h"
#include "../../entity/components.h"
#include "../../level/room.h"
#include "../../utility/entity_info.h"
#include "base_system.h"
#ifndef ROGUE_LIKE_DECISION_MAKING_SYSTEM_H
#define ROGUE_LIKE_DECISION_MAKING_SYSTEM_H
namespace roguelike {
    class decision_making_system {
      public:
        template <simple_strategy>
        inline static void make_decision(entity_type& var_ent) {}
    };

    template <>
    inline void decision_making_system::make_decision<simple_strategy::passive>(entity_type& var_ent) {
        std::visit(
            [](auto* ent) {
                if constexpr (has_member_decision_making_component<std::remove_pointer_t<decltype(ent)>>::value) {
                    ent->dm_cpt.decision = cmd::PASS;
                }
            },
            var_ent);
    }

    template <>
    inline void decision_making_system::make_decision<simple_strategy::random>(entity_type& var_ent) {
        std::visit(
            [](auto* ent_ptr) {
                if constexpr (has_member_decision_making_component<std::remove_pointer_t<decltype(ent_ptr)>>::value) {
                    switch (abs(rand()) % 5) {
                        case 0:
                            ent_ptr->dm_cpt.decision = cmd::PASS;
                            break;
                        case 1:
                            ent_ptr->dm_cpt.decision = cmd::UP;
                            break;
                        case 2:
                            ent_ptr->dm_cpt.decision = cmd::DOWN;
                            break;
                        case 3:
                            ent_ptr->dm_cpt.decision = cmd::LEFT;
                            break;
                        case 4:
                            ent_ptr->dm_cpt.decision = cmd::RIGHT;
                            break;
                    }
                }
            },
            var_ent);
    }
}  // namespace roguelike

#endif  // ROGUE_LIKE_DECISION_MAKING_SYSTEM_H
