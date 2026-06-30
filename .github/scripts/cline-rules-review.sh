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
RAW_OUTPUT_FILE="$REVIEW_DIR/cline-output.jsonl"
REVIEW_TEXT_FILE="$REVIEW_DIR/review.md"
DETERMINISTIC_REVIEW_FILE="$REVIEW_DIR/deterministic-review.md"
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

node - "$DIFF_FILE.full" "$DETERMINISTIC_REVIEW_FILE" <<'NODE'
const fs = require('fs');

const [diffPath, outPath] = process.argv.slice(2);
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
  process.exit(0);
}

const issueCount = Math.min(issues.length, 50);
const lines = [`감지된 이슈 ${issueCount}건`, ''];

for (const [index, issue] of issues.slice(0, 50).entries()) {
  lines.push(`<details>`);
  lines.push(`<summary>(${index + 1}) components가 foundation internal/integration header를 include함</summary>`);
  lines.push('');
  lines.push('#### 규칙');
  lines.push('component-boundaries.md / Components Use Foundation Public and Provider APIs');
  lines.push('');
  lines.push('#### 위치');
  lines.push(`\`${issue.file}:${issue.line}\``);
  lines.push('');
  lines.push('#### 문제');
  lines.push(`\`dali-ui-components\`는 foundation public/provider API만 사용해야 하지만 \`${issue.include}\`를 추가했습니다.`);
  lines.push('');
  lines.push('#### 권장 조치');
  lines.push('foundation internal/integration header 의존을 제거하고 public/provider API로 필요한 정보를 전달하세요.');
  lines.push('');
  lines.push('</details>');
  lines.push('');
}

if (issues.length > 50) {
  lines.push(`추가 이슈 ${issues.length - 50}건 생략`);
  lines.push('');
}

fs.writeFileSync(outPath, lines.join('\n'));
NODE

cat > "$CONTEXT_FILE" <<EOF
# PR 정보

- PR 번호: ${PR_NUMBER:-unknown}
- PR 제목: ${PR_TITLE:-unknown}
- 작성자: ${PR_AUTHOR:-unknown}
- Base SHA: $BASE_SHA
- Head SHA: $HEAD_SHA

# 리뷰 정책

