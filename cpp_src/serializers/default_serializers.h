//
// Created by Kirill Golubev on 27.04.2021.
//
#include "../../deps/json/single_include/nlohmann/json.hpp"
#include "../entity/components.h"
#include "../utility/entity_info.h"

#ifndef ROGUE_LIKE_DEFAULT_SERIALIZERS_H
#define ROGUE_LIKE_DEFAULT_SERIALIZERS_H

namespace roguelike {
    //--------------components default serializers------------------------

    //    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(component);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(move_component, x, y, residency);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(atk_component, damage);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(health_component, health);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(repr_component, repr);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(decision_making_component, decision, eye_sight);

    //--------------end of components default serializers-----------------
    //--------------entity default serializers----------------------------

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(entity, id, level);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(player, id, lvl, h_cpt, m_cpt, a_cpt, dm_cpt, lg_cpt, repr_cpt);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(potion, id, repr_cpt);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(goblin, id, dm_cpt, h_cpt, a_cpt, m_cpt, repr_cpt);
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(wall, id, repr_cpt);

    //--------------end of entity default serializers----------------------------


}  // namespace roguelike

#endif  // ROGUE_LIKE_DEFAULT_SERIALIZERS_H
