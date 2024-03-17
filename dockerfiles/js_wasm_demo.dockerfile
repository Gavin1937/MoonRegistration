FROM gavin1937/emsdk-cv-wasm:4.9.0

# setup enviroments
RUN \
    apt update -y && \
    apt install -y nano vim less git

# setup working dir & copy source code
WORKDIR /src/MoonRegistration
COPY . .

# build MoonRegistration lib into wasm
RUN \
    mkdir /src/MoonRegistration/platforms/js/build_wasm && \
    cd /src/MoonRegistration/platforms/js/build_wasm && \
    emcmake cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_SRC="$OPENCV_SRC" -DMRWASM_BUILD_MODULE="All" .. && \
    emmake make && \
    cmake --install . --prefix install && \
    mkdir "release" && mv "install/moon-registration" "release"

ENTRYPOINT ["/bin/bash"]
