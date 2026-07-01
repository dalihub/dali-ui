#!/usr/bin/env bash

set -euo pipefail

if [ -z "${PR_WORKSPACE:-}" ]; then
  echo "PR_WORKSPACE is required." >&2
  exit 1
fi

if [ -z "${BASE_SHA:-}" ] || [ -z "${HEAD_SHA:-}" ]; then
  echo "BASE_SHA and HEAD_SHA are required." >&2
  exit 1
fi

REVIEW_DIR="${RUNNER_TEMP:-/tmp}/dali-ui-rules-review"
RULES_FILE="$REVIEW_DIR/rules.md"
DIFF_FILE="$REVIEW_DIR/pr.diff"
CONTEXT_FILE="$REVIEW_DIR/context.md"
INPUT_JSON_FILE="$REVIEW_DIR/input.json"
RAW_OUTPUT_FILE="$REVIEW_DIR/cline-output.jsonl"
REVIEW_TEXT_FILE="$REVIEW_DIR/review.md"
DETERMINISTIC_REVIEW_FILE="$REVIEW_DIR/deterministic-review.md"
DETERMINISTIC_ISSUES_FILE="$REVIEW_DIR/deterministic-issues.json"
COMMENT_FILE="${GITHUB_WORKSPACE:-$PR_WORKSPACE}/rules-review-comment.md"
CHANGED_RULES_FILE="$REVIEW_DIR/changed-rules.txt"

MAX_DIFF_BYTES="${MAX_DIFF_BYTES:-180000}"
COMMENT_MAX_CHARS="${COMMENT_MAX_CHARS:-60000}"

mkdir -p "$REVIEW_DIR"
cd "$PR_WORKSPACE"

if [ ! -d rules ]; then
  echo "rules directory does not exist in PR workspace." >&2
  exit 1
fi

git diff --name-only "$BASE_SHA" "$HEAD_SHA" -- rules > "$CHANGED_RULES_FILE" || true

{
  echo "# dali-ui/rules"
  echo
  find rules -maxdepth 1 -type f -name '*.md' | sort | while read -r ruleFile
  do
    echo
    echo "## File: $ruleFile"
    echo
    sed -n '1,260p' "$ruleFile"
  done
} > "$RULES_FILE"

DIFF_TRUNCATED=false
git diff --find-renames --unified=80 "$BASE_SHA" "$HEAD_SHA" -- . \
  ':(exclude).github/workflows/rules-review.yml' \
  > "$DIFF_FILE.full" || true

if [ "$(wc -c < "$DIFF_FILE.full")" -gt "$MAX_DIFF_BYTES" ]; then
  head -c "$MAX_DIFF_BYTES" "$DIFF_FILE.full" > "$DIFF_FILE"
  DIFF_TRUNCATED=true
else
  cp "$DIFF_FILE.full" "$DIFF_FILE"
fi

RULES_CHANGED_WARNING=""
if [ -s "$CHANGED_RULES_FILE" ]; then
  RULES_CHANGED_WARNING=$(cat <<'EOF'
⚠️ 이 PR은 `dali-ui/rules/*`를 변경합니다.

이번 자동 리뷰는 base branch의 규칙이 아니라 이 PR에 포함된 규칙을 기준으로 수행되었습니다.
리뷰 결과를 판단하기 전에 규칙 변경이 의도된 것인지 확인해 주세요.
EOF
)
fi

DIFF_TRUNCATED_NOTE=""
if [ "$DIFF_TRUNCATED" = true ]; then
  DIFF_TRUNCATED_NOTE="PR diff가 ${MAX_DIFF_BYTES} bytes로 잘려서 분석되었습니다. 큰 PR에서는 일부 변경이 자동 리뷰에 포함되지 않았을 수 있습니다."
fi

node - "$DIFF_FILE.full" "$DETERMINISTIC_REVIEW_FILE" "$DETERMINISTIC_ISSUES_FILE" <<'NODE'
const fs = require('fs');

