# Emoji Test Skill

이 문서는 사람이 순서대로 실행 방법을 확인하는 manual이 아니라, agent가
`dali-ui` emoji sequence 테스트를 수정하거나 해석하기 전에 먼저 읽어야 하는
유지보수 skill이다.

적용 범위:

- `dali-ui` emoji sequence UTC
- Unicode emoji fixture generator/fixture
- manual sweep 도구
- visual viewer 도구
- `multi-language-support`의 emoji script boundary 변경

이 문서는 `dali-ui`의 emoji sequence 테스트를 유지보수할 때 지켜야 하는
정책이다. 목적은 두 가지다.

- Unicode 표준 emoji sequence가 `SetScripts`, font fallback, shaping 단계에서
  깨지지 않는지 검증한다.
- 표준이 아니더라도 실제 입력에서 자주 들어오는 잘못된 VS15/VS16/ZWJ/keycap
  조합을 DALI compatibility fallback으로 안정적으로 처리한다.

`multi-language-support`는 emoji만 처리하는 코드가 아니다. Latin, Hangul,
Arabic, Hebrew, Devanagari, 숫자, 기호 등 모든 문자의 script run을 결정한다.
따라서 emoji 수정은 항상 일반 문자 script compatibility까지 함께 검토해야 한다.

## 기준

공식 기준은 repo에 고정된 fixture다.

```text
automated-tests/emoji/res/emoji-test-latest.tsv
```

현재 fixture는 Unicode Emoji 17.0의 `emoji-test.txt`에서 생성했다.
Unicode data는 CI에서 매번 다운로드하지 않는다. fixture 갱신은 명시적인 작업으로
수행하고, 생성된 TSV를 커밋에 포함한다.

참고 공식 문서:

- https://www.unicode.org/reports/tr51/
- https://www.unicode.org/Public/emoji/latest/emoji-test.txt
- https://www.unicode.org/Public/emoji/latest/emoji-sequences.txt
- https://www.unicode.org/Public/emoji/latest/emoji-zwj-sequences.txt
- https://www.unicode.org/Public/emoji/latest/emoji-variation-sequences.txt

## 테스트 분류

### 1. Strict Unicode

Unicode fixture에 들어 있는 sequence를 기준으로 한다.

검증해야 하는 것:

- `A + emoji_sequence + B` 형태에서 Latin guard가 유지된다.
- emoji sequence 내부가 여러 script run으로 쪼개지지 않는다.
- emoji run은 `EMOJI`, `EMOJI_COLOR`, `EMOJI_TEXT` 중 하나로 분류된다.
- font fallback run이 전체 문자를 덮는다.
- shaping 결과 glyph가 0개가 아니고 glyph map이 일관된다.
- emoji sequence/item 영역에 glyph index 0이 없어야 한다. glyph index 0은 화면에서
  네모(tofu)로 보일 가능성이 높은 font fallback failure로 취급한다.
- ZWJ, keycap, modifier, regional indicator flag, tag sequence처럼 합성되어야 하는
  multi-codepoint emoji는 하나의 visible glyph로 렌더링되어야 한다.

Strict Unicode 테스트는 표준 conformance 관점이다. 비표준 입력을 여기에 섞지 않는다.

### 2. DALI Compatibility

실제 앱 입력에서 들어올 수 있는 잘못된 sequence를 위한 별도 정책이다.

예:

- emoji에 중복 VS15/VS16이 붙은 경우
- VS15 뒤에 VS16이 다시 붙은 경우
- ZWJ sequence 중간에 VS15가 잘못 들어간 경우
- keycap base에 VS15가 붙은 경우
- dangling ZWJ가 있는 경우
- 표준 ZWJ sequence에서 VS16이 빠진 경우

이 bucket은 Unicode conformance가 아니다. 목표는 crash 없이, 가능하면 하나의
emoji-family run으로 유지하고, 기존 DALI 동작과 호환되는 fallback을 제공하는 것이다.

### 3. Non-Emoji Script Compatibility

emoji workaround가 일반 문자 script를 침범하지 않는지 확인한다.

반드시 지켜야 하는 것:

- Latin guard를 emoji run이 먹으면 안 된다.
- Hangul, Arabic, Hebrew, Devanagari 같은 일반 script가 `EMOJI_*`로 바뀌면 안 된다.
- keycap 처리 때문에 주변 ASCII digits나 Hangul이 keycap run에 합쳐지면 안 된다.
- malformed emoji fallback이 앞뒤 RTL script를 소비하면 안 된다.

이 정책은 `multi-language-support`가 전체 text stack의 script boundary를 결정하기
때문에 중요하다.

