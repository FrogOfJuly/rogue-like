//
// Created by Kirill Golubev on 24.04.2021.
//
#include "gamestate.h"
#include <variant>
#include "../serializers/serialize_info.h"

roguelike::gamestate::gamestate() noexcept : mv_system(this), inter_system(this) { srand(time(NULL)); }
roguelike::gamestate::gamestate(gamestate &&rhs) noexcept : gamestate() { *this = std::move(rhs); }
roguelike::gamestate &roguelike::gamestate::operator=(roguelike::gamestate &&rhs) noexcept {
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
        bool is_dead = std::visit([this](auto *ent_ptr) { return level.dead.count(ent_ptr->id.value) != 0; }, var_ent);
        if (is_dead) {
            continue;
        }
        drawing_system::general_draw(var_ent);
    }
}
void roguelike::gamestate::move_nonplayers() {
    for (auto &var_ent : level.residents) {
        bool is_dead = std::visit([this](auto *ent_ptr) { return level.dead.count(ent_ptr->id.value) != 0; }, var_ent);
        if (is_dead) {
            continue;
        }
        lwlog_info("moving nonplayer");
        mv_system.more_general_move(var_ent);
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
int roguelike::gamestate::receive_player_command(int player_id, roguelike::cmd command) {
    lwlog_info("getting player command");
    if (player_id >= player_num) {
        throw std::runtime_error("No such player id: " + std::to_string(player_id));
    }
    received_command.insert(player_id);
    if (dead_players.count(player_id) == 0) {
        players[player_id].dm_cpt.decision = command;
    }
    while (not command_to_receive.empty()) {
        auto next_player_id = command_to_receive.front();
        if (received_command.count(next_player_id) == 0 and dead_players.count(next_player_id) == 0) {
            return next_player_id;
        }
        command_to_receive.pop();
    }
    // if you are here that means that all players received their commands
    received_command.clear();  // clean list of players who received command
    for (int i = 0; i < player_num; ++i) {
        command_to_receive.push(i);  // enqueue all players for commands
    }
    return -1;
}
void roguelike::gamestate::initialize(int player_number) {
    lwlog_info("Initializning gamestate object");
    lwlog_info("allocating  player objects");
    lvl_num = 0;
    level.generate_level(lvl_num);
    player_num = player_number;
    players = (player *)new char[sizeof(player) * player_num];  // they do not have default constructor
    for (int i = 0; i < player_num; ++i) {
        lwlog_info("placing player");
        new (&players[i]) player(i);
        players[i].dm_cpt.decision = LEFT;
        entity_type var_ent = &players[i];
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
    }
    /*{
        for (int i = 0; i < 2; ++i) {
            lwlog_info("placing goblin");
            auto g = new goblin(i);
            g->dm_cpt.decision = LEFT;
            entity_type var_ent = g;
            level.spawn_on_level(var_ent);
            level.residents.emplace_back(g);
        }
    }*/
    for (int i = 0; i < 3; ++i) {
        lwlog_info("placing goblin");
        auto new_id = level.residents.size();
        auto g = new goblin(new_id);
        g->dm_cpt.decision = DOWN;
        entity_type var_ent = g;
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
        level.residents.emplace_back(g);
    }
    /*{
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
    }*/
}
void roguelike::gamestate::clean_logs() {
    for (int i = 0; i < player_num; ++i) {
        players[i].lg_cpt.log.clear();
    }
}
void roguelike::gamestate::end_turn() {
    clean_decisions();
    clean_logs();
}
void roguelike::gamestate::decide_next_move() {
    for (auto &var_ent : level.residents) {
        bool is_dead = std::visit([this](auto *ent_ptr) { return level.dead.count(ent_ptr->id.value) != 0; }, var_ent);
        if (is_dead) {
            continue;
        }
        decision_making_system::make_decision<simple_strategy::random>(var_ent);
    }
}

roguelike::gamestate::~gamestate() {
    for (int i = 0; i < player_num; ++i) {
        players[i].~player();
    }
    delete[](char *) players;
}
