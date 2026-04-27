# TextVisualizer 구현 리뷰: Prepare 단계 직전

## 목적

이 문서는 `Commit 1 ~ Commit 3`까지 구현된 `Dali::Ui::TextVisualizer`의 현재 상태를 리뷰하고, 다음 단계인 `Commit 4: PreparedText / TextPreparer` 구현 범위를 명확히 고정하기 위한 기준 문서이다.

이 문서의 초점은 아래와 같다.

- 현재 구현이 기존 설계 문서와 얼마나 일치하는지 확인
- public API naming이 Dali UI 스타일과 맞는지 점검
- `prepare / layout / render dirty` 분리가 현재 수준에서 적절한지 평가
- Commit 4에서 실제로 재사용할 기존 text 모듈 파일과 함수 후보를 구체적으로 식별
- Commit 4에서 건드리면 범위가 커지거나 edit 오버헤드가 유입되는 파일을 분리

애매하거나 아직 코드 확인이 부족한 항목은 추측하지 않고 `확인 필요`로 표시한다.

## 1. 현재 구현 요약

현재 구현된 파일:

- `dali-ui-foundation/public-api/text-visualizer.h`
- `dali-ui-foundation/public-api/text-visualizer.cpp`
- `dali-ui-foundation/public-api/text/text-visualizer-properties.h`
- `dali-ui-foundation/integration-api/text-visualizer-impl.h`
- `dali-ui-foundation/integration-api/text-visualizer-impl.cpp`
- `dali-ui-foundation/integration-api/text-visualizer-property-handler.h`
- `dali-ui-foundation/integration-api/text-visualizer-property-handler.cpp`
- `automated-tests/src/dali-ui-foundation/utc-Dali-TextVisualizer.cpp`

현재 구현된 수준:

1. `TextVisualizer` public handle skeleton이 존재한다.
2. `TEXT`, `FONT_FAMILY`, `FONT_SIZE`, `TEXT_COLOR` property가 연결되어 있다.
3. internal impl에 기본 state storage가 있다.
4. `Prepare()` public/internal API가 존재하지만 아직 stub이다.
5. `prepare / layout / render dirty` bool이 impl 내부에 분리되어 있다.
6. exclusion region storage와 API가 존재한다.
7. 실제 `PreparedText`, shaping cache, layout engine, atlas renderer 연결은 아직 없다.

실제 구현 위치:

- `TextVisualizer::New()`:
  - `dali-ui-foundation/public-api/text-visualizer.cpp`
- `TextVisualizer::Prepare()`:
  - `dali-ui-foundation/public-api/text-visualizer.cpp`
- `TextVisualizerImpl::Prepare()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:134`
- `TextVisualizerImpl::SetText()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:78`
- `TextVisualizerImpl::SetFontFamily()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:92`
- `TextVisualizerImpl::SetFontSize()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:106`
- `TextVisualizerImpl::SetTextColor()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:120`
- `TextVisualizerImpl::SetExclusionRegions()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:141`
- `TextVisualizerImpl::ClearExclusionRegions()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:156`
- dirty helper:
  - `MarkPrepareDirty()` `:222`
  - `MarkLayoutDirty()` `:229`
  - `MarkRenderDirty()` `:234`
  - `ClearPrepareDirty()` `:239`

## 2. 설계 문서와 일치하는 부분

### 2.1 public/internal 구조

`InputField` 스타일을 따라 `View` 기반 public handle + `ViewImpl` 기반 internal impl 구조를 잡은 점은 설계와 일치한다.

- public handle:
  - `dali-ui-foundation/public-api/text-visualizer.h`
