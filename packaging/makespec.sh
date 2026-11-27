#!/bin/bash
set -euo pipefail

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
REPO_DIR=$(dirname "$SCRIPT_DIR")
SPEC="$SCRIPT_DIR/dali-ui.spec"
TEMPLATE="$SPEC.in"

if (( $# != 0 )); then
  echo "Usage: $0" >&2
  exit 1
fi

if ! BASE_VERSION=$(grep -Ev '^[[:space:]]*(#|$)' "$REPO_DIR/version.txt") ||
   [[ ! $BASE_VERSION =~ ^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$ ]]; then
  echo 'version.txt must contain one X.Y.Z version line.' >&2
  exit 1
fi

if [[ $(git -C "$REPO_DIR" rev-parse --show-toplevel) != "$REPO_DIR" ||
      $(git -C "$REPO_DIR" rev-parse --is-shallow-repository) != false ]]; then
  echo 'Spec generation requires a complete dali-ui Git checkout.' >&2
  exit 1
fi

COMMIT_COUNT=$(git -C "$REPO_DIR" rev-list --count HEAD)
RPM_VERSION="$BASE_VERSION.$((10000 + COMMIT_COUNT))"

TEMP_SPEC=$(mktemp "$SPEC.XXXXXX")
trap 'rm -f -- "$TEMP_SPEC"' EXIT
{
  echo '# Auto-generated from dali-ui.spec.in by makespec.sh'
  sed "s/@rpm_version@/$RPM_VERSION/g" "$TEMPLATE"
} > "$TEMP_SPEC"
chmod 644 "$TEMP_SPEC"
mv -fT -- "$TEMP_SPEC" "$SPEC"

printf '%s\n' "$RPM_VERSION"
