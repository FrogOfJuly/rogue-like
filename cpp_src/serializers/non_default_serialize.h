//
// Created by Kirill Golubev on 27.04.2021.
//

#include "nlohmann/json.hpp"
#include "../common.h"


#ifndef ROGUE_LIKE_SERIALIZE_H
namespace roguelike {
    class gamestate;

    void to_json(nlohmann::json &j, const general_id &p);
    void from_json(const nlohmann::json &j, general_id &p);

    void to_json(nlohmann::json &j, const entity_type &p);
    void from_json(const nlohmann::json &j, entity_type &p);

    void to_json(nlohmann::json &j, const logging_component &p);
    void from_json(const nlohmann::json &j, logging_component &p);

    void to_json(nlohmann::json &j, const simple_inventory_component &p);
    void from_json(const nlohmann::json &j, simple_inventory_component &p);

    void to_json(nlohmann::json &j, const expirience_components &p);
    void from_json(const nlohmann::json &j, expirience_components &p);

    void to_json(nlohmann::json &j, const gamestate &p);
    void from_json(const nlohmann::json &j, gamestate &p);
}  // namespace roguelike
#define ROGUE_LIKE_SERIALIZE_H

#endif  // ROGUE_LIKE_SERIALIZE_H
