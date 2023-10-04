FROM ubuntu:22.04

# setup enviroments
ENV DEBIAN_FRONTEND=noninteractive \
    TZ=America/Los_Angeles
RUN apt update -y && \
    apt install -y tzdata gcc g++ gdb cmake make libopencv-dev nano vim less git

# setup working dir & copy source code
WORKDIR /src/MoonRegistration
COPY . .

# build MoonRegistration lib
RUN \
    mkdir build && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build && \
    cd demo && mkdir build && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build

ENTRYPOINT ["/bin/bash"]