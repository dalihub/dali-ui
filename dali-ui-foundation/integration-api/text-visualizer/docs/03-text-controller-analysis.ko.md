# 기존 text-controller 분석

## 목적

이 문서의 목적은 기존 `Text::Controller`가 수행하는 작업을 `Prepare / Layout / Render` 관점으로 분해하고, `TextVisualizer`에서 재사용할 최소 기능과 제거해야 할 기능을 구분하는 것이다.

이번 문서에서는 다음을 중점적으로 본다.

- `SetText()` 이후 내부 상태가 어떻게 바뀌는가
- font 관련 설정이 들어오면 어떤 단계가 다시 수행되는가
- shaping, font fallback, layout, visual model 생성이 정확히 언제 일어나는가
- renderer로 전달되는 데이터가 어디서 만들어지는가
- dirty / invalidation 구조가 어떻게 생겼는가
- `InputField` / 편집 기능 때문에 어떤 오버헤드가 생기는가
- `TextVisualizer`용 경량 controller가 왜 필요한가

애매한 내용은 추측하지 않고 `확인 필요`로 표시한다.

## 분석 대상 파일

### 직접 분석한 파일

- `dali-ui-foundation/internal/text/controller/text-controller.h`
- `dali-ui-foundation/internal/text/controller/text-controller.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-impl.h`
- `dali-ui-foundation/internal/text/controller/text-controller-impl.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-text-updater.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-input-font-handler.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-relayouter.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp`
- `dali-ui-foundation/internal/text/text-model.h`
- `dali-ui-foundation/internal/text/text-model.cpp`
- `dali-ui-foundation/internal/text/logical-model-impl.h`
- `dali-ui-foundation/internal/text/visual-model-impl.h`
- `dali-ui-foundation/internal/text/layouts/layout-engine.h`
- `dali-ui-foundation/internal/text/layouts/layout-engine.cpp`
- `dali-ui-foundation/internal/text/text-view.h`
- `dali-ui-foundation/internal/text/text-view-interface.h`

### 필요한 경우 함께 참고한 파일

- `dali-ui-foundation/internal/text/shaper.cpp`
- `dali-ui-foundation/internal/text/segmentation.cpp`
- `dali-ui-foundation/internal/text/multi-language-support.cpp`
- `dali-ui-foundation/internal/text/glyph-metrics-helper.cpp`

## 현재 구조 요약

현재 `Text::Controller`는 단순한 controller가 아니라, 사실상 아래를 묶은 텍스트 runtime이다.

- text property 저장
- text 변경 추적
- logical model 갱신
- visual model 갱신
- shaping / font validation / bidi / line break / glyph metrics
- layout / align / reorder
- natural size / height-for-width 계산
- placeholder / hidden text 처리
- scroll state 처리
- 입력기 상태 처리
- selection / cursor / decorator / popup 처리

즉, `TextVisualizer` 입장에서 보면 유용한 텍스트 엔진과 불필요한 입력기 오버헤드가 한 객체 안에 같이 들어 있다.

## SetText 이후 내부 상태 변화

### 관련 함수

- `Text::Controller::SetText()` in `internal/text/controller/text-controller.cpp`
- `Controller::TextUpdater::SetText()` in `internal/text/controller/text-controller-text-updater.cpp`

### 실제 흐름

`SetText()` 호출 후 핵심 흐름은 다음과 같다.

1. `TextUpdater::SetText()` 진입
2. `impl.ResetInputMethodContext()`
3. `ResetText(controller)`로 기존 text 제거
4. `impl.ClearStyleData()`로 style run 제거
5. `impl.mRawText = text`
6. markup enabled면 `ProcessMarkupString(...)`
7. UTF-8 -> UTF-32 변환
8. `logicalModel->mText` 갱신
9. `mTextUpdateInfo.mNumberOfCharactersToAdd` 설정
10. `impl.QueueModifyEvent(ModifyEvent::TEXT_REPLACED)`
11. `impl.mRecalculateNaturalSize = true`
12. `impl.mRecalculateLayoutSize = true`
13. `impl.mUpdateTextDirection = true`
14. `impl.mOperationsPending = ALL_OPERATIONS`
15. `impl.RequestRelayout()`

### 해석

#### TextVisualizer에 필요한 부분

