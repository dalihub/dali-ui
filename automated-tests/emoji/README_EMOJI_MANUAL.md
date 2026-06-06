# Emoji Manual Test

`automated-tests/emoji`는 Unicode emoji sequence를 수동으로 검증하고 확인하기 위한
도구와 fixture를 모아둔 경로다. 기본 UTC는 작고 빠르게 유지하고, 전체 Unicode
fixture, font fallback, visual review, performance 측정이 필요할 때 이 도구들을
사용한다.

Agent용 유지보수 규칙은 [`EMOJI_TEST_SKILL.md`](EMOJI_TEST_SKILL.md)에 둔다.

## 빠른 사용법

`dali-ui` repository root에서 실행한다.

```bash
./automated-tests/emoji/build.sh
./automated-tests/emoji/sweep-test.sh
./automated-tests/emoji/visual-test.sh /tmp/dali-emoji-sequence-sweep-failures.log
./automated-tests/emoji/perf.sh
```

각 helper의 역할은 하나씩 분리되어 있다.

| Helper | 역할 |
| --- | --- |
| `build.sh` | emoji UTC target과 manual emoji tool을 빌드한다. |
| `sweep-test.sh` | emoji sequence의 script/font/shaping 상태를 검증하고 failure log를 만든다. |
| `visual-test.sh` | full/review/failure case를 viewer로 열거나 HTML evidence를 생성한다. |
| `perf.sh` | 실제 `Dali::Ui::Label` rendering 시간과 sweep pipeline timing을 측정한다. |

failure log를 HTML evidence로 만들려면 `-html`을 붙인다.

```bash
./automated-tests/emoji/visual-test.sh /tmp/dali-emoji-sequence-sweep-failures.log -html
./automated-tests/emoji/visual-test.sh /tmp/dali-emoji-sequence-sweep-failures.log -html ~/Downloads/emoji-failures.html
```

## 빌드

```bash
./automated-tests/emoji/build.sh
```

빌드되는 target은 다음과 같다.

```text
automated-tests/build/src/dali-ui-foundation-internal/tct-dali-ui-foundation-internal-core
automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-sweep
automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-visual
automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-label-perf
```

build directory나 job 수를 바꾸려면 다음 env를 사용한다.

```bash
DALI_EMOJI_BUILD_DIR=/path/to/build \
DALI_EMOJI_BUILD_JOBS=8 \
./automated-tests/emoji/build.sh
```

일부 local 환경에서는 실행 전에 DALi runtime path가 필요할 수 있다.

```bash
export LD_LIBRARY_PATH=/path/to/dali-env/opt/lib:$LD_LIBRARY_PATH
```

## Sweep Validation

기본 sweep은 Unicode emoji fixture 전체를 가장 강한 일반 모드로 검증한다.
검증 범위는 script boundary, font coverage, shaping map consistency, missing glyph,
그리고 multi-codepoint emoji composition이다.

```bash
./automated-tests/emoji/sweep-test.sh
```

`-a`는 같은 full sweep을 명시적으로 실행하는 옵션이다.

```bash
./automated-tests/emoji/sweep-test.sh -a
```

DALI compatibility fallback case만 확인하려면 `-c`를 사용한다.

```bash
./automated-tests/emoji/sweep-test.sh -c
```

`-c`는 full sweep의 failure log를 덮어쓰지 않도록
`/tmp/dali-emoji-compat-sweep-failures.log`를 사용한다.

대표 출력은 다음 형태다.

```text
EMOJI_SWEEP_SUMMARY mode=all totalRows=5225 executed=5225 passed=5151 failed=74 skipped=0 elapsedMs=1503.33
failureLogPath: /tmp/dali-emoji-sequence-sweep-failures.log
```

`failed`가 0이 아니면 `failureLogPath`를 visual viewer로 연다.

```bash
./automated-tests/emoji/visual-test.sh /tmp/dali-emoji-sequence-sweep-failures.log
```

## Visual Viewer

기본 viewer는 full fixture를 연다. 한 번에 전체 row actor를 만들지 않고 page 단위로
렌더링하며, page 이동 시 row actor를 재사용한다. 기본 page size는 100개다.

```bash
./automated-tests/emoji/visual-test.sh
```

대표 review case만 빠르게 보려면 `-r`을 사용한다.

