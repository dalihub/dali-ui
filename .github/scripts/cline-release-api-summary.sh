#!/usr/bin/env bash

set -euo pipefail

: "${RELEASE_TAG:?RELEASE_TAG is required}"
: "${PREVIOUS_RELEASE_TAG:?PREVIOUS_RELEASE_TAG is required}"

WORK_DIR="${RUNNER_TEMP:-/tmp}/dali-ui-release-api-summary"
INPUT_DIR=".release-api-summary-input"
DIFF_FILE="$INPUT_DIR/api.diff"
RAW_OUTPUT_FILE="$WORK_DIR/cline-output.jsonl"
RESULT_FILE="${GITHUB_WORKSPACE:-$PWD}/release-api-summary.md"
DEBUG_DIR="${GITHUB_WORKSPACE:-$PWD}/release-api-summary-debug"

rm -rf "$WORK_DIR" "$INPUT_DIR" "$DEBUG_DIR"
mkdir -p "$WORK_DIR" "$INPUT_DIR" "$DEBUG_DIR"

git diff --find-renames --unified=20 "$PREVIOUS_RELEASE_TAG" "$RELEASE_TAG" -- \
  ':(glob)dali-ui-foundation/public-api/**/*.h' \
  ':(glob)dali-ui-components/public-api/**/*.h' \
  ':(glob)dali-ui-foundation/extension-api/**/*.h' \
  ':(glob)dali-ui-foundation/integration-api/**/*.h' \
  ':(glob)dali-ui-components/integration-api/**/*.h' > "$DIFF_FILE"

cat > "$INPUT_DIR/input.json" <<EOF
{
  "previousReleaseTag": "${PREVIOUS_RELEASE_TAG}",
  "releaseTag": "${RELEASE_TAG}",
  "apiDiffFile": ".release-api-summary-input/api.diff"
}
EOF

cat > "$WORK_DIR/prompt.md" <<'EOF'
Read .release-api-summary-input/input.json and the unified diff in apiDiffFile.

Produce a declaration-level API change summary between the two releases. Analyze only C++ declarations in these directories:
- public-api -> section "Public API"
- extension-api -> section "Extension API"
- integration-api -> section "Integration API"

Ignore implementation-only changes, comments, whitespace, includes, forward declarations, and changes that do not change an exposed declaration. Group changes by their owning class. A class must use its complete C++ namespace, for example "Dali::Ui::View".

Return only a JSON array. Each array item represents exactly one API change and must have this schema:
{
  "section": "Public API | Extension API | Integration API",
  "className": "Dali::Ui::ClassName",
  "kind": "class | api",
  "type": "Add | Change | Remove",
  "before": "previous declaration; required only for Change and Remove",
  "after": "current declaration; required only for Add and Change",
  "memberName": "after API method name; required only when kind is api and type is Add or Change"
}

For kind "api", each item must describe one declaration only: one method, constructor, destructor, operator, enum value, property, or type alias. Never combine multiple declarations in one before or after string.

When an entire class is newly added, return exactly one item with kind "class" and type "Add". Do not return the new class's member declarations separately. For this item, use the complete class namespace in className and set after to the class name only. Do not report an unchanged class as a class-level change.

Do not generate URLs. Keep declarations concise but unambiguous, including argument types and relevant qualifiers. Return [] when there are no declaration-level changes.

Your final submit summary must be exactly the JSON array.
EOF

set +e
cline -y --act --json --timeout 900 "$(cat "$WORK_DIR/prompt.md")" > "$RAW_OUTPUT_FILE"
CLINE_STATUS=$?
set -e

node - "$RAW_OUTPUT_FILE" "$RESULT_FILE" "$CLINE_STATUS" <<'NODE'
const fs = require('fs');

const [rawPath, resultPath, statusText] = process.argv.slice(2);
const raw = fs.readFileSync(rawPath, 'utf8');
let completion = '';
let buffer = '';

for (const line of raw.split(/\r?\n/)) {
  const trimmed = line.trim();
  if (!buffer && (!trimmed || !trimmed.startsWith('{'))) continue;
  buffer = buffer ? `${buffer}\n${line}` : line;
  try {
    const event = JSON.parse(buffer);
    buffer = '';
    if ((event.say === 'completion_result' || event.type === 'run_result') && typeof event.text === 'string') completion = event.text;
    if (event.type === 'agent_event' && event.event?.type === 'done' && typeof event.event.text === 'string') completion = event.event.text;
  } catch {
    // Cline may emit a multi-line JSON event.
  }
}

