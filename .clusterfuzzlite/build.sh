#!/bin/bash -eu

# Build the project
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Copy the fuzzers to the output directory
cp fuzz_nexus_broker $OUT/
cp fuzz_packet_parser $OUT/
cp fuzz_database $OUT/
