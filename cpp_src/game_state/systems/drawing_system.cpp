//
// Created by Kirill Golubev on 24.04.2021.
//
#include <variant>
#include "drawing_system.h"
#include "../../utility/entity_info.h"

void roguelike::drawing_system::general_draw(entity_type &var_ent) {
    std::visit(
        [](auto *ent) {
            assert(ent);
            if constexpr (has_member_repr_component<std::remove_pointer_t<decltype(ent)>>::value) {
                ent->repr_cpt.repr = repr_component::compute_representation(ent);
                return;
            }
        },
        var_ent);
}
