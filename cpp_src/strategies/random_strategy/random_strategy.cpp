//
// Created by Kirill Golubev on 02.05.2021.
//

#include "random_strategy.h"
#include "../../utility/entity_info.h"

#include "../../utility/utils.h"

void roguelike::random_strategy::form_decision(roguelike::decision_making_component& dm_cpt) {
    dm_cpt.decision = utils::get_random_active_action();
}
