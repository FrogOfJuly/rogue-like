//
// Created by Kirill Golubev on 27.04.2021.
//

#include "non_default_serialize.h"

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
        [](auto *ent_ptr) {
            using entT = std::remove_pointer_t<decltype(ent_ptr)>;
            auto j_local = nlohmann::json();
            to_json(j_local, *ent_ptr);
            j_local["repr_cpt"]["repr"] = repr_component::compute_representation(ent_ptr);
            j_local["repr_cpt"]["repr"] = repr_component::compute_representation(ent_ptr);
            if constexpr (has_member_level_component<entT>::value) {
                auto lvl_p = ent_ptr->lvl_cpt.get_level(ent_ptr);
                j_local["lvl_cpt"]["level"] = lvl_p.first;
                j_local["lvl_cpt"]["exp_until_next_level"] = lvl_p.second;
                j_local["lvl_cpt"]["experience"] = ent_ptr->lvl_cpt.exp;
            }
            if constexpr (std::is_same_v<entT, player>) {
                auto ret_j = nlohmann::json();
                ret_j["player"] = j_local;
                return ret_j;
            } else {
                auto ret_j = nlohmann::json();
                ret_j["entity"] = j_local;
                return ret_j;
            }
        },
        p);
}
void roguelike::from_json(const nlohmann::json &j, roguelike::entity_type &p) {
    throw std::runtime_error("One CAN NOT construct entity type from it's serialization");
}

void roguelike::to_json(nlohmann::json &j, const logging_component &p) { j["log"] = p.log.str(); }
void roguelike::from_json(const nlohmann::json &j, logging_component &p) { p.log << j["log"]; }
void roguelike::to_json(nlohmann::json &j, const roguelike::simple_inventory_component &p) {
    if (not p.spot.has_value()) {
        j["spot"] = "none";
        return;
    }
    to_json(j["spot"], p.spot.value());
}
void roguelike::from_json(const nlohmann::json &j, roguelike::simple_inventory_component &p) {
    throw std::runtime_error("One cant not construct simple inventory component from it's json serialization");
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
        auto id = tle.resident.value();
        entity_type resident = p.get_entity(id);
        auto resident_json = nlohmann::json();
        to_json(resident_json, resident);
        /*std::visit(
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
        tle.resident.value());*/
        cur_tile_json["tile"] = resident_json;
        room_json.push_back(cur_tile_json);
    }
    j["level"] = room_json;
}
void roguelike::from_json(const nlohmann::json &j, roguelike::gamestate &p) {
    throw std::runtime_error("One CAN NOT construct game state form it's serialization!!");
}
