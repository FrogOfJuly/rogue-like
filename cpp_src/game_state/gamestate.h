//
// Created by Kirill Golubev on 17.04.2021.
//
#include <iostream>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "../common.h"
#include "../entity/player.h"
#include "../level/room.h"
#include "../serializers/serialize_info.h"
#include "systems/decision_making_system.h"
#include "systems/interaction_system.h"
#include "systems/move_system.h"

#ifndef ROGUE_LIKE_GAMESTATE_H
#define ROGUE_LIKE_GAMESTATE_H

namespace roguelike {
    class player;

    class gamestate {
        room level;
        mutable std::unordered_map<int, player> players;
        // to remove this mutable, there need to be std::variant<const player*, const entity* ... and so on>.
        // but for now there is none, so I ma basically discarding const qualifier
        std::unordered_set<int> dead_players;
        int lvl_num = -1;

        std::unordered_set<int> received_command;
        std::queue<int> command_to_receive;

        move_system mv_system;
        interaction_system inter_system;
        decision_making_system dm_system;

        player *get_player(player_id id) const;
        entity_type get_entity(general_id id) const;

        void report_murder(general_id mdred_id, general_id mdrer_id);

      public:
        void initialize(int player_num);
        int receive_player_command(int player_id, cmd command);
        void move_players();
        void move_nonplayers();
        void decide_next_move();
        void resolve_all_interactions();
        void clean_dead();
        void clean_decisions();
        void clean_logs();
        void end_turn();

        [[nodiscard]] std::string get_serialization() const;

        gamestate() noexcept;
        gamestate &operator=(const gamestate &) = delete;
        gamestate(const gamestate &) = delete;
        gamestate &operator=(gamestate &&rhs) noexcept;
        gamestate(gamestate &&rhs) noexcept;

        friend class move_system;
        friend class interaction_system;
        friend class decision_making_system;

        friend void to_json(nlohmann::json &j, const gamestate &p);
        friend void from_json(const nlohmann::json &j, gamestate &p);
    };

}  // namespace roguelike

#endif  // ROGUE_LIKE_GAMESTATE_H
