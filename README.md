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
  > git clone https://github.com/Neargye/magic_enum
  > git clone https://github.com/nlohmann/json.git
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
  > pip install ./rouglelike
  ```
  after this you should be able to import this module from anywhere.

After installation you can test if everything is ok:

```bash
> python ./python_src/main.py 
```

### Dependencies

 - [pybind11](https://github.com/pybind/pybind11)
 - [magic enum](https://github.com/Neargye/magic_enum)
 - [awesome json](https://github.com/nlohmann/json)