const [diffPath, outPath, jsonOutPath] = process.argv.slice(2);
const diff = fs.readFileSync(diffPath, 'utf8');
const issues = [];
let currentFile = '';
let newLine = 0;

for (const line of diff.split(/\r?\n/)) {
  const fileMatch = line.match(/^\+\+\+ b\/(.+)$/);
  if (fileMatch) {
    currentFile = fileMatch[1];
    continue;
  }

  const hunkMatch = line.match(/^@@ -\d+(?:,\d+)? \+(\d+)(?:,\d+)? @@/);
  if (hunkMatch) {
    newLine = Number(hunkMatch[1]);
    continue;
  }

  if (!currentFile || line.startsWith('--- ')) continue;

  if (line.startsWith('+') && !line.startsWith('+++ ')) {
    if (
      currentFile.startsWith('dali-ui-components/') &&
      /#include\s+<dali-ui-foundation\/(integration-api|internal)\//.test(line)
    ) {
      issues.push({
        file: currentFile,
        line: newLine,
        include: line.slice(1).trim()
      });
    }
    newLine += 1;
  } else if (!line.startsWith('-')) {
    newLine += 1;
  }
}

if (issues.length === 0) {
  fs.writeFileSync(outPath, '');
  fs.writeFileSync(jsonOutPath, '[]\n');
  process.exit(0);
}

const issueCount = Math.min(issues.length, 50);
const lines = [`감지된 이슈 ${issueCount}건`, ''];

for (const [index, issue] of issues.slice(0, 50).entries()) {
  issue.summary = 'components가 foundation internal/integration header를 include함';
  issue.rule = 'component-boundaries.md / Components Use Foundation Public and Provider APIs';
  issue.location = `${issue.file}:${issue.line}`;
  issue.severity = 'required';
  issue.problem = `dali-ui-components는 foundation public/provider API만 사용해야 하지만 ${issue.include}를 추가했습니다.`;
  issue.recommendation = 'foundation internal/integration header 의존을 제거하고 public/provider API로 필요한 정보를 전달하세요.';

  lines.push(`<details>`);
  lines.push(`<summary>(${index + 1}) ${issue.summary}</summary>`);
  lines.push('');
  lines.push('#### 규칙');
  lines.push(issue.rule);
  lines.push('');
  lines.push('#### 위치');
  lines.push(`\`${issue.location}\``);
  lines.push('');
  lines.push('#### 문제');
  lines.push(`\`dali-ui-components\`는 foundation public/provider API만 사용해야 하지만 \`${issue.include}\`를 추가했습니다.`);
  lines.push('');
  lines.push('#### 권장 조치');
  lines.push(issue.recommendation);
  lines.push('');
  lines.push('</details>');
  lines.push('');
}

if (issues.length > 50) {
  lines.push(`추가 이슈 ${issues.length - 50}건 생략`);
  lines.push('');
}

fs.writeFileSync(outPath, lines.join('\n'));
fs.writeFileSync(
  jsonOutPath,
  JSON.stringify(
    issues.slice(0, 50).map(issue => ({
      summary: issue.summary,
      rule: issue.rule,
      location: issue.location,
      severity: issue.severity,
      problem: issue.problem,
      recommendation: issue.recommendation
    })),
    null,
    2
  ) + '\n'
);
NODE

node - "$DIFF_FILE" "$DETERMINISTIC_ISSUES_FILE" "$INPUT_JSON_FILE" <<'NODE'
const fs = require('fs');

const [diffPath, knownIssuesPath, outPath] = process.argv.slice(2);
const rules = fs.readdirSync('rules')
  .filter(file => file.endsWith('.md'))
  .sort()
  .map(file => ({
    path: `rules/${file}`,
    content: fs.readFileSync(`rules/${file}`, 'utf8').split(/\r?\n/).slice(0, 260).join('\n')
  }));

