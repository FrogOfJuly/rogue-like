//
// Created by Kirill Golubev on 17.04.2021.
//
#include <queue>
#include <map>
#include <memory>
#include "../level/room.hpp"

#ifndef ROGUE_LIKE_GAMESTATE_H
#define ROGUE_LIKE_GAMESTATE_H


namespace roguelike {
    constexpr int room_size = 4;

    class gamestate {
        using entity_pair = std::pair<general_id, general_id>;
        using room = room<room_size, room_size>;
        room level;
        int lvl_num = -1;
        std::queue<entity_pair> interactions;
        std::vector<player> players;

        void move_to_tile(general_id id, tile_idx dest_tile) {
            auto pr = room::pairFromIdx(dest_tile);
            auto &tile = level.get_tile(pr.first, pr.second);
            tile.resident = id;
            //pattern match for id
            //general_id@(Left entity_id)  <- take from player array
            //general_id@(Right player_id) <- take from level.residents array
            std::visit(overloaded{
                    [this, pr, dest_tile](entity_id id) {
                        //if taken from residents array then check for
                        //all we need to check is existence of move component;
                        //entry_ptr : std::variant<player*, goblin*, ... etc>
                        std::visit(
                                [pr, dest_tile](auto &entity_ptr) {
                                    auto &ent = *entity_ptr;
                                    if constexpr(has_member_m_cpt<
                                            std::remove_reference_t<decltype(ent)>
                                    >::value) {
                                        auto &ent_m_cpt = ent.m_cpt;
                                        ent_m_cpt.residency = dest_tile;
                                        ent_m_cpt.x = pr.first;
                                        ent_m_cpt.y = pr.second;
                                    }
                                }, level.residents[id.value]);
                    },
                    [this, dest_tile, pr](player_id id) {
                        auto &pl_m_cpt = players[id.value].m_cpt;
                        pl_m_cpt.residency = dest_tile;
                        pl_m_cpt.x = pr.first;
                        pl_m_cpt.y = pr.second;
                    }}, id);
        }

        template<typename entityType>
        bool general_move(entityType &ent) {
            bool moved = false;
            if constexpr (
                    has_member_dm_cpt<entityType>::value and
                    has_member_m_cpt<entityType>::value) {

                auto v = ent.dm_cpt.get_velocity();
                if (v.first == 0 and v.second == 0) {
                    return moved;
                }
                auto des_x = ent.m_cpt.x + v.first;
                auto des_y = ent.m_cpt.y + v.second;
                auto opt_tile = level.get_tile_if_exists(des_x, des_y);
                if (not opt_tile.has_value()) {
                    return moved;
                }
                auto tle = opt_tile.value();
                if (not tle.resident.has_value()) {
                    move_to_tile(ent.id, level.idxFromPair(des_x, des_y));
                    moved = true;
                } else {
                    general_id interacted_id = tle.resident.value();
                    general_id interacting_id = ent.id;
                    interactions.push(std::make_pair(
                            interacted_id,
                            interacting_id
                    ));
                }
                return moved;
            }
            return moved;
        }

    public:
        gamestate() = default;

        void initialize(int player_num) {
            lwlog_info("Initializning gamestate object");
            lwlog_info("allocating  player objects");

            lvl_num = 0;
            level.generate_level(lvl_num);

            for (int i = 0; i < player_num; ++i) {
                lwlog_info("placing player");
                auto p = player(i);
                p.dm_cpt.decision = LEFT;
                players.push_back(p);
                level.spawn_on_level(players[i]);
            }

            for (int i = 0; i < 2; ++i) {
                lwlog_info("placing goblin");
                auto g = std::make_unique<goblin>();
                g->id = entity_id{(int) level.residents.size()}; //bad cast, do something better
                level.spawn_on_level(*g);
                level.residents.emplace_back(std::move(g));
            }
            auto e = std::make_unique<entity>();
            lwlog_info("placing entity");
            e->id = entity_id{(int) level.residents.size()}; //bad cast, do something better
            level.spawn_on_level(*e);
            level.residents.emplace_back(std::move(e));


            auto p = std::make_unique<potion>();
            lwlog_info("placing potion");
            p->id = entity_id{(int) level.residents.size()}; //do better
            level.spawn_on_level(*p);
            level.residents.emplace_back(std::move(p));
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
                general_move<player>(plr);
            }
        }

