//
// Created by Kirill Golubev on 02.05.2021.
//

#include "passive_strategy.h"

#include "../../utility/entity_info.h"

void roguelike::passive_strategy::form_decision(roguelike::decision_making_component& dm_cpt) {
    dm_cpt.decision = cmd::PASS;
}