```bash
./automated-tests/emoji/visual-test.sh -r
```

sweep failure만 열려면 failure log path를 전달한다.

```bash
./automated-tests/emoji/visual-test.sh /tmp/dali-emoji-sequence-sweep-failures.log
```

viewer를 열지 않고 HTML evidence만 만들려면 `-html`을 사용한다.

```bash
./automated-tests/emoji/visual-test.sh /tmp/dali-emoji-sequence-sweep-failures.log -html
```

`-html` 뒤에 path를 생략하면 `/tmp` 아래에 자동 생성된다. HTML evidence는 DALi
screenshot이 아니라 case 목록과 diagnostics를 공유하기 위한 파일이다.

포함되는 주요 diagnostics:

| Diagnostic | 의미 |
| --- | --- |
| `scriptRuns` | guarded text 전체의 script run 결과다. 앞뒤 Latin guard가 emoji run에 먹히지 않았는지 확인한다. |
| `fontRuns` | guarded text 전체의 font fallback 결과다. |
| `missingGlyphs` | emoji item range 안의 glyph index 0 여부를 보여준다. |
| `compositionGlyphs` | composition 실패 시에만 출력되며, visible glyph가 어떻게 쪼개졌는지 보여준다. |

`scriptRuns`와 `fontRuns`는 `A+item+B` guarded text 기준으로 출력한다.
`missingGlyphs`와 `compositionGlyphs`는 그중 emoji item range만 검사한다.

viewer에서 emoji preview 영역을 누르면 중앙 확대 view가 열린다. 확대 view는 TextFit을
사용하며, 다시 누르면 닫힌다.

## Label Performance

`perf.sh`는 실제 `Dali::Ui::Label` async rendering 경로를 측정한다. 기본적으로 sweep
pipeline도 profile 모드로 같이 실행하므로, 실제 Label rendering 시간과 text stack 단계별
비용을 한 report에서 볼 수 있다.

```bash
./automated-tests/emoji/perf.sh
```

결과는 기본적으로 `/tmp` 아래 timestamped directory에 저장된다.

```text
/tmp/dali-emoji-perf-20260607-130000/
  label.log
  pipeline.log
  pipeline-failures.log
  summary.txt
  report.html
```

report의 상단은 실제 `Label`이 `AsyncRenderFinishedSignal`을 모두 받을 때까지의 시간이다.
카드는 전체 label 수와 phase별 평균 시간, `ms/label`을 보여준다.

Label phase:

| Phase | 의미 |
| --- | --- |
| `initial_48` | initial font size에서 첫 text render를 측정한다. |
| `same_text_48` | 같은 font size에서 text만 변경하는 비용을 측정한다. |
| `scale_32` | 32px text update 비용을 측정한다. |
| `scale_64` | 64px text update 비용을 측정한다. |

`Pipeline Stack Time`은 sweep profile 결과를 사용한다. script validation, font validation,
shaping, glyph metrics, bitmap creation이 text stack에서 어느 정도 비중인지 확인하기 위한
자료다. 실제 사용자 체감 성능은 report 상단의 Label timing으로 판단해야 한다.

출력 directory parent를 바꾸려면 다음처럼 실행한다.

```bash
DALI_EMOJI_PERF_OUTPUT_DIR=~/Downloads/test ./automated-tests/emoji/perf.sh
```

빠른 smoke 측정이 필요하면 row 수와 반복 횟수를 줄인다.

```bash
DALI_EMOJI_PERF_MAX_ITEMS=16 \
DALI_EMOJI_PERF_RUNS=1 \
DALI_EMOJI_PERF_SWEEP_MAX_ROWS=16 \
./automated-tests/emoji/perf.sh
```

`perf.sh`는 local font를 변경하지 않는다. report에는 현재 `Noto Color Emoji`의
`fc-match` 결과가 기록되므로 COLR/CBDT나 patch 전후 결과를 나중에 비교할 수 있다.

## 결과 해석

### Missing Glyph

`missing glyph`는 shaping 결과에서 emoji item range 안에 glyph index `0`이 있다는 뜻이다.
보통 다음 중 하나다.

- 현재 fallback 순서에서 선택된 font에 해당 codepoint가 없다.
- sequence를 구성하는 base emoji 중 일부가 오래된 font에 없다.
- variation selector 또는 fallback policy에 regression이 있다.

