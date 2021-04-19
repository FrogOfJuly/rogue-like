import pickle
import select
import socket
import sys
import json
from enum import Enum
import os


def cls():
    os.system('cls' if os.name=='nt' else 'clear')


BUFFERSIZE = 2048

serverAddr = '127.0.0.1'
if len(sys.argv) == 2:
    serverAddr = sys.argv[1]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((serverAddr, 4321))

player_id = 0


class Action(Enum):
    UP = 1
    DOWN = 2
    LEFT = 3
    RIGHT = 4
    USE = 5


def render(game_state: dict):
    cls()
    if(game_state == {'start'}):
        return
    game_state = game_state['GameState']
    print("#------------------------------------------------------------------------#")
    print(f"# current room: {game_state['lvl_num']}/10             |stats: --------------------------------#")
    print(f"# score: {game_state['score']}                     | lvl         : {game_state['player']['level']}                       #")


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

            action = Action.USE
            key = input("Next action: ")
            if key is not None:
                if key == 'w':
                    action = Action.UP
                elif key == 's':
                    action = Action.DOWN
                elif key == 'a':
                    action = Action.LEFT
                elif key == 'd':
                    action = Action.RIGHT
                elif key == 'x':
                    s.close()
                    exit()
            ge = ['action', player_id, action]
            s.send(pickle.dumps(ge))
