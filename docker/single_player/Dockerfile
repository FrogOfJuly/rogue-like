FROM ubuntu
ENV TERM=xterm-256color
ENV CC=gcc-10
ENV CXX=g++-10
ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /root/

RUN apt-get update && \
    apt-get install -y --no-install-recommends gcc-10 g++-10 cmake git make python3-dev python3-pip

RUN git clone --single-branch --branch devel --recurse-submodules https://github.com/FrogOfJuly/roguelike.git

RUN cd roguelike
RUN pip3 install ./roguelike
CMD cd roguelike/python_src &&   \
    echo "nohup python3 server.py &\n" | bash  && \
    sleep 1s && \
    python3 client.py