        void move_nonplayers() {
            for (auto &ent : level.residents) {
                std::visit([this](auto &ent_ptr) {
                    auto &ent_ref = *ent_ptr;
                    general_move<
                            std::remove_reference_t<decltype(ent_ref)>
                    >(ent_ref);
                }, ent);
            }
        }

        static void perform_interaction(entity_type &interacted, entity_type &interacting) {
            std::visit([](auto &left_ptr, auto &right_ptr) {
                lwlog_info("calling interaction method");
                assert(left_ptr);
                assert(right_ptr);
                auto &left = *left_ptr;
                auto &right = *right_ptr;
                interacter<
                        std::remove_reference_t<decltype(left)>,
                        std::remove_reference_t<decltype(right)>
                >::interact(left, right);
            }, interacted, interacting);
        }

        void pp(player_id left_id, player_id right_id) {
            // if there is a way to perform player to variational cast better??
            assert(right_id.value < players.size());
            assert(left_id.value < players.size());
            lwlog_info("interaction between player and player");
            entity_type left_player = std::unique_ptr<player>(&players[left_id.value]);
            entity_type right_player = std::unique_ptr<player>(&players[right_id.value]);
            perform_interaction(left_player,
                                right_player);
            lwlog_info("releasing left pointer");
            std::visit([](auto &ptr) {
                (void) ptr.release(); //release ptr. Not freed, bc memory is managed by vector of players.
            }, left_player);
            lwlog_info("releasing right pointer");
            std::visit([](auto &ptr) {
                (void) ptr.release(); //release ptr. Not freed, bc memory is managed by vector of players.
            }, right_player);

        }

        void ep(entity_id left_id, player_id right_id) {
            lwlog_info("interaction between unknown %d and player", left_id.value);
            assert(right_id.value < players.size());
            assert(left_id.value < level.residents.size());
            entity_type right_player = std::unique_ptr<player>(&players[right_id.value]);
            perform_interaction(level.residents[left_id.value],
                                right_player);
            lwlog_info("releasing right pointer");
            std::visit([](auto &ptr) {
                (void) ptr.release(); //release ptr. Not freed, bc memory is managed by vector of players.
            }, right_player);
        }

        void ee(entity_id left_id, entity_id right_id) {
            lwlog_info("interaction between unknown%d and unknown%d",
                    left_id.value,
                    right_id.value);
            assert(right_id.value < level.residents.size());
            assert(left_id.value < level.residents.size());
            perform_interaction(level.residents[left_id.value],
                                level.residents[right_id.value]);
        }

        void pe(player_id left_id, entity_id right_id) {
            lwlog_info("interaction between player and unknown %d", right_id.value);
            assert(left_id.value < players.size());
            assert(right_id.value < level.residents.size());
            entity_type left_player = std::unique_ptr<player>(&players[left_id.value]);
            std::visit([](auto &ptr) {
                (void) ptr.release(); //release ptr. Not freed, bc memory is managed by vector of players.
            }, left_player);
            lwlog_info("releasing left pointer");
            perform_interaction(left_player,
                                level.residents[right_id.value]);
        }


        static std::pair<int, bool> unsafe_recover_id(general_id id) {
            return std::visit(
                    [](auto &&id) -> std::pair<int, bool> {
                        lwlog_info("recovering");
                        if constexpr (std::is_same_v<decltype(id), entity_id>) {
                            lwlog_info("entity");
                            return std::make_pair(id.value, false);
                        } else {
                            lwlog_info("player");
                            return std::make_pair(id.value, true);
                        }
                    }, id);
        }

