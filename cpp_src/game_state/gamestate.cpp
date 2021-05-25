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
    for (auto &it : players) {
        auto &plr = it.second;
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
    for (auto &it : players) {
        auto &plr = it.second;
        plr.lg_cpt.log.str("");
        plr.lg_cpt.log.clear();
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
        bool is_dead = std::visit(
            [this](auto *ent_ptr) {
                lwlog_info("-checking if entity %d is despawned", ent_ptr->id.value);
                return level.despawned.count(ent_ptr->id.value) != 0;
            },
            var_ent);
        if (is_dead) {
            lwlog_info("-it is despawned. Skipping");
            continue;
        }
        lwlog_info("-it is not despawned. Making move");
        mv_system.more_general_move(var_ent);
    }
}
void roguelike::gamestate::move_players() {
    for (auto &it : players) {
        auto &plr = it.second;
        lwlog_info("moving player %d with decision %d", plr.id.value, plr.dm_cpt.decision);
        entity_type var_ent = &plr;
        mv_system.more_general_move(var_ent);
    }
}
int roguelike::gamestate::receive_player_command(int player_id, roguelike::cmd command) {
    lwlog_info("getting command %d for player %d", command, player_id);
    if (player_id == -1) {
        while (not command_to_receive.empty()) {
            auto next_player_id = command_to_receive.front();
            if (received_command.count(next_player_id) == 0 and dead_players.count(next_player_id) == 0) {
                lwlog_info("next player to receive command is %d", next_player_id);
                command_to_receive.pop();
                return next_player_id;
            }
            command_to_receive.pop();
        }
        throw std::runtime_error("Everyone is dead");
    }
    if (players.count(player_id) == 0) {
        throw std::runtime_error("No such player id: " + std::to_string(player_id));
    }
    if (player_id >= 0) {  // if player_id < 0, then just return next player in queue
        received_command.insert(player_id);
        if (dead_players.count(player_id) == 0) {
            players.at(player_id).dm_cpt.decision = command;
        }
    }
    while (not command_to_receive.empty()) {
        auto next_player_id = command_to_receive.front();
        if (received_command.count(next_player_id) == 0 and dead_players.count(next_player_id) == 0) {
            lwlog_info("next player to receive command is %d", next_player_id);
            command_to_receive.pop();
            return next_player_id;
        }
        command_to_receive.pop();
    }
    // if you are here that means that all players received their commands
    received_command.clear();  // clean list of players who received command
    for (auto &it : players) {
        auto &plr = it.second;
        command_to_receive.push(plr.id.value);  // enqueue all players for commands
    }
    lwlog_info("all players got their commands");
    return -1;
}
void roguelike::gamestate::initialize(int player_number) {
    lwlog_info("Initializning gamestate object");
    lvl_num = 0;
    level.generate_terrain(lvl_num);
    lwlog_info("generated level");
    for (const auto& it : players) {
        lwlog_info("placing player");
        auto player_id = it.first;
        entity_type var_ent = &players.at(player_id);
        auto rnd_tile = level.get_random_empty_tile();
        level.spawn_on_level(var_ent, rnd_tile);
    }
    lwlog_info("spawned players %ld", players.size());
    level.generate_enemies(lvl_num);
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
                lwlog_info("-checking if entity %d is despawned", ent_ptr->id.value);
                return level.despawned.count(ent_ptr->id.value) != 0;
            },
            var_ent);
        if (is_dead) {
            lwlog_info("-it is despawned. Skipping");
            continue;
        }
        lwlog_info("-it is not despawned. Making decision");
        dm_system.make_decision(var_ent);
    }
    for (auto &it : players) {
        auto &plr = it.second;
        lwlog_info("-recording common log for player %d", plr.id.value);
        plr.lg_cpt.log << level.common_log.str();
    }
}

const roguelike::player *roguelike::gamestate::get_player(roguelike::player_id id) const {
    return &players.at(id.value);
}
roguelike::const_entity_type roguelike::gamestate::get_entity(roguelike::general_id id) const {
    return std::visit(
        overloaded{
            [this](player_id id) {
                lwlog_info("-player with id %d", id.value);
                const_entity_type plr = get_player(id);
                return plr;
            },
            [this](entity_id id) {
                lwlog_debug("-residnet with id %d", id.value);
                const_entity_type var_ent = level.get_resident(id);
                return var_ent;
            }},
        id);
}

roguelike::player *roguelike::gamestate::get_player(roguelike::player_id id) { return &players.at(id.value); }
roguelike::entity_type roguelike::gamestate::get_entity(roguelike::general_id id) {
    return std::visit(
        overloaded{
            [this](player_id id) {
                lwlog_info("-player with id %d", id.value);
                entity_type plr = get_player(id);
                return plr;
            },
            [this](entity_id id) {
                lwlog_debug("-residnet with id %d", id.value);
                entity_type var_ent = level.get_resident(id);
                return var_ent;
            }},
        id);
}
void roguelike::gamestate::report_despawn(roguelike::general_id mdred_id) {
    lwlog_info("reporting murder");
    std::visit(
        overloaded{
            [this](player_id id) {
                lwlog_info("player %d got murdered", id.value);
                dead_players.insert(id.value);
                get_player(id)->lg_cpt.log << "you are dead!\n";
            },
            [this](entity_id id) {
                lwlog_info("entity %d got despwaned", id.value);
                level.despawned.insert(id.value);
            }},
        mdred_id);
}
void roguelike::gamestate::initialize_player(int player_id) {
    lwlog_info("placing player %d", player_id);
    players.emplace(std::make_pair(player_id, player_id));
    players.at(player_id).dm_cpt.decision = cmd::PASS;
}
