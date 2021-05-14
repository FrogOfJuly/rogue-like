FROM ubuntu
ENV TERM=xterm-256color
ENV CC=gcc-10
ENV CXX=g++-10
WORKDIR /root/

RUN apt-get update && \
    apt-get install -y --no-install-recommends gcc-10 g++-10

RUN apt-get update && \
    apt-get install -y --no-install-recommends cmake

RUN apt-get update && \
    apt-get install -y --no-install-recommends git make python3-dev python3-pip

RUN git clone https://github.com/FrogOfJuly/roguelike.git

RUN cd roguelike && mkdir deps && cd deps &&               \
    git clone https://github.com/pybind/pybind11.git &&    \
    git clone https://github.com/Neargye/magic_enum.git && \
    git clone https://github.com/Akagi201/lwlog.git &&     \
    git clone --depth 1 https://github.com/nlohmann/json.git &&      \
    git checkout devel
RUN pip3 install ./roguelike
CMD cd roguelike/python_src &&   \
    echo "nohup python3 server.py &\n" | bash  && \
    sleep 2s && \
    python3 client.py


