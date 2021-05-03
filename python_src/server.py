import socket
import asyncore
import random
import pickle
import json
import roguelike as rl
from enum import Enum
from roguelike import cmd

class Action(Enum):
    UP = 1
    DOWN = 2
    LEFT = 3
    RIGHT = 4
    ESC = 5
    PASS = 6
    ENTER = 7


BUFFERSIZE = 8192

outgoing = []


class Backend:
    def __init__(self, difficulty: str="normal"):
        self.state = rl.GameState()
        self.state.initialize(1)
        self.actions = []

    def turn(self):
        pass

    def get_state(self):
        self.state.redraw_players()
        self.state.redraw_nonplayers()
        return json.loads(self.state.get_serialization())

    def player_action(self, player_id: int, action: cmd):
        # Currently the game is single player
        # We'll have to decide on the multiplayer model eventually
        # And most of this will be moved to the turn function.
        print(f"received action {action} from player {player_id}")
        self.state.receive_player_command(0, action)
        self.state.move_players()
        self.state.resolve_all_interactions()
        self.state.move_nonplayers()
        self.state.resolve_all_interactions()
        self.state.redraw_players()
        self.state.end_turn()
        self.state.decide_next_move()
        self.state.redraw_nonplayers()

        data = json.loads(self.state.get_serialization())
        return data, player_id


backend = Backend()


class Client:
    def __init__(self, conn: socket, addr: str, player_id: int):
        self.connection = conn
        self.address = addr
        self.player_id = player_id


class RemoteDrawClient(Client):
    def update(self, game_state: str):
        try:
            self.connection.send(pickle.dumps(["state", game_state]))
        except Exception:
            pass

    def request(self, game_state: str):
        try:
            self.connection.send(pickle.dumps(["move", game_state]))
        except Exception:
            pass


minionmap = {}


def updateWorld(message):
    arr = pickle.loads(message)
    player_id = arr[1]
    action = arr[2]

    new_game_state, next_player_id = backend.player_action(player_id, action)

    for client in outgoing:
        print(client.player_id)
        if client.player_id == next_player_id:
            print(1)
            client.request(new_game_state)
        else:
            print(2)
            client.update(new_game_state)

        print('sent update data')


class MainServer(asyncore.dispatcher):
    def __init__(self, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bind(('', port))
        self.listen(10)

    def handle_accept(self):
        conn, addr = self.accept()
        print('Connection address:' + addr[0] + " " + str(addr[1]))
        player_id = random.randint(1000, 1000000)
        outgoing.append(RemoteDrawClient(conn, addr, player_id))
        conn.send(pickle.dumps(['id', player_id]))
        SecondaryServer(conn)
        conn.send(pickle.dumps(['move', backend.get_state()]))


class SecondaryServer(asyncore.dispatcher_with_send):
    def handle_read(self):
        recievedData = self.recv(BUFFERSIZE)
        if recievedData:
            updateWorld(recievedData)
        else:
            self.close()


MainServer(4321)
asyncore.loop()