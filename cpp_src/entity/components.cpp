//
// Created by Kirill Golubev on 24.04.2021.
//

#include "components.h"

void roguelike::to_json(nlohmann::json& j, const logging_component& p) {
    //        j["id"] = p.id;
    j["log"] = p.log.str();
}
void roguelike::from_json(const nlohmann::json& j, logging_component& p) {
    //    p.id = j["id"];
    p.log << j["log"];
}
