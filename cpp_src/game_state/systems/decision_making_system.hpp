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
        template <strategy>
        inline static void make_decision(entity_type& var_ent) {}
    };

    template <>
    inline void decision_making_system::make_decision<strategy::passive>(entity_type& var_ent) {
        std::visit(
            [](auto* ent) {
                if constexpr (has_member_decision_making_component<std::remove_pointer_t<decltype(ent)>>::value) {
                    ent->dm_cpt.decision = cmd::PASS;
                }
            },
            var_ent);
    }

    template <>
    inline void decision_making_system::make_decision<strategy::random>(entity_type& var_ent) {
        std::visit(
            [](auto* ent_ptr) {
                std::cout << "trying to make decision" << std::endl;
                if constexpr (has_member_decision_making_component<std::remove_pointer_t<decltype(ent_ptr)>>::value) {
                    std::cout << "making decision" << std::endl;
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

    /*
        template <>
        inline void decision_making_system::make_decision<strategy::aggressive>(
            const room& level, entity_type& var_ent, tile_idx ent_place) {
            std::visit(
                [&level, ent_place](auto* ent) {
                    if constexpr (not has_member_decision_making_component<std::remove_pointer_t<decltype(ent)>>::value)
       { return;
                    }
                    auto p = room::pairFromIdx(ent_place);
                    auto x = p.first, y = p.second;
                    auto ent_tile = level.get_tile_if_exists(x, y);
                    assert(ent_tile.has_value());

                },
                var_ent);
        }*/
}  // namespace roguelike

#endif  // ROGUE_LIKE_DECISION_MAKING_SYSTEM_H
