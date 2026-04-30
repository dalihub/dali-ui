# TextVisualizer Experimental Sample

## 목적

`samples/text/text-visualizer-experimental-example.cpp`는 기존 `samples/text/text-experimental-example.cpp`와 같은 ball / trail / text grid 시뮬레이션을 `TextVisualizer`로 확인하기 위한 비교 샘플이다.

기존 샘플은 `Label` tile 여러 개를 사용해 앱 레벨 partial rendering을 구성한다. 새 샘플은 같은 시뮬레이션 로직을 유지하면서 text surface를 `TextVisualizer`로 렌더링한다.

## Render Mode

기본 mode는 `SINGLE_TEXT_VISUALIZER`이다.

| mode | 설명 |
|---|---|
| `SINGLE_TEXT_VISUALIZER` | 하나의 `TextVisualizer`가 전체 grid text를 렌더링한다. 매 frame 전체 text string을 만들고 hash가 바뀐 경우에만 `SetText()`를 호출한다. |
| `PARTIAL_TEXT_VISUALIZER_TILES` | 기존 Label tile 샘플처럼 grid를 tile로 나눈다. dirty tile만 text를 rebuild하고 해당 tile `TextVisualizer`에 `SetText()`를 호출한다. |

`5` key로 두 mode를 전환한다.

## Key Bindings

유지한 key:

- `1`: random ball 추가
- `4`: noise quality toggle
- `5`: render mode toggle
- `7`: star twinkle toggle
- `9`: dirty tile debug toggle
- `0`: grid toggle
- ESC / BACK: quit

TextVisualizer에서 아직 직접 지원하지 않는 Label-specific 기능은 no-op 또는 unsupported log로 둔다.

- async rendering toggle
- markup color toggle
- DALI `Label::Property::TEXT_COLOR` animation
- overflow / rich style 계열 설정

## 구현 메모

- 기존 `text-experimental-example.cpp`는 수정하지 않는다.
- 새 샘플은 `TextVisualizerExperimentalController`를 사용한다.
- tile 구조는 `Label label` 대신 `TextVisualizer textVisualizer`를 가진다.
- title은 단순 UI label로 남겨두고, simulation text와 info text는 `TextVisualizer`를 사용한다.
- monospace 느낌을 위해 `"Ubuntu Mono"` font family, `16.0f` font size, `1.0f` line height를 사용한다.

## 확인 필요

- single mode의 full string update 비용과 partial tile mode의 dirty tile update 비용 비교
- TextVisualizer word wrap이 fixed-cell grid rendering과 얼마나 잘 맞는지 확인
- 장기적으로 fixed-cell / no-wrap style option이 필요한지 검토
