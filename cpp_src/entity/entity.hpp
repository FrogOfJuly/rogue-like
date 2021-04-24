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

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(entity, id, level);

    template <typename interacted, typename interacting>
    struct interacter {  // C++ does not permit partial function template specialization
      public:
        static inline void interact(interacted &inted, interacting &inting) {
            //            std::cout << "unknown interacted by unknown" << std::endl;
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_ENTITY_HPP