const input = {
  alreadyReportedIssues: JSON.parse(fs.readFileSync(knownIssuesPath, 'utf8')),
  rules,
  diff: fs.readFileSync(diffPath, 'utf8')
};

fs.writeFileSync(outPath, JSON.stringify(input, null, 2) + '\n');
NODE

cat > "$CONTEXT_FILE" <<EOF
You review a pull request by comparing input.diff against the development rules in input.rules.

input.diff is a unified git diff for the pull request.
input.rules contains the rule documents to apply.
input.alreadyReportedIssues contains findings that are already reported by another checker.

Find code or documentation changes in input.diff that violate, or may violate, the rules in input.rules.
Return only findings that are not already covered by input.alreadyReportedIssues.

Your entire response is a JSON array.

Each issue item has this schema:
{
  "summary": "short issue summary",
  "rule": "rule file and rule name",
  "location": "file:line or file",
  "severity": "required | recommended | contextual | needs_confirmation",
  "problem": "reason",
  "recommendation": "recommended fix"
}

Return [] when there are no issue items.
Return at most 50 issue items.
Preserve file paths, rule filenames, and code identifiers.
Write user-facing text in Korean.

Input is a JSON object with these fields:
- alreadyReportedIssues: issue items that are already reported
- rules: rule documents, each with path and content
- diff: unified git diff text

Input:
$(cat "$INPUT_JSON_FILE")
EOF

TASK_PROMPT="Review the pull request using the provided input. Return a JSON array of issue items."

set +e
cline -y --ask --json --timeout 900 "$TASK_PROMPT" < "$CONTEXT_FILE" > "$RAW_OUTPUT_FILE"
CLINE_STATUS=$?
set -e

node - "$RAW_OUTPUT_FILE" "$REVIEW_TEXT_FILE" <<'NODE'
const fs = require('fs');

const [rawPath, outPath] = process.argv.slice(2);
const raw = fs.readFileSync(rawPath, 'utf8');
let result = '';

function normalizeText(text) {
  return text.replace(/\\n/g, '\n').trim();
}

function escapeHtml(text) {
  return text
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;');
}

function truncateText(text, maxLength) {
  if (text.length <= maxLength) return text;
  return `${text.slice(0, maxLength).trimEnd()}\n\n... 생략 ...`;
}

function asText(value) {
  if (typeof value === 'string') return value.trim();
  if (value === undefined || value === null) return '';
  return String(value).trim();
}

function normalizeSeverity(value) {
  const severity = asText(value);
  return ['required', 'recommended', 'contextual', 'needs_confirmation'].includes(severity)
    ? severity
    : 'needs_confirmation';
}

function extractJsonArray(text) {
  const normalized = normalizeText(text);
  if (!normalized) return [];

  try {
    const parsed = JSON.parse(normalized);
    return Array.isArray(parsed) ? parsed : null;
  } catch {
    // Some providers can add transport text around the model output.
  }

  const start = normalized.indexOf('[');
  const end = normalized.lastIndexOf(']');
  if (start !== -1 && end > start) {
    try {
      const parsed = JSON.parse(normalized.slice(start, end + 1));
      return Array.isArray(parsed) ? parsed : null;
    } catch {
      return null;
    }
  }

  return null;
}

function sanitizeIssue(issue) {
  return {
    summary: asText(issue?.summary) || 'Cline 결과 확인 필요',
    rule: asText(issue?.rule) || '확인 필요',
    location: asText(issue?.location) || '확인 필요',
    severity: normalizeSeverity(issue?.severity),
    problem: asText(issue?.problem) || 'Cline이 문제 설명을 비워 두었습니다.',
    recommendation: asText(issue?.recommendation) || 'Cline 결과와 rules-review-debug artifact를 확인하세요.'
  };
}

