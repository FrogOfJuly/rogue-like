import rogue_like as rl

if __name__ == '__main__':
    g = rl.GameState()
    g.initialize(5)
    g.receive_player_command(1, rl.up)
    print(g.get_serialization())