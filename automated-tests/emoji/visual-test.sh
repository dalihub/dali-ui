#!/usr/bin/env bash
set -euo pipefail

usage()
{
  cat <<'USAGE'
Usage:
  ./visual-test.sh
  ./visual-test.sh -r
  ./visual-test.sh FAILURE_LOG_PATH
  ./visual-test.sh FAILURE_LOG_PATH -html [HTML_PATH]

  default       Open the full visual viewer.
  -r            Open the compact review set.
  FAILURE_LOG   Open sweep failures from the given log.
  -html         Export HTML only. If HTML_PATH is omitted, /tmp is used.
  -h  Show this help.
USAGE
}

MODE="full"
INPUT_PATH=""
EXPORT_HTML_PATH=""
EXPORT_HTML="0"

while [[ $# -gt 0 ]]; do
  case "$1" in
    -r)
      MODE="review"
      shift
      ;;
    -html)
      EXPORT_HTML="1"
      if [[ $# -ge 2 && "$2" != -* ]]; then
        EXPORT_HTML_PATH="$2"
        shift 2
      else
        shift
      fi
      ;;
    -h)
      usage
      exit 0
      ;;
    -*)
      usage >&2
      exit 2
      ;;
    *)
      if [[ -n "${INPUT_PATH}" ]]; then
        echo "Only one failure log path can be provided." >&2
        usage >&2
        exit 2
      fi
      INPUT_PATH="$1"
      shift
      ;;
  esac
done

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${DALI_EMOJI_BUILD_DIR:-${REPO_ROOT}/automated-tests/build}"
BINARY="${BUILD_DIR}/src/dali-ui-foundation-internal/dali-emoji-sequence-visual"

if [[ ! -x "${BINARY}" ]]; then
  echo "Missing visual test binary: ${BINARY}" >&2
  echo "Run ${SCRIPT_DIR}/build.sh first." >&2
  exit 1
fi

unset DALI_EMOJI_VISUAL_INPUT
unset DALI_EMOJI_VISUAL_FIXTURE
unset DALI_EMOJI_VISUAL_SIGNATURE_FILTER
unset DALI_EMOJI_VISUAL_MAX_ITEMS
unset DALI_EMOJI_VISUAL_PAGE_SIZE
unset DALI_EMOJI_VISUAL_FONT_SIZE
unset DALI_EMOJI_VISUAL_EXPORT_HTML
unset DALI_EMOJI_VISUAL_EXPORT_ONLY
unset DALI_EMOJI_VISUAL_EXIT_AFTER_EXPORT
unset DALI_EMOJI_VISUAL_CAPTURE
unset DALI_EMOJI_VISUAL_CAPTURE_DELAY_MS
unset DALI_EMOJI_VISUAL_EXIT_AFTER_CAPTURE

default_html_path()
{
  if [[ -n "${INPUT_PATH}" ]]; then
    local name
    name="$(basename "${INPUT_PATH}")"
    name="${name%.*}"
    echo "/tmp/${name}.html"
  else
    echo "/tmp/dali-emoji-${MODE}.html"
  fi
}

export DALI_EMOJI_VISUAL_MODE="${MODE}"
if [[ -n "${INPUT_PATH}" ]]; then
  export DALI_EMOJI_VISUAL_INPUT="${INPUT_PATH}"
else
  unset DALI_EMOJI_VISUAL_INPUT
fi

if [[ "${EXPORT_HTML}" == "1" ]]; then
  if [[ -z "${EXPORT_HTML_PATH}" ]]; then
    EXPORT_HTML_PATH="$(default_html_path)"
  fi
  export DALI_EMOJI_VISUAL_EXPORT_HTML="${EXPORT_HTML_PATH}"
  export DALI_EMOJI_VISUAL_EXPORT_ONLY=1
fi

cd "${REPO_ROOT}"
exec "${BINARY}"
