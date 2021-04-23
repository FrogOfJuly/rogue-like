//
// Created by Kirill Golubev on 17.04.2021.
//
#include <iostream>
#include <map>
#include <memory>
#include <queue>

#include "../level/room.hpp"

#ifndef ROGUE_LIKE_GAMESTATE_H
#define ROGUE_LIKE_GAMESTATE_H

namespace roguelike {
    constexpr int room_size = 4;

    enum entity_names {
#define register_entity(entity_type_name, representation_function) entity_type_name##_entity,
#include "../utility/register_for_entities.h"
#undef register_entity
    };

    class gamestate {
        using entity_pair = std::pair<general_id, general_id>;
        using room = room<room_size, room_size>;
        room level;
        int lvl_num = -1;
        std::vector<player> players;

       public:
        gamestate() noexcept : mv_system(this), inter_system(this), draw_system(this) {}

        void initialize(int player_num) {
            lwlog_info("Initializning gamestate object");
            lwlog_info("allocating  player objects");

            lvl_num = 0;
            level.generate_level(lvl_num);

            for (int i = 0; i < player_num; ++i) {
                lwlog_info("placing player");
                auto p = player(i);
                p.dm_cpt.decision = LEFT;
                players.push_back(std::move(p));
                level.spawn_on_level(players[i]);
            }

            for (int i = 0; i < 2; ++i) {
                lwlog_info("placing goblin");
                auto g = new goblin(i);
                g->dm_cpt.decision = LEFT;
                level.spawn_on_level(*g);
                level.residents.emplace_back(g);
            }
            lwlog_info("placing goblin");
            auto g = new goblin(2);
            g->dm_cpt.decision = DOWN;
            level.spawn_on_level(*g);
            level.residents.emplace_back(g);

            auto e = new entity((int)level.residents.size());
            lwlog_info("placing entity");
            level.spawn_on_level(*e);
            level.residents.emplace_back(e);

            auto p = new potion((int)level.residents.size());
            lwlog_info("placing potion");
            level.spawn_on_level(*p);
            level.residents.emplace_back(p);
        }

        void receive_player_command(int player_id, cmd command) {
            lwlog_info("getting player command");
            if (player_id >= players.size()) {
                throw std::runtime_error("No such player id: " + std::to_string(player_id));
            }

            players[player_id].dm_cpt.decision = command;
        }

        void move_players() {
            for (auto &plr : players) {
                lwlog_info("moving player");
                mv_system.general_move<player>(plr);
            }
        }

        void move_nonplayers() {
            for (auto &ent : level.residents) {
                std::visit(
                    [this](auto &ent_ptr) {
                        auto &ent_ref = *ent_ptr;
                        mv_system.general_move<std::remove_reference_t<decltype(ent_ref)>>(ent_ref);
                    },
                    ent);
            }
        }

        void redraw_nonplayers() {
            for (auto &var_ent : level.residents) {
                drawing_system::general_draw(var_ent);
            }
        }

        void redraw_players() {
            for (auto &player : players) {
                entity_type var_ent(&player);
                drawing_system::general_draw(var_ent);
            }
        }

        void resolve_all_interactions() { inter_system.resolve_all_interactions(); }

        void clean_dead() {}

        void clean_decisions() {
            for (auto &plr : players) {
                plr.dm_cpt.decision = PASS;
            }
            for (auto &var_ent : level.residents) {
                std::visit(
                    [](auto &ent_ptr) {
                        auto &ent_ref = *ent_ptr;
                        if constexpr (has_member_decision_making_component<
                                          std::remove_reference_t<decltype(ent_ref)>>::value) {
                            ent_ref.dm_cpt.decision = PASS;
                        }
                    },
                    var_ent);
            }
        }

        [[nodiscard]] std::string get_serialization() const {
            lwlog_info("getting serialization");
            auto j = nlohmann::json();
            to_json(j, *this);
            return to_string(j);
        }

        gamestate &operator=(const gamestate &) = delete;

