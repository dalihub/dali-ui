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
COMMENT_FILE="${GITHUB_WORKSPACE:-$PR_WORKSPACE}/rules-review-comment.md"
CHANGED_RULES_FILE="$REVIEW_DIR/changed-rules.txt"

MAX_DIFF_BYTES="${MAX_DIFF_BYTES:-180000}"

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

# 답변 템플릿

감지된 이슈가 없으면 아래 문구만 출력한다.

\`\`\`md
감지된 이슈 없음
\`\`\`

감지된 이슈가 있으면 아래 형식을 지켜서 출력한다.

\`\`\`md
감지된 이슈 N건

- [severity] \`file:line\` - 한 줄 요약
- [severity] \`file:line\` - 한 줄 요약

<details>
<summary>[severity] file:line - 한 줄 요약</summary>

#### 규칙
관련 rules 문서와 규칙 이름

#### 문제
왜 규칙 위반 또는 확인 필요 사항인지 설명

#### 권장 조치
권장 수정 방향

</details>

<details>
<summary>[severity] file:line - 한 줄 요약</summary>

#### 규칙
관련 rules 문서와 규칙 이름

#### 문제
왜 규칙 위반 또는 확인 필요 사항인지 설명

#### 권장 조치
권장 수정 방향

</details>
\`\`\`

severity는 \`required\`, \`recommended\`, \`contextual\`, \`확인 필요\` 중 하나를 사용한다.
각 이슈는 반드시 상단 요약 리스트와 하단 \`details\` 블록에 모두 포함한다.
자동 리뷰 한계나 diff 제한 사항은 실제로 필요한 경우 마지막에 짧게 덧붙인다.

# rules 변경 경고

$RULES_CHANGED_WARNING

# diff 제한 참고

$DIFF_TRUNCATED_NOTE

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

for (const line of raw.split(/\r?\n/)) {
  const trimmed = line.trim();
  if (!trimmed.startsWith('{')) continue;

  try {
    const message = JSON.parse(trimmed);
    if (message.say === 'completion_result' && typeof message.text === 'string') {
      result = message.text;
    }
  } catch {
    // Ignore non-JSON lines from CLI startup logs.
  }
}

if (!result.trim()) {
  const fallback = raw
    .split(/\r?\n/)
    .filter(line => line.trim())
    .slice(-80)
    .join('\n');
  result = fallback || 'Cline CLI 결과를 파싱하지 못했습니다.';
}

fs.writeFileSync(outPath, result.replace(/\\n/g, '\n').trim() + '\n');
NODE

{
  echo "<!-- dali-ui-rules-review -->"
  echo "## DALI UI Rules Review"
  echo
  if [ -n "$RULES_CHANGED_WARNING" ]; then
    echo "$RULES_CHANGED_WARNING"
    echo
  fi
  echo "### 기준"
  echo "- 검사 기준: 이 PR에 포함된 \`dali-ui/rules/*\`"
  echo "- 분석 범위: PR diff"
  echo "- 실행 방식: Cline CLI 자동 리뷰"
  if [ "$DIFF_TRUNCATED" = true ]; then
    echo "- 참고: PR diff가 ${MAX_DIFF_BYTES} bytes로 잘려서 분석되었습니다."
  fi
  echo
  echo "### 결과"
  echo
  cat "$REVIEW_TEXT_FILE"
  echo
  echo "### 참고"
  echo "이 리뷰는 \`rules\` 준수 여부를 보조적으로 확인하기 위한 자동 리뷰입니다. 최종 판단은 maintainer review를 따릅니다."
} > "$COMMENT_FILE"

if [ "$CLINE_STATUS" -ne 0 ]; then
  {
    echo
    echo "> Cline CLI exited with status $CLINE_STATUS. 위 결과가 불완전할 수 있습니다."
  } >> "$COMMENT_FILE"
fi