오래된 bitmap emoji font로 최신 Unicode fixture를 돌리면 일부 failure가 정상일 수 있다.
예를 들어 Emoji 17.0 fixture에 포함된 codepoint가 설치된 Emoji 15.x font에는 없을 수 있다.
이 경우 visual diagnostics의 `missingGlyphs`를 확인하고, font cmap이나 fontconfig 상태와
대조한다.

### Composition Failure

`emoji sequence was not rendered as a single visible glyph`는 component glyph가 존재하더라도
sequence가 하나의 visible emoji glyph로 compose되지 않았다는 뜻이다. ZWJ, keycap,
modifier, regional indicator flag, tag sequence가 쪼개져 보이는 case를 잡기 위한 검사다.

composition check는 emoji item range 안의 visible glyph 수를 기준으로 판단한다.

- one visible glyph: pass
- zero visible glyphs: failure
- two or more visible glyphs: failure

일부 font는 single composed glyph를 glyph-to-character map에서 base codepoint만 커버하는
것처럼 보고한다. 따라서 char range가 전체 sequence를 덮는지까지 강제하지 않고,
visible glyph가 정확히 하나면 pass로 본다.

### Visual Diagnostics

`missingGlyphs: none`인데 `compositionGlyphs`에 `actual: split visible glyphs (...)`가
보이면, font에 component glyph는 있지만 shaping이 single emoji로 compose하지 못했다는 뜻이다.
`compositionGlyphs`는 composition failure에서만 출력된다. 두 번째 줄은 각 visible glyph가
어떤 guarded character range와 codepoint에서 나왔는지 보여준다.

`missingGlyphs: glyph#...`는 tofu/missing-glyph path가 감지되었다는 뜻이다. failure log를
visual viewer로 열면 이런 case는 `font missing glyph` section으로 분리된다.

## Fixture

fixture path:

```text
automated-tests/emoji/res/emoji-test-latest.tsv
```

fixture는 Unicode emoji data에서 생성해 repository에 고정한다. 일반 build, UTC, CI에서
매번 Unicode latest data를 다운로드하면 안 된다.

새 Unicode Emoji version을 채택하거나 fixture format/classification을 변경할 때만 갱신한다.

```bash
python3 automated-tests/scripts/update-unicode-emoji-fixtures.py
```

갱신 후에는 TSV metadata의 Unicode version, source URL, SHA-256, row count, signature
count를 확인한다.

## 고급 사용법

helper script는 일반 사용 경로만 최소한으로 노출한다. 특정 signature만 좁혀 보거나
row 수를 제한하거나 profile timing을 보고 싶으면 binary를 직접 env와 함께 실행한다.

### Sweep Env

```bash
DALI_EMOJI_SWEEP_MODE=all \
DALI_EMOJI_SWEEP_SIGNATURE_FILTER=keycap \
DALI_EMOJI_SWEEP_FAILURE_LOG=/tmp/dali-emoji-keycap-failures.log \
./automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-sweep
```

| Env | Default | 설명 |
| --- | --- | --- |
| `DALI_EMOJI_SWEEP_MODE` | `all` | `script`, `font`, `shape`, `all`, `compat` |
| `DALI_EMOJI_SWEEP_SIGNATURE_FILTER` | empty | comma-separated signature allow-list |
| `DALI_EMOJI_SWEEP_FAILURE_LOG` | `/tmp/dali-emoji-sequence-sweep-failures.log` | failure log path |
| `DALI_EMOJI_SWEEP_MAX_FAILURES` | `50` | stdout에 출력할 failure preview 개수 |
| `DALI_EMOJI_SWEEP_MAX_ROWS` | `0` | 실행 row 제한. `0`은 제한 없음 |
| `DALI_EMOJI_SWEEP_FIXTURE` | auto-detect | fixture TSV path override |
| `DALI_EMOJI_SWEEP_PROFILE` | unset | `1`이면 pipeline timing과 bitmap counter를 출력 |

### Visual Env

```bash
DALI_EMOJI_VISUAL_INPUT=/tmp/dali-emoji-keycap-failures.log \
DALI_EMOJI_VISUAL_EXPORT_HTML=/tmp/dali-emoji-keycap.html \
DALI_EMOJI_VISUAL_EXPORT_ONLY=1 \
./automated-tests/build/src/dali-ui-foundation-internal/dali-emoji-sequence-visual
```

