import math
import pickle
from time import sleep

import select
import socket
import sys
import json  # for logging only
import curses
from roguelike import cmd
from enum import Enum
import os

BUFFERSIZE = 8192

serverAddr = '127.0.0.1'
if len(sys.argv) == 2:
    serverAddr = sys.argv[1]

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((serverAddr, 4321))

player_id = 0
log = []
printed_log_len = 7


def init_log():
    log_file = f'{player_id}.json'
    with open(log_file, "w") as file:
        file.write("[]")


def dump_log(game_state: dict):
    log_file = f'{player_id}.json'

    with open(log_file, "r+") as file:
        data = json.load(file)
        data.append(game_state)
        file.seek(0)
        json.dump(data, file)


def disconnect(string):
    s.close()
    curses.nocbreak()
    curses.echo()
    curses.endwin()
    print(string)
    print()
    exit()


def color(level):
    if level > 10:
        return 197
    else:
        return 207 - level


def render(stdscr, game_state: dict, with_help = False):
    stdscr.clear()
    stdscr.refresh()
    global log
    # print(game_state)
    if game_state == {'start'}:
        return
    game_state = game_state['level']
    H = int(math.sqrt(len(game_state)))
    W = H
    player = None
    for tile_ in game_state:
        tile = tile_["tile"]
        if tile and "player" in tile:
            player = tile["player"]
            newlog = player["lg_cpt"]["log"].split("\n")
            log += list(filter(lambda x: x != '', newlog))
            break
    for i in range(H):
        for j in range(W):
            tile = game_state[i * W + j]['tile']
            to_print = None
            lvl = None
            if tile is None:
                to_print = "   "
            elif 'player' in tile:
                to_print = f" {tile['player']['repr_cpt']['repr']} "
                lvl = tile['player']['lvl']
            elif 'entity' in tile:
                repr = tile['entity']['repr_cpt']['repr']
                if repr == '█':
                    to_print = '███'
                else:
                    to_print = f' {repr} '
                    if 'h_cpt' in tile['entity']:
                        lvl = tile['entity']['h_cpt']['health']
            if (lvl):
                stdscr.addstr(i, j*3, to_print, curses.color_pair(color(lvl)))
            else:
                stdscr.addstr(i, j*3, to_print)
    if not player:
        s.send(pickle.dumps(['exit']))
        disconnect("You died!")
    stdscr.addstr(0, (W)*3+1, f"{'Room:':9} {1}/10")  # TODO
    stdscr.addstr(1, (W)*3+1, f"{'Score:':9} {123}")  # TODO
    stdscr.addstr(3, (W)*3+1, f"{'Level:':9} {player['lvl']}")
    stdscr.addstr(4, (W)*3+1, f"{'Health:':9} {player['h_cpt']['health']}")
    stdscr.addstr(5, (W)*3+1, f"{'Damage:':9} {player['a_cpt']['damage']}")
    stdscr.addstr(7, (W)*3+1, f'log:')

    for i, entry in enumerate(log[-printed_log_len:]):
        stdscr.addstr(8+i, (W)*3+1, f'> {entry}')
    if len(log) < printed_log_len:
        for i in range(printed_log_len - len(log)):
            stdscr.addstr(8+len(log)+i, (W)*3+1, '>')

    if not with_help:
        stdscr.addstr(H+2, 0, f'Press H for help.')
    else:
        stdscr.addstr(H+2, 0, f'WASD for movement.')
        stdscr.addstr(H+3, 0, f'E for potion.')
        stdscr.addstr(H+4, 0, f'F to skip turn.')
        stdscr.addstr(H+5, 0, f'P to pause game (temporarily disconnect).')
        stdscr.addstr(H+6, 0, f'X to exit game.')
    stdscr.refresh()


def main(stdscr):
    global player_id
    stdscr.clear()
    curses.start_color()
    curses.use_default_colors()
    for i in range(0, curses.COLORS):
        curses.init_pair(i + 1, i, -1)
    while True:
        ins, outs, ex = select.select([s], [], [], 0)
        for inm in ins:
            gameEvent = pickle.loads(inm.recv(BUFFERSIZE))
            if gameEvent[0] == 'reject':
                disconnect(gameEvent[1])
            if gameEvent[0] == 'id':
                player_id = gameEvent[1]
                init_log()
                stdscr.addstr(0, 0, f"You're player {player_id}, awaiting game start")
                stdscr.refresh()
            else:
                dump_log(gameEvent[1])
                render(stdscr, gameEvent[1])

            if gameEvent[0] == 'state':
                pass
            if gameEvent[0] == 'move':
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
                            s.send(pickle.dumps(['exit']))
                            disconnect("Exited game.")
                        elif key == ord('f'):
                            pass
                        elif key == ord('h'):
                            render(stdscr, gameEvent[1], True)
                            valid = False
                        else:
                            valid = False
                ge = ['action', player_id, action]
                s.send(pickle.dumps(ge))


curses.wrapper(main)