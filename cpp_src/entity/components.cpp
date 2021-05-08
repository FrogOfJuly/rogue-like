//
// Created by Kirill Golubev on 09.05.2021.
//

#include "components.h"
#include "../utility/entity_info.h"

void roguelike::simple_inventory_component::manage() {
    lwlog_info("managing time!");
    if (not spot.has_value()) {
        lwlog_info("do have item!");
        return;
    }
    std::visit(
        [this](auto *ent_ptr) {
            using entT = std::remove_pointer_t<decltype(ent_ptr)>;
            lwlog_info("item has type of %s", typeid(entT).name());
            if constexpr (has_member_pickable_component<entT>::value) {
                lwlog_info("item does have pickable component!");
                auto spt = ent_ptr->pk_cpt.desired_spot;
                if (spots.count(spt) == 0) {
                    lwlog_info("desired spot is free!");
                    spots[spt] = ent_ptr;
                    spot.reset();
                    return;
                }
                lwlog_info("desired spot is not free!");
                return;
            }

            lwlog_info("item does not have pickable component!");
        },
        spot.value());
}
std::pair<int, int> roguelike::decision_making_component::get_velocity() const {
    switch (decision) {
        case UP:
            return std::make_pair(0, -1);
        case DOWN:
            return std::make_pair(0, 1);
        case LEFT:
            return std::make_pair(-1, 0);
        case RIGHT:
            return std::make_pair(1, 0);
        case ENTER:
        case ESC:
        case PASS:
            return std::make_pair(0, 0);
    }
}
