//
// Created by Kirill Golubev on 09.05.2021.
//

#include "components.h"

#include "../utility/entity_info.h"

void roguelike::simple_inventory_component::manage() {
    lwlog_info("managing time!");
    if (not spot.has_value()) {
        lwlog_info("do not have item!");
        return;
    }
    if (locked) {
        lwlog_info("No managing for locked inventory");
        return;
    }
    std::visit(
        [this](auto* ent_ptr) {
            using entT = std::remove_pointer_t<decltype(ent_ptr)>;
            lwlog_info("item has type of %s", typeid(entT).name());
            if constexpr (has_member_pickable_component<entT>::value) {
                lwlog_info("item does have pickable component!");
                auto spt = ent_ptr->pk_cpt.desired_spot;
                if (spots.count(spt) == 0) {
                    lwlog_info("desired spot is free!");
                    spots[spt] = ent_ptr;
                    spot.reset();
                } else {
                    lwlog_info("desired spot is NOT free! Swapping items");
                    auto tmp = spots[spt];
                    spots[spt] = ent_ptr;
                    spot = tmp;
                }
                return;
            }

            lwlog_info("item does NOT have pickable component!");
        },
        spot.value());
}
int roguelike::simple_inventory_component::get_damage_bonus() const {
    lwlog_info("computing bonus damage for inventory of size %ld", spots.size());
    int dmg_bonus = 0;
    for (const auto& it : spots) {
        entity_type item = it.second;
        dmg_bonus += std::visit(
            [](auto* ent_ptr) {
                using entT = std::remove_pointer_t<decltype(ent_ptr)>;
                int dmg = 0;
                lwlog_info("checking if there is an atk component for %s", typeid(entT).name());
                if constexpr (has_member_atk_component<entT>::value) {
                    lwlog_info("There is an atk component!");
                    dmg = atk_component::calculate_damage(ent_ptr);
                }
                lwlog_info("additional damage is %d", dmg);
                return dmg;
            },
            item);
    }
    return dmg_bonus;
}
int roguelike::simple_inventory_component::get_defence_bonus() const {
    int dmg_red = 0;
    for (const auto& it : spots) {
        entity_type item = it.second;
        dmg_red += std::visit(
            [](auto* ent_ptr) {
                using entT = std::remove_pointer_t<decltype(ent_ptr)>;
                int dmg = 0;
                if constexpr (has_member_dfc_component<entT>::value) {
                    dmg = dfc_component::calculate_damage_reduction(ent_ptr);
                }
                return dmg;
            },
            item);
    }
    return dmg_red;
}
double roguelike::simple_inventory_component::get_protection_bonus() const {
    double prot_bonus = 1.0;
    for (const auto& it : spots) {
        entity_type item = it.second;
        prot_bonus *= std::visit(
            [](auto* ent_ptr) {
                using entT = std::remove_pointer_t<decltype(ent_ptr)>;
                double prot = 1.0;
                if constexpr (has_member_prot_component<entT>::value) {
                    prot = prot_component::calculate_protection_scale(ent_ptr);
                }
                return prot;
            },
            item);
    }
    return prot_bonus;
}
roguelike::simple_inventory_component roguelike::simple_inventory_component::get_locked_invetory() {
    auto inv = simple_inventory_component();
    inv.locked = true;
    return inv;
}
std::optional<roguelike::entity_type> roguelike::simple_inventory_component::get_some_item() const {
    auto res = std::optional<entity_type>();
    for (const auto& it : spots) {
        return std::optional<entity_type>(it.second);
    }
    return res;
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
        default:
            return std::make_pair(0, 0);
    }
}
int roguelike::level_component::experience_on_kill() const { return lvl * 10; }
