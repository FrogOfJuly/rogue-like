import math
import pickle
import select
import socket
import sys
import json  # for logging only
import curses
from roguelike import cmd
import os
import argparse

parser = argparse.ArgumentParser(description="Client for Crypt of the Darkness")
parser.add_argument("-i", "--player_id", default=0, type=int, dest='player_id',
                    help="Player id to rejoin a session. Will be assigned by the server by default")
parser.add_argument("-p", "--port", default=4321, type=int, dest='port',
                    help="Port number of the server")
parser.add_argument("-a", "--address", default='127.0.0.1', type=str, dest='server_addr',
                    help="IP address of the server")
parser.add_argument("-l", "--log", default=False, action="store_true",
                    help="Enable game state logging")
args = parser.parse_args()

player_id = args.player_id
force_id = player_id != 0

BUFFERSIZE = 16384

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((args.server_addr, args.port))

log = []
printed_log_len = 7


def init_log():
    log_file = f'{player_id}.json'
    with open(log_file, "w") as file:
        file.write("[\n]")


def dump_log(game_state: dict):
    log_file = f'{player_id}.json'
    with open(log_file, "a+") as file:
        file.seek(file.tell() - 2, os.SEEK_SET)
        file.truncate()
        json.dump(game_state, file)
        file.write(",\n")
        file.writelines("]")


def disconnect(string):
    s.close()
    curses.nocbreak()
    curses.echo()
    curses.endwin()
    print(string)
    print()
    exit()


def color(level, health):
    clr = [[0, 250, 245],
           [83, 41, 43],
           [22, 21, 28],
           [202, 166, 199],
           [209, 203, 167],
           [10, 161, 89],
           [214, 207, 205],
           [88, 76, 70],
           [227, 12, 187],
           [239, 241, 243]]
    if health > 2:
        health = 2
    elif health < 0:
        health = 0
    if level >= 9:
        return clr[9][health]
    elif level <= 0:
        return clr[0][health]
    return clr[level][health]


def render(stdscr, game_state: dict, flags: dict):
    stdscr.clear()
    stdscr.refresh()
    global log
    # print(game_state)
    if game_state == {'start'}:
        return
    player = None
    if str(player_id) in game_state['players']:
        player = game_state['players'][str(player_id)]['player']
        newlog = player["lg_cpt"]["log"].split("\n")
        log += list(filter(lambda x: x != '', newlog))
        while len(log) > printed_log_len:
            log.pop(0)
    if not player or player['h_cpt']['health'] <= 0:
        s.send(pickle.dumps(['death', False]))
        disconnect(f"You died!")
    game_state = game_state['level']
    H = int(math.sqrt(len(game_state)))
    W = H
    legend = {}
    # Level
    for i in range(H):
        for j in range(W):
            tile = game_state[i * W + j]['tile']
            to_print = None
            lvl = None
            hlth = 0
            if tile is None:
                to_print = "   "
            elif 'player' in tile:
                to_print = f" {tile['player']['repr_cpt']['repr']} "
                legend['player'] = tile['player']['repr_cpt']['repr']
                lvl = tile['player']['exp_cpt']['level']
            elif 'entity' in tile:
                repr = tile['entity']['repr_cpt']['repr']
                if repr == '█':
                    to_print = '███'
                else:
                    if 'nm_cpt' in tile['entity']:
                        if tile['entity']['nm_cpt']['name'] not in legend:
                            legend[tile['entity']['nm_cpt']['name']] = [tile['entity']['repr_cpt']['repr']]
                        elif tile['entity']['repr_cpt']['repr'] not in legend[tile['entity']['nm_cpt']['name']]:
                            legend[tile['entity']['nm_cpt']['name']].append(tile['entity']['repr_cpt']['repr'])
                    to_print = f' {repr} '
                    if 'h_cpt' in tile['entity'] and tile['entity']['h_cpt']['max_health'] > 0:
                        hlth = 2 - int(tile['entity']['h_cpt']['health'] * 3 / tile['entity']['h_cpt']['max_health'])
                    if 'lvl_cpt' in tile['entity']:
                        lvl = tile['entity']['lvl_cpt']['lvl']
            if (lvl):
                stdscr.addstr(i, j * 3, to_print, curses.color_pair(color(lvl, hlth)))
            else:
                stdscr.addstr(i, j * 3, to_print)

    add_damage = 0

    x = 0
    # Inventory
    for item in player["s_inv_cpt"]:
        for key, value in item.items():
            section = " "
            if key == "sword":
                section = "W: "
            elif key == "shield":
                section = "S: "
            elif key == "armor":
                section = "A: "
            elif key == "potion":
                section = "P: "

            stdscr.addstr(H, x, section)
            to_print = ("" if value == "none" else value["entity"]["repr_cpt"]["repr"])
            lvl = 0 if value == "none" else value["entity"]["lvl_cpt"]["lvl"]
            stdscr.addstr(H, x+3, to_print, curses.color_pair(color(lvl, 0)))
            x = x + 5

    # Stats and info
    stdscr.addstr(0, W * 3 + 1, f"{'Room:':9} {1}/10")  # TODO
    stdscr.addstr(1, W * 3 + 1, f"{'Score:':9} {123}")  # TODO
    stdscr.addstr(3, W * 3 + 1, f"{'Level:':9} {player['exp_cpt']['level']}")
    stdscr.addstr(3, W * 3 + 1,
                  f"{'Exp:':9} {player['exp_cpt']['experience']}/{player['exp_cpt']['exp_until_next_level']}")
    stdscr.addstr(4, W * 3 + 1, f"{'Health:':9} {player['h_cpt']['health']}")
    stdscr.addstr(5, W * 3 + 1, f"{'Damage:':9} {player['a_cpt']['damage']}")
    stdscr.addstr(7, W * 3 + 1, f'log:')

    for i, entry in enumerate(log):
        stdscr.addstr(8 + i, W * 3 + 1, f'> {entry}')
    if len(log) < printed_log_len:
        for i in range(printed_log_len - len(log)):
            stdscr.addstr(8 + len(log) + i, W * 3 + 1, '>')

    # Help or legend
    if len(flags) == 0:
        stdscr.addstr(H + 2, 0, f'Press H for help.')
    elif 'legend' in flags:
        for i, key in enumerate(sorted(legend.keys())):
            if " " in legend[key]:
                legend[key].remove(" ")
            if not legend[key]:
                continue
            stdscr.addstr(H + 2 + i, 0, f'{f"{key}:":20} {", ".join(legend[key])}')
    elif 'help' in flags:
        stdscr.addstr(H + 2, 0, f'WASD for movement.')
        stdscr.addstr(H + 3, 0, f'E for potion.')
        stdscr.addstr(H + 4, 0, f'F to skip turn.')
        stdscr.addstr(H + 4, 0, f'L for the legend.')
        stdscr.addstr(H + 5, 0, f'P to pause game (temporarily disconnect).')
        stdscr.addstr(H + 6, 0, f'X to exit game.')
    elif 'waiting' in flags:
        stdscr.addstr(H + 2, 0, 'Awaiting turn')
    stdscr.refresh()


