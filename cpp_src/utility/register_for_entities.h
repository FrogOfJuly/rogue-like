//
// Created by Kirill Golubev on 20.04.2021.
//

register_entity(goblin, [](goblin* g) -> void {
    switch (g->dm_cpt.decision) {
        case LEFT:
            g->repr_cpt.repr = "<";
            break;
        case RIGHT:
            g->repr_cpt.repr = ">";
            break;
        case DOWN:
            g->repr_cpt.repr = "v";
            break;
        case UP:
            g->repr_cpt.repr = "^";
            break;
        default:
            g->repr_cpt.repr = "x";
    }
}) register_entity(player, [](player* p) -> void {
    p->repr_cpt.repr = std::to_string(p->id.value);
}) register_entity(potion, [](potion* ptr) -> void { ptr->repr_cpt.repr = "p"; })