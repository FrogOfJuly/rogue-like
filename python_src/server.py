import socket
import asyncore
import random
import pickle
import json
import roguelike as rl
from enum import Enum
from roguelike import cmd
import argparse

parser = argparse.ArgumentParser(description="Server for Crypt of the Darkness")
parser.add_argument("-n", "--num_of_players", default=1, type=int, dest='num_players',
                    help="Number of players (currently only one is supported properly)")
parser.add_argument("-p", "--port", default=4321, type=int, dest='port',
                    help="Port number for the server")
args = parser.parse_args()

BUFFERSIZE = 16384

outgoing = []
active_players = 0

num_players = args.num_players
port = args.port

last_player_id = None
last_state = None

started = False

class Backend:
    def __init__(self, difficulty: str = "normal"):
        self.state = rl.GameState()

    def initialize_backend(self):
        global num_players
        print(f"Initializing the backend for {num_players} players")
        self.state.initialize(num_players)
        # player_id = self.state.get_next_player()
        player_id = outgoing[0].player_id
        data = json.loads(self.state.get_serialization())
        return player_id, data

    def turn(self):
        pass

    def get_state(self):
        return json.loads(self.state.get_serialization())

    def player_action(self, player_id: int, action: cmd):
        # Currently the game is single player
        # We'll have to decide on the multiplayer model eventually
        # And most of this will be moved to the turn function.
        print(f"Sent action {action} from player {player_id} to backend")
        self.state.receive_player_command(0, action)
        self.state.move_players()
        self.state.resolve_all_interactions()
        self.state.move_nonplayers()
        self.state.resolve_all_interactions()

        self.state.clean_decisions()
        self.state.decide_next_move()

        data = json.loads(self.state.get_serialization())
        self.state.clean_logs()
        return data, player_id

    def player_connect(self, player_id: int):
        print(f"Initializing player {player_id} at the backend")
        # self.state.initialize_player(player_id)

    def player_disconnect(self, player_id):
        print(f"Disconnecting player {player_id} at the backend")
        # self.state.set_player_inactive(player_id)
        # if (len(outgoing) == 0):
        #   maybe add timer to disable server if inactive for a while?


backend = Backend()


class Client:
    def __init__(self, conn: socket, player_id: int):
        self.connection = conn
        self.player_id = player_id
        self.active = True
        self.alive = True


class RemoteDrawClient(Client):
    def update_state(self, game_state: dict):
        try:
            binary_string = pickle.dumps(["state", game_state])
            # print(f'Sending {binary_string} to {self.player_id}')
            self.connection.send(binary_string)
        except Exception:
            pass

    def request_action(self, game_state: dict):
        try:
            binary_string = pickle.dumps(["move", game_state])
            # print(f'Sending {binary_string} to {self.player_id}')
            self.connection.send(binary_string)
        except Exception:
            pass


def next_turn(next_player_id, new_game_state):
    global last_player_id, last_state
    last_player_id, last_state = next_player_id, new_game_state
    print(f'Verification: new_game_state is {"not " if last_state is not None else ""}None')
    for client in outgoing:
        if client.player_id == next_player_id:
            print(f'Requested action from player {next_player_id}')
            client.request_action(new_game_state)
        else:
            print(f'Sent updated state to player {client.player_id}')
            client.update_state(new_game_state)
        print('Sent update data')


def start_game():
    global started
    started = True
    first_player, game_state = backend.initialize_backend()
    next_turn(first_player, game_state)


def forward_player_action(player_action):
    player_id, action = player_action
    print(f"Received action {action} from player {player_id}")
    new_game_state, next_player_id = backend.player_action(player_id, action)
    next_turn(next_player_id, new_game_state)


def append_or_activate_client(new_client):
    global active_players, outgoing
    print(f'Active player count: {active_players}')
    for i in range(len(outgoing)):
        if outgoing[i].player_id == new_client.player_id:
            outgoing[i] = new_client
            print(f'Re-activated player {new_client.player_id}')
            active_players += 1
            print(f'New active player count: {active_players}')
            global last_state
            print(f'Last_state is {"not " if last_state is not None else ""}None')
            if active_players == 1 and last_state is not None:
                print('Re-starting game')
                next_turn(new_client.player_id, last_state)
            return
    outgoing.append(new_client)
    active_players += 1
    print(f'New active player count: {active_players}')
    print(f'Added player {new_client.player_id}')


class MainServer(asyncore.dispatcher):
    def __init__(self, port):
        asyncore.dispatcher.__init__(self)
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.bind(('', port))
        self.listen(10)

    def handle_accepted(self, sock, addr):
        print('Connection address:' + addr[0] + " " + str(addr[1]))
        if active_players >= num_players:
            sock.send(pickle.dumps(['reject', "Too many players"]))
            print("Rejected connection: too many active players")
        player_id = random.randint(1000, 1000000)
        sock.send(pickle.dumps(['id', player_id]))
        SecondaryServer(sock, player_id)


class SecondaryServer(asyncore.dispatcher_with_send):
    def __init__(self, conn, id):
        super().__init__(conn)
        self.id = id
        self.sock = conn

    def handle_read(self):
        recievedData = self.recv(BUFFERSIZE)
        if recievedData:
            message, content = pickle.loads(recievedData)
            if message == 'exit':
                print(f'Player {self.id} has closed the game.')
                self.close()
                exit()
            if message == 'acknowledge':
                if content != self.id:
                    self.id = content
                    print(f'Player has chosen id {self.id}')
                backend.player_connect(self.id)
                append_or_activate_client(RemoteDrawClient(self.sock, self.id))
                if active_players == num_players and not started:
                    start_game()
            if message == "action":
                forward_player_action(content)
        else:
            print(f"Received no data from {self.id}, setting the connection to inactive")
            self.handle_close()

    def handle_close(self):
        for client in outgoing:
            if client.player_id == self.id:
                if client.active:
                    print(f'Player {self.id} has disconnected.')
                    client.active = False
                    global active_players
                    active_players -= 1
                    backend.player_disconnect(self.id)
                    print(f"New active player count: {active_players}")
                break
        self.close()
        # raise Exception("Unregistered Player")


MainServer(port)
asyncore.loop()
