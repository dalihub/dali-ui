# TextVisualizer 구현 계획

## 목적

이 문서의 목적은 앞선 분석 문서를 바탕으로 `Dali::Ui::TextVisualizer` 실제 구현 순서를 작은 커밋 단위로 나누고, 각 단계의 검증 방법과 중단/롤백 지점을 명확히 정의하는 것이다.

이번 문서는 구현 시작 전 기준 문서로 사용한다.

핵심 목표:

- 작은 커밋 단위로 나누어 위험을 낮춘다
- 각 커밋이 독립적으로 빌드/검증 가능해야 한다
- `Prepare -> Layout -> AtlasRenderer Adapter` 흐름을 순차적으로 완성한다
- 1차 구현 범위를 넘는 기능은 의도적으로 제외한다

애매한 내용은 추측하지 않고 `확인 필요`로 표시한다.

## 전제

- `TextVisualizer`는 `View` 계열 public handle이다
- `InputField`의 public/internal 구조를 참고하되 editing stack은 제외한다
- `Prepare`와 `Layout`은 분리한다
- 렌더링은 `AtlasRenderer`를 사용한다
- 1차 구현에서는 `style`, `ellipsis`, `selection`, `cursor`, `editing`을 제외한다
- exclusion region 대응 multi-line layout이 핵심 목표다

## 추가할 파일 목록

아래 파일은 1차 구현에서 새로 추가하는 것을 권장한다.

### public API

- `dali-ui-foundation/public-api/text-visualizer.h`
- `dali-ui-foundation/public-api/text-visualizer.cpp`
- `dali-ui-foundation/public-api/text/text-visualizer-properties.h`

### integration API

- `dali-ui-foundation/integration-api/text-visualizer-impl.h`
- `dali-ui-foundation/integration-api/text-visualizer-impl.cpp`
- `dali-ui-foundation/integration-api/text-visualizer-property-handler.h`
- `dali-ui-foundation/integration-api/text-visualizer-property-handler.cpp`

### internal text visualizer core

- `dali-ui-foundation/internal/text/text-visualizer/prepared-text.h`
- `dali-ui-foundation/internal/text/text-visualizer/prepared-text.cpp`
- `dali-ui-foundation/internal/text/text-visualizer/text-preparer.h`
- `dali-ui-foundation/internal/text/text-visualizer/text-preparer.cpp`
- `dali-ui-foundation/internal/text/text-visualizer/layout-types.h`
- `dali-ui-foundation/internal/text/text-visualizer/layout-engine.h`
- `dali-ui-foundation/internal/text/text-visualizer/layout-engine.cpp`
- `dali-ui-foundation/internal/text/text-visualizer/atlas-view-adapter.h`
- `dali-ui-foundation/internal/text/text-visualizer/atlas-view-adapter.cpp`

### automated tests

- `automated-tests/src/dali-ui-foundation/utc-Dali-TextVisualizer.cpp`

### sample

- `samples/text/text-visualizer-example.cpp`

### 자동 생성 또는 생성 결과 확인 필요 파일

- `dali-ui-foundation/public-api/text-visualizer.autogen.h`

확인 필요:

- chaining autogen이 새 public class에 대해 자동 생성되는지
- 생성 결과를 저장소에 포함할지 현재 팀 관례 확인 필요

## 수정할 파일 목록

아래 파일은 1차 구현에서 수정 가능성이 높다.

### public umbrella / include

- `dali-ui-foundation/dali-ui-foundation.h`

### tests build

- `automated-tests/src/dali-ui-foundation/CMakeLists.txt`

### sample build

- `samples/text/CMakeLists.txt`

### 필요 시 수정 가능성 있는 공용 유틸

- `dali-ui-foundation/internal/controls/text-controls/common-text-utils.h`
- `dali-ui-foundation/internal/controls/text-controls/common-text-utils.cpp`

이 두 파일은 가능하면 수정 범위를 최소화한다.

### 수정하지 않아도 될 가능성이 높은 파일

- `build/tizen/dali-ui-foundation/CMakeLists.txt`

이유:

