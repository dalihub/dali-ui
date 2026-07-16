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
## 1. Role and objective

You are a C++ API declaration-diff analyzer.
Analyze every reportable C++ API declaration change between the supplied old and new release revisions.
Limit the analysis to the supplied API diff.
Return a machine-readable list of Add, Remove, and Change records for the release API summary.

## 2. Input and analysis scope

Read INPUT_FILE.
INPUT_FILE is JSON with `previousReleaseTag`, `releaseTag`, `apiLevel`, and `apiDiffFile` fields.
`previousReleaseTag` is the old Git release tag and `releaseTag` is the new Git release tag.
`apiLevel` is exactly one of `public-api`, `extension-api`, or `integration-api`.
`apiDiffFile` is the unified Git diff to analyze for this execution.

This execution receives one diff file for one API category.
The diff can contain headers from multiple source directories.
For example, the `public-api` diff can contain both `dali-ui-foundation/public-api/` and `dali-ui-components/public-api/` headers.
The workflow runs separate executions for `public-api`, `extension-api`, and `integration-api`.

Read the unified diff at `apiDiffFile`.
Analyze only declarations in headers included by that diff.
Do not report declarations from headers outside that diff.
When the diff lacks enough context, inspect both complete header revisions with `git show <previousReleaseTag>:<header-path>` and `git show <releaseTag>:<header-path>`.

## 3. Required output format

Return exactly one JSON array and nothing else.
Do not return Markdown, a code fence, an explanation, or conversational text.
Each array item represents exactly one declaration.
Do not combine overloads or multiple declarations into one item.
Do not put multiple declarations in `before` or `after`.
Do not separate declarations with semicolons, commas, newlines, bullet points, or explanatory text.

Each item must use exactly this schema:
[
  {
    "section": "public-api | extension-api | integration-api",
    "className": "fully-qualified class, struct, or enum namespace name",
    "ownerKind": "class | namespace",
    "kind": "class | struct | enum | type-alias | data-member | constructor | destructor | method | operator | function",
    "type": "Add | Remove | Change",
    "before": "old declaration, or empty string",
    "after": "new declaration, or empty string",
    "beforeMemberName": "old member name, or empty string",
    "memberName": "new member name, or empty string",
    "file": "repository-relative header path"
  }
]

Set `section` to the exact `apiLevel` value from INPUT_FILE.
Set `file` to the repository-relative header path containing the changed declaration.
Set `type` to exactly one of `Add`, `Remove`, or `Change`.
Set `kind` to exactly one value permitted by the schema.

For Add, set `before` and `beforeMemberName` to empty strings.
For Remove, set `after` and `memberName` to empty strings.
For Change, set `before` to the complete old declaration and `after` to the complete new declaration.
For a class or struct record, set `memberName` and `beforeMemberName` to empty strings.
For a member function, set `memberName` to its declaration name without the owning class or struct name.
For example, use `SetPadding` for `Dali::Ui::View::SetPadding(...)`.
For a changed member function, set `beforeMemberName` to the old declaration name without its owner name.
For a constructor, use the class or struct name as `memberName`.
For a destructor, use the destructor name as `memberName`.
For an operator, use the complete operator name as `memberName`.

## 4. Analysis and classification rules

Report these declaration types: class, struct, enum, type alias, data member, constructor, destructor, operator function, and member function.
For class or struct members, determine access from the complete header.
Report public and protected members.
Do not report private members.
Remember that a `class` defaults to private member access and a `struct` defaults to public member access.

Use the complete namespace-qualified class or struct name in `className`.
For example, use `Dali::Ui::InputField`, not `InputField`.
Set `ownerKind` to `class` for declarations owned by a class or struct.
Report a namespace-level enum, using its complete namespace as `className` and `ownerKind: "namespace"`.

For a class or struct addition, create exactly one Add record with `kind: "class"`.
For a class or struct removal, create exactly one Remove record with `kind: "class"`.
Do not create separate member records when the containing class or struct itself was added or removed.
For all other declarations, create one record for each added, removed, or changed declaration.

Use Add only when the declaration exists only at `releaseTag`.
Use Remove only when the declaration exists only at `previousReleaseTag`.
Use Change only when the same declaration exists in both releases but its declaration-level API signature changed.
An API signature includes fully-qualified namespace, owning class or struct, declaration name, parameter types, parameter order, parameter count, return type, `const`, ref qualifiers, `static`, `virtual`, `override`, `noexcept`, and template parameters.
Treat a namespace change as Change when the declarations otherwise represent the same API.
For example, moving `Dali::Ui::Provider::Foo::Bar()` to `Dali::Ui::Extension::Foo::Bar()` is one Change record.
Do not represent that namespace change as unrelated Remove and Add records.
Treat a class or struct rename as Change when the old and new declarations otherwise represent the same API.

## 5. Exclusions and final verification

Do not report namespace-level free functions or namespace-level type aliases.
Do not report comments, whitespace-only changes, include directives, forward declarations, friend declarations, private implementation helpers, function-body-only changes, or unchanged declarations moved only by a file rename.

Before returning the JSON array, verify that each item represents exactly one declaration.
Verify that each added or removed class or struct has exactly one class record.
Verify that no private member, friend declaration, comment-only change, or whitespace-only change is included.
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
  if (section === 'extensionapi') return 'Extension API';
  if (section === 'integrationapi') return 'Integration API';
  return '';
}

function normalizedKind(value) {
  const kind = text(value).toLowerCase().replace(/[ _-]/g, '');
  if (kind === 'class' || kind === 'struct') return 'class';
  if (['api', 'method', 'function', 'member', 'constructor', 'destructor', 'operator', 'property', 'enum', 'typealias', 'datamember'].includes(kind)) return 'api';
  return '';
}