## 실행 계층

### 기본 UTC

PR에서 기본으로 돌릴 수 있는 빠른 regression이다.

대상 파일:

```text
automated-tests/src/dali-ui-foundation-internal/utc-Dali-EmojiSequence-internal.cpp
```

포함해야 하는 것:

- 대표 emoji sequence script run
- keycap font fallback
- DALI malformed fallback
- mixed emoji sequence boundary
- representative shaping smoke
- non-emoji script compatibility

전체 Unicode fixture 5,225개를 기본 UTC에서 항상 순회하면 안 된다. 긴 테스트는
명시적인 env를 켰을 때만 실행한다.

### Env-Gated UTC

CI nightly나 수동 검증용이다.

```bash
DALI_RUN_UNICODE_EMOJI_REPRESENTATIVE_SWEEP=1 \
./automated-tests/build/src/dali-ui-foundation-internal/tct-dali-ui-foundation-internal-core \
UtcDaliEmojiSequenceUnicodeRepresentativePatternsP

DALI_RUN_FULL_EMOJI_SWEEP=1 \
./automated-tests/build/src/dali-ui-foundation-internal/tct-dali-ui-foundation-internal-core \
UtcDaliEmojiSequenceUnicodeFullSweepP

DALI_RUN_FULL_EMOJI_SHAPE_SWEEP=1 \
./automated-tests/build/src/dali-ui-foundation-internal/tct-dali-ui-foundation-internal-core \
UtcDaliEmojiSequenceUnicodeFullShapingSmokeP
```

### Manual Sweep

개발자가 변경 전후를 빠르게 비교하거나 failure log를 얻기 위한 도구다.

```bash
./automated-tests/emoji/sweep-test.sh

./automated-tests/emoji/sweep-test.sh -a

./automated-tests/emoji/sweep-test.sh -c
```

모드 의미:

- `script`: Unicode fixture 전체의 script boundary를 검증한다.
- `font`: `script` 검증에 font fallback coverage를 더한다.
- `shape`: `font` 검증에 shaping smoke, missing glyph, composition split 검사를 더한다.
- `all`: `shape`의 alias다.
- `compat`: DALI compatibility mutation set을 검증한다.

helper option은 일반 사용을 위해 `-a`와 `-c`만 노출한다. 기본값은 `all`이며,
`-a`도 같은 동작이다. signature filter나 failure log path를 직접 지정해야 하면
binary를 env와 함께 실행한다.

```bash
DALI_EMOJI_SWEEP_MODE=all \
DALI_EMOJI_SWEEP_SIGNATURE_FILTER=keycap \
DALI_EMOJI_SWEEP_FAILURE_LOG=/tmp/dali-emoji-keycap-failures.log \
./automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-sweep
```

### Visual Viewer

리뷰어가 실제 렌더링 결과를 확인해야 할 때 사용한다.

```bash
DALI_EMOJI_VISUAL_MODE=review \
./automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-visual
```

visible helper script는 HTML export 없이 viewer만 실행하며 기본값은 `full`이다.
빠른 리뷰 범위만 열 때는 `-r`을 사용한다.

```bash
./automated-tests/emoji/visual-test.sh
./automated-tests/emoji/visual-test.sh -r
```

`review` mode는 agent가 리뷰 evidence를 만들 때 사용하는 좁은 범위다. 내부적으로는
Unicode representative, DALI compatibility fallback, keycap, symbol variation,
mixed text case로 나뉘지만 실행 옵션은 하나로 유지한다.

large/full visual viewer는 page 단위로 row를 생성해야 한다. 전체 fixture를 열 때
모든 row actor를 한 번에 생성하는 구조로 되돌리면 안 된다. 기본 page size는 100개이며,
`DALI_EMOJI_VISUAL_PAGE_SIZE`로 조절한다. section jump button과 `N/P`, `[`/`]`
keyboard navigation은 full visual을 훑기 위한 기본 UX다.

증거 파일이 필요하면 HTML export 또는 PNG capture를 사용한다. HTML export에는
section 바로가기가 포함되어야 한다.

```bash
DALI_EMOJI_VISUAL_MODE=review \
DALI_EMOJI_VISUAL_EXPORT_HTML=/tmp/dali-emoji-review.html \
DALI_EMOJI_VISUAL_EXPORT_ONLY=1 \
./automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-visual
```

```bash
DALI_EMOJI_VISUAL_MODE=review \
DALI_EMOJI_VISUAL_CAPTURE=/tmp/dali-emoji-review.png \
DALI_EMOJI_VISUAL_EXIT_AFTER_CAPTURE=1 \
./automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-visual
```