function renderIssueDetails(issues) {
  return issues.map(issue => `<details>
<summary>${escapeHtml(issue.summary)}</summary>

#### 규칙
${escapeHtml(issue.rule)}

#### 위치
\`${escapeHtml(issue.location)}\`

#### 문제
${escapeHtml(issue.problem)}

#### 권장 조치
${escapeHtml(issue.recommendation)}

</details>`).join('\n\n');
}

function fallbackIssueDetails(text) {
  const normalized = normalizeText(text);
  if (!normalized) return '';

  const escaped = escapeHtml(truncateText(normalized, 3000));
  return `<details>
<summary>Cline JSON 결과 파싱 실패</summary>

#### 규칙
Cline CLI output format / JSON issue array

#### 위치
확인 필요

#### 문제
Cline이 JSON issue array를 반환하지 않았습니다. 출력 일부:

${escaped}

#### 권장 조치
rules-review-debug artifact의 \`cline-output.jsonl\`를 확인하세요.

</details>`;
}

function extractIssueDetails(text) {
  const parsed = extractJsonArray(text);
  if (Array.isArray(parsed)) {
    return renderIssueDetails(parsed.slice(0, 50).map(sanitizeIssue));
  }

  return fallbackIssueDetails(text);
}

function acceptMessage(message) {
  if (message.say === 'completion_result' && typeof message.text === 'string') {
    result = message.text;
  } else if (message.type === 'run_result' && typeof message.text === 'string') {
    result = message.text;
  } else if (
    message.type === 'agent_event' &&
    message.event?.type === 'done' &&
    typeof message.event.text === 'string'
  ) {
    result = message.event.text;
  }
}

let buffer = '';
for (const line of raw.split(/\r?\n/)) {
  const trimmed = line.trim();
  if (!trimmed && !buffer) continue;
  if (!buffer && !trimmed.startsWith('{')) continue;

  buffer = buffer ? `${buffer}\n${line}` : line;
  try {
    acceptMessage(JSON.parse(buffer));
    buffer = '';
  } catch {
    // Keep buffering. Some Cline JSON events can contain formatted text.
  }
}

if (!result.trim()) {
  result = '';
}

fs.writeFileSync(outPath, extractIssueDetails(result) + '\n');
NODE

{
  echo "<!-- dali-ui-rules-review -->"
  echo "## DALI UI Rules Review"
  echo
  if [ -n "$RULES_CHANGED_WARNING" ]; then
    echo "$RULES_CHANGED_WARNING"
    echo
  fi
  if [ "$DIFF_TRUNCATED" = true ]; then
    echo "> PR diff가 ${MAX_DIFF_BYTES} bytes로 잘려서 분석되었습니다. 큰 PR에서는 일부 변경이 자동 리뷰에 포함되지 않았을 수 있습니다."
    echo
  fi
  node - "$DETERMINISTIC_REVIEW_FILE" "$REVIEW_TEXT_FILE" <<'NODE'
const fs = require('fs');

const [deterministicPath, clinePath] = process.argv.slice(2);

function readResult(path) {
  if (!fs.existsSync(path)) return '';
  return fs.readFileSync(path, 'utf8').trim();
}

function extractDetails(result) {
  if (!result || result === 'NO_ISSUES') return [];
  return [...result.matchAll(/<details>[\s\S]*?<\/details>/g)].map(match => match[0]);
}

function extractSection(detail, title) {
  return detail.match(new RegExp(`#### ${title}\\n([\\s\\S]*?)(?=\\n#### |\\n</details>|$)`))?.[1]?.trim() ?? '';
}

function normalizeKeyText(text) {
  return text
    .toLowerCase()
    .replace(/&lt;/g, '<')
    .replace(/&gt;/g, '>')
    .replace(/&amp;/g, '&')
    .replace(/:\d+\b/g, '')
    .replace(/\s+/g, ' ')
    .trim();
}

function detailKey(detail) {
  const rule = normalizeKeyText(extractSection(detail, '규칙'));
  const location = normalizeKeyText(extractSection(detail, '위치'));
  const path = location.match(/(?:dali-ui-foundation|dali-ui-components|rules|docs|automated-tests|manual-tests|samples|wiki)\/[a-z0-9_./-]+\.(?:h|cpp|md|txt|cmake|sh)/)?.[0] ?? '';
  const ruleFile = rule.match(/[a-z0-9-]+\.md/)?.[0] ?? rule.match(/component boundar|api naming|docs and wiki|handle-body|public api/)?.[0] ?? '';

  if (path && ruleFile) {
    return `${path}|${ruleFile}`;
  }

  const summary = detail.match(/<summary>\s*(?:\(\d+\)|\d+[.)])?\s*([\s\S]*?)<\/summary>/)?.[1] ?? detail;
  return normalizeKeyText(summary);
}