function normalizedDeclarationKind(value) {
  const kind = text(value).toLowerCase().replace(/[ _-]/g, '');
  return ['class', 'struct', 'enum', 'typealias', 'datamember', 'constructor', 'destructor', 'method', 'operator', 'function', 'api'].includes(kind) ? kind : '';
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

function isReportableChange(change) {
  return !/\bfriend\s+(?:class|struct)\b/.test(`${change.before}\n${change.after}`);
}

function containsMultipleDeclarations(declaration) {
  return /;\s*(?:class|struct|enum|using|typedef|(?:virtual\s+)?[A-Za-z_~])/.test(tableText(declaration));
}

function hasRequiredFields(change) {
  if (containsMultipleDeclarations(change.before) || containsMultipleDeclarations(change.after)) return false;
  if (change.type === 'Add') return Boolean(change.after) && !change.before;
  if (change.type === 'Remove') return Boolean(change.before) && !change.after;
  return Boolean(change.before) && Boolean(change.after);
}

function changeKey(change) {
  if (change.kind === 'class') return [change.kind, change.ownerKind, change.type].join('\u0000');
  return [change.kind, change.ownerKind, change.type, change.before, change.after, change.memberName].join('\u0000');
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
    const declarationKind = normalizedDeclarationKind(item?.kind);
    const ownerKind = normalizedOwnerKind(item?.ownerKind);
    const type = normalizedType(item?.type);
    if (!validSections.includes(section) || !/^Dali(?:::[A-Za-z_][A-Za-z0-9_]*)+$/.test(className) || !['class', 'api'].includes(kind) || !declarationKind || !['Add', 'Change', 'Remove'].includes(type)) {
      skippedItems += 1;
      continue;
    }
    if ((ownerKind === 'namespace' && declarationKind !== 'enum') || (ownerKind === 'class' && declarationKind === 'api')) {
      skippedItems += 1;
      continue;
    }
    const change = {
      kind,
      declarationKind,
      ownerKind,
      type,
      before: text(item?.before),
      after: text(item?.after),
      beforeMemberName: text(item?.beforeMemberName) || memberNameFromDeclaration(item?.before),
      memberName: text(item?.memberName) || memberNameFromDeclaration(item?.after),
      file: text(item?.file)
    };
    if (!hasRequiredFields(change) || !isReportableChange(change)) {
      skippedItems += 1;
      continue;
    }
    if (!sections.has(section)) sections.set(section, new Map());
    const classes = sections.get(section);
    if (!classes.has(className)) classes.set(className, []);
    classes.get(className).push(change);
  }

  if (sections.size === 0) {
    output += '\n선언 수준의 Public API, Extension API, Integration API 변경이 없습니다.\n';
  } else {
    for (const section of validSections) {
      const classes = sections.get(section);
      if (!classes) continue;
      let rows = '';
      for (const [className, changes] of [...classes.entries()].sort(([a], [b]) => a.localeCompare(b))) {
        const outputChanges = changes.flatMap(change => {
          const splitChange = change.kind === 'api' && change.type === 'Change' && change.beforeMemberName && change.memberName && change.beforeMemberName !== change.memberName;
          return splitChange
            ? [
                {...change, type: 'Remove', after: '', memberName: ''},
                {...change, type: 'Add', before: '', beforeMemberName: ''}
              ]
            : [change];
        });
        const uniqueChanges = [...new Map(outputChanges.map(change => [changeKey(change), change])).values()].sort((a, b) => {
          const typeOrder = {Add: 0, Remove: 1, Change: 2};
          const typeDifference = typeOrder[a.type] - typeOrder[b.type];
          if (typeDifference !== 0) return typeDifference;
          return `${a.after || a.before}`.localeCompare(`${b.after || b.before}`);
        });
        for (const outputChange of uniqueChanges) {
          const type = outputChange.type;
          const before = apiText(outputChange.before, className);
          const after = apiText(outputChange.after, className);
          const classAddition = outputChange.kind === 'class' && type === 'Add';
          const afterText = classAddition ? `class ${shortClassName(className)}` : after;
          const link = outputChange.kind === 'class' || ['enum', 'datamember'].includes(outputChange.declarationKind)
            ? ownerUrl(className, outputChange.ownerKind)
            : apiUrl(className, outputChange.ownerKind, outputChange.memberName);
          const linkedAfter = link ? `[${afterText}](${link})` : afterText;
          const api = type === 'Remove' ? before : type === 'Change' ? `${before} → ${linkedAfter}` : linkedAfter;
          const owner = outputChange.ownerKind === 'namespace' ? `${className} (namespace)` : className;
          rows += `| ${type} | \`${owner}\` | ${api} |\n`;
        }
      }
      if (!rows) continue;
      if (output.includes('\n### ')) output += '\n<br/>\n';
      output += `\n### ${section.toLowerCase().replace(' ', '-')}\n\n| Type | Class | API |\n| --- | --- | --- |\n${rows}`;
    }
  }

  if (skippedItems > 0) output += `\n> Cline 결과 ${skippedItems}건은 schema가 맞지 않아 제외되었습니다. debug artifact의 \`cline-output.jsonl\`을 확인하세요.\n`;
}

if (Number(statusText) !== 0) output += `\n> Cline CLI가 status ${statusText}로 종료되었습니다. 결과가 불완전할 수 있습니다.\n`;
fs.writeFileSync(resultPath, output);
NODE

cp "$WORK_DIR/prompt.md" "$DEBUG_DIR/prompt.md"
cp "$INPUT_DIR"/* "$DEBUG_DIR/"
cp "$WORK_DIR"/cline-output-*.jsonl "$DEBUG_DIR/"
cp "$RESULT_FILE" "$DEBUG_DIR/summary.md"
