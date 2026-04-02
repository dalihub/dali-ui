#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/_build"

BUILD_TYPE="${1:-Debug}"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "############# Configuring all samples (${BUILD_TYPE}) ##############"
CXXFLAGS='-g -O0 --coverage' LDFLAGS='--coverage' \
  cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        "$SCRIPT_DIR"

echo "############# Building all samples ##############"
make -j$(nproc)