function parseArray(text) {
  const normalized = text.replace(/\\n/g, '\n').trim();
  try {
    const value = JSON.parse(normalized);
    return Array.isArray(value) ? value : null;
  } catch {}
  const start = normalized.indexOf('[');
  const end = normalized.lastIndexOf(']');
  if (start === -1 || end <= start) return null;
  try {
    const value = JSON.parse(normalized.slice(start, end + 1));
    return Array.isArray(value) ? value : null;
  } catch {
    return null;
  }
}

function text(value) {
  return typeof value === 'string' ? value.trim() : '';
}

function tableText(value) {
  return text(value).replace(/\|/g, '\\|').replace(/\r?\n/g, ' ');
}

const validSections = ['Public API', 'Extension API', 'Integration API'];
const baseUrl = 'https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/';
const parsed = parseArray(completion);
let output = '<!-- dali-ui-release-api-summary -->\n## API Changes (AI-Generated)\n';

function classUrl(className) {
  return `${baseUrl}class${className.replace(/::/g, '_1_1')}.html`;
}

function apiUrl(className, memberName) {
  const anchor = text(memberName).toLowerCase();
  return anchor ? `${classUrl(className)}?h=${anchor}#function-${anchor}` : '';
}

if (!parsed) {
  output += '\n> Cline 결과를 API 변경 목록으로 해석하지 못했습니다. workflow artifact의 `cline-output.jsonl`을 확인하세요.\n';
} else {
  const sections = new Map();
  for (const item of parsed) {
    const section = text(item?.section);
    const className = text(item?.className);
    const kind = text(item?.kind);
    const type = text(item?.type);
    if (!validSections.includes(section) || !/^Dali(?:::[A-Za-z_][A-Za-z0-9_]*)+$/.test(className) || !['class', 'api'].includes(kind) || !['Add', 'Change', 'Remove'].includes(type)) continue;
    if (!sections.has(section)) sections.set(section, new Map());
    const classes = sections.get(section);
    if (!classes.has(className)) classes.set(className, []);
    classes.get(className).push({
      kind,
      type,
      before: text(item?.before),
      after: text(item?.after),
      memberName: text(item?.memberName)
    });
  }

  if (sections.size === 0) {
    output += '\n선언 수준의 Public API, Extension API, Integration API 변경이 없습니다.\n';
  } else {
    for (const section of validSections) {
      const classes = sections.get(section);
      if (!classes) continue;
      output += `\n### ${section}\n`;
      for (const [className, changes] of [...classes.entries()].sort(([a], [b]) => a.localeCompare(b))) {
        output += `\n**${className}**\n\n| Type | API |\n| --- | --- |\n`;
        for (const change of changes) {
          const type = change.type;
          const before = tableText(change.before);
          const after = tableText(change.after);
          const classAddition = change.kind === 'class' && type === 'Add';
          const afterText = classAddition ? `class ${className}` : after;
          const link = change.kind === 'class' ? classUrl(className) : apiUrl(className, change.memberName);
          const linkedAfter = link ? `[${afterText}](${link})` : afterText;
          const api = type === 'Remove' ? before : type === 'Change' ? `${before} → ${linkedAfter}` : linkedAfter;
          output += `| ${type} | ${api} |\n`;
        }
      }
    }
  }
}

if (Number(statusText) !== 0) output += `\n> Cline CLI가 status ${statusText}로 종료되었습니다. 결과가 불완전할 수 있습니다.\n`;
fs.writeFileSync(resultPath, output);
NODE

cp "$WORK_DIR/prompt.md" "$DEBUG_DIR/prompt.md"
cp "$INPUT_DIR/input.json" "$DEBUG_DIR/input.json"
cp "$DIFF_FILE" "$DEBUG_DIR/api.diff"
cp "$RAW_OUTPUT_FILE" "$DEBUG_DIR/cline-output.jsonl"
cp "$RESULT_FILE" "$DEBUG_DIR/summary.md"