- impl:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.h`

### 2.2 property와 method의 역할 분리

설계 문서 `06-api-design.ko.md`의 방향대로 다음 분리가 지켜졌다.

- property/state:
  - `TEXT`
  - `FONT_FAMILY`
  - `FONT_SIZE`
  - `TEXT_COLOR`
- command/method:
  - `Prepare()`
  - `SetExclusionRegions()`
  - `ClearExclusionRegions()`

이 점은 Dali UI 스타일과도 잘 맞는다.

### 2.3 `Prepare`와 `Layout`의 분리 방향

현재 `Prepare()`는 stub이지만, 적어도 public/internal API 레벨에서 `Prepare`를 별도 command로 분리했다. 이는 설계 문서의 핵심 방향과 일치한다.

### 2.4 exclusion region을 property가 아닌 method로 처리

`SetExclusionRegions(const Dali::Vector<Rect<float>>& regions)`와 `ClearExclusionRegions()`를 method로 둔 점은 설계 문서와 일치한다.

### 2.5 dirty flag를 단계별로 분리

설계 문서의 의도대로 다음 3개가 분리되었다.

- `mPrepareDirty`
- `mLayoutDirty`
- `mRenderDirty`

또한 변경 전파도 방향상 맞다.

- text/font/fontSize 변경 -> prepare/layout/render dirty
- textColor 변경 -> render dirty
- exclusion 변경 -> layout/render dirty

## 3. 설계 문서와 달라진 부분

### 3.1 exclusion getter 형태

`06-api-design.ko.md`의 초안 코드 블록 일부에서는 `GetExclusionRegionCount()`가 보였지만, 현재 구현은 다음으로 정리되었다.

- `SetExclusionRegions(const Dali::Vector<Rect<float>>& regions)`
- `Dali::Vector<Rect<float>> GetExclusionRegions() const`
- `void ClearExclusionRegions()`

이 차이는 큰 문제는 아니다. 현재 구현은 “설정한 컬렉션을 그대로 되돌려 받는다”는 점에서 test 작성과 debugging에는 더 편하다.

### 3.2 `Prepare()`의 실제 의미는 아직 약하다

설계 문서에서 `Prepare()`는 expensive text processing을 수행하는 entry point로 정의되어 있다. 현재 구현에서는 아래만 수행한다.

- `mPrepareDirty = false`
- `mLayoutDirty = true`
- `mRenderDirty = true`

즉, API 이름은 설계와 같지만 실제 동작은 아직 “prepare cache 생성”이 아니다.

이것은 Commit 3 범위로는 허용되지만, Commit 4에서 반드시 보완되어야 한다.

### 3.3 lifecycle과 dirty 소비 경로가 아직 연결되지 않았다

설계 문서에서는 장기적으로 `OnRelayout()`에서 layout이 자동 수행되는 방향을 전제로 한다. 현재 구현은 아래 수준이다.

- `TextVisualizerImpl::OnRelayout()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:171`
  - 현재는 `ViewImpl::OnRelayout()` 호출만 수행
- `TextVisualizerImpl::OnMeasure()`:
  - `dali-ui-foundation/integration-api/text-visualizer-impl.cpp:176`
  - 현재는 `ViewImpl::OnMeasure()` 위임만 수행

즉, dirty flag는 존재하지만 아직 소비되지 않는다.

### 3.4 `PreparedText` 존재 여부와 `mPrepareDirty` 의미가 분리되어 있지 않다

현재는 `mPrepareDirty == false`가 되어도 실제 prepared cache가 존재하지 않는다. 따라서 Commit 4에서 아래 둘을 명확히 분리해야 한다.

1. prepared cache 존재 여부
2. prepared cache가 현재 state와 동기화되어 있는지 여부

이 점을 분리하지 않으면 Commit 4 이후 `Prepare()` skip 조건이 잘못될 수 있다.

## 4. API naming 검토

### 4.1 `Prepare()`

적절하다.

이유:

- `PrepareText()`보다 generic command 스타일에 맞다.
- `RunPrepare()`나 `DoPrepare()`보다 Dali public API 톤과 잘 맞는다.
- `Layout()`을 public에 노출하지 않는 현재 방향과도 조화롭다.

주의:

- Commit 4 이후에는 “무엇이 prepare 완료 상태인지” 구현 의미를 실제로 맞춰야 한다.

### 4.2 `SetExclusionRegions()`

적절하다.

이유:

- `SetExclusionBounds()`보다 collection 성격이 분명하다.
- 설계 문서의 최종 naming 방향과 맞다.

### 4.3 `GetExclusionRegions()`

현재 naming 자체는 자연스럽다.

다만 검토 포인트:

- `Dali::Vector<Rect<float>>`를 값으로 반환하므로 copy cost가 있다.
- 현재 단계에서는 region 수가 작고 API 단순성이 더 중요하므로 허용 가능하다.

### 4.4 `GetTextColor()`

현재 시그니처는 아래와 같다.

- `UiColor GetTextColor();`

이 형태는 `Label` / `InputField`의 기존 패턴과 맞는다. 따라서 Dali UI 스타일 측면에서는 문제가 없다.

### 4.5 `SetText`, `SetFontFamily`, `SetFontSize`, `SetTextColor`

모두 Dali UI의 chaining setter 패턴과 일치한다.

결론:

- 현재 public API naming은 전반적으로 적절하다.
- Commit 4 전에 public naming을 다시 바꿔야 할 이유는 없다.

## 5. dirty flag 정책 검토

### 5.1 현재 정책

현재 구현 정책:

- `SetText()` / `SetFontFamily()` / `SetFontSize()`
  - `MarkPrepareDirty()`
  - 결과적으로 `prepare/layout/render` 모두 dirty
- `SetTextColor()`
  - `MarkRenderDirty()`
- `SetExclusionRegions()` / `ClearExclusionRegions()`
  - `MarkLayoutDirty()`
  - `MarkRenderDirty()`
- `Prepare()`
  - `ClearPrepareDirty()`
  - `MarkLayoutDirty()`
  - `MarkRenderDirty()`

### 5.2 좋은 점

1. invalidation 전파 방향이 직관적이다.
2. text/font/fontSize와 textColor를 구분한 점이 설계 의도와 맞다.
3. exclusion 변경이 prepare를 건드리지 않는 점이 맞다.

### 5.3 보완이 필요한 점

#### `layout/render clear` 경로가 아직 없다

현재는 set 쪽 helper만 있고 clear helper가 없다. Commit 4에서 최소한 아래 정책을 정해야 한다.

- prepare 성공 시:
  - `mPrepareDirty = false`
- layout 미수행이므로:
  - `mLayoutDirty = true` 유지 가능
- render 미수행이므로:
  - `mRenderDirty = true` 유지 가능

즉 Commit 4 시점에는 `prepare clear`만 실제로 의미가 있고, `layout/render clear`는 Commit 5~6까지 미뤄도 된다.

#### `Prepare()`가 항상 layout/render dirty를 올린다

현재는 `Prepare()` 호출만 해도 layout/render dirty를 다시 세팅한다. stub 단계에서는 허용 가능하지만 Commit 4 이후에는 아래를 검토해야 한다.

- 실제 prepared 결과가 바뀐 경우에만 layout dirty를 유지할지
- prepared 결과가 동일해도 explicit `Prepare()`는 layout dirty를 강제로 세팅할지

현재로서는 “explicit eager prepare 이후 layout은 다시 계산해도 된다”는 보수적 정책이라 크게 문제는 없다.

#### `OnRelayout()`와 연계되지 않는다

현재 dirty flag는 상태만 저장한다. Commit 4에서는 여전히 layout 미구현이므로 괜찮지만, Commit 5부터는 `OnRelayout()` 또는 내부 layout trigger와 연결되어야 한다.

### 5.4 결론

현재 dirty policy는 Commit 3 목표에는 적절하다.

다만 Commit 4에서 반드시 추가해야 할 것:

- prepared cache 포인터 또는 value
- prepared cache 존재 여부 확인
- prepare 성공 시점의 clear 규칙

## 6. Commit 4에서 재사용할 기존 text-controller/model/layout 함수 후보

Commit 4의 목표는 “기존 controller를 그대로 재사용”이 아니라, 기존 text pipeline에서 prepare에 해당하는 helper를 선별적으로 재사용하는 것이다.

핵심 원칙:

- `ControllerImplModelUpdater::Update()` 전체를 호출하는 방식은 피한다.
- 그 안에서 prepare 단계에 해당하는 helper만 분리 사용한다.
- edit / preedit / color run / decorator / relayout 연계는 가져오지 않는다.

### 6.1 UTF-8 -> UTF-32 변환

후보 파일:

- `dali-ui-foundation/internal/text/character-set-conversion.h`
- `dali-ui-foundation/internal/text/character-set-conversion.cpp`

후보 함수:

- `Utf8ToUtf32()`

참고 위치:

- `dali-ui-foundation/internal/text/controller/text-controller-text-updater.cpp:141`

용도:

- `Dali::String` 또는 UTF-8 source를 `PreparedText`의 logical character buffer로 변환

### 6.2 line break candidate 생성

후보 파일:

- `dali-ui-foundation/internal/text/segmentation.h`
- `dali-ui-foundation/internal/text/segmentation.cpp`

후보 함수:

- `Text::SetLineBreakInfo()`

참고 위치:

- `dali-ui-foundation/internal/text/segmentation.cpp:46`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:183`