- 현재 라이브러리 빌드는 `FILE(GLOB_RECURSE ALL_SOURCES "*.cpp")` 방식이라 새 `*.cpp`는 자동 수집될 가능성이 높다

## 구현 순서

권장 구현 순서는 아래와 같다.

1. public/internal skeleton 추가
2. property/state storage와 dirty flag 추가
3. `Prepare` 모델과 preparer 추가
4. layout data type 및 interval builder 추가
5. layout engine을 impl lifecycle에 연결
6. atlas view adapter와 renderer host 연결
7. exclusion region을 실제 placement에 반영
8. automated test 확장
9. sample app 추가

이 순서는 "보이는 기능"보다 "구조 안정화"를 먼저 만든다.

## 각 커밋 단위

아래 커밋 분해를 권장한다.

### Commit 1. Public / Impl Skeleton

목표:

- `TextVisualizer` public handle과 internal impl 골격 추가
- `New()`, `DownCast()`, 기본 lifecycle 연결

추가/수정 파일:

- `public-api/text-visualizer.h`
- `public-api/text-visualizer.cpp`
- `integration-api/text-visualizer-impl.h`
- `integration-api/text-visualizer-impl.cpp`
- `dali-ui-foundation/dali-ui-foundation.h`

검증:

- 라이브러리 빌드 성공
- 빈 `TextVisualizer::New()`가 생성 가능
- scene attach 시 crash 없음

중단/롤백 가능 지점:

- 가능
- 이 시점은 API skeleton만 추가된 상태라 되돌리기 가장 쉽다

### Commit 2. Property / State / Basic UTC

목표:

- `TEXT`, `FONT_FAMILY`, `FONT_SIZE`, `TEXT_COLOR` property 연결
- setter/getter와 property handler 연결

추가/수정 파일:

- `public-api/text/text-visualizer-properties.h`
- `integration-api/text-visualizer-property-handler.h`
- `integration-api/text-visualizer-property-handler.cpp`
- `automated-tests/src/dali-ui-foundation/utc-Dali-TextVisualizer.cpp`
- `automated-tests/src/dali-ui-foundation/CMakeLists.txt`

검증:

- constructor / `New()` / `DownCast()` UTC
- setter/getter UTC
- `SetProperty()` / `GetProperty()` UTC

중단/롤백 가능 지점:

- 가능
- API surface가 확정되지만 구현 의존성은 아직 낮다

### Commit 3. Dirty Flags + Prepare API Stub

목표:

- `Prepare()` public command 추가
- impl에 `prepare dirty` / `layout dirty` / `render dirty` 분리
- exclusion region storage 추가

추가/수정 파일:

- `integration-api/text-visualizer-impl.h`
- `integration-api/text-visualizer-impl.cpp`
- `utc-Dali-TextVisualizer.cpp`

검증:

- `Prepare()` 호출이 crash 없이 동작
- `SetText()` / `SetFontFamily()` / `SetFontSize()` 후 `prepare dirty`가 설정됨
- `SetExclusionRegions()` / `ClearExclusionRegions()` API smoke test

중단/롤백 가능 지점:

- 가능
- 아직 text prepare/layout 실체는 없으므로 되돌리기 쉽다

### Commit 4. PreparedText / TextPreparer 추가

목표:

- `PreparedText` 구조 추가
- shaping / fallback / glyph metrics / cluster mapping 경로 연결

추가/수정 파일:

- `internal/text/text-visualizer/prepared-text.h`
- `internal/text/text-visualizer/prepared-text.cpp`
- `internal/text/text-visualizer/text-preparer.h`
- `internal/text/text-visualizer/text-preparer.cpp`
- `integration-api/text-visualizer-impl.cpp`

검증:

- `Prepare()` 후 prepared cache 생성 확인
- empty text / ASCII / 한글 / emoji 문자열 smoke test
- 동일 text/font/fontSize에서 `Prepare()` 재호출 시 결과 안정성 확인

확인 필요:

- automated test에서 prepare 실행 여부를 어떤 방식으로 관측할지
- 내부 debug counter 또는 test seam이 필요한지 검토 필요

