//
// Created by Kirill Golubev on 17.04.2021.
//
#include <iostream>
#include <queue>

#include "../common.h"
#include "../level/room.h"
#include "systems/drawing_system.h"
#include "systems/interaction_system.h"
#include "systems/move_system.h"

#ifndef ROGUE_LIKE_GAMESTATE_H
#define ROGUE_LIKE_GAMESTATE_H

namespace roguelike {
    class player;

    class gamestate {
        room level;
        player *players = nullptr;
        int player_num = -1;
        int lvl_num = -1;

        move_system mv_system;
        interaction_system inter_system;
        /*drawing_system draw_system; -- all methods are static so no need*/

      public:
        void initialize(int player_num);
        void receive_player_command(int player_id, cmd command);
        void move_players();
        void move_nonplayers();
        void redraw_nonplayers();
        void redraw_players();
        void resolve_all_interactions();
        void clean_dead();
        void clean_decisions();

        [[nodiscard]] std::string get_serialization() const;

        gamestate() noexcept;
        gamestate &operator=(const gamestate &) = delete;
        gamestate(const gamestate &) = delete;
        gamestate &operator=(gamestate &&rhs) noexcept;
        gamestate(gamestate &&rhs) noexcept;
        ~gamestate();

        friend class move_system;
        friend class interaction_system;

        friend void to_json(nlohmann::json &j, const gamestate &p);
        friend void from_json(const nlohmann::json &j, gamestate &p);
    };

    void to_json(nlohmann::json &j, const gamestate &p);

    void from_json(const nlohmann::json &j, gamestate &p);

}  // namespace roguelike

#endif  // ROGUE_LIKE_GAMESTATE_H