용도:

- cluster/line-break candidate 계산의 입력이 되는 line break info 생성

추가 후보:

- `Text::MultilanguageSupport::IsICULineBreakNeeded()`
- `Text::MultilanguageSupport::UpdateICULineBreak()`

후보 파일:

- `dali-ui-foundation/internal/text/multi-language-support.h`
- `dali-ui-foundation/internal/text/multi-language-support.cpp`
- `dali-ui-foundation/internal/text/multi-language-support-impl.cpp`

이유:

- 기존 controller는 ICU 기반 line break 보정 경로를 조건부로 호출한다.
- Commit 4에서도 locale-dependent break 동작을 유지하려면 이 경로를 검토해야 한다.

### 6.3 paragraph 정보 생성

후보 파일:

- `dali-ui-foundation/internal/text/logical-model-impl.h`
- `dali-ui-foundation/internal/text/logical-model-impl.cpp`

후보 함수:

- `LogicalModel::CreateParagraphInfo()`

참고 위치:

- `dali-ui-foundation/internal/text/logical-model-impl.cpp:487`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:232`

용도:

- paragraph 경계 생성
- 향후 cluster / line band 단위 계산의 보조 정보

### 6.4 script 분석

후보 파일:

- `dali-ui-foundation/internal/text/multi-language-support.h`
- `dali-ui-foundation/internal/text/multi-language-support.cpp`
- `dali-ui-foundation/internal/text/multi-language-support-impl.cpp`

후보 함수:

- `Text::MultilanguageSupport::SetScripts()`

참고 위치:

- `dali-ui-foundation/internal/text/multi-language-support.cpp:55`
- `dali-ui-foundation/internal/text/multi-language-support-impl.cpp:378`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:251`

