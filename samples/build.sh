#!/bin/sh

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/_build"

BUILD_TYPE="${1:-Debug}"

mkdir -p "$BUILD_DIR"

echo "############# Configuring all samples (${BUILD_TYPE}) ##############"
CXXFLAGS='-g -O0 --coverage' LDFLAGS='--coverage' \
  cmake -S "$SCRIPT_DIR" \
        -B "$BUILD_DIR" \
        -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "############# Building all samples ##############"
cmake --build "$BUILD_DIR" -- -j$(nproc)

echo "############# Installing all samples ##############"
cmake --install "$BUILD_DIR"
