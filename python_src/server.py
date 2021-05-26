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

BUFFERSIZE = 32768

outgoing = []
active_players = 0
dead_players = 0

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

    def get_state(self):
        return json.loads(self.state.get_serialization())

    def turn(self):
        self.state.move_nonplayers()
        self.state.resolve_all_interactions()

        self.state.clean_decisions()
        self.state.decide_next_move()

        data = json.loads(self.state.get_serialization())
        self.state.clean_logs()
        return data

    def player_action(self, player_id: int, action: cmd):
        print(f"Sent action {action} from player {player_id} to backend")
        next_player = self.state.receive_player_command(player_id, action)
        self.state.move_target_player(player_id)
        self.state.resolve_all_interactions()
        if next_player == -1:
            print("All players moved, moving enemies")
            data = self.turn()
            next_player = self.state.receive_player_command(-1)
        else:
            data = json.loads(self.state.get_serialization())
        print(f'Next player to move is {next_player}')
        return data, next_player

    def player_connect(self, player_id: int):
        print(f"Initializing player {player_id} at the backend")
        self.state.initialize_player(player_id)

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

    def shut_down(self, message: str):
        try:
            binary_string = pickle.dumps(["shutdown", message])
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


def is_active_and_alive(player_id):
    for client in outgoing:
        if player_id == client.player_id:
            return client.active and client.alive
    return False

def forward_player_action(player_action):
    player_id, action = player_action
    print(f"Received action {action} from player {player_id}.")
    new_game_state, next_player_id = backend.player_action(player_id, action)
    while not is_active_and_alive(next_player_id):
        print(f'Player {next_player_id} is dead or inactive, requesting new action.')
        new_game_state, next_player_id = backend.player_action(player_id, cmd.PASS)
    next_turn(next_player_id, new_game_state)


def append_or_activate_client(new_client):
    global active_players, outgoing
    print(f'Active player count: {active_players}')
    for i in range(len(outgoing)):
        if outgoing[i].player_id == new_client.player_id:
            if not outgoing[i].alive:
                print(f'Player {new_client.player_id} is already dead, rejecting')
                return "reject"
            outgoing[i] = new_client
            print(f'Re-activated player {new_client.player_id}')
            active_players += 1
            print(f'New active player count: {active_players}')
            new_client.update_state(last_state)
            return "reactivate"
    if active_players >= num_players:
        return "reject"
    outgoing.append(new_client)
    active_players += 1
    print(f'New active player count: {active_players}')
    print(f'Added player {new_client.player_id}')


def kill_player(player_id):
    for client in outgoing:
        if player_id == client.player_id:
            client.alive = False
            global dead_players
            dead_players += 1
            if dead_players == num_players:
                shutdown('Everyone is dead')


def shutdown(reason: str):
    for client in outgoing:
        if client.active:
            client.active = False
            client.shut_down(reason)
            global active_players
            active_players -= 1
            backend.player_disconnect(client.player_id)
            print(f'Player {client.player_id} has been disconnected due to shutdown.')
    print(f'The server has shut down successfully')
    exit()



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
                reason = f'Player {self.id} has closed the game.'
                print(reason)
                self.handle_close("closed")
                shutdown(reason)
            if message == 'acknowledge':
                if content != self.id:
                    self.id = content
                    print(f'Player has chosen id {self.id}')
                res = append_or_activate_client(RemoteDrawClient(self.sock, self.id))
                if res == 'reject':
                    self.handle_close('reject')
                    return

                if res == 'reactivate':
                    global last_state
                    print(f'Last_state is {"not " if last_state is not None else ""}None')
                    if active_players == 1 and last_state is not None:
                        print('Re-starting game')
                        next_turn(self.id, last_state)
                        return
                else:
                    backend.player_connect(self.id)
                    if active_players == num_players and not started:
                        start_game()
                    return

            if message == 'death':
                print(f'Player {self.id} has died')
                self.handle_close('death')
            if message == "action":
                forward_player_action(content)
        else:
            print(f"Received no data from {self.id}, setting the connection to inactive")
            self.handle_close()

    def handle_close(self, reason=''):
        if reason == 'reject':
            print(f"Rejecting connection from player f{self.id}")
        else:
            for client in outgoing:
                if client.player_id == self.id:
                    if client.active:
                        if reason == 'closed':
                            print(f'Player {self.id} has been disconnected due to shutdown.')
                        else:
                            print(f'Player {self.id} has disconnected.')
                        client.active = False
                        global active_players
                        active_players -= 1
                        backend.player_disconnect(self.id)
                        print(f"New active player count: {active_players}")
                        if last_player_id == self.id and active_players > 0 and reason != 'closed' and reason != 'death':
                            print(f"Player {self.id} was active, sending PASS to the backend.")
                            forward_player_action([self.id, cmd.PASS])
                        if reason == 'death':
                            kill_player(self.id)
                    break
        self.close()
        # raise Exception("Unregistered Player")


MainServer(port)
asyncore.loop()
