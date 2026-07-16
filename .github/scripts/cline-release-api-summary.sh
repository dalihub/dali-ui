#!/usr/bin/env bash

set -euo pipefail

: "${RELEASE_TAG:?RELEASE_TAG is required}"
: "${PREVIOUS_RELEASE_TAG:?PREVIOUS_RELEASE_TAG is required}"

WORK_DIR="${RUNNER_TEMP:-/tmp}/dali-ui-release-api-summary"
INPUT_DIR=".release-api-summary-input"
RESULT_FILE="${GITHUB_WORKSPACE:-$PWD}/release-api-summary.md"
DEBUG_DIR="${GITHUB_WORKSPACE:-$PWD}/release-api-summary-debug"

rm -rf "$WORK_DIR" "$INPUT_DIR" "$DEBUG_DIR"
mkdir -p "$WORK_DIR" "$INPUT_DIR" "$DEBUG_DIR"

create_api_diff() {
  local section=$1
  local diff_file="$INPUT_DIR/${section}.diff"

  case "$section" in
    public-api)
      git diff --find-renames --unified=20 "$PREVIOUS_RELEASE_TAG" "$RELEASE_TAG" -- \
        ':(glob)dali-ui-foundation/public-api/**/*.h' \
        ':(glob)dali-ui-components/public-api/**/*.h' > "$diff_file"
      ;;
    extension-api)
      git diff --find-renames --unified=20 "$PREVIOUS_RELEASE_TAG" "$RELEASE_TAG" -- \
        ':(glob)dali-ui-foundation/provider-api/**/*.h' \
        ':(glob)dali-ui-foundation/extension-api/**/*.h' > "$diff_file"
      ;;
    integration-api)
      git diff --find-renames --unified=20 "$PREVIOUS_RELEASE_TAG" "$RELEASE_TAG" -- \
        ':(glob)dali-ui-foundation/integration-api/**/*.h' \
        ':(glob)dali-ui-components/integration-api/**/*.h' > "$diff_file"
      ;;
  esac

  cat > "$INPUT_DIR/${section}.json" <<EOF
{
  "previousReleaseTag": "${PREVIOUS_RELEASE_TAG}",
  "releaseTag": "${RELEASE_TAG}",
  "apiLevel": "${section}",
  "apiDiffFile": ".release-api-summary-input/${section}.diff"
}
EOF
}

for section in public-api extension-api integration-api
do
  create_api_diff "$section"
done

cat > "$WORK_DIR/prompt.md" <<'EOF'
Read INPUT_FILE and the unified diff in apiDiffFile.

Produce an exhaustive declaration-level API change summary between the two releases. Analyze only C++ declarations in these directories:
- public-api -> section "Public API"
- extension-api -> section "Extension API"
- integration-api -> section "Integration API"

In older releases, extension-api was named provider-api. Treat provider-api and extension-api as the same Extension API surface. A path-only rename is not an API change. However, a public declaration renamed from Dali::Ui::Provider to Dali::Ui::Extension is a Change: report its before and after fully-qualified declarations.

Ignore implementation-only changes, comments, whitespace, includes, forward declarations, and changes that do not change an exposed declaration. Group changes by their owning class. A class must use its complete C++ namespace, for example "Dali::Ui::View".

Return only a JSON array. Each array item represents exactly one API change and must have this schema:
{
  "section": "Public API | Extension API | Integration API",
  "className": "Dali::Ui::ClassName",
  "ownerKind": "class | namespace",
  "kind": "class | api",
  "type": "Add | Change | Remove",
  "before": "previous declaration; required only for Change and Remove",
  "after": "current declaration; required only for Add and Change",
  "beforeMemberName": "previous API method name; required for api Change",
  "memberName": "after API method name; required only when kind is api and type is Add or Change",
  "file": "current or previous API header path"
}

For kind "api", each item must describe one declaration only: one method, constructor, destructor, operator, enum value, property, or type alias. Never combine multiple declarations in one before or after string.