const deterministicResult = readResult(deterministicPath);
const clineResult = readResult(clinePath);
const seen = new Set();
const details = [];

for (const detail of [
  ...extractDetails(deterministicResult),
  ...extractDetails(clineResult)
]) {
  const key = detailKey(detail);
  if (seen.has(key)) continue;
  seen.add(key);
  details.push(detail);
}

const renumberedDetails = details.map((detail, index) =>
  detail.replace(/<summary>\s*(?:\(\d+\)|\d+[.)])?\s*/, `<summary>(${index + 1}) `)
);

if (renumberedDetails.length === 0) {
  console.log('감지된 이슈 없음');
} else {
  console.log(`감지된 이슈 ${renumberedDetails.length}건\n`);
  console.log(renumberedDetails.join('\n\n'));
}
NODE
} > "$COMMENT_FILE"

{
  echo
  echo "---"
  echo
  echo "_by ${CLINE_MODEL:-unknown}_"
} >> "$COMMENT_FILE"

if [ "$CLINE_STATUS" -ne 0 ]; then
  {
    echo
    echo "> Cline CLI exited with status $CLINE_STATUS. 위 결과가 불완전할 수 있습니다."
  } >> "$COMMENT_FILE"
fi

node - "$COMMENT_FILE" "$COMMENT_MAX_CHARS" <<'NODE'
const fs = require('fs');

const [commentPath, maxCharsText] = process.argv.slice(2);
const maxChars = Number(maxCharsText);
const omissionNotice = '\n\n> GitHub comment 길이 제한을 피하기 위해 일부 결과를 생략했습니다.\n';
const body = fs.readFileSync(commentPath, 'utf8');

if (body.length <= maxChars) {
  process.exit(0);
}

const truncatedLength = Math.max(0, maxChars - omissionNotice.length);
fs.writeFileSync(commentPath, body.slice(0, truncatedLength).trimEnd() + omissionNotice);
NODE

DEBUG_DIR="${GITHUB_WORKSPACE:-$PR_WORKSPACE}/rules-review-debug"
rm -rf "$DEBUG_DIR"
mkdir -p "$DEBUG_DIR"
cp "$CONTEXT_FILE" "$DEBUG_DIR/context.md"
cp "$INPUT_JSON_FILE" "$DEBUG_DIR/input.json"
cp "$RULES_FILE" "$DEBUG_DIR/rules.md"
cp "$DIFF_FILE" "$DEBUG_DIR/pr.diff"
cp "$RAW_OUTPUT_FILE" "$DEBUG_DIR/cline-output.jsonl"
cp "$DETERMINISTIC_ISSUES_FILE" "$DEBUG_DIR/deterministic-issues.json"
cp "$REVIEW_TEXT_FILE" "$DEBUG_DIR/cline-review.md"
cp "$DETERMINISTIC_REVIEW_FILE" "$DEBUG_DIR/deterministic-review.md"
cp "$COMMENT_FILE" "$DEBUG_DIR/comment.md"
