FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY CMakeLists.txt .
COPY src/ src/
COPY test/ test/
COPY tools/ tools/
COPY bench/ bench/
COPY data/sample.itch data/sample.itch

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j$(nproc)

RUN cd build && ctest --output-on-failure

FROM ubuntu:22.04

COPY --from=builder /src/build/itch_parser /usr/local/bin/itch_parser

ENTRYPOINT ["itch_parser"]
