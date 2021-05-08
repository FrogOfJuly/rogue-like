//
// Created by Kirill Golubev on 24.04.2021.
//
#include "gamestate.h"

#include <variant>

#include "../serializers/serialize_info.h"

roguelike::gamestate::gamestate() noexcept : mv_system(this), inter_system(this), dm_system(this) { srand(time(NULL)); }
roguelike::gamestate::gamestate(gamestate &&rhs) noexcept : gamestate() { *this = std::move(rhs); }
roguelike::gamestate &roguelike::gamestate::operator=(roguelike::gamestate &&rhs) noexcept {
    std::swap(level, rhs.level);
    std::swap(lvl_num, rhs.lvl_num);
    std::swap(players, rhs.players);

    std::swap(mv_system, rhs.mv_system);
    std::swap(inter_system, rhs.inter_system);
    std::swap(dm_system, rhs.dm_system);

    mv_system.reset_owner(this);
    inter_system.reset_owner(this);
    dm_system.reset_owner(this);

    return *this;
}

std::string roguelike::gamestate::get_serialization() const {
    lwlog_info("getting serialization");
    auto j = nlohmann::json();
    to_json(j, *this);
    return to_string(j);
}

void roguelike::gamestate::clean_decisions() {
    lwlog_info("cleaning decisions");
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
void roguelike::gamestate::clean_logs() {
    for (int i = 0; i < player_num; ++i) {
        players[i].lg_cpt.log.str("");
        players[i].lg_cpt.log.clear();
    }
    level.common_log.str("");
    level.common_log.clear();
}
void roguelike::gamestate::resolve_all_interactions() {
    lwlog_info("resolving interactions");
    inter_system.resolve_all_interactions();
}
void roguelike::gamestate::move_nonplayers() {
    lwlog_info("moving nonplayers");
    for (auto &var_ent : level.residents) {
        bool is_dead = std::visit([this](auto *ent_ptr) {
            lwlog_info("-checking if entity %d is dead", ent_ptr->id.value);
            return level.dead.count(ent_ptr->id.value) != 0; }, var_ent);
        if (is_dead) {
            lwlog_info("-it is dead. Skipping");
            continue;
        }
        lwlog_info("-it is not dead. Making move");
        mv_system.more_general_move(var_ent);
    }
}
void roguelike::gamestate::move_players() {
    for (int i = 0; i < player_num; ++i) {
        auto &plr = players[i];
        lwlog_info("moving player %d with decision %d", plr.id.value, players->dm_cpt.decision);
        entity_type var_ent = &plr;
        mv_system.more_general_move(var_ent);
    }
}
int roguelike::gamestate::receive_player_command(int player_id, roguelike::cmd command) {
    lwlog_info("getting command %d for player %d", command, player_id);
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
    lwlog_info("all players got their commands");
    return -1;
}
void roguelike::gamestate::initialize(int player_number) {
    lwlog_info("Initializning gamestate object");
    lwlog_info("allocating  player objects");
    lvl_num = 0;
    level.generate_level(lvl_num);
    player_num = player_number;
    players = (player *)new char[sizeof(player) * player_num];
    // They do not have default constructor and
    // I can't declare vector of them in header bc it requires complete type
    for (int i = 0; i < player_num; ++i) {
        lwlog_info("placing player");
        new (&players[i]) player(i);
        players[i].dm_cpt.decision = LEFT;
        entity_type var_ent = &players[i];
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
    }
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
    for (int i = 3; i < 5; ++i) {
        lwlog_info("placing goblin guard");
        auto new_id = level.residents.size();
        auto gg = new goblin_guard(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
        level.residents.emplace_back(gg);
    }
    for (int i = 5; i < 7; ++i) {
        lwlog_info("placing trap");
        auto new_id = level.residents.size();
        auto gg = new trap(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
        level.residents.emplace_back(gg);
    }
    for (int i = 7; i < 10; ++i) {
        lwlog_info("placing goblin worker");
        auto new_id = level.residents.size();
        auto gg = new goblin_worker(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
        level.residents.emplace_back(gg);
    }
    for (int i = 10; i < 12; ++i) {
        lwlog_info("placing potion");
        auto new_id = level.residents.size();
        auto gg = new potion(new_id);
        entity_type var_ent = gg;
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
        level.residents.emplace_back(gg);
    }
    lwlog_info("spawned everybody");
}
void roguelike::gamestate::end_turn() {
    lwlog_info("ending turn");
    clean_decisions();
    clean_logs();
}
void roguelike::gamestate::decide_next_move() {
    lwlog_info("deviding next move");
    for (auto &var_ent : level.residents) {
        bool is_dead = std::visit(
            [this](auto *ent_ptr) {
                lwlog_info("-checking if entity %d is dead", ent_ptr->id.value);
                return level.dead.count(ent_ptr->id.value) != 0;
            },
            var_ent);
        if (is_dead) {
            lwlog_info("-it is dead. Skipping");
            continue;
        }
        lwlog_info("-it is not dead. Making decision");
        dm_system.make_decision(var_ent);
    }
    for (int i = 0; i < player_num; ++i) {
        lwlog_info("-recording common log for player %d", i);
        players[i].lg_cpt.log << level.common_log.str();
    }
}

roguelike::gamestate::~gamestate() {
    for (int i = 0; i < player_num; ++i) {
        players[i].~player();
    }
    delete[](char *) players;
}

roguelike::player *roguelike::gamestate::get_player(roguelike::player_id id) const { return &players[id.value]; }
roguelike::entity_type roguelike::gamestate::get_entity(roguelike::general_id id) const {
    return std::visit(
        overloaded{
            [this](player_id id) {
                lwlog_info("-player with id %d", id.value);
                entity_type plr = get_player(id);
                return plr;
            },
            [this](entity_id id) {
                lwlog_info("-residnet with id %d", id.value);
                return level.get_resident(id);
            }},
        id);
}
void roguelike::gamestate::report_murder(roguelike::general_id mdred_id, roguelike::general_id mdrer_id) {
    lwlog_info("reporting murder");
    std::visit(
        overloaded{
            [this](player_id id) {
                lwlog_info("player %d got murdered", id.value);
                dead_players.insert(id.value);
                get_player(id)->lg_cpt.log << "you are dead!\n";
            },
            [this](entity_id id) {
                lwlog_info("entity %d got murdered", id.value);
                level.dead.insert(id.value);
            }},
        mdred_id);

    entity_type mdred_ent = get_entity(mdred_id);
    entity_type mdrer_ent = get_entity(mdrer_id);

    std::visit(
        [](auto *mdred_ent_ptr, auto *mdrer_ent_ptr) {
            using mdrerT = std::remove_pointer_t<decltype(mdrer_ent_ptr)>;
            using mdredT = std::remove_pointer_t<decltype(mdred_ent_ptr)>;
            std::string victim_name = "your victim";
            if constexpr (has_member_name_component<mdredT>::value) {
                victim_name = mdred_ent_ptr->nm_cpt.name;
            }
            if constexpr (has_member_logging_component<mdrerT>::value) {
                mdrer_ent_ptr->lg_cpt.log << "you have killed " << victim_name << std::endl;
            }
        },
        mdred_ent,
        mdrer_ent);
}