When an entire class is newly added, return exactly one item with kind "class" and type "Add". Do not return the new class's member declarations separately. For this item, use the complete class namespace in className and set after to the class name only. Do not report an unchanged class as a class-level change. For free functions, use ownerKind "namespace" and the complete owning namespace in className.

Do not generate URLs. Keep declarations concise but unambiguous, including argument types and relevant qualifiers. Return [] when there are no declaration-level changes.

The Class column is rendered separately. Do not prefix an API declaration with its owning class name (for example, write "SetPadding(...)" rather than "Dali::Ui::View::SetPadding(...)"). Preserve namespaces that are part of return types and parameter types.

Examine every API-header diff hunk before responding. Do not stop after reporting newly added classes: also report every changed, added, and removed declaration in existing classes. Return all findings even when the array is long.

Your final submit summary must be exactly the JSON array.
EOF

CLINE_STATUS=0
for section in public-api extension-api integration-api
do
  set +e
  cline -y --act --json --timeout 900 "$(sed "s|INPUT_FILE|.release-api-summary-input/${section}.json|" "$WORK_DIR/prompt.md")" > "$WORK_DIR/cline-output-${section}.jsonl"
  status=$?
  set -e
  if [ "$status" -ne 0 ]; then
    CLINE_STATUS=$status
  fi
done

node - "$WORK_DIR" "$RESULT_FILE" "$CLINE_STATUS" <<'NODE'
const fs = require('fs');

const [workDir, resultPath, statusText] = process.argv.slice(2);

