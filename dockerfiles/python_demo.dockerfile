FROM gavin1937/emsdk-cv-wasm:4.9.0

# setup enviroments
RUN \
    apt update -y && \
    apt install -y gdb nano vim less git python3 python3-pip libpython3-dev

# setup working dir & copy source code
WORKDIR /src/MoonRegistration
COPY . .

# build MoonRegistration lib
RUN cd /src/MoonRegistration && \
    mkdir build && \
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build && \
    cd platforms/python && \
    pip3 install wheel && \
    python3 setup.py install && \
    python3 setup.py bdist_wheel --py-limited-api cp310 && \
    pip3 install -r requirements.txt

ENTRYPOINT ["/bin/bash"]
