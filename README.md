### Rogue-like 
Rough starting point for the rogue-like game
- clone:
  ```bash
  > git clone https://github.com/FrogOfJuly/roguelike.git
  ```
- get dependencies:
  ```bash
  > cd rogue-like
  > mkdir deps
  > cd deps
  > git clone https://github.com/pybind/pybind11.git
  > git clone https://github.com/Neargye/magic_enum.git
  > git clone https://github.com/nlohmann/json.git
  > git clone https://github.com/Akagi201/lwlog.git
  > cd ..
  ```
- build:
  ```bash
   > cd rogue-like 
   > mkdir build && cd build
   > cmake .. && make 
  ```
  you should get ``.so`` file which can be used as python module.
- install:
  ```bash
  > cd ../.. # you should be outside of project directory 
  > pip install ./roguelike # install project folder
  ```
  after this you should be able to import this module from anywhere.

After installation, you can test if everything is ok:

```bash
> python
> import roguelike
```

- run:

For server
```bash
> python server.py
```

For client
```bash
> python client.py
```

Currently, you can walk around a labyrinth with interactable immortal goblins.
Don't worry, you're immortal too! But that will change soon.

Sike!! Not anymore. But goblins are mortal too :)

And they are hunting you now

### Dependencies

 - [pybind11](https://github.com/pybind/pybind11)
 - [magic enum](https://github.com/Neargye/magic_enum)
 - [awesome json](https://github.com/nlohmann/json)