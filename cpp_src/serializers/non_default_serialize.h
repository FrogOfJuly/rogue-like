//
// Created by Kirill Golubev on 27.04.2021.
//

#include "../../deps/json/single_include/nlohmann/json.hpp"
#include "../common.h"

#ifndef ROGUE_LIKE_SERIALIZE_H
namespace roguelike {
    void to_json(nlohmann::json &j, const general_id &p);
    void from_json(const nlohmann::json &j, general_id &p);

    void to_json(nlohmann::json &j, const entity_type &p);
    void from_json(const nlohmann::json &j, entity_type &p);

    void to_json(nlohmann::json &j, const logging_component &p);
    void from_json(const nlohmann::json &j, logging_component &p);
}  // namespace roguelike
#define ROGUE_LIKE_SERIALIZE_H

#endif  // ROGUE_LIKE_SERIALIZE_H
