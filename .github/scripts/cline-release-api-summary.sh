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
  ':(glob)dali-ui-foundation/provider-api/**/*.h' \
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

Produce an exhaustive declaration-level API change summary between the two releases. Analyze only C++ declarations in these directories:
- public-api -> section "Public API"
- extension-api -> section "Extension API"
- integration-api -> section "Integration API"

In older releases, extension-api was named provider-api. Treat provider-api and extension-api as the same Extension API surface. A provider-api -> extension-api path rename is not an Add or Remove API change by itself; report only declarations that actually changed across the rename.

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

The Class column is rendered separately. Do not prefix an API declaration with its owning class name (for example, write "SetPadding(...)" rather than "Dali::Ui::View::SetPadding(...)"). Preserve namespaces that are part of return types and parameter types.

Examine every API-header diff hunk before responding. Do not stop after reporting newly added classes: also report every changed, added, and removed declaration in existing classes. Return all findings even when the array is long.

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

function normalizedSection(value) {
  const section = text(value).toLowerCase().replace(/[ _-]/g, '');
  if (section === 'publicapi') return 'Public API';
  if (section === 'extensionapi' || section === 'providerapi') return 'Extension API';
  if (section === 'integrationapi') return 'Integration API';
  return '';
}

function normalizedKind(value) {
  const kind = text(value).toLowerCase().replace(/[ _-]/g, '');
  if (kind === 'class') return 'class';
  if (['api', 'method', 'function', 'member', 'constructor', 'destructor', 'operator', 'property', 'enum', 'typealias'].includes(kind)) return 'api';
  return '';
}

function normalizedType(value) {
  const type = text(value).toLowerCase();
  if (type === 'add') return 'Add';
  if (type === 'change') return 'Change';
  if (type === 'remove') return 'Remove';
  return '';
}

function isCandidateItem(item) {
  return Boolean(
    normalizedSection(item?.section) &&
    /^Dali(?:::[A-Za-z_][A-Za-z0-9_]*)+$/.test(text(item?.className)) &&
    normalizedKind(item?.kind) &&
    normalizedType(item?.type)
  );
}

function parseArray(text) {
  const normalized = text.replace(/\\n/g, '\n').trim();
  try {
    const value = JSON.parse(normalized);
    return Array.isArray(value) ? value : null;
  } catch {}

  const candidates = [];
  for (let start = 0; start < normalized.length; start += 1) {
    if (normalized[start] !== '[') continue;
    let depth = 0;
    let inString = false;
    let escaped = false;
    for (let end = start; end < normalized.length; end += 1) {
      const char = normalized[end];
      if (inString) {
        if (escaped) escaped = false;
        else if (char === '\\') escaped = true;
        else if (char === '"') inString = false;
        continue;
      }
      if (char === '"') inString = true;
      else if (char === '[') depth += 1;
      else if (char === ']') {
        depth -= 1;
        if (depth !== 0) continue;
        try {
          const value = JSON.parse(normalized.slice(start, end + 1));
          if (Array.isArray(value)) candidates.push(value);
        } catch {}
        break;
      }
    }
  }

  return candidates.sort((a, b) => b.filter(isCandidateItem).length - a.filter(isCandidateItem).length)[0] || null;
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

function memberNameFromDeclaration(declaration) {
  const match = tableText(declaration).match(/(?:^|\s)(operator\s*[^\s(]+|~?[A-Za-z_][A-Za-z0-9_]*)\s*\(/);
  return match?.[1]?.replace(/\s+/g, '') || '';
}

function apiText(declaration, className) {
  return tableText(declaration).replaceAll(`${className}::`, '');
}

function shortClassName(className) {
  return className.slice(className.lastIndexOf('::') + 2);
}

if (!parsed) {
  output += '\n> Cline 결과를 API 변경 목록으로 해석하지 못했습니다. workflow artifact의 `cline-output.jsonl`을 확인하세요.\n';
} else {
  const sections = new Map();
  let skippedItems = 0;
  for (const item of parsed) {
    const section = normalizedSection(item?.section);
    const className = text(item?.className);
    const kind = normalizedKind(item?.kind);
    const type = normalizedType(item?.type);
    if (!validSections.includes(section) || !/^Dali(?:::[A-Za-z_][A-Za-z0-9_]*)+$/.test(className) || !['class', 'api'].includes(kind) || !['Add', 'Change', 'Remove'].includes(type)) continue;
    if (!sections.has(section)) sections.set(section, new Map());
    const classes = sections.get(section);
    if (!classes.has(className)) classes.set(className, []);
    classes.get(className).push({
      kind,
      type,
      before: text(item?.before),
      after: text(item?.after),
      memberName: text(item?.memberName) || memberNameFromDeclaration(item?.after)
    });
  }

  if (sections.size === 0) {
    output += '\n선언 수준의 Public API, Extension API, Integration API 변경이 없습니다.\n';
  } else {
    for (const section of validSections) {
      const classes = sections.get(section);
      if (!classes) continue;
      output += `\n### ${section}\n\n| Type | Class | API |\n| --- | --- | --- |\n`;
      for (const [className, changes] of [...classes.entries()].sort(([a], [b]) => a.localeCompare(b))) {
        for (const change of changes) {
          const type = change.type;
          const before = apiText(change.before, className);
          const after = apiText(change.after, className);
          const classAddition = change.kind === 'class' && type === 'Add';
          const afterText = classAddition ? `class ${shortClassName(className)}` : after;
          const link = change.kind === 'class' ? classUrl(className) : apiUrl(className, change.memberName);
          const linkedAfter = link ? `[${afterText}](${link})` : afterText;
          const api = type === 'Remove' ? before : type === 'Change' ? `${before} → ${linkedAfter}` : linkedAfter;
          output += `| ${type} | \`${className}\` | ${api} |\n`;
        }
      }
    }
  }

  skippedItems = parsed.length - [...sections.values()].reduce((count, classes) => count + [...classes.values()].reduce((sum, changes) => sum + changes.length, 0), 0);
  if (skippedItems > 0) output += `\n> Cline 결과 ${skippedItems}건은 schema가 맞지 않아 제외되었습니다. debug artifact의 \`cline-output.jsonl\`을 확인하세요.\n`;
}

if (Number(statusText) !== 0) output += `\n> Cline CLI가 status ${statusText}로 종료되었습니다. 결과가 불완전할 수 있습니다.\n`;
fs.writeFileSync(resultPath, output);
NODE

cp "$WORK_DIR/prompt.md" "$DEBUG_DIR/prompt.md"
cp "$INPUT_DIR/input.json" "$DEBUG_DIR/input.json"
cp "$DIFF_FILE" "$DEBUG_DIR/api.diff"
cp "$RAW_OUTPUT_FILE" "$DEBUG_DIR/cline-output.jsonl"
cp "$RESULT_FILE" "$DEBUG_DIR/summary.md"
