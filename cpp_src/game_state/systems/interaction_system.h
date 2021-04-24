//
// Created by Kirill Golubev on 24.04.2021.
//

#include <queue>

#include "base_system.h"
#include "../../common.h"

#ifndef ROGUE_LIKE_INTERACTION_SYSTEM_H
#define ROGUE_LIKE_INTERACTION_SYSTEM_H
namespace roguelike {
    class gamestate;
    using entity_pair = std::pair<general_id, general_id>;
    class interaction_system : public base_system{
        std::queue<entity_pair> interactions;
      public:
        static void perform_interaction(entity_type &interacted, entity_type &interacting);

        void push_interaction(general_id inted, general_id inting);

        void resolve_all_interactions();

        interaction_system(const interaction_system &) = delete;

        interaction_system(interaction_system &&) = default;

        interaction_system &operator=(const interaction_system &) = delete;

        interaction_system &operator=(interaction_system &&) = default;

        using base_system::base_system;
        friend gamestate;
    };

}  // namespace roguelike

#endif  // ROGUE_LIKE_INTERACTION_SYSTEM_H