failure log를 시각화할 때는 `DALI_EMOJI_VISUAL_INPUT`을 사용한다. 이 경우
`DALI_EMOJI_VISUAL_MODE`보다 input log가 우선한다. `DALI_EMOJI_VISUAL_SIGNATURE_FILTER`
는 일반 실행 옵션이 아니라 조사 범위를 좁히는 고급 debug 옵션으로 취급한다.

HTML evidence와 detail text에는 `scriptRuns`, `fontRuns`, `missingGlyphs`
diagnostics가 들어가야 한다. `scriptRuns`와 `fontRuns`는 `A+item+B` guarded text
기준이고, `missingGlyphs`와 `compositionGlyphs`는 emoji item range만 검사한다.
Composition이 성공한 case는 `compositionGlyphs`를 생략하고, 실패한 case만 `compositionGlyphs`를 출력한다.
`missingGlyphs: none`이어도 `compositionGlyphs`의 actual 값이 split visible glyphs이면
font에 component glyph는 있지만 sequence composition이 실패한 것이다.

### Unicode/Vendor Sample Preview

visual viewer는 optional Unicode/vendor sample image를 actual DALi preview 오른쪽에
보여줄 수 있다. sample image는 사람이 눈으로 비교하기 위한 helper이며 pass/fail 기준,
golden image, image diff oracle로 취급하지 않는다. 코드와 문서에서는
가능하면 `reference` 대신 `sample`, `unicode sample`, `vendor sample` 용어를 쓴다.

sample preview는 `visual-test.sh`에서 다음 option으로 켠다.

```bash
./automated-tests/emoji/visual-test.sh -s
./automated-tests/emoji/visual-test.sh --sample-dir /tmp/dali-emoji-unicode-samples
```

`-s`는 `/tmp/dali-emoji-unicode-samples`를 사용한다. `--sample-dir PATH`는 지정한
sample pack directory를 사용한다. directory가 없으면 warning만 출력하고 sample env를
설정하지 않은 상태로 기존 viewer를 실행해야 한다. `--no-sample`이나 `--sample-set`
script option은 추가하지 않는다. sample set 선택이 필요하면 app env를 직접 사용한다.

app env:

```bash
DALI_EMOJI_VISUAL_SAMPLE_DIR=/tmp/dali-emoji-unicode-samples
DALI_EMOJI_VISUAL_SAMPLE_SET=sample
```

`DALI_EMOJI_VISUAL_SAMPLE_SET` 기본값은 `sample`이다. legacy 호환을 위해 app은
`DALI_EMOJI_VISUAL_SAMPLE_VENDOR`도 fallback으로 읽을 수 있지만, 새 문서와 script는
`SAMPLE_SET`을 우선한다.

sample pack layout:

```text
/tmp/dali-emoji-unicode-samples/
  metadata.json
  index.tsv
  sample/
    1f436.png
    1f9ae-200d-1f415.png
    23-fe0f-20e3.png
```

sample image path 규칙은 `${DALI_EMOJI_VISUAL_SAMPLE_DIR}/${sample_set}/${sequence_key}.png`다.
`sequence_key`는 lowercase hex codepoint를 hyphen으로 연결한다. `U+` prefix, whitespace,
comma, hyphen separator는 normalize할 수 있어야 한다. ZWJ, VS15/VS16, skin tone modifier,
tag character, combining enclosing keycap은 제거하지 않는다.

예:

- `U+1F436` -> `1f436`
- `U+1F9AE U+200D U+1F415` -> `1f9ae-200d-1f415`
- `U+0023 U+FE0F U+20E3` -> `23-fe0f-20e3`

sample image가 없거나 key를 만들 수 없어도 viewer가 실패하면 안 된다. actual preview와
diagnostics는 그대로 보여주고 sample 영역에는 `No sample` placeholder를 표시한다.
sample ImageView는 aspect ratio를 유지해야 하며, sample label은 preview 내부 하단 overlay라
row 높이를 늘리면 안 된다.

### Label Performance

실제 `Dali::Ui::Label` 렌더링 시간을 비교해야 하면 perf helper를 사용한다.

```bash
./automated-tests/emoji/perf.sh
```

perf는 timestamped output directory 아래에 `label.log`, `pipeline.log`,
`pipeline-failures.log`, `summary.txt`, `report.html`을 모은다. report의 상단 카드와
`Label Rendering Time`은 실제 Label async render time이고, `Pipeline Stack Time`은
sweep profile을 이용한 text stack 단계별 비용이다. 두 측정은 목적이 다르므로 섞어서
해석하지 않는다.