용도:

- shaping 이전 script run 계산

### 6.5 font fallback / font validation

후보 파일:

- `dali-ui-foundation/internal/text/multi-language-support.h`
- `dali-ui-foundation/internal/text/multi-language-support.cpp`
- `dali-ui-foundation/internal/text/multi-language-support-impl.cpp`
- `dali-ui-foundation/internal/text/multi-language-helper-functions.h`

후보 함수:

- `Text::MultilanguageSupport::ValidateFonts()`
- 참고 helper: `MergeFontDescriptions()`

참고 위치:

- `dali-ui-foundation/internal/text/multi-language-support.cpp:61`
- `dali-ui-foundation/internal/text/multi-language-support-impl.cpp:611`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:301`

용도:

- default font + explicit font description + fallback을 합쳐 character 단위 valid font run 생성

주의:

- Commit 4에서는 `InputField`/placeholder 전용 font branch를 그대로 가져오지 말고, `TextVisualizer`의 단순한 default font family / size 조건만 연결해야 한다.

### 6.6 shaping

후보 파일:

- `dali-ui-foundation/internal/text/shaper.h`
- `dali-ui-foundation/internal/text/shaper.cpp`

후보 함수:

- `Text::ShapeText()`

참고 위치:

- `dali-ui-foundation/internal/text/shaper.cpp:52`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:373`

용도:

- validated font run + script run + line break info를 기반으로 glyph 생성
- `glyphs`
- `glyphToCharacterMap`
- `charactersPerGlyph`
- `newParagraphGlyphs`

### 6.7 glyph/character mapping 생성

후보 파일:

- `dali-ui-foundation/internal/text/visual-model-impl.h`
- `dali-ui-foundation/internal/text/visual-model-impl.cpp`

후보 함수:

- `VisualModel::CreateGlyphsPerCharacterTable()`
- `VisualModel::CreateCharacterToGlyphTable()`

참고 위치:

- `dali-ui-foundation/internal/text/visual-model-impl.cpp:35`
- `dali-ui-foundation/internal/text/visual-model-impl.cpp:112`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:378`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:380`

용도:

- `PreparedText`에 필요한:
  - glyph -> character
  - character -> glyph
  - character -> glyph count
  - cluster derivation용 기초 매핑

### 6.8 glyph metrics 확보

후보 파일:

