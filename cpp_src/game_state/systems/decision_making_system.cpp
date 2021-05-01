//
// Created by Kirill Golubev on 02.05.2021.
//
#include "decision_making_system.h"

#include "../../level/room_view.h"
#include "../gamestate.h"

void roguelike::decision_making_system::make_decision(entity_type& var_ent) {
    std::visit(
        [this](auto* ent_ptr) {
            if constexpr (has_member_decision_making_component<std::remove_pointer_t<decltype(ent_ptr)>>::value
                          and has_member_move_component<std::remove_pointer_t<decltype(ent_ptr)>>::value) {
                auto residency = ent_ptr->m_cpt.residency;
                auto radius = ent_ptr->dm_cpt.eye_sight;
                auto view = game_ptr->level.get_area_around_tile(residency, radius);
                if (ent_ptr->dm_cpt.strat == nullptr){
                    ent_ptr->dm_cpt.decision = cmd::PASS;
                    return;
                }
                ent_ptr->dm_cpt.strat->set_view(view);
                ent_ptr->dm_cpt.strat->form_decision();
                ent_ptr->dm_cpt.decision = ent_ptr->dm_cpt.strat->get_decision();
                ent_ptr->dm_cpt.strat->discard_view();
            }
        },
        var_ent);
}
