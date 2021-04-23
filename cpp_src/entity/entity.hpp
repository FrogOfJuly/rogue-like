//
// Created by Kirill Golubev on 17.04.2021.
//

#include "../common.h"
#include "components.h"

#ifndef ROGUE_LIKE_ENTITY_HPP
#define ROGUE_LIKE_ENTITY_HPP
namespace roguelike {
    struct entity {
        entity_id id;
        int level = -1;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(entity, id, level);

    template <typename interacted, typename interacting>
    struct interacter {  // bc C++ does not permit partial function specialization
       public:
        static void interact(interacted &inted, interacting &inting) {
            //            std::cout << "unknown interacted by unknown" << std::endl;
            return;
        }
    };
}  // namespace roguelike

#endif  // ROGUE_LIKE_ENTITY_HPP
