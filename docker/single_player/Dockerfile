FROM ubuntu
ENV TERM=xterm-256color
WORKDIR /root/

RUN apt-get update && \
    apt-get install -y --no-install-recommends gcc g++

RUN apt-get update && \
    apt-get install -y --no-install-recommends cmake

RUN apt-get update && \
    apt-get install -y --no-install-recommends git make python3-dev python3-pip

RUN git clone https://github.com/FrogOfJuly/roguelike.git

RUN cd roguelike && mkdir deps && cd deps &&               \
    git clone https://github.com/pybind/pybind11.git &&    \
    git clone https://github.com/Neargye/magic_enum.git && \
    git clone https://github.com/Akagi201/lwlog.git &&     \
    git clone https://github.com/nlohmann/json.git &&      \
    git checkout devel
RUN pip3 install ./roguelike
CMD cd roguelike/python_src &&   \
    echo "nohup python3 server.py &\n" | bash  && \
    python3 client.py