중단/롤백 가능 지점:

- 가능
- 이 시점은 renderer 미연결 상태여도 준비 데이터만 검증 가능하다

### Commit 5. Layout Types + Interval Builder

목표:

- `AvailableInterval`, `TextLineFragment`, `TextLine`, `GlyphPlacement`, `LayoutResult` 구조 추가
- exclusion region에서 line interval을 만드는 순수 알고리즘 추가

추가/수정 파일:

- `internal/text/text-visualizer/layout-types.h`
- `internal/text/text-visualizer/layout-engine.h`
- `internal/text/text-visualizer/layout-engine.cpp`
- `utc-Dali-TextVisualizer.cpp`

검증:

- exclusion region 없음 -> content bounds 하나의 interval 생성
- exclusion region 하나 -> 2개 interval 생성
- 여러 region 겹침 -> merge 후 interval 생성
- line band overlap 규칙 테스트

중단/롤백 가능 지점:

- 가능
- prepare와 layout 데이터 구조가 분리되어 있어 되돌리기 쉽다

### Commit 6. Multi-line Layout Integration

목표:

- `PreparedText`를 소비하여 cluster 기준 multi-line layout 수행
- `OnMeasure()` / `GetNaturalSize()` / `GetHeightForWidth()`와 연결

추가/수정 파일:

- `layout-engine.cpp`
- `text-visualizer-impl.cpp`
- `utc-Dali-TextVisualizer.cpp`

검증:

- width 변경 시 line 수 변경
- exclusion region 반영 전 기본 multiline placement 정상
- `WRAP_CONTENT` 측정값이 text 길이에 따라 달라짐
- `Prepare` 재실행 없이 `Layout`만 반복 수행 가능 여부 확인

중단/롤백 가능 지점:

- 가능
- 아직 renderer 미연결 상태여도 size/layout 검증이 가능하다

### Commit 7. Atlas View Adapter + Renderer Host

목표:

- `LayoutResult`를 `AtlasRenderer`가 읽을 수 있는 adapter로 연결
- plain glyph rendering 표시

추가/수정 파일:

- `internal/text/text-visualizer/atlas-view-adapter.h`
- `internal/text/text-visualizer/atlas-view-adapter.cpp`
- `integration-api/text-visualizer-impl.cpp`

검증:

- scene에 텍스트가 실제로 보이는지 manual 확인
- multiline plain text 표시 확인
- whitespace only text에서도 crash 없음
- empty text에서도 render actor handling 정상

중단/롤백 가능 지점:

- 중요 지점
- 여기까지 오면 최소 기능 텍스트 표시 view로 사용 가능

### Commit 8. Exclusion-aware Placement Integration

목표:

- multi-fragment line placement를 실제 렌더링과 연결
- exclusion region 변경 시 layout-only invalidation 동작

추가/수정 파일:

- `layout-engine.cpp`
- `text-visualizer-impl.cpp`
- `atlas-view-adapter.cpp`
- `utc-Dali-TextVisualizer.cpp`

검증:

- 한 줄이 여러 fragment로 나뉘는 케이스
- exclusion region 추가/삭제/이동 후 relayout 결과 변경
- bounds만 변경할 때 `Prepare` 재실행이 발생하지 않음

중단/롤백 가능 지점:

- 가능
- exclusion 관련 회귀가 크면 Commit 7 상태까지 되돌려도 기본 text view는 유지된다

### Commit 9. Sample App

목표:

- `TextVisualizer` 동작을 사람이 직접 확인할 sample 추가

추가/수정 파일:

- `samples/text/text-visualizer-example.cpp`
- `samples/text/CMakeLists.txt`

검증:

- sample 빌드 성공
- 창 크기 변경 시 multiline relayout 확인
- exclusion region on/off 또는 이동 시 배치 변화 확인

중단/롤백 가능 지점:

- 가능
- sample은 기능 이해와 디버깅용이므로 library 구현과 분리 가능

### Commit 10. UTC 확장 / 안정화

목표:

- automated test를 1차 구현 범위 기준으로 보강

추가/수정 파일:

