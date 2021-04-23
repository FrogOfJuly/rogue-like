import roguelike as rl
import json
import math


def print_serialized_gamestate(jsoned_game_state):
    level = state["level"]
    n = int(math.sqrt(len(level)))
    assert n * n == len(level)
    for j in range(n):
        print("-" * (n + 2))
        print("|", end="")
        for i in range(n):

            cur_tile = level[i + j * n]["tile"]
            if not cur_tile:
                print(" ", end='')
            else:
                key, = cur_tile.keys()
                print(cur_tile[key]["repr_cpt"]["repr"], end='')
        print("|")


if __name__ == '__main__':
    g = rl.GameState()
    g.initialize(5)
    g.redraw_players()
    g.redraw_nonplayers()
    state = json.loads(g.get_serialization())
    print("after init:")
    print_serialized_gamestate(state)

    g.move_players()
    g.resolve_all_interactions()
    g.move_nonplayers()
    g.resolve_all_interactions()
    g.clean_decisions()
    g.redraw_players()
    g.redraw_nonplayers()
    str_state = g.get_serialization()
    state = json.loads(str_state)
    print("after interactions")
    print_serialized_gamestate(state)
    with open("sample_game_state.json", 'w') as f:
        f.write(str_state)
