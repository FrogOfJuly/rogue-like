//
// Created by Kirill Golubev on 24.04.2021.
//
#include "common.h"
#include "utility/entity_info.h"

void roguelike::to_json(nlohmann::json &j, const roguelike::general_id &p) {
    auto res = std::visit([](auto ent) { return ent.value; }, p);
    j = nlohmann::json{{"general_id", res}};
}
void roguelike::from_json(const nlohmann::json &j, roguelike::general_id &p) { p = j.at("general_id").get_to(p); }

void roguelike::to_json(nlohmann::json &j, const roguelike::entity_type &p) {
    j = std::visit(
        [](auto &ent_ptr) {
            auto j_local = nlohmann::json();
            to_json(j_local, *ent_ptr);
            return j_local;
        },
        p);
}
void roguelike::from_json(const nlohmann::json &j, roguelike::entity_type &p) {
    throw std::runtime_error("One CAN NOT construct entity type from it's serialization");
}