- raw text 저장
- markup 적용 여부에 따라 text preprocessing
- UTF-32 변환
- text update 범위(`mTextUpdateInfo`) 설정
- `ALL_OPERATIONS`를 통해 prepare + layout 전체 재실행 예약

#### 버릴 부분

- `ResetInputMethodContext()`
- `EventData` 상태 전환
- cursor position reset
- selection 상태 정리
- editable callback

## font 관련 설정이 들어왔을 때 처리 흐름

### 관련 함수

- `Controller::SetDefaultFontFamily()` in `text-controller.cpp`
- `Controller::SetDefaultFontSize()` in `text-controller.cpp`
- `Controller::SetDefaultFontWeight()` in `text-controller.cpp`
- `Controller::SetDefaultFontWidth()` in `text-controller.cpp`
- `Controller::SetDefaultFontSlant()` in `text-controller.cpp`
- `Controller::Impl::ClearFontData()` in `text-controller-impl.cpp`

### 실제 흐름

font 기본값이 바뀌면 대부분 아래 흐름을 따른다.

1. default font property 저장
2. `mImpl->ClearFontData()`
3. `RequestRelayout()`

`ClearFontData()`는 아래를 수행한다.

- cached font id 초기화
- `mTextUpdateInfo`를 전체 텍스트 기준으로 재설정
- `mRecalculateNaturalSize = true`
- `mRecalculateLayoutSize = true`
- `mOperationsPending |= VALIDATE_FONTS | SHAPE_TEXT | BIDI_INFO | GET_GLYPH_METRICS | LAYOUT | UPDATE_LAYOUT_SIZE | REORDER | ALIGN`

### 해석

#### TextVisualizer에 필요한 부분

- font 변경 시 shaping과 glyph metrics를 다시 수행해야 한다는 현재 dirty 규칙
- natural size / layout size cache 무효화

#### 버릴 부분

- editable state일 때 decorator/cursor 위치 갱신

## shaping이 언제 발생하는지

### 관련 함수

- `ControllerImplModelUpdater::Update()` in `text-controller-impl-model-updater.cpp`

### 실제 발생 시점

shaping은 `UpdateModel()` 내부에서 `operations`에 `SHAPE_TEXT`가 포함될 때 수행된다.

구체적으로는 아래 코드 경로다.

- `if(Controller::NO_OPERATION != (Controller::SHAPE_TEXT & operations))`
- `ShapeText(...)`

주요 입력은 다음이다.

- UTF-32 text
- line break info
- scripts
- validated fonts
- `startIndex`
- `requestedNumberOfCharacters`

즉, shaping은 property setter가 직접 하는 것이 아니라,
dirty가 쌓인 뒤 `Relayout()` 또는 size 계산 경로에서 `UpdateModel()`이 실행될 때 발생한다.

### TextVisualizer 해석

#### 직접 필요한 내용

- shaping은 `Prepare` 단계에 해당한다
- width/height만 바뀌는 relayout에서는 shaping이 다시 일어나지 않도록 dirty를 분리해야 한다

## font fallback이 언제 발생하는지

### 관련 함수

- `ControllerImplModelUpdater::Update()`
- `MultilanguageSupport::ValidateFonts(...)`

### 실제 발생 시점

font fallback은 `VALIDATE_FONTS` 단계에서 수행된다.

코드상 흐름:

1. default font description 계산
2. font variation map 확인
3. `multilanguageSupport.ValidateFonts(...)`
4. validated font run이 logical model에 기록됨

즉, fallback은 shaping보다 먼저 수행된다.

### TextVisualizer 해석

#### 직접 필요한 내용

- font fallback은 `Prepare` 단계에 포함되어야 한다
- text / font / fontSize가 바뀌지 않는 한 다시 하지 않는 구조가 필요하다

## layout이 언제 발생하는지

### 관련 함수

- `Text::Controller::Relayout()` in `text-controller.cpp`
- `Controller::Relayouter::Relayout()` in `text-controller-relayouter.cpp`
- `Controller::Relayouter::DoRelayout()` in `text-controller-relayouter.cpp`
- `Layout::Engine::LayoutText()` in `internal/text/layouts/layout-engine.h`

### 실제 발생 시점

layout은 `Relayout()` 경로에서 `operationsPending`에 `LAYOUT`이 포함되어 있을 때 수행된다.

대표 상황:

- text 변경 후
- font 관련 변경 후
- control size 변경 후
- elide 상태 변경 후
- layout direction 변경 후