        void interact() {
            lwlog_info("there are %lu interctions to perform", interactions.size());
            while (not interactions.empty()) {
                entity_pair idx_pair = interactions.front();
                interactions.pop();
                //                      variational-------v---------------v
                auto perform_interaction = [](auto &interacted, auto &interacting) {
                    std::visit([](auto &left_ptr, auto &right_ptr) {
                        auto &left = *left_ptr;
                        auto &right = *right_ptr;
                        interacter<
                                std::remove_reference_t<decltype(left)>,
                                std::remove_reference_t<decltype(right)>
                        >::interact(left, right);
                    }, interacted, interacting);
                };

                auto release_player = [](auto &plr_uniq) {
                    (void) plr_uniq.release();
                };

                std::visit(
                        overloaded{
                                [this, perform_interaction, release_player](player_id left_id, player_id right_id) {
                                    entity_type left_player = std::unique_ptr<player>(&players[left_id.value]);
                                    entity_type right_player = std::unique_ptr<player>(&players[right_id.value]);
                                    perform_interaction(left_player,
                                                        right_player);
                                    std::visit(release_player, left_player);
                                    std::visit(release_player, right_player);
                                },
                                [this, perform_interaction, release_player](entity_id left_id, player_id right_id) {
                                    entity_type right_player = std::unique_ptr<player>(&players[right_id.value]);
                                    perform_interaction(level.residents[left_id.value],
                                                        right_player);
                                    std::visit(release_player, right_player);
                                },
                                [this, perform_interaction, release_player](player_id left_id, entity_id right_id) {
                                    entity_type left_player = std::unique_ptr<player>(&players[left_id.value]);
                                    perform_interaction(left_player,
                                                        level.residents[right_id.value]);
                                    std::visit(release_player, left_player);
                                },
                                [this, perform_interaction](entity_id left_id, entity_id right_id) {
                                    perform_interaction(level.residents[left_id.value],
                                                        level.residents[right_id.value]);
                                },
                        }, idx_pair.first, idx_pair.second
                );
            }
        }

        void clean_dead() {

        }

        void clean_decisions() {
            for (auto &plr : players) {
                plr.dm_cpt.decision = PASS;
            }
            for (auto &var_ent : level.residents) {
                std::visit([](auto &ent_ptr) {
                    auto &ent_ref = *ent_ptr;
                    if constexpr(has_member_dm_cpt<
                            std::remove_reference_t<decltype(ent_ref)>
                    >::value) {
                        ent_ref.dm_cpt.decision = PASS;
                    }
                }, var_ent);
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

        gamestate &operator=(gamestate &&)
        = default;

        gamestate(gamestate
                  &&) = default;

        /*
         * init() //I am sure something additional is needed before first level
         * generate_level(int lvl_num)
         * void move()      // moving everybody
         * void interact()  // resolving interactions
         * void clean()     // clean level from dead bodies and etc.
        */

        friend void to_json(nlohmann::json &j, const gamestate &p);

        friend void from_json(const nlohmann::json &j, gamestate &p);
    };

    void to_json(nlohmann::json &j, const gamestate &p) {
        auto room_json = nlohmann::json();
        for (const auto &tle : p.level.tiles) {
            auto cur_tile_json = nlohmann::json();
            if (tle.empty()) {
                cur_tile_json["tile"] = "";
                room_json.push_back(cur_tile_json);
                continue;
            }
            auto resident_json = std::visit(overloaded{
                    [&p](player_id id) {
                        auto j_local = nlohmann::json();
                        nlohmann::to_json(j_local["player"], p.players[id.value]);
                        return j_local;
                    },
                    [&p](entity_id id) {
                        auto j_local = nlohmann::json();
                        const auto &var_ent = p.level.residents[id.value];
                        nlohmann::to_json(j_local["entity"], var_ent);
                        //j_local["entity"]["repr_cpt"]["repr"] = var_ent.repr_cpt.repr();
                        // ^--- only if entity has repr component.
                        if (not j_local["entity"].contains("repr_cpt")) {
                            j_local["entity"]["repr_cpt"]["repr"] = "?";
                            return j_local;
                        }
                        return j_local;
                    }}, tle.resident.value());
            cur_tile_json["tile"] = resident_json;
            room_json.push_back(cur_tile_json);
        }
        j["level"] = room_json;
    }

    void from_json(const nlohmann::json &j, gamestate &p) {
        throw std::runtime_error("One CAN NOT construct game state form it's serialization!!");
    }

} //namespace roguelike

#endif //ROGUE_LIKE_GAMESTATE_H
