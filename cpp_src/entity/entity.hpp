//
// Created by Kirill Golubev on 17.04.2021.
//

#include "../common.h"
#include "components.h"

#ifndef ROGUE_LIKE_ENTITY_HPP
#define ROGUE_LIKE_ENTITY_HPP
namespace roguelike {
    struct entity {
        explicit entity(int id) : id{id} {}

        entity_id id{-1};
        int level = -1;
    };

    template <typename interacted, typename interacting>
    struct interacter {  // C++ does not permit partial function template specialization
      public:
        static inline void interact(interacted &inted, interacting &inting) {
            std::string inted_name = "unknown";
            std::string inting_name = "unknown";
            if constexpr (has_member_name_component<interacted>::value) {
                inted_name = inted.nm_cpt.name;
            }
            if constexpr (has_member_name_component<interacting>::value) {
                inting_name = inting.nm_cpt.name;
            }
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_ENTITY_HPP