        gamestate(const gamestate &) = delete;

        gamestate &operator=(gamestate &&rhs) noexcept {
            std::swap(level, rhs.level);
            std::swap(lvl_num, rhs.lvl_num);
            std::swap(players, rhs.players);

            std::swap(mv_system, rhs.mv_system);
            std::swap(inter_system, rhs.inter_system);

            rhs.mv_system.reset_owner(this);
            rhs.inter_system.reset_owner(this);

            return *this;
        }

        gamestate(gamestate &&rhs) noexcept : gamestate() { *this = std::move(rhs); }

       private:
        class move_system {
            gamestate *game_ptr;

           public:
            void move_to_tile(general_id id, tile_idx dest_tile) {
                assert(game_ptr);
                auto pr = room::pairFromIdx(dest_tile);
                auto &tile = game_ptr->level.get_tile(pr.first, pr.second);
                tile.resident = id;
                // pattern match for id
                // general_id@(Left entity_id)  <- take from player array
                // general_id@(Right player_id) <- take from level.residents array
                std::visit(
                    overloaded{
                        [this, pr, dest_tile](entity_id id) {
                            // if taken from residents array then check for
                            // all we need to check is existence of move component;
                            // entry_ptr : std::variant<player*, goblin*, ... etc>
                            std::visit(
                                [this, pr, dest_tile](auto &entity_ptr) {
                                    auto &ent = *entity_ptr;
                                    if constexpr (has_member_move_component<
                                                      std::remove_reference_t<decltype(ent)>>::value) {
                                        auto &ent_m_cpt = ent.m_cpt;
                                        auto &old_tile = game_ptr->level.tiles[ent_m_cpt.residency];
                                        old_tile.resident.reset();
                                        ent_m_cpt.residency = dest_tile;
                                        ent_m_cpt.x = pr.first;
                                        ent_m_cpt.y = pr.second;
                                    }
                                },
                                game_ptr->level.residents[id.value]);
                        },
                        [this, dest_tile, pr](player_id id) {
                            auto &pl_m_cpt = game_ptr->players[id.value].m_cpt;
                            auto &old_tile = game_ptr->level.tiles[pl_m_cpt.residency];
                            old_tile.resident.reset();
                            pl_m_cpt.residency = dest_tile;
                            pl_m_cpt.x = pr.first;
                            pl_m_cpt.y = pr.second;
                        }},
                    id);
            }

            template <typename entityType>
            bool general_move(entityType &ent) {
                assert(game_ptr);
                bool moved = false;
                if constexpr (
                    has_member_decision_making_component<entityType>::value and
                    has_member_move_component<entityType>::value) {
                    auto v = ent.dm_cpt.get_velocity();
                    if (v.first == 0 and v.second == 0) {
                        return moved;
                    }
                    auto des_x = ent.m_cpt.x + v.first;
                    auto des_y = ent.m_cpt.y + v.second;
                    auto opt_tile = game_ptr->level.get_tile_if_exists(des_x, des_y);
                    if (not opt_tile.has_value()) {
                        return moved;
                    }
                    auto tle = opt_tile.value();
                    if (not tle.resident.has_value()) {
                        move_to_tile(ent.id, gamestate::room::idxFromPair(des_x, des_y));
                        moved = true;
                    } else {
                        general_id interacted_id = tle.resident.value();
                        general_id interacting_id = ent.id;
                        game_ptr->inter_system.push_interaction(interacted_id, interacting_id);
                    }
                    return moved;
                }
                return moved;
            }

            move_system() = delete;

            explicit move_system(gamestate *game_ptr) noexcept : game_ptr(game_ptr) {}

            move_system(const move_system &) = delete;

            move_system(move_system &&) = default;

            move_system &operator=(const move_system &) = delete;

            move_system &operator=(move_system &&) = default;

            void reset_owner(gamestate *ptr) noexcept { game_ptr = ptr; }
        };

