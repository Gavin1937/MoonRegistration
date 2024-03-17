FROM gavin1937/emsdk-cv-wasm:4.9.0

# setup enviroments
RUN \
    apt update -y && \
    apt install -y git libpython3-dev

# setup working dir & copy source code
WORKDIR /src/MoonRegistration
COPY . .

# build MoonRegistration lib in Release Mode

RUN python3 Release.py ./release_config.json
RUN python3 Release.py ./platforms/python/release_config.json
RUN python3 Release.py ./platforms/js/release_config.json
RUN mkdir archives && mv *.zip archives

ENTRYPOINT ["/bin/bash"]
