import math
import pickle
import select
import socket
import sys
from roguelike import cmd
from enum import Enum
import os


def cls():
    os.system('cls' if os.name == 'nt' else 'clear')


BUFFERSIZE = 8192

serverAddr = '127.0.0.1'
if len(sys.argv) == 2:
    serverAddr = sys.argv[1]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((serverAddr, 4321))

player_id = 0


def render(game_state: dict):
    cls()
    if game_state == {'start'}:
        return
    game_state = game_state['level']
    H = int(math.sqrt(len(game_state)))
    W = H
    player = None
    newlog = None
    for tile_ in game_state:
        tile = tile_["tile"]
        if tile and "player" in tile:
            player = tile["player"]
            newlog = player["lg_cpt"]["log"].split("\n")
            break
    print(f"# Room: {1}/10")  # TODO
    print(f"# Score: {123:5}")  # TODO
    for i in range(H):
        for j in range(W):
            tile = game_state[i * W + j]['tile']
            if tile is None:
                print(" ", end='')
            elif 'player' in tile:
                print(tile['player']['repr_cpt']['repr'], end='')
            elif 'entity' in tile:
                print(tile['entity']['repr_cpt']['repr'], end='')
        print()
    print(f'Level: {player["lvl"]:3}, Health: {player["h_cpt"]["health"]:3}, Damage: {player["a_cpt"]["damage"]}')
    if not newlog:
        return
    print(f'log:')
    for entry in newlog[-5:]:
        print(f'> {entry}')
    if len(newlog) < 5:
        for i in range(5 - len(newlog)):
            print('>')




while True:
    ins, outs, ex = select.select([s], [], [], 0)
    for inm in ins:
        gameEvent = pickle.loads(inm.recv(BUFFERSIZE))
        if gameEvent[0] == 'id':
            player_id = gameEvent[1]
            print(player_id)
        if gameEvent[0] == 'state':
            render(gameEvent[1])
        if gameEvent[0] == 'move':
            render(gameEvent[1])

            action = cmd.PASS
            key = input("Next action: ")
            if key is not None:
                if key == 'w':
                    action = cmd.UP
                elif key == 's':
                    action = cmd.DOWN
                elif key == 'a':
                    action = cmd.LEFT
                elif key == 'd':
                    action = cmd.RIGHT
                elif key == 'x':
                    s.close()
                    exit()
            ge = ['action', player_id, action]
            s.send(pickle.dumps(ge))