        class interaction_system {
            gamestate *game_ptr;
            std::queue<entity_pair> interactions;

           public:
            static void perform_interaction(entity_type &interacted, entity_type &interacting) {
                std::visit(
                    [](auto &left_ptr, auto &right_ptr) {
                        lwlog_info("calling interaction method");
                        assert(left_ptr);
                        assert(right_ptr);
                        auto &left = *left_ptr;
                        auto &right = *right_ptr;
                        interacter<std::remove_reference_t<decltype(left)>, std::remove_reference_t<decltype(right)>>::
                            interact(left, right);
                    },
                    interacted,
                    interacting);
            }

            void push_interaction(general_id inted, general_id inting) { interactions.emplace(inted, inting); }

            void resolve_all_interactions() {
                lwlog_info("there are %lu interctions to perform", game_ptr->interactions.size());
                while (not interactions.empty()) {
                    entity_pair idx_pair = interactions.front();
                    interactions.pop();
                    //                      variational-------v---------------v
                    auto perform_interaction = [](auto &interacted, auto &interacting) {
                        std::visit(
                            [](auto &left_ptr, auto &right_ptr) {
                                auto &left = *left_ptr;
                                auto &right = *right_ptr;
                                interacter<
                                    std::remove_reference_t<decltype(left)>,
                                    std::remove_reference_t<decltype(right)>>::interact(left, right);
                            },
                            interacted,
                            interacting);
                    };

                    std::visit(
                        overloaded{
                            [this, perform_interaction](player_id left_id, player_id right_id) {
                                entity_type left_player = &game_ptr->players[left_id.value];
                                entity_type right_player = &game_ptr->players[right_id.value];
                                perform_interaction(left_player, right_player);
                            },
                            [this, perform_interaction](entity_id left_id, player_id right_id) {
                                entity_type right_player = &game_ptr->players[right_id.value];
                                perform_interaction(game_ptr->level.residents[left_id.value], right_player);
                            },
                            [this, perform_interaction](player_id left_id, entity_id right_id) {
                                entity_type left_player = &game_ptr->players[left_id.value];
                                perform_interaction(left_player, game_ptr->level.residents[right_id.value]);
                            },
                            [this, perform_interaction](entity_id left_id, entity_id right_id) {
                                perform_interaction(
                                    game_ptr->level.residents[left_id.value],
                                    game_ptr->level.residents[right_id.value]);
                            },
                        },
                        idx_pair.first,
                        idx_pair.second);
                }
            }

            interaction_system() = delete;

            explicit interaction_system(gamestate *game_ptr) noexcept : game_ptr(game_ptr) {}

            interaction_system(const interaction_system &) = delete;

            interaction_system(interaction_system &&) = default;

            interaction_system &operator=(const interaction_system &) = delete;

            interaction_system &operator=(interaction_system &&) = default;

            void reset_owner(gamestate *ptr) noexcept { game_ptr = ptr; }
        };

        class drawing_system {
            gamestate *game_ptr;

           public:
            static void general_draw(entity_type &var_ent) {
                std::visit(
                    overloaded{
#define register_entity(entity_type_name, representation_function) representation_function,
#include "../utility/register_for_entities.h"
#undef register_entity
                        [](entity *ptr) -> void {}},
                    var_ent);
            }

            explicit drawing_system(gamestate *ptr) : game_ptr(ptr) {}

            void reset_owner(gamestate *ptr) { game_ptr = ptr; }
        };
        move_system mv_system;
        interaction_system inter_system;
        drawing_system draw_system;

       public:
        friend void to_json(nlohmann::json &j, const gamestate &p);

        friend void from_json(const nlohmann::json &j, gamestate &p);
    };

    void to_json(nlohmann::json &j, const gamestate &p) {
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

    void from_json(const nlohmann::json &j, gamestate &p) {
        throw std::runtime_error("One CAN NOT construct game state form it's serialization!!");
    }

}  // namespace roguelike

#endif  // ROGUE_LIKE_GAMESTATE_H