- `dali-ui-foundation/internal/text/metrics.h`

후보 함수:

- `Metrics::GetGlyphMetrics()`

참고 위치:

- `dali-ui-foundation/internal/text/metrics.h:90`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:398`

용도:

- shaped glyph에 width / advance / bearing / ascender 관련 metric 채우기

### 6.9 cluster 관련 참고용 함수

후보 파일:

- `dali-ui-foundation/internal/text/characters-helper-functions.h`
- `dali-ui-foundation/internal/text/characters-helper-functions.cpp`

참고 함수:

- `RetrieveClusteredCharactersOfCharacterIndex()`

참고 위치:

- `dali-ui-foundation/internal/text/characters-helper-functions.cpp:13`

판단:

- Commit 4의 직접 재사용 함수라기보다 “현재 엔진이 cluster를 어떻게 해석하는지 보는 참고용”에 가깝다.
- emoji/cursor 편의 함수 성격이 강하므로 `PreparedCluster` 생성의 직접 기반으로 삼기에는 부족하다.

### 6.10 Commit 4에서 직접 수정 대상이 되어서는 안 되는 기존 monolith

아래 함수는 “읽고 참고”는 가능하지만, Commit 4에서 직접 수정 대상으로 삼는 것은 권장하지 않는다.

- `ControllerImplModelUpdater::Update()`
  - `dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.cpp:74`

이유:

- prepare 외의 color/preedit/markup 연계까지 뒤섞여 있다.
- 여기서 직접 분기 추가를 시작하면 `TextVisualizer` 전용 경량 경로가 아니라 기존 controller 변형 작업이 된다.

## 7. Commit 4에서 절대 건드리지 말아야 할 파일

### 7.1 editing / selection / cursor / IME 경로

- `dali-ui-foundation/internal/text/controller/text-controller-event-handler.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-event-handler.h`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-event-handler.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-impl-event-handler.h`
- `dali-ui-foundation/internal/text/controller/text-controller-input-font-handler.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-input-font-handler.h`
- `dali-ui-foundation/internal/text/controller/text-controller-input-properties.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-input-properties.h`
- `dali-ui-foundation/internal/text/controller/text-controller-placeholder-handler.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-placeholder-handler.h`
- `dali-ui-foundation/internal/text/decorator/text-decorator.cpp`
- `dali-ui-foundation/internal/text/decorator/text-decorator.h`
- `dali-ui-foundation/internal/text/text-selection-handle-controller.cpp`
- `dali-ui-foundation/internal/text/text-selection-handle-controller.h`
- `dali-ui-foundation/internal/text/cursor-helper-functions.cpp`
- `dali-ui-foundation/internal/text/cursor-helper-functions.h`

이유:

- `TextVisualizer` 1차 범위에서 제외한 기능이다.
- Commit 4의 목적은 prepare cache 생성이지 interactive text control 확장이 아니다.

### 7.2 기존 controller의 text mutation 경로

- `dali-ui-foundation/internal/text/controller/text-controller-text-updater.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-text-updater.h`

이유:

- 여기에는 insert/remove/preedit/cursor reset path가 섞여 있다.
- UTF-8 변환 reference 정도로만 보고, 실제 구현은 `TextVisualizer` 전용 `text-preparer.cpp`로 분리하는 것이 맞다.

### 7.3 relayout / layout monolith

- `dali-ui-foundation/internal/text/controller/text-controller-relayouter.cpp`
- `dali-ui-foundation/internal/text/controller/text-controller-relayouter.h`
- `dali-ui-foundation/internal/text/layouts/layout-engine.cpp`
- `dali-ui-foundation/internal/text/layouts/layout-engine.h`

이유:

- Commit 4 범위는 prepare 단계다.
- layout은 Commit 5 이후 separate engine으로 구현하는 것이 설계 기준이다.

### 7.4 renderer / atlas 경로

- `dali-ui-foundation/internal/text/rendering/atlas/text-atlas-renderer.cpp`
- `dali-ui-foundation/internal/text/rendering/atlas/text-atlas-renderer.h`
- `dali-ui-foundation/internal/text/rendering/text-renderer.cpp`
- `dali-ui-foundation/internal/text/rendering/text-renderer.h`
- `dali-ui-foundation/internal/text/text-view.cpp`
- `dali-ui-foundation/internal/text/text-view.h`

