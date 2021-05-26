### Rogue-like 
Rough starting point for a rogue-like game
- clone:
  ```bash
  git clone --single-branch --branch soft_deadline_2 --recurse-submodules https://github.com/FrogOfJuly/roguelike.git
  ```
  If you want, you can install the game library right after this with
  ```bash
  CC=gcc-10 CXX=g++-10 pip install ./roguelike
  ```
  after this you should be able to import this module from anywhere.

  
  Whether you do it like this or build it manually, please ensure your `CC` and `CXX` variables are set
  to compilers supporting the `C++-20` concepts feature (Apple clang won't fit).
- build (optional):
  
  You can just install the whole thing via `pip`, but you can optionally build it yourself first.
   ```bash
   cd roguelike 
   mkdir build && cd build
   cmake .. && make roguelike
  ```
  You should get a ``.so`` file which can be used as python module.
  Move it to the `python_src` directory to use local build, instead of global installation.

After installation, you can test if everything is ok:

```bash
python
>>> import roguelike
```

- run:

For server
```bash
python server.py
```

For client
```bash
python client.py
```

You can walk around a labyrinth with interactable goblins, chests and traps.
There are items and potions to pick up, and there's also a shovel.
We track a score and level the player up, but there are currently no progression goals to speak of. 

### Dependencies

 - [pybind11](https://github.com/pybind/pybind11)
 - [magic enum](https://github.com/Neargye/magic_enum)
 - [awesome json](https://github.com/nlohmann/json)
 - [lwlog](https://github.com/Akagi201/lwlog)


### Known issues

1. When starting client there is message similar to

```
You're player 970123, awaiting game start
```

And the game won't start. This is fixed by rerunning server and client processes.
You might need a couple of times :)

I think we will fix it during multiplayer development. 

UPDATE: fixed


 2. When switching items message

```
log:
> you can't pick this item 
```
is displayed despite actually picking an item and dropping old one. 

 3. Sticky actions

While performing an action if one had pressed control buttons more then once their sequence is recorded and
    then - executed. It looks like you are skipping turns and your character is doing something
on his own. Do not worry. Just press desired button exactly one time once stashed sequence is depleted. 


### Design doc

The design document along with various diagrams can be found at 
[Notion](https://www.notion.so/c751085ebb244bcf8942cf17ddce652b).

### Multiplayer

There are some options for multiplayer. 
  1. build/install everythin as above.
  2. `python server.py -n N`, where `N` is expected number of players.
  3. `python client.py` in each terminal window. For now there is only multiplayer in bounds of one physical machine.

If one wants to test multiple clients without developing multiple personal disorder, then 
run `python client.py -r`, where `-r` stands for random. Such move will make full scale client, but with actions
picked from random pool instead of human input.
