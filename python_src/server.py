import socket
import asyncore
import random
import pickle
import json
import roguelike as rl
from enum import Enum
from roguelike import cmd


BUFFERSIZE = 8192

num_players = 1
outgoing = []


class Backend:
    def __init__(self, difficulty: str="normal"):
        self.state = rl.GameState()
        self.state.initialize(num_players)

    def turn(self):
        pass

    def get_state(self):
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

        self.state.clean_decisions()
        self.state.decide_next_move()

        data = json.loads(self.state.get_serialization())
        self.state.clean_logs()
        return data, player_id

    def handle_disconnection(self, player_id):
        # self.state.set_player_inactive(player_id)
        # if (len(outgoing) == 0):
        pass



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
        if (len(outgoing) >= num_players):
            conn.send(pickle.dumps(['reject', "Too many players"]))
        player_id = random.randint(1000, 1000000)
        outgoing.append(RemoteDrawClient(conn, addr, player_id))
        conn.send(pickle.dumps(['id', player_id]))
        SecondaryServer(conn, player_id)
        if(len(outgoing) == num_players):
            conn.send(pickle.dumps(['move', backend.get_state()]))


class SecondaryServer(asyncore.dispatcher_with_send):
    def __init__(self, conn, id):
        super().__init__(conn)
        self.id = id

    def handle_read(self):
        recievedData = self.recv(BUFFERSIZE)
        if recievedData:
            if pickle.loads(recievedData)[0] == 'exit':
                print(f'Player {self.id} has closed the game.')
                self.close()
                exit()
            updateWorld(recievedData)
        else:
            self.handle_close()

    def handle_close(self):
        self.close()
        for i in range(len(outgoing)):
            if outgoing[i].player_id == self.id:
                print(f'Player {self.id} has disconnected.')
                outgoing.pop(i)
                backend.handle_disconnection(self.id)
                return None
        # raise Exception("Unregistered Player")


MainServer(4321)
asyncore.loop()