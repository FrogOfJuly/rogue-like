//
// Created by Kirill Golubev on 24.04.2021.
//
#include "gamestate.h"

#include <variant>

#include "../utility/entity_info.h"

roguelike::gamestate::gamestate() noexcept : mv_system(this), inter_system(this) {}
roguelike::gamestate::gamestate(gamestate &&rhs) noexcept : gamestate() { *this = std::move(rhs); }

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

roguelike::gamestate &roguelike::gamestate::operator=(roguelike::gamestate &&rhs) noexcept {
    std::cout << "doing swapping " << std::endl;
    std::swap(level, rhs.level);
    std::swap(lvl_num, rhs.lvl_num);
    std::swap(players, rhs.players);

    std::swap(mv_system, rhs.mv_system);
    std::swap(inter_system, rhs.inter_system);

    mv_system.reset_owner(this);
    inter_system.reset_owner(this);

    return *this;
}
std::string roguelike::gamestate::get_serialization() const {
    lwlog_info("getting serialization");
    auto j = nlohmann::json();
    to_json(j, *this);
    return to_string(j);
}

void roguelike::gamestate::clean_decisions() {
    for (int i = 0; i < player_num; ++i) {
        auto &plr = players[i];
        plr.dm_cpt.decision = PASS;
    }
    for (auto &var_ent : level.residents) {
        std::visit(
            [](auto &ent_ptr) {
                auto &ent_ref = *ent_ptr;
                if constexpr (has_member_decision_making_component<std::remove_reference_t<decltype(ent_ref)>>::value) {
                    ent_ref.dm_cpt.decision = PASS;
                }
            },
            var_ent);
    }
}
void roguelike::gamestate::clean_dead() {}
void roguelike::gamestate::resolve_all_interactions() { inter_system.resolve_all_interactions(); }
void roguelike::gamestate::redraw_players() {
    for (int i = 0; i < player_num; ++i) {
        auto &plr = players[i];
        entity_type var_ent(&plr);
        drawing_system::general_draw(var_ent);
    }
}
void roguelike::gamestate::redraw_nonplayers() {
    for (auto &var_ent : level.residents) {
        drawing_system::general_draw(var_ent);
    }
}
void roguelike::gamestate::move_nonplayers() {
    for (auto &ent : level.residents) {
        lwlog_info("moving nonplayer");
        mv_system.more_general_move(ent);
    }
}
void roguelike::gamestate::move_players() {
    for (int i = 0; i < player_num; ++i) {
        auto &plr = players[i];
        lwlog_info("moving player %d", plr.id);
        entity_type var_ent = &plr;
        mv_system.more_general_move(var_ent);
    }
}
void roguelike::gamestate::receive_player_command(int player_id, roguelike::cmd command) {
    lwlog_info("getting player command");
    if (player_id >= player_num) {
        throw std::runtime_error("No such player id: " + std::to_string(player_id));
    }

    players[player_id].dm_cpt.decision = command;
}
void roguelike::gamestate::initialize(int player_number) {
    lwlog_info("Initializning gamestate object");
    lwlog_info("allocating  player objects");
    lvl_num = 0;
    level.generate_level(lvl_num);
    player_num = player_number;
    players = (player *)new char[sizeof(player) * player_num];
    for (int i = 0; i < player_num; ++i) {
        lwlog_info("placing player");
        new (&players[i]) player(i);
        players[i].dm_cpt.decision = LEFT;
        entity_type var_ent = &players[i];
        level.spawn_on_level(var_ent);
    }
    {
        for (int i = 0; i < 2; ++i) {
            lwlog_info("placing goblin");
            auto g = new goblin(i);
            g->dm_cpt.decision = LEFT;
            entity_type var_ent = g;
            level.spawn_on_level(var_ent);
            level.residents.emplace_back(g);
        }
    }
    {
        lwlog_info("placing goblin");
        auto g = new goblin(2);
        g->dm_cpt.decision = DOWN;
        entity_type var_ent = g;
        level.spawn_on_level(var_ent);
        level.residents.emplace_back(g);
    }
    {
        auto e = new entity((int)level.residents.size());
        lwlog_info("placing entity");
        entity_type var_ent = e;
        level.spawn_on_level(var_ent);
        level.residents.emplace_back(e);
    }
    {
        auto p = new potion((int)level.residents.size());
        lwlog_info("placing potion");
        entity_type var_ent = p;
        level.spawn_on_level(var_ent);
        level.residents.emplace_back(p);
    }
}
roguelike::gamestate::~gamestate() {
    for(int i = 0; i < player_num; ++i){
        delete &players[i];
    }
    delete[](char *) players;
}