- 이 리뷰는 PR checkout에 포함된 \`dali-ui/rules/*\`를 기준으로 수행한다.
- \`rules/*\`가 PR에서 변경된 경우에도 변경된 rules를 기준으로 판단한다.
- 단, \`rules/*\` 변경이 감지되면 결과 상단에 경고 메시지를 포함한다.
- 분석 범위는 아래 PR diff와 rules 문서로 제한한다.
- 확실하지 않은 내용은 추측하지 말고 "확인 필요"로 표시한다.
- 파일 수정, 커밋, push, 외부 게시, 명령 실행을 시도하지 않는다.
- 리뷰 결과는 한국어로 작성한다.
- 규칙 위반 또는 확인 필요 항목이 없으면 "감지된 이슈 없음"만 출력한다.
- 감지된 이슈가 있으면 최대 50개까지만 보고한다.
- 50개를 초과하는 이슈가 있으면 required, 확인 필요, recommended, contextual 순서로 우선순위를 정해 50개를 고르고, 마지막에 "추가 이슈 N건 생략"을 짧게 표시한다.
- 각 이슈의 요약은 한 줄로 작성한다.
- 각 details 블록은 규칙, 위치, 문제, 권장 조치를 합쳐 10줄 이내로 작성한다.
- 전체 결과는 50000자 이내로 작성한다.
- rules 전문이나 diff 내용을 반복 인용하지 않는다.
- rules 문서의 Validation 섹션과 validation-checks.md에 기록된 검색/검증 관점을 우선적으로 적용한다.
- 분석 과정, 규칙별 전체 점검 로그, OK 항목 목록은 출력하지 않는다.
- 최종 답변은 반드시 답변 템플릿의 markdown 본문만 출력한다.
- 아래 "이미 감지된 deterministic 이슈"에 있는 이슈는 최종 리뷰에 별도로 포함되므로 반복해서 보고하지 않는다.
- deterministic 이슈와 다른 추가 이슈만 보고한다.

# 답변 템플릿

감지된 이슈가 없으면 아래 문구만 출력한다.

\`\`\`md
감지된 이슈 없음
\`\`\`

감지된 이슈가 있으면 아래 형식을 지켜서 출력한다.

\`\`\`md
감지된 이슈 N건

<details>
<summary>(1) 이슈 한 줄 요약</summary>

#### 규칙
관련 rules 문서와 규칙 이름

#### 위치
\`file:line\`

#### 문제
왜 규칙 위반 또는 확인 필요 사항인지 설명

#### 권장 조치
권장 수정 방향

</details>

<details>
<summary>(2) 이슈 한 줄 요약</summary>

#### 규칙
관련 rules 문서와 규칙 이름

#### 위치
\`file:line\`

#### 문제
왜 규칙 위반 또는 확인 필요 사항인지 설명

#### 권장 조치
권장 수정 방향

</details>

추가 이슈 N건 생략
\`\`\`

severity는 \`required\`, \`recommended\`, \`contextual\`, \`확인 필요\` 중 하나를 사용한다.
severity는 우선순위 판단에만 사용하고 출력에는 표시하지 않는다.
각 이슈는 \`details\` 블록 하나로 출력한다.
\`summary\`에는 파일 이름을 쓰지 말고, 번호와 이슈 요약만 쓴다.
이슈가 50개 이하이면 "추가 이슈 N건 생략" 문구는 출력하지 않는다.
자동 리뷰 한계나 diff 제한 사항은 실제로 필요한 경우 마지막에 짧게 덧붙인다.

# rules 변경 경고

$RULES_CHANGED_WARNING

# diff 제한 참고

$DIFF_TRUNCATED_NOTE

# 이미 감지된 deterministic 이슈

아래 이슈는 자동 검사에서 이미 감지되어 최종 리뷰에 포함된다.
Cline 리뷰에서는 아래 이슈를 반복해서 보고하지 말고, 이와 다른 추가 이슈만 보고한다.

$(if [ -s "$DETERMINISTIC_REVIEW_FILE" ]; then cat "$DETERMINISTIC_REVIEW_FILE"; else echo "감지된 이슈 없음"; fi)

$(cat "$RULES_FILE")

# PR diff

\`\`\`diff
$(cat "$DIFF_FILE")
\`\`\`
EOF

TASK_PROMPT="dali-ui/rules 준수 여부를 PR diff 기준으로 리뷰해 주세요. 제공된 rules와 diff만 근거로 사용하고, 답변 템플릿을 지켜 한국어로 작성해 주세요."

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

function extractTemplateBody(text) {
  const normalized = normalizeText(text);
  const issueMatch = normalized.match(/감지된 이슈\s+\d+건[\s\S]*/);
  if (issueMatch) {
    return issueMatch[0].trim();
  }

  if (normalized.includes('감지된 이슈 없음')) {
    return '감지된 이슈 없음';
  }

  return 'Cline CLI 결과가 답변 템플릿과 일치하지 않습니다.';
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
  result = 'Cline CLI 결과를 파싱하지 못했습니다.';
}

fs.writeFileSync(outPath, extractTemplateBody(result) + '\n');
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
  if (!/^감지된 이슈\s+\d+건/.test(result)) return [];
  return [...result.matchAll(/<details>[\s\S]*?<\/details>/g)].map(match => match[0]);
}

const deterministicResult = readResult(deterministicPath);
const clineResult = readResult(clinePath);
const details = [
  ...extractDetails(deterministicResult),
  ...extractDetails(clineResult)
];
const renumberedDetails = details.map((detail, index) =>
  detail.replace(/<summary>\(\d+\)\s*/, `<summary>(${index + 1}) `)
);

if (renumberedDetails.length === 0) {
  console.log('감지된 이슈 없음');
} else {
  console.log(`감지된 이슈 ${renumberedDetails.length}건\n`);
  console.log(renumberedDetails.join('\n\n'));
}
NODE
} > "$COMMENT_FILE"

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
cp "$RULES_FILE" "$DEBUG_DIR/rules.md"
cp "$DIFF_FILE" "$DEBUG_DIR/pr.diff"
cp "$RAW_OUTPUT_FILE" "$DEBUG_DIR/cline-output.jsonl"
cp "$REVIEW_TEXT_FILE" "$DEBUG_DIR/cline-review.md"
cp "$DETERMINISTIC_REVIEW_FILE" "$DEBUG_DIR/deterministic-review.md"
cp "$COMMENT_FILE" "$DEBUG_DIR/comment.md"
