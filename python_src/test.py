import roguelike as rl

if __name__ == '__main__':
    g = rl.GameState()
    g.initialize(5)
    g.receive_player_command(1, rl.UP)
    # print(g.get_serialization())
    g.move_players()
    g.perform_interactions()
    # print(g.get_serialization())