`Relayouter::Relayout()` 안에서 새 size가 들어오면 자동으로 아래 dirty가 추가된다.

- `LAYOUT`
- `ALIGN`
- `UPDATE_LAYOUT_SIZE`
- `REORDER`

즉, 현재 controller는 size 변화만으로도 layout 단계는 다시 수행한다.

### TextVisualizer 해석

#### 직접 필요한 내용

- 현재 구조에서도 `Prepare`와 `Layout`이 완전히 같은 단계는 아니다
- size 변화 시 layout만 다시 수행하는 흐름이 일부 이미 존재한다

#### 한계

- 현재 controller는 multi-interval / exclusion bounds layout을 지원하지 않는다
- rect 하나를 기준으로 한 기존 `Layout::Engine` 구조다

## visual model이 언제 만들어지는지

### 관련 함수

- `ControllerImplModelUpdater::Update()`
- `Text::Model`
- `VisualModelImpl`

### 실제 생성/갱신 시점

visual model은 별도 한 번에 "생성"된다기보다 `UpdateModel()` 단계에서 점진적으로 채워진다.

핵심 데이터 생성 시점:

- `ShapeText(...)` 후 `mVisualModel->mGlyphs`
- `CreateGlyphsPerCharacterTable(...)`
- `CreateCharacterToGlyphTable(...)`
- `GetGlyphMetrics(...)`
- color segmentation 후 `mVisualModel->mColors`, `mColorIndices`
- `DoRelayout(...)` 후 `mGlyphPositions`, `mLines`, `layoutSize`

즉, visual model은 `Prepare`와 `Layout` 양쪽 결과를 모두 담는 구조다.

### TextVisualizer 해석

#### 직접 필요한 내용

- glyph / character mapping
- glyph metrics
- glyph positions
- line list

#### 참고용 내용

- underline / strikethrough / background color runs
- preedit용 visual decoration 데이터

## renderer로 전달되는 데이터가 어디서 만들어지는지

### 관련 파일

- `internal/text/text-model.cpp`
- `internal/text/text-view.h`
- `internal/text/text-view-interface.h`
- `internal/text/rendering/atlas/text-atlas-renderer.cpp`

### 실제 흐름

1. `UpdateModel()`이 logical/visual model을 갱신
2. `DoRelayout()`이 glyph positions와 lines를 계산
3. `Text::View`가 `Model`을 기반으로 renderer용 getter 제공
4. `AtlasRenderer::Render(Text::ViewInterface& view, ...)` 호출

즉, renderer로 전달되는 최종 데이터는 `Text::ViewInterface`를 통해 나간다.

대표 데이터:

- glyphs
- glyph positions
- default color
- glyph별 color index
- layout size
- control size

### TextVisualizer 해석

#### 직접 필요한 내용

- `TextVisualizer`도 최종적으로 atlas renderer에 glyph와 position을 넘겨야 한다
- 현재 계약은 `Text::ViewInterface` 중심이다

## Controller 내부 dirty / invalidation 구조

### 핵심 상태

#### `mOperationsPending`

재실행할 연산 비트마스크다.

예:

- `ALL_OPERATIONS`
- `VALIDATE_FONTS | SHAPE_TEXT | GET_GLYPH_METRICS | LAYOUT ...`

#### `mTextUpdateInfo`

어디부터 얼마만큼 다시 계산할지 기록한다.

주요 필드:

- `mCharacterIndex`
- `mNumberOfCharactersToRemove`
- `mNumberOfCharactersToAdd`
- `mParagraphCharacterIndex`
- `mRequestedNumberOfCharacters`
- `mStartGlyphIndex`
- `mStartLineIndex`
- `mClearAll`
- `mFullRelayoutNeeded`

#### cache invalidation flag

- `mRecalculateNaturalSize`
- `mRecalculateLayoutSize`
- `mUpdateTextDirection`

### 의미

현재 controller는 꽤 정교한 dirty 구조를 이미 갖고 있다.
문제는 이 dirty 구조가 입력기 상태와 혼합되어 있다는 점이다.

## InputField / Edit 기능 때문에 생기는 오버헤드

### 관련 구조

- `EventData` in `text-controller-impl.h`
- `text-controller-event-handler.cpp`
- `text-controller-impl-event-handler.cpp`
- `text-controller-input-font-handler.cpp`

