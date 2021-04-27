//
// Created by Kirill Golubev on 27.04.2021.
//

#include "../game_state/gamestate.h"
#include "../utility/entity_info.h"
#include "serialize_info.h"

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

void roguelike::to_json(nlohmann::json &j, const logging_component &p) {
    //        j["id"] = p.id;
    j["log"] = p.log.str();
}
void roguelike::from_json(const nlohmann::json &j, logging_component &p) {
    //    p.id = j["id"];
    p.log << j["log"];
}

void roguelike::to_json(nlohmann::json &j, const gamestate &p) {
    auto room_json = nlohmann::json();
    for (const auto &tle : p.level.tiles) {
        auto cur_tile_json = nlohmann::json();
        if (tle.empty()) {
            cur_tile_json["tile"] = nlohmann::json();
            room_json.push_back(cur_tile_json);
            continue;
        }
        auto resident_json = std::visit(
            overloaded{
                [&p](player_id id) {
                    auto j_local = nlohmann::json();
                    nlohmann::to_json(j_local["player"], p.players[id.value]);
                    std::string str_repr = j_local["player"]["repr_cpt"]["repr"];
                    return j_local;
                },
                [&p](entity_id id) {
                    auto j_local = nlohmann::json();
                    const auto &var_ent = p.level.residents[id.value];
                    nlohmann::to_json(j_local["entity"], var_ent);
                    bool has_repr = std::visit(
                        [](auto *ent) {
                            return has_member_repr_component<std::remove_pointer_t<decltype(ent)>>::value;
                        },
                        var_ent);
                    if (not has_repr) {
                        j_local["entity"]["repr_cpt"]["repr"] = "?";
                        return j_local;
                    }
                    return j_local;
                }},
            tle.resident.value());
        cur_tile_json["tile"] = resident_json;
        room_json.push_back(cur_tile_json);
    }
    j["level"] = room_json;
}
void roguelike::from_json(const nlohmann::json &j, roguelike::gamestate &p) {
    throw std::runtime_error("One CAN NOT construct game state form it's serialization!!");
}