| Env | Default | 설명 |
| --- | --- | --- |
| `DALI_EMOJI_VISUAL_MODE` | `review` | `review`, `representative`, `full` |
| `DALI_EMOJI_VISUAL_INPUT` | unset | sweep failure log path |
| `DALI_EMOJI_VISUAL_EXPORT_HTML` | unset | HTML evidence output path |
| `DALI_EMOJI_VISUAL_EXPORT_ONLY` | unset | `1`이면 GUI 없이 HTML만 생성 |
| `DALI_EMOJI_VISUAL_SIGNATURE_FILTER` | empty | debug용 signature/section filter |
| `DALI_EMOJI_VISUAL_MAX_ITEMS` | `0` | debug용 row 제한 |
| `DALI_EMOJI_VISUAL_PAGE_SIZE` | `100` | debug용 page row 수 |
| `DALI_EMOJI_VISUAL_FONT_SIZE` | `52` | preview emoji font size |
| `DALI_EMOJI_VISUAL_CAPTURE` | unset | visible viewer capture path |
| `DALI_EMOJI_VISUAL_EXIT_AFTER_CAPTURE` | unset | capture 후 종료 |

### Perf Env

```bash
DALI_EMOJI_PERF_OUTPUT_DIR=~/Downloads/test \
DALI_EMOJI_PERF_RUNS=5 \
DALI_EMOJI_PERF_MAX_ITEMS=5225 \
./automated-tests/emoji/perf.sh
```

| Env | Default | 설명 |
| --- | --- | --- |
| `DALI_EMOJI_PERF_OUTPUT_DIR` | `/tmp` | timestamped result directory의 parent path |
| `DALI_EMOJI_PERF_RUNS` | `5` | Label phase 반복 횟수 |
| `DALI_EMOJI_PERF_MAX_ITEMS` | `0` | Label row 제한. `0`은 full fixture |
| `DALI_EMOJI_PERF_FONT_FAMILY` | `SamsungOneUI_400` | test `Label`에 설정할 font family |
| `DALI_EMOJI_PERF_FONT_SIZE` | `48` | initial font size |
| `DALI_EMOJI_PERF_TIMEOUT_MS` | `60000` | phase별 timeout |
| `DALI_EMOJI_PERF_FIXTURE` | auto-detect | fixture TSV path override |
| `DALI_EMOJI_PERF_PIPELINE` | `1` | `0`이면 sweep profile 수집 생략 |
| `DALI_EMOJI_PERF_SWEEP_MODE` | `all` | pipeline profile에 사용할 sweep mode |
| `DALI_EMOJI_PERF_SWEEP_MAX_ROWS` | `0` | pipeline profile용 sweep row 제한 |
| `DALI_EMOJI_PERF_SWEEP_MAX_FAILURES` | `0` | perf 중 sweep failure preview 출력 개수 |

### Mode 의미

| Mode | 검사 또는 선택 case |
| --- | --- |
| `script` | script run boundary, Latin guard, emoji run classification |
| `font` | `script` + font fallback coverage |
| `shape` | `font` + shaping smoke, glyph map consistency, missing glyph, composition check |
| `all` | 가장 강한 일반 sweep. 현재는 `shape`와 동일 |
| `compat` | non-standard DALI VS/ZWJ/keycap compatibility fallback case |
| `review` | 대표 Unicode signature + DALI compatibility/keycap/symbol/mixed evidence |
| `representative` | Unicode fixture signature별 첫 번째 row |
| `full` | Unicode fixture 전체 row |

## 현재 한계

- 이 도구들은 manual debug path이며 default PR CI에서 실행하지 않는다.
- visual viewer는 image diff test가 아니며 visual quality를 자동 판정하지 않는다.
- font fallback 결과는 설치된 font와 system fontconfig 순서에 따라 달라질 수 있다.
- pipeline timing은 text stack 병목 위치를 찾는 데 유용하지만, 실제 사용자 체감 성능은
  perf report의 Label timing으로 판단해야 한다.
- cursor movement, selection, delete behavior, grapheme boundary, line break, bidi layout은
  완전히 검증하지 않는다.