### 실제 오버헤드

#### 직접 확인되는 오버헤드

- `EventData` 전체 상태 유지
- key/tap/pan/long-press event queue
- selection / cursor / highlight 상태
- decorator update flag
- IME preedit state
- placeholder active/inactive state
- input style tracking
- clipboard / popup / handle 처리

#### layout와 결합된 오버헤드

- relayout 시 `ProcessInputEvents()`
- font 변경 시 selection handle 재계산
- size 변경 시 decorator 위치 갱신
- editable 상태면 scroll-to-cursor / clamp / handle reposition 실행

### TextVisualizer 해석

이 오버헤드는 표시용 View에는 대부분 불필요하다.

## TextVisualizer용 경량 controller가 필요한 이유

1. `Prepare`와 `Layout`만 필요하지만 현재 controller는 edit runtime까지 함께 가진다.
2. multi-interval / exclusion bounds layout은 기존 `Layout::Engine`로 직접 표현하기 어렵다.
3. `TextVisualizer`는 shaping을 다시 하지 않는 phase 분리가 핵심인데, 현재 controller API는 그 경계가 명시적이지 않다.
4. atlas renderer에는 최종적으로 glyph 데이터만 주면 되는데, 현재 controller는 그 전에 너무 많은 편집 상태를 관리한다.

## 기존 Controller를 그대로 쓰는 경우의 장단점

### 장점

- 기존 shaping / font fallback / bidi / line break / glyph metrics 코드를 바로 재사용 가능
- natural size / height-for-width 계산 체계가 이미 존재
- `Text::ViewInterface`와 renderer 연결 구조가 이미 있다
- partial update를 위한 dirty 구조(`mOperationsPending`, `mTextUpdateInfo`)가 있다

### 단점

- edit/runtime 오버헤드가 크다
- prepare와 layout이 public API 수준에서 분리되어 있지 않다
- exclusion bounds 기반 multi-interval layout을 넣기 어렵다
- `EventData`가 존재하는 순간 interactive 로직이 많이 따라온다
- `TextVisualizer`가 원하는 "plain display pipeline"이 구조적으로 드러나지 않는다

## 새 Controller를 만드는 경우 필요한 최소 책임

### 최소 책임

#### Prepare 책임

- text 저장
- UTF-32 변환
- script 추출
- font fallback / validate
- shaping
- glyph metrics
- cluster mapping
- line break candidate 생성

#### Layout 책임

- prepared glyph run 입력
- bounds / exclusion region 기반 line interval 계산
- glyph positioning
- line 정보 생성
- layout size 계산

#### Render 전달 책임

- atlas renderer에 전달할 glyph list 제공
- glyph positions 제공
- 기본 color 제공
- 필요 시 `Text::ViewInterface` adapter 제공

### 제거해야 할 책임

- IME
- selection
- cursor
- decorator
- popup / clipboard
- placeholder interaction
- input style
- editable state machine

## TextVisualizer에 필요한 부분

### 직접 필요한 내용

- `mOperationsPending` 같은 dirty 비트마스크 개념
- `mTextUpdateInfo`처럼 update 범위를 좁히는 개념
- `UpdateModel()` 안의 prepare 단계
  - font fallback
  - shaping
  - glyph metrics
- `Relayouter`의 size 계산 패턴
- `Text::ViewInterface`로 renderer에 데이터 넘기는 패턴

### 참고용 내용

- `GetNaturalSize()`
- `GetHeightForWidth()`
- layout direction 변경 시 전체 relayout 처리

## 버릴 부분

- `EventData`
- `ProcessInputEvents()`
- `EnableTextInput()`
- `InputFontHandler`의 selection/editing 상태 의존 처리
- `HiddenInput`, placeholder editing state
- selection/cursor/clipboard/popup/decorator 전체

## 설계 결론

현재 `Text::Controller`는 `TextVisualizer`에 필요한 prepare 엔진을 많이 포함하고 있지만, 그 자체를 그대로 쓰기에는 입력기 오버헤드와 기존 rectangular layout 전제가 너무 크다.

따라서 `TextVisualizer`는 아래 방향이 적절하다.

- 기존 controller에서 prepare 단계에 해당하는 책임은 최대한 재사용
- layout은 `TextVisualizer` 전용 multi-interval engine으로 분리
- renderer로 나가는 최종 데이터 계약은 `Text::ViewInterface` 또는 그에 준하는 adapter로 유지