function completionFromRaw(raw) {
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

  return completion;
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

function normalizedOwnerKind(value) {
  const kind = text(value).toLowerCase();
  return kind === 'namespace' ? 'namespace' : 'class';
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
const sectionInputs = new Map([
  ['Public API', 'public-api'],
  ['Extension API', 'extension-api'],
  ['Integration API', 'integration-api']
]);
const inputDir = '.release-api-summary-input';
const baseUrl = 'https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/';
const rawFiles = fs.readdirSync(workDir)
  .filter(name => name.startsWith('cline-output-') && name.endsWith('.jsonl'))
  .sort();
const parsedArrays = rawFiles
  .map(name => parseArray(completionFromRaw(fs.readFileSync(`${workDir}/${name}`, 'utf8'))))
  .filter(Array.isArray);
const parsed = parsedArrays.flat();
let output = '<!-- dali-ui-release-api-summary -->\n## API Changes (AI-Generated)\n';

function ownerUrl(className, ownerKind) {
  const page = ownerKind === 'namespace' ? 'namespace' : 'class';
  return `${baseUrl}${page}${className.replace(/::/g, '_1_1')}.html`;
}

function apiUrl(className, ownerKind, memberName) {
  const anchor = text(memberName).toLowerCase();
  const url = ownerUrl(className, ownerKind);
  return anchor ? `${url}?h=${anchor}#function-${anchor}` : url;
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

if (parsedArrays.length === 0) {
  output += '\n> Cline 결과를 API 변경 목록으로 해석하지 못했습니다. workflow artifact의 `cline-output.jsonl`을 확인하세요.\n';
} else {
  const sections = new Map();
  let skippedItems = 0;
  for (const item of parsed) {
    const section = normalizedSection(item?.section);
    const className = text(item?.className);
    const kind = normalizedKind(item?.kind);
    const ownerKind = normalizedOwnerKind(item?.ownerKind);
    const type = normalizedType(item?.type);
    if (!validSections.includes(section) || !/^Dali(?:::[A-Za-z_][A-Za-z0-9_]*)+$/.test(className) || !['class', 'api'].includes(kind) || !['Add', 'Change', 'Remove'].includes(type)) continue;
    if (!sections.has(section)) sections.set(section, new Map());
    const classes = sections.get(section);
    if (!classes.has(className)) classes.set(className, []);
    classes.get(className).push({
      kind,
      ownerKind,
      type,
      before: text(item?.before),
      after: text(item?.after),
      beforeMemberName: text(item?.beforeMemberName) || memberNameFromDeclaration(item?.before),
      memberName: text(item?.memberName) || memberNameFromDeclaration(item?.after),
      file: text(item?.file)
    });
  }

  if (sections.size === 0) {
    output += '\n선언 수준의 Public API, Extension API, Integration API 변경이 없습니다.\n';
  } else {
    for (const section of validSections) {
      const classes = sections.get(section);
      if (!classes) continue;
      if (output.includes('\n### ')) output += '\n<br/>\n';
      output += `\n### ${section.toLowerCase().replace(' ', '-')}\n\n| Type | Class | API |\n| --- | --- | --- |\n`;
      for (const [className, changes] of [...classes.entries()].sort(([a], [b]) => a.localeCompare(b))) {
        const outputChanges = changes.flatMap(change => {
          const splitChange = change.kind === 'api' && change.type === 'Change' && change.beforeMemberName && change.memberName && change.beforeMemberName !== change.memberName;
          return splitChange
            ? [
                {...change, type: 'Remove', after: '', memberName: ''},
                {...change, type: 'Add', before: '', beforeMemberName: ''}
              ]
            : [change];
        }).sort((a, b) => {
          const typeOrder = {Add: 0, Remove: 1, Change: 2};
          const typeDifference = typeOrder[a.type] - typeOrder[b.type];
          if (typeDifference !== 0) return typeDifference;
          return `${a.after || a.before}`.localeCompare(`${b.after || b.before}`);
        });
        for (const outputChange of outputChanges) {
          const type = outputChange.type;
          const before = apiText(outputChange.before, className);
          const after = apiText(outputChange.after, className);
          const classAddition = outputChange.kind === 'class' && type === 'Add';
          const afterText = classAddition ? `class ${shortClassName(className)}` : after;
          const link = outputChange.kind === 'class' ? ownerUrl(className, outputChange.ownerKind) : apiUrl(className, outputChange.ownerKind, outputChange.memberName);
          const linkedAfter = link ? `[${afterText}](${link})` : afterText;
          const api = type === 'Remove' ? before : type === 'Change' ? `${before} → ${linkedAfter}` : linkedAfter;
          const owner = outputChange.ownerKind === 'namespace' ? `${className} (namespace)` : className;
          output += `| ${type} | \`${owner}\` | ${api} |\n`;
        }
      }
    }
  }

  skippedItems = parsed.length - [...sections.values()].reduce((count, classes) => count + [...classes.values()].reduce((sum, changes) => sum + changes.length, 0), 0);
  if (skippedItems > 0) output += `\n> Cline 결과 ${skippedItems}건은 schema가 맞지 않아 제외되었습니다. debug artifact의 \`cline-output.jsonl\`을 확인하세요.\n`;

  for (const [section, inputName] of sectionInputs) {
    const diffPath = `${inputDir}/${inputName}.diff`;
    const changedFiles = new Set(
      [...fs.readFileSync(diffPath, 'utf8').matchAll(/^\+\+\+ b\/(.+\.h)$/gm)].map(match => match[1])
    );
    const reportedFiles = new Set(
      [...(sections.get(section)?.values() || [])]
        .flat()
        .map(change => change.file.replace('dali-ui-foundation/provider-api/', 'dali-ui-foundation/extension-api/'))
        .filter(Boolean)
    );
    const missingFiles = [...changedFiles].filter(file => !reportedFiles.has(file));
    if (missingFiles.length > 0) {
      output += `\n> ${section.toLowerCase().replace(' ', '-')} header ${missingFiles.length}개에 대한 Cline 결과가 없습니다: ${missingFiles.map(file => `\`${file}\``).join(', ')}\n`;
    }
  }
}

if (Number(statusText) !== 0) output += `\n> Cline CLI가 status ${statusText}로 종료되었습니다. 결과가 불완전할 수 있습니다.\n`;
fs.writeFileSync(resultPath, output);
NODE

cp "$WORK_DIR/prompt.md" "$DEBUG_DIR/prompt.md"
cp "$INPUT_DIR"/* "$DEBUG_DIR/"
cp "$WORK_DIR"/cline-output-*.jsonl "$DEBUG_DIR/"
cp "$RESULT_FILE" "$DEBUG_DIR/summary.md"