- `automated-tests/src/dali-ui-foundation/utc-Dali-TextVisualizer.cpp`

검증:

- constructor/new/downcast
- property
- `Prepare()`
- `SetExclusionRegions()`
- multiline layout
- layout-only invalidation
- empty/whitespace/emoji smoke test

중단/롤백 가능 지점:

- 가능
- tests-only commit이므로 기능 rollback과 분리 가능

## 각 커밋의 검증 방법

아래 검증 규칙을 각 커밋마다 공통으로 적용한다.

### 공통 검증

1. `git diff` 확인
2. 의도한 파일만 수정되었는지 확인
3. 라이브러리 빌드 또는 최소 compile 확인
4. 가능하면 관련 UTC 실행

### 권장 명령 예시

환경에 따라 달라질 수 있으므로 아래는 예시다.

```bash
cmake --build build/tizen -j
cmake --build automated-tests/src/dali-ui-foundation -j
ctest --test-dir automated-tests/src/dali-ui-foundation --output-on-failure
cmake --build samples/text -j
```

확인 필요:

- 현재 개발 환경에서 실제 표준 빌드 명령이 무엇인지 구현 착수 시점에 맞춰 재확인 필요

## automated test 계획

권장 test 파일:

- `automated-tests/src/dali-ui-foundation/utc-Dali-TextVisualizer.cpp`

권장 test 항목:

1. `UtcDaliTextVisualizerConstructorP`
2. `UtcDaliTextVisualizerNewP`
3. `UtcDaliTextVisualizerDownCastP`
4. `UtcDaliTextVisualizerText`
5. `UtcDaliTextVisualizerFontFamily`
6. `UtcDaliTextVisualizerFontSize`
7. `UtcDaliTextVisualizerTextColor`
8. `UtcDaliTextVisualizerSetProperty`
9. `UtcDaliTextVisualizerGetProperty`
10. `UtcDaliTextVisualizerPrepare`
11. `UtcDaliTextVisualizerExclusionRegions`
12. `UtcDaliTextVisualizerMeasureWrapContent`
13. `UtcDaliTextVisualizerRelayoutWidthChange`
14. `UtcDaliTextVisualizerEmptyText`
15. `UtcDaliTextVisualizerEmojiText`

### test seam 필요 가능성

아래 항목은 public API만으로 검증하기 어려울 수 있다.

- `Prepare`가 재실행되었는지
- width 변경 시 prepare 없이 layout만 다시 수행되었는지

대응 후보:

- impl 내부 debug counter
- test-only accessor
- log hook

이 부분은 `확인 필요`다.

## sample app 계획

권장 sample:

- `samples/text/text-visualizer-example.cpp`

sample 목표:

1. 기본 multiline text 표시
2. exclusion region이 있을 때 줄이 fragment로 나뉘는 모습 표시
3. window resize 시 relayout 반응 확인
4. same text / moved exclusion에서 shaping 재실행 없이 layout만 바뀌는지 관찰

권장 sample 구성:

- 왼쪽: exclusion 없음
- 오른쪽: exclusion 하나 또는 둘
- 키 입력 또는 timer로 exclusion region 이동
- 같은 text를 유지한 채 relayout 차이를 눈으로 확인

sample 검증 포인트:

- glyph overlap 없음
- exclusion bounds 내부 비워짐
- 크기 변경 시 즉시 재배치
- crash / flicker 없음

## 위험 요소

### 위험 1. prepare 단계가 기존 controller와 너무 강하게 얽혀 있을 수 있음

영향:

- shaping/fallback 재사용 비용이 예상보다 커질 수 있다

대응:

- prepare 관련 로직을 early commit에서 먼저 고립시켜 본다
- 필요 시 1차는 최소 경로만 연결하고 나머지는 `확인 필요`로 남긴다

### 위험 2. cluster 기준 line breaking이 예상보다 복잡할 수 있음

영향:

- exclusion fragment 경계에서 줄바꿈 품질 문제가 생길 수 있다

대응:

- 1차는 penalty 최적화보다 안정성 우선
- break candidate와 강제 배치 정책을 문서 기준으로 단순화한다