## 확인 필요 사항

- `TextVisualizer` 1차 구현에서 기존 controller의 `UpdateModel()` 경로를 직접 재사용할지, prepare 전용 모델을 별도 추출할지 확정 필요
- `Layout::Engine` 일부를 exclusion bounds 대응용으로 확장할지, 완전히 새 layout engine을 둘지 확정 필요
- `Text::ViewInterface` 최소 구현 집합으로 atlas renderer를 바로 재사용할 수 있는지 추가 확인 필요

## TextVisualizer 설계에 주는 결론

1. 기존 `Text::Controller`는 `Prepare` 단계의 핵심 로직을 이미 가지고 있다.
2. 그러나 `Layout`은 현재 단일 rectangular box 전제가 강하고, edit/runtime 오버헤드가 크다.
3. 따라서 `TextVisualizer`는 "기존 controller 전체 재사용"보다 "prepare 단계 재사용 + layout 분리"가 더 적합하다.
4. 최종적으로는 atlas renderer가 요구하는 glyph 데이터만 공급하는 경량 controller 또는 model 계층이 필요하다.

## 기능 매핑 표

| 기능 | 기존 Controller 위치 | TextVisualizer 필요 여부 | 재사용 가능성 | 비고 |
|---|---|---|---|---|
| SetText 후 text update 범위 설정 | `text-controller-text-updater.cpp`, `TextUpdater::SetText()` | 필요 | 높음 | edit callback 제외 필요 |
| UTF-8 -> UTF-32 변환 | `text-controller-text-updater.cpp`, `TextUpdater::SetText()` | 필요 | 높음 | prepare 단계 |
| Markup 처리 | `text-controller-text-updater.cpp`, `ProcessMarkupString(...)` | 확인 필요 | 보통 | 1차 범위에서는 제외 가능 |
| font fallback | `text-controller-impl-model-updater.cpp`, `ValidateFonts(...)` | 필요 | 높음 | prepare 단계 핵심 |
| shaping | `text-controller-impl-model-updater.cpp`, `ShapeText(...)` | 필요 | 높음 | prepare 단계 핵심 |
| glyph metrics | `text-controller-impl-model-updater.cpp`, `GetGlyphMetrics(...)` | 필요 | 높음 | prepare 단계 핵심 |
| line break candidate | `text-controller-impl-model-updater.cpp`, `GET_LINE_BREAKS` | 필요 | 높음 | prepare 단계 핵심 |
| bidi 정보 | `text-controller-impl-model-updater.cpp`, `BIDI_INFO` | 필요 | 높음 | prepare 단계에 포함 가능 |
| visual model glyph 생성 | `text-controller-impl-model-updater.cpp` | 필요 | 높음 | glyph/cluster mapping 생성 |
| layout 수행 | `text-controller-relayouter.cpp`, `DoRelayout()` / `Layout::Engine::LayoutText()` | 필요 | 낮음 | exclusion bounds 대응에는 부족 |
| natural size 계산 | `text-controller-relayouter.cpp`, `GetNaturalSize()` | 필요 | 보통 | 새 layout 모델과 조정 필요 |
| height-for-width 계산 | `text-controller-relayouter.cpp`, `GetHeightForWidth()` | 필요 | 보통 | 새 layout 모델과 조정 필요 |
| renderer 전달용 view | `text-view.h`, `text-view-interface.h` | 필요 | 높음 | atlas adapter 형태로 재사용 가능 |
| dirty 비트마스크 | `mOperationsPending` in `text-controller-impl.*` | 필요 | 높음 | 구조는 재사용 가치 높음 |
| text update 범위 추적 | `mTextUpdateInfo` in `text-controller-impl.*` | 필요 | 보통 | 더 단순화 가능 |
| selection / cursor | `EventData`, `text-controller-impl-event-handler.cpp` | 불필요 | 낮음 | 제거 대상 |
| decorator / popup / handle | `EventData`, event handler 계열 | 불필요 | 낮음 | 제거 대상 |
| IME / preedit | `EventData`, `TextUpdater::InsertText()` | 불필요 | 낮음 | 제거 대상 |
| clipboard / editing runtime | `text-controller-impl.cpp` | 불필요 | 낮음 | 제거 대상 |
