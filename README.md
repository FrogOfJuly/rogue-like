### Rogue-like 
Rough starting point for the rogue-like game
- get:
  ```bash
  
  > git clone url/rogue-like.git
  > cd deps
  > git clone https://github.com/pybind/pybind11.git
  > git clone git clone https://github.com/Neargye/magic_enum
  > cd ..
  ```
- build:
  ```bash
   > git clone url/rogue-like.git 
   > cd rogue-like
   > cd deps
   > git clone https://github.com/pybind/pybind11.git
   > git clone git clone https://github.com/Neargye/magic_enum
   > cd .. 
   > mkdir build && cd build
   > cmake .. && make 
  ```
  you should get ``.so`` file which can be used as python module.
- install:
  ```bash
  > git clone url/rogue-like.git
  > cd rogue-like/deps
  > git clone https://github.com/pybind/pybind11.git
  > git clone git clone https://github.com/Neargye/magic_enum
  > cd .. 
  > pip install ./rougle-like
  ```
  after this you should be able to import this module from anywhere.

After installation you can test if everything is ok:

```bash
> python ./python_src/main.py 
```

### Dependencies

 - [pybind11](https://github.com/pybind/pybind11)
 - [magic enum](https://github.com/Neargye/magic_enum)