### 위험 3. AtlasRenderer adapter 범위가 넓을 수 있음

영향:

- `ViewInterface` dummy 구현이 많아질 수 있다

대응:

- 1차는 minimal path만 유효 값 반환
- 나머지는 `zero / false / nullptr` 반환

### 위험 4. automated test에서 prepare/layout 재실행 횟수 관측이 어려울 수 있음

영향:

- 핵심 최적화 요구사항 검증이 약해질 수 있다

대응:

- test seam을 최소 범위로 도입하는 방안 검토

### 위험 5. autogen/property registration 흐름이 새 control에서 예상과 다를 수 있음

영향:

- build break 또는 generated header mismatch 발생 가능

대응:

- skeleton commit에서 가장 먼저 확인

## 구현 중 중단 / 롤백 가능한 지점

아래 지점들은 구현 중 pause 또는 rollback 기준점으로 적절하다.

### 지점 A. Commit 2 완료 후

상태:

- public API와 property skeleton만 존재
- build 가능

의미:

- API surface를 먼저 리뷰받기 좋은 시점

### 지점 B. Commit 4 완료 후

상태:

- prepare cache는 동작
- renderer 미연결 가능

의미:

- shaping/fallback reuse 가능성만 먼저 검증 가능

### 지점 C. Commit 6 완료 후

상태:

- layout은 동작
- basic measure / relayout 가능
- renderer는 아직 단순 또는 미연결일 수 있음

의미:

- engine 품질을 먼저 점검 가능

### 지점 D. Commit 7 완료 후

상태:

- 텍스트가 실제로 보임
- exclusion 없는 기본 TextVisualizer 사용 가능

의미:

- 최소 usable milestone

### 지점 E. Commit 8 완료 후

상태:

- 핵심 feature인 exclusion-aware multi-fragment layout 완성

의미:

- 1차 핵심 목표 달성 지점

## 이번 1차 구현에서 제외할 기능

아래 기능은 1차 구현 범위에서 제외한다.

- selection
- cursor
- decorator
- IME / editing
- clipboard
- placeholder interaction
- underline
- shadow
- strikethrough
- outline
- background
- ellipsis
- per-character rich style
- hit-test / selection geometry
- non-rectangular exclusion
- async render
- public `Layout()` / `Relayout()`

## 구현 완료 정의

이번 계획서 기준의 1차 구현 완료는 아래를 의미한다.

1. `TextVisualizer`를 public handle로 생성할 수 있다.
2. `Text`, `FontFamily`, `FontSize`, `TextColor`를 설정할 수 있다.
3. `Prepare()`를 호출할 수 있고, 내부적으로 eager prepare가 가능하다.
4. exclusion region을 설정할 수 있다.
5. `OnRelayout()`에서 shaping 없이 layout만 반복 수행할 수 있다.
6. atlas renderer를 통해 plain glyph text를 실제로 표시할 수 있다.
7. exclusion region으로 인해 한 줄이 여러 fragment로 나뉘는 배치가 동작한다.

## 확인 필요 사항

- generated file(`text-visualizer.autogen.h`)를 저장소에 포함할지 팀 규칙 확인 필요
- prepare/layout 재실행 횟수 검증용 test seam이 필요한지 확정 필요
- `TextVisualizer` property header를 별도 `public-api/text/` 아래 두는 방식 최종 확정 필요

## TextVisualizer 설계에 주는 결론

1. 구현은 한 번에 크게 들어가기보다 `API skeleton -> Prepare -> Layout -> Atlas adapter -> Exclusion integration` 순으로 잘게 쪼개는 것이 맞다.
2. 가장 먼저 안정화해야 할 것은 public API가 아니라 `Prepare`와 `Layout`의 경계다.
3. 1차 usable milestone은 "plain multiline text가 atlas renderer로 보이는 상태"이고, 핵심 목표 달성 milestone은 "exclusion-aware multi-fragment layout이 동작하는 상태"다.
4. rollback 기준점을 커밋 단위로 명확히 두면 구현 중 구조 변경이 생겨도 안전하게 되돌릴 수 있다.