이유:

- Commit 4는 prepare cache 생성 단계다.
- renderer adapter 연결은 Commit 6 이후 범위다.

## 8. 확인 필요 사항

1. `PreparedText`가 `LogicalModel` / `VisualModel` 전체를 멤버로 가져야 하는지, 아니면 필요한 vector만 복사/소유해야 하는지 확인 필요
   - 현재 설계 의도상으로는 “필요 최소 vector만 소유”가 더 맞다.
2. ICU line break 보정 경로를 Commit 4에서 바로 포함할지 확인 필요
   - 기존 controller는 `IsICULineBreakNeeded()` / `UpdateICULineBreak()`를 사용한다.
3. `PreparedCluster`를 만드는 canonical 규칙을 무엇으로 둘지 확인 필요
   - `charactersPerGlyph` 기반 역추적만으로 충분한지
   - 추가 cluster table이 필요한지
4. `Prepare()` 성공 조건을 무엇으로 볼지 확인 필요
   - empty text도 “prepared success”로 볼지
   - invalid font fallback 상황에서도 빈 prepared cache를 성공으로 볼지
5. Commit 4에서 `PreparedText` 존재 여부를 impl에 어떤 형태로 저장할지 확인 필요
   - raw member
   - `std::unique_ptr`
   - intrusive/shared style

## 9. 최종 권장 Commit 4 작업 범위

Commit 4는 아래 범위로 제한하는 것을 권장한다.

### 9.1 새 파일 추가

- `dali-ui-foundation/internal/text/text-visualizer/prepared-text.h`
- `dali-ui-foundation/internal/text/text-visualizer/prepared-text.cpp`
- `dali-ui-foundation/internal/text/text-visualizer/text-preparer.h`
- `dali-ui-foundation/internal/text/text-visualizer/text-preparer.cpp`

### 9.2 수정 파일

- `dali-ui-foundation/integration-api/text-visualizer-impl.h`
- `dali-ui-foundation/integration-api/text-visualizer-impl.cpp`
- `automated-tests/src/dali-ui-foundation/utc-Dali-TextVisualizer.cpp`

### 9.3 Commit 4에서 구현할 책임

1. `Prepare()` 호출 시:
   - UTF-8 -> UTF-32 변환
   - line break info 생성
   - script run 생성
   - font validation / fallback
   - shaping
   - glyph metrics 확보
   - glyph/character mapping 생성
2. 결과를 `PreparedText`에 저장
3. 성공 시 `mPrepareDirty` clear
4. `mLayoutDirty`, `mRenderDirty` 유지

### 9.4 Commit 4에서 구현하지 말 것

1. exclusion region 기반 실제 layout
2. line fragment 생성
3. `Layout::Engine` 연결
4. `AtlasRenderer` 연결
5. `Text::Controller` 수정
6. edit / selection / cursor / decorator 경로 유입

### 9.5 권장 구현 방식

가장 권장되는 방식은 아래다.

1. 기존 `ControllerImplModelUpdater::Update()`를 읽어 prepare 단계 helper 호출 순서를 복제한다.
2. 실제 구현은 `TextVisualizer` 전용 `TextPreparer`로 새로 작성한다.
3. helper 재사용은 아래 순서로 제한한다.

- `Utf8ToUtf32()`
- `SetLineBreakInfo()`
- `MultilanguageSupport::SetScripts()`
- `MultilanguageSupport::ValidateFonts()`
- `LogicalModel::CreateParagraphInfo()`
- `ShapeText()`
- `VisualModel::CreateGlyphsPerCharacterTable()`
- `VisualModel::CreateCharacterToGlyphTable()`
- `Metrics::GetGlyphMetrics()`

### 9.6 TextVisualizer 설계에 주는 결론

현재 Commit 1~3 구현은 skeleton, property/state, dirty separation 관점에서 대체로 설계와 잘 맞는다.

다음 Commit 4의 핵심은 기존 `Text::Controller`를 더 건드리는 것이 아니라, 기존 text pipeline에서 prepare 단계 helper만 분리 채택해 `TextVisualizer` 전용 `PreparedText` 캐시를 만드는 것이다.

즉 다음 단계는 “controller reuse”가 아니라 “controller pipeline extraction”으로 접근하는 것이 맞다.
