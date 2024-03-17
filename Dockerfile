FROM gavin1937/emsdk-cv-wasm:4.9.0

# setup enviroments
RUN \
    apt update -y && \
    apt install -y gdb nano vim less git

# setup working dir & copy source code
WORKDIR /src/MoonRegistration
COPY . .

# build MoonRegistration lib
RUN cd /src/MoonRegistration && \
    mkdir build && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build && \
    cd demo && mkdir build && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build

ENTRYPOINT ["/bin/bash"]
