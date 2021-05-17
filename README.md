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

  
  Whether you do it like this or build it manually, please ensure your `CC` and `CXX` variables are set to compilers supporting the `C++-20` concepts feature.
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