## Fixture 갱신 정책

fixture는 다음 script로 갱신한다.

```bash
python3 automated-tests/emoji/tools/update-unicode-emoji-fixtures.py
```

갱신해야 하는 경우:

- 프로젝트가 새 Unicode Emoji version을 채택할 때
- signature 분류 로직이 바뀔 때
- fixture format이나 metadata가 바뀔 때
- 새 표준 sequence가 이번 변경의 regression 검증에 필요할 때

갱신 PR에는 다음을 포함한다.

- fixture generator 변경
- 새 TSV
- row count/status count/signature count 변화 설명
- 새로 추가되거나 실패하는 signature에 대한 해석

CI나 일반 build에서 Unicode latest를 자동 다운로드하면 안 된다. 최신 표준이 바뀌면
fixture update PR로 고정된 데이터를 갱신한다.

## Unicode Chart Sample Pack

Unicode chart sample pack은 fixture가 아니라 visual viewer용 local PNG cache다.
생성 script는 다음 위치에 둔다.

```bash
python3 automated-tests/emoji/tools/generate-unicode-emoji-samples.py \
  --output /tmp/dali-emoji-unicode-samples \
  --sample-set sample
```

기본 chart URL은 Unicode emoji chart의 full list, modifiers, ZWJ sequences, emoji
sequences를 포함해야 한다. `--chart-file`은 다운로드한 chart HTML로 offline 재생성할 때
사용한다. `--vendor`는 legacy alias이고 새 사용법은 `--sample-set`이다.

생성물인 `metadata.json`, `index.tsv`, PNG directory는 repository에 커밋하지 않는다.
script나 parser를 고칠 때는 spot key를 확인한다.

```text
1f436
1f44b-1f3fb
1f9d1-200d-1f4bb
23-fe0f-20e3
1f1f0-1f1f7
```

## 변경 시 체크리스트

emoji sequence 관련 코드를 수정한 에이전트는 최소한 다음을 확인한다.

- 기본 emoji UTC가 통과하는가?
- full `all` manual sweep이 5,225개 전체 통과하는가?
- `compat` manual sweep이 통과하는가?
- 위험한 변경이면 `shape` manual sweep 또는 full shaping UTC가 통과하는가?
- non-emoji script compatibility가 유지되는가?
- failure가 있다면 Strict Unicode failure인지, DALI Compatibility failure인지 분류했는가?
- visual evidence가 필요한 failure는 HTML/PNG로 남겼는가?
- visual evidence의 `missingGlyphs`와 `compositionGlyphs`가 failure 원인을 설명하는가?
- sequence 조합 문제가 의심되면 `compositionGlyphs`가 생략되었는지, 또는 actual 값이 failure 원인을 설명하는지 확인했는가?
- full visual viewer 변경이라면 page 단위 렌더링과 section jump가 유지되는가?
- sample preview 변경이라면 sample env가 없을 때 기존 layout이 깨지지 않고, sample이 없을 때 placeholder가 나오는가?
- sample pack/script 변경이라면 생성 PNG가 git status에 포함되지 않았는가?
- 성능 변경이면 `perf.sh` report에서 actual Label timing과 pipeline timing을 모두 확인했는가?

## 금지 사항

- 전체 Unicode full sweep을 기본 UTC 경로에 무조건 넣지 않는다.
- emoji sequence UTC를 unrelated internal UTC file에 추가하지 않는다.
- 비표준 VS/ZWJ fallback을 Strict Unicode conformance로 설명하지 않는다.
- fixture를 CI에서 매번 latest로 다운로드하지 않는다.
- sample pack을 visual test script에서 자동 생성하거나 Unicode chart를 자동 다운로드하지 않는다.
- sample image를 pass/fail 기준으로 사용하지 않는다.
- 생성된 sample PNG pack을 repository에 커밋하지 않는다.
- emoji workaround를 추가하면서 일반 script boundary 테스트를 생략하지 않는다.
- 성능 측정용 임시 코드를 emoji sequence 개선 코드와 섞어 커밋하지 않는다.

## Coverage 한계

현재 테스트는 sequence boundary, fallback, font coverage, shaping smoke, missing glyph,
composition split 검출에 강하다.
하지만 다음은 완전 검증이 아니다.

- 모든 emoji의 픽셀 단위 시각적 정확성
- cursor 이동, selection, delete, grapheme boundary
- line break, bidi layout과 emoji sequence의 조합
- device별 font package 차이
- keycap 성능 병목

이 한계 때문에 visual viewer와 manual sweep을 함께 유지한다.