def get_action(game_state: dict):
    action = cmd.PASS
    valid = False
    while not valid:
        valid = True
        key = stdscr.getch()
        if key is not None:
            if key == ord('w'):
                action = cmd.UP
            elif key == ord('s'):
                action = cmd.DOWN
            elif key == ord('a'):
                action = cmd.LEFT
            elif key == ord('d'):
                action = cmd.RIGHT
            elif key == ord('e'):
                action = cmd.ENTER
            elif key == ord('p'):
                disconnect(f"Paused game. Return with id {player_id}")
            elif key == ord('x'):
                s.send(pickle.dumps(['exit', False]))
                disconnect("Exited game.")
            elif key == ord('f'):
                pass
            elif key == ord('h'):
                render(stdscr, game_state, {"help": True})
                valid = False
            elif key == ord('l'):
                render(stdscr, game_state, {"legend": True})
                valid = False
            else:
                valid = False
    return action


def main(stdscr):
    global player_id
    stdscr.clear()
    curses.start_color()
    curses.use_default_colors()
    for i in range(0, curses.COLORS):
        curses.init_pair(i + 1, i, -1)
    while True:
        # ins, outs, ex = select.select([s], [], [], 0)
        # for inm in ins:
        # data = []
        # while True:
        #     packet = s.recv(BUFFERSIZE)
        #     if not packet:
        #         break
        #     data.append(packet)
        # message, content = pickle.loads(b"".join(data))
        message, content = pickle.loads(s.recv(BUFFERSIZE))
        if message == 'reject' or message == 'shutdown':
            disconnect(content)
        elif message == 'id':
            if not force_id:
                player_id = content
            if args.log:
                init_log()

            s.send(pickle.dumps(["acknowledge", player_id]))
            stdscr.addstr(0, 0, f"You're player {player_id}, awaiting game start")
            stdscr.refresh()
        else:
            #  Related to the game state
            if args.log:
                dump_log(content)

        if message == 'state':
            render(stdscr, content, {'waiting': True})
        if message == 'move':
            render(stdscr, content, {})
            ge = ['action', (player_id, get_action(content))]
            s.send(pickle.dumps(ge))


stdscr = curses.initscr()
curses.noecho()
curses.cbreak()
main(stdscr)
