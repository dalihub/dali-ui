#!/usr/bin/env bash
set -euo pipefail

usage()
{
  cat <<'USAGE'
Usage:
  ./sweep-test.sh
  ./sweep-test.sh -a
  ./sweep-test.sh -c

  default, -a  Run the full Unicode emoji sweep.
  -c           Run DALI compatibility cases.
  -h  Show this help.
USAGE
}

MODE="all"

while getopts ":ach" option; do
  case "${option}" in
    a)
      MODE="all"
      ;;
    c)
      MODE="compat"
      ;;
    h)
      usage
      exit 0
      ;;
    *)
      usage >&2
      exit 2
      ;;
  esac
done

shift $((OPTIND - 1))
if [[ $# -ne 0 ]]; then
  usage >&2
  exit 2
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${DALI_EMOJI_BUILD_DIR:-${REPO_ROOT}/automated-tests/build}"
BINARY="${BUILD_DIR}/src/dali-ui-foundation-internal/dali-emoji-sequence-sweep"

if [[ ! -x "${BINARY}" ]]; then
  echo "Missing sweep test binary: ${BINARY}" >&2
  echo "Run ${SCRIPT_DIR}/build.sh first." >&2
  exit 1
fi

unset DALI_EMOJI_SWEEP_SIGNATURE_FILTER
unset DALI_EMOJI_SWEEP_FAILURE_LOG
unset DALI_EMOJI_SWEEP_MAX_FAILURES
unset DALI_EMOJI_SWEEP_MAX_ROWS
unset DALI_EMOJI_SWEEP_VISUAL_FAILURES
unset DALI_EMOJI_SWEEP_PROFILE
unset DALI_EMOJI_SWEEP_FIXTURE
export DALI_EMOJI_SWEEP_MODE="${MODE}"
if [[ "${MODE}" == "compat" ]]; then
  export DALI_EMOJI_SWEEP_FAILURE_LOG="/tmp/dali-emoji-compat-sweep-failures.log"
fi

cd "${REPO_ROOT}"
exec "${BINARY}"
