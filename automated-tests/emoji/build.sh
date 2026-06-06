#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${DALI_EMOJI_BUILD_DIR:-${REPO_ROOT}/automated-tests/build}"
JOBS="${DALI_EMOJI_BUILD_JOBS:-$(nproc)}"

cd "${REPO_ROOT}"

cmake --build "${BUILD_DIR}" --target tct-dali-ui-foundation-internal-core -j"${JOBS}"
cmake --build "${BUILD_DIR}" --target dali-emoji-sequence-sweep -j"${JOBS}"
cmake --build "${BUILD_DIR}" --target dali-emoji-sequence-visual -j"${JOBS}"
cmake --build "${BUILD_DIR}" --target dali-emoji-label-perf -j"${JOBS}"
