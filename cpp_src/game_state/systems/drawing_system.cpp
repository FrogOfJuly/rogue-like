//
// Created by Kirill Golubev on 24.04.2021.
//
#include "drawing_system.h"

#include <variant>

#include "../../utility/entity_info.h"

void roguelike::drawing_system::general_draw(entity_type &var_ent) {
    std::visit(
        [](auto *ent) {
            lwlog_info("asserting entity pointer");
            assert(ent);
            if constexpr (has_member_repr_component<std::remove_pointer_t<decltype(ent)>>::value) {
                lwlog_info("computing representation");
                ent->repr_cpt.repr = repr_component::compute_representation(ent);
                lwlog_info("computed representation: %s", ent->repr_cpt.repr.c_str());
                return;
            }
        },
        var_ent);
}
