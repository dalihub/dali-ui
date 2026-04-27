# TextVisualizer Glyph Cache Lifecycle Design

## 1. 목적

현재 `TextVisualizerGlyphRenderer` prototype은 already-cached glyph만 처리한다.

즉 `Render(preparedText, layoutResult, adapter, textColor)` 또는 adapter-only `Render(adapter)`는 `AtlasGlyphManager::IsCached()`가 성공한 glyph에 대해서만 atlas id별 mesh actor를 만들 수 있고, cache miss가 발생하면 안전하게 `false`를 반환한다.

이 상태는 layout-dynamic workload에서 lightweight renderer와 geometry-only update의 가치를 확인하기에는 충분하지만, production default path로 전환하기에는 아직 부족하다. production path로 전환하려면 다음 책임이 필요하다.

- cache miss 시 glyph bitmap 생성
- atlas cache add
- glyph reference count 증가
- renderer clear / text change / fallback 시 reference count 감소
- render 실패 중간 상태 rollback
- layout-dynamic workload에서 glyph sequence가 같을 때 ref count churn 방지

이 문서는 `TextVisualizerGlyphRenderer`가 기존 `Text::AtlasRenderer`와 glyph cache behavior를 어떻게 공유하거나 분리할지 설계한다.

## 2. 현재 TextVisualizerGlyphRenderer 구현 상태

현재 구현은 다음 상태다.

- `TextVisualizerGlyphRenderer::Render()`는 `PreparedText`, `LayoutResult`, `AtlasViewAdapter`, single `textColor`를 입력으로 받을 수 있다.
- adapter-only `Render(const AtlasViewAdapter&)` overload도 있다.
- `AtlasViewAdapter`의 cached renderer glyph positions를 사용한다.
- `AtlasGlyphManager::IsCached()`와 `AtlasGlyphManager::GenerateMeshData()`로 already-cached glyph mesh를 만든다.
- atlas id별 pending mesh를 구성하고 L8 / BGRA atlas pixel format에 따라 shader를 선택한다.
- output actor 아래 mesh actor를 attach한다.
- already-cached glyph에 대해 reference acquire / release / rollback을 수행한다.
- 같은 glyph cache signature에서는 ref count churn 없이 기존 glyph cache ownership을 유지한다.
- 같은 glyph cache signature와 같은 mesh topology에서는 actor / renderer / geometry를 유지하고 `VertexBuffer::SetData()`로 geometry-only update를 시도한다.
- `AtlasRendererBridge`의 internal compile-time flag 뒤에서 optional bridge path로 연결되어 있다.
- 기본 flag는 `false`이므로 production 기본 경로는 기존 `Text::AtlasRenderer` path를 유지한다.
- flag ON 상태에서도 `TextVisualizerImpl`은 exclusion regions가 있는 TextVisualizer에만 lightweight renderer를 허용한다.
- cache miss / invalid glyph / invalid position / empty mesh / texture failure는 `false`로 반환하고 기존 renderer fallback을 사용한다.
- glyph bitmap 생성은 아직 없다.
- atlas cache add는 아직 없다.

중요한 점은 `AtlasGlyphManager::GenerateMeshData()`가 mesh data를 만들 때 atlas image reference count를 증가시키지 않는다는 것이다. `AtlasGlyphManager::GenerateMeshData()`는 내부적으로 `AtlasManager::GenerateMeshData(..., false)`를 호출한다.

따라서 현재 prototype은 already-cached glyph에 대해서는 직접 references를 보유하지만, cache miss에서 glyph bitmap을 만들고 atlas에 추가하는 ownership 시작점은 아직 없다.

결론:

- rendering 가능성과 layout-dynamic geometry-only update 가치는 확인됐다.
- production default path로 쓰려면 cache miss handling과 기존 `AtlasRenderer::CacheGlyph()` behavior 공유 전략이 필요하다.

## 3. 기존 AtlasRenderer CacheGlyph / RemoveText 분석

`text-atlas-renderer.cpp`의 `AtlasRenderer::Impl`은 glyph cache lifecycle을 private 구현으로 보유한다.

### TextCacheEntry

기존 renderer의 `TextCacheEntry`는 다음 정보를 저장한다.

```cpp
struct TextCacheEntry
{
  FontId           mFontId;
  Text::GlyphIndex mIndex;
  uint32_t         mImageId;
  uint16_t         mOutlineWidth;
  bool             isItalic : 1;
  bool             isBold : 1;
};
```

의미:

- `fontId`, `glyphIndex`, outline / italic / bold style 조합이 atlas glyph identity다.
- `imageId`는 atlas image slot이다.
- `RemoveText()`는 이 entry를 순회해 같은 identity의 reference count를 감소시킨다.

### CacheGlyph()

`CacheGlyph()`의 책임은 단순 lookup이 아니다.

| 단계 | 역할 |
|---|---|
| default / maximum atlas size 조회 | `FontClient::GetDefaultTextAtlasSize()`, `GetMaximumTextAtlasSize()` 사용 |
| cached lookup | `AtlasGlyphManager::IsCached(fontId, glyphIndex, style, slot)` |
| cache hit | `AtlasGlyphManager::AdjustReferenceCount(..., +1)` |
| cache miss | glyph bitmap 생성 경로로 진입 |
| glyph bitmap 생성 | `FontClient::CreateBitmap()` 사용 |
| compressed bitmap 처리 | 필요 시 decompress 후 owned buffer로 교체 |
| `PixelData` 생성 | glyph buffer를 `PixelData::New(..., PixelData::FREE)`로 전달 |
| block size 갱신 | glyph bitmap width / height에 맞춰 needed block size 확장 |
| atlas size 정책 | default atlas size를 maximum까지 2배씩 확장 가능 |
| atlas add | `AtlasGlyphManager::SetNewAtlasSize()` 후 `AtlasGlyphManager::Add()` |
| initial ref count | `AtlasGlyphManager::Add()`는 glyph record를 만들고 count를 `1`로 설정 |

즉 cache hit은 explicit `+1`, cache miss는 `Add()`가 initial count `1`을 만든다.

### GenerateMesh()

`GenerateMesh()`는 `AtlasGlyphManager::GenerateMeshData(slot.mImageId, position, mesh)`로 glyph quad를 만들고, vertex color를 적용한다.

또한 `isGlyphCached == false`일 때 `TextCacheEntry`를 `newTextCache`에 추가한다. 현재 `AddGlyphs()` 호출 흐름에서는 일반 glyph mesh 생성 시 `false`를 넘기므로, render 결과에 포함된 glyph ref는 새 `mTextCache`가 소유하게 된다.

### RemoveText()

`RemoveText()`는 기존 `mTextCache`를 순회하며 다음을 수행한다.

```cpp
AtlasGlyphManager::GlyphStyle style;
style.outline  = oldTextIter->mOutlineWidth;
style.isItalic = oldTextIter->isItalic;
style.isBold   = oldTextIter->isBold;
mGlyphManager.AdjustReferenceCount(oldTextIter->mFontId, oldTextIter->mIndex, style, -1);
```

그 뒤 `mTextCache.Resize(0)`으로 old ownership을 비운다.

### 기존 renderer lifecycle의 핵심

기존 `AddGlyphs()`는 새 text cache를 만들 때, old text cache를 바로 release하지 않는다.

```text
new glyph refs acquire
-> RemoveText()
-> mTextCache.Swap(newTextCache)
```

이 순서는 같은 glyph sequence를 다시 렌더링할 때 old references를 먼저 제거해서 glyph가 atlas에서 사라지는 상황을 피한다. `TextVisualizerGlyphRenderer`도 failure rollback과 old cache 유지 정책을 설계할 때 이 순서를 따라야 한다.

## 4. TextVisualizerGlyphRenderer에 필요한 cache ownership

새 renderer도 자체 cache entry가 필요하다.

후보:

```cpp
struct GlyphCacheEntry
{
  Text::FontId                  fontId{0u};
  Text::GlyphIndex              glyphIndex{0u};
  AtlasGlyphManager::GlyphStyle style;
  uint32_t                      atlasId{0u};
  uint32_t                      imageId{0u};
};
```

필요 member:

```cpp
std::vector<GlyphCacheEntry> mGlyphCacheEntries;
uint64_t                     mGlyphCacheSignature{0u};
```

필요 method:

- `ClearGlyphCache()`
- `ReleaseGlyphReferences()`
- `AcquireGlyphReferences(...)`
- `HasGlyphCache()`
- `GetGlyphCacheEntryCount()`
- `CalculateGlyphCacheSignature(...)`

정책:

- `Clear()`는 mesh / output actor뿐 아니라 glyph references도 release한다.
- `Render()` 성공 시 renderer가 필요한 glyph references를 보유한다.
- `Render()` 실패 시 새로 acquire한 refs는 rollback한다.
- old cache는 새 cache acquire와 mesh build가 성공하기 전까지 유지한다.
- same glyph sequence라면 ref count를 유지하고 positions / mesh update만 수행한다.

### entry granularity

두 가지 방식이 가능하다.

| 방식 | 설명 | 장점 | 단점 |
|---|---|---|---|
| per glyph occurrence | renderable glyph occurrence마다 entry 하나 저장 | 기존 `AtlasRenderer` lifecycle과 가장 유사 | repeated glyph가 많으면 entry 수 증가 |
| unique glyph + count | glyph identity별 count 저장 | cache state compact | acquire / rollback 구현이 조금 복잡 |

추천은 **per glyph occurrence**다. Phase 2 primary workload에서는 glyph sequence stable이므로 entry 수는 한 번 만들어진 뒤 재사용된다. 기존 `AtlasRenderer`의 `TextCacheEntry`도 rendered glyph ownership에 가깝게 동작하므로, 처음 구현은 동일한 mental model이 안전하다.

## 5. Layout-dynamic workload 최적화 정책

Phase 2 primary workload는 다음이다.

- text content stable
- glyph sequence stable
- font / font size stable
- style stable
- exclusion / layout / glyph positions만 dynamic

따라서 매 render마다 glyph refs를 `-1/+1` 하면 안 된다. 그것은 기존 `AtlasRenderer::Render()` full rebuild와 비슷한 비용을 다시 만드는 일이다.

필요한 정책:

- renderer가 현재 glyph cache signature를 저장한다.
- 새 render 요청의 glyph sequence signature가 같으면 glyph cache lifecycle을 건드리지 않는다.
- positions-only render에서는:
  - no glyph cache add
  - no ref count increase
  - no ref count decrease
  - mesh / vertex position update만 수행
- text / font / fontSize / glyph sequence / style 변경으로 signature가 바뀌면:
  - new glyph references acquire
  - mesh build 성공
  - old references release
  - new entries commit

### glyph cache signature 후보

signature 입력:

- renderable glyph count
- `fontId`
- `glyphIndex`
- `GlyphStyle::outline`
- `GlyphStyle::isItalic`
- `GlyphStyle::isBold`

signature에 포함하지 않는 것이 좋은 값:

- renderer glyph position
- text color
- layout size
- atlas id / image id

이유:

- positions는 layout-dynamic workload에서 매 frame 바뀌는 값이다.
- text color는 glyph cache identity가 아니다.
- atlas id / image id는 acquire 결과이지 glyph identity 자체는 아니다.

다만 geometry-only update 조건에는 별도의 **mesh topology signature**가 필요할 수 있다.

mesh topology signature 후보:

- glyph cache signature
- atlas id sequence 또는 atlas page split
- shader type sequence
- mesh record count
- vertex / index count per mesh record

## 6. Cache miss 처리 후보

### A. TextVisualizerGlyphRenderer에 CacheGlyph equivalent 제한 구현

내용:

- 새 renderer가 `FontClient::CreateBitmap()`, `PixelData`, `AtlasGlyphManager::Add()`를 직접 사용한다.
- `AtlasRenderer::Impl::CacheGlyph()`의 최소 subset을 복제한다.
- unsupported style은 fallback으로 둔다.

장점:

- 기존 `text-atlas-renderer.*`를 수정하지 않는다.
- isolated prototype으로 빠르게 검증할 수 있다.
- `TextVisualizer` scope에 맞춰 single color / no decoration path를 작게 유지할 수 있다.

단점:

- glyph bitmap generation / atlas size policy / compressed bitmap 처리 중복이 생긴다.
- 기존 renderer와 behavior drift 위험이 있다.
- color glyph / fallback font / atlas block edge case 검증이 필요하다.

### B. AtlasRenderer::Impl CacheGlyph helper extraction

내용:

- `CacheGlyph()`, `TextCacheEntry`, block size policy 등을 공용 internal helper로 추출한다.
- 기존 `AtlasRenderer`와 `TextVisualizerGlyphRenderer`가 같은 helper를 쓴다.

장점:

- behavior drift를 줄일 수 있다.
- glyph cache lifecycle 정책을 공유할 수 있다.
- 장기 유지보수에 유리하다.

단점:

- `text-atlas-renderer.*` 수정이 필요하다.
- 기존 `Label` / `InputField` / text pipeline 회귀 위험이 있다.
- helper 경계가 커질 수 있다.

### C. AtlasGlyphManager에 EnsureGlyphCached helper 추가

내용:

- `AtlasGlyphManager` 또는 adjacent helper에 `EnsureGlyphCached()`를 추가한다.
- glyph bitmap 생성과 atlas add를 manager-level 책임으로 이동한다.

장점:

- renderer-neutral helper가 될 수 있다.
- `TextVisualizerGlyphRenderer`와 기존 renderer가 함께 쓸 수 있다.
- glyph cache ownership API가 더 명확해진다.

단점:

- `AtlasGlyphManager` API 확장이다.
- `FontClient`, atlas size policy, style policy까지 manager에 넣을지 경계가 애매하다.
- 영향 범위 검토가 필요하다.

### D. Existing AtlasRenderer warmup에 의존

내용:

- 기존 `Text::AtlasRenderer` path를 한 번 호출해 glyph를 atlas에 warmup한다.
- 이후 `TextVisualizerGlyphRenderer`는 already-cached glyph만 사용한다.

장점:

- 구현이 쉽다.
- 현재 MVP와 맞다.

단점:

- ownership이 불명확하다.
- warmup renderer가 references를 release하면 `TextVisualizerGlyphRenderer` mesh가 stale될 수 있다.
- production path로 부적합하다.

### 추천

단기 추천은 다음 순서다.

1. **already-cached glyph에 대한 reference acquire / release skeleton을 먼저 구현한다.**
2. cache miss는 계속 `false`로 둔다.
3. failure rollback과 same sequence no-ref-churn 정책을 검증한다.
4. 이후 cache miss handling은 후보 A 또는 B 중 하나로 별도 커밋에서 결정한다.

이렇게 하면 active path 연결 전에 ownership 기반을 만들 수 있고, `CacheGlyph()` 중복 구현이라는 큰 리스크를 바로 열지 않아도 된다.

## 7. 추천 구현 전략

추천 단계:

1. `GlyphCacheEntry`와 `mGlyphCacheEntries` 추가
2. `ReleaseGlyphReferences()` 추가
3. `Clear()`에서 glyph references release
4. `Render()`에서 already-cached glyph에 대해 `AdjustReferenceCount(..., +1)` acquire
5. cache miss는 여전히 `false`
6. render 실패 시 temporary acquired refs rollback
7. render 성공 시 old refs release 후 new entries commit
8. same glyph sequence면 ref count acquire / release 생략

이유:

- cache miss 구현보다 작고 안전하다.
- active path 연결 전 ownership 정책을 검증할 수 있다.
- geometry-only update 전 필수 기반이다.
- 기존 `Text::AtlasRenderer`를 수정하지 않는다.

권장 다음 커밋:

```text
Add TextVisualizerGlyphRenderer glyph cache references
```

## 8. Failure / rollback policy

`Render()` 중 실패 지점은 여러 곳에 있다.

- adapter validation 실패
- glyph cache miss
- `AdjustReferenceCount(+1)` 대상 glyph lookup 실패 가능성
- mesh generation 실패
- texture set lookup 실패
- shader / renderer / actor 생성 실패

권장 policy:

### render 시작

- old cache entries는 유지한다.
- old mesh records는 positions-only update가 아닌 full rebuild라면 clear 가능하지만, active path 전에는 old output 보존 여부를 별도 판단한다.
- new cache entries는 temporary vector에 쌓는다.

### acquire 중

- cached glyph hit이면 `AdjustReferenceCount(..., +1)` 후 temporary entry 추가
- cache miss면:
  - temporary entries를 reverse 또는 forward 순회하며 `AdjustReferenceCount(..., -1)`
  - mesh records clear
  - old cache 유지
  - `false` 반환

### mesh build 중 실패

- temporary acquired refs rollback
- new mesh records clear
- old cache 유지
- `false` 반환

### 성공

- old cache release
- temporary cache entries commit
- new mesh records commit
- new signature 저장

### same glyph sequence

- old cache 유지
- temporary acquire 없음
- ref count 변경 없음
- mesh / vertex data만 update 또는 rebuild

이 정책은 `AtlasRenderer::Impl::AddGlyphs()`가 old refs를 즉시 제거하지 않고 new cache를 먼저 만드는 구조와 같은 방향이다.

## 9. Geometry-only update와의 관계

glyph cache ownership이 안정되면 geometry-only update 조건을 세울 수 있다.

필요 조건:

- same glyph cache signature
- same atlas page split
- same shader type per mesh record
- same mesh record count
- compatible vertex / index count
- output actor / mesh actors alive

가능한 update:

- existing `VertexBuffer::SetData()`로 vertex positions / colors update
- index buffer가 같으면 유지
- `Geometry` / `Renderer` / `Actor` reuse
- no glyph cache reference count change

fallback 조건:

- glyph sequence changed
- atlas page split changed
- glyph cache miss
- color glyph / monochrome shader split changed
- mesh actor missing
- vertex/index topology incompatible

이 단계에서 중요한 점은 geometry-only update가 glyph cache lifecycle 위에 올라간다는 것이다. glyph cache owner가 없으면 mesh가 참조하는 atlas slot이 renderer lifetime 동안 유효하다는 보장이 없다.

## 10. 다음 구현 커밋 후보

### 1. Add TextVisualizerGlyphRenderer glyph cache references

범위:

- `GlyphCacheEntry` 추가
- `mGlyphCacheEntries` member 추가
- `ClearGlyphCache()` / `ReleaseGlyphReferences()` 추가
- `Render()`에서 already-cached glyph refs acquire
- cache miss는 `false` 유지
- failure rollback
- same glyph sequence no-ref-churn 준비
- active path 미연결
- UTC 추가

### 2. Design or implement glyph cache miss handling

후보:

- `TextVisualizerGlyphRenderer` limited `CacheGlyphForTextVisualizer()`
- shared helper extraction 설계
- `AtlasGlyphManager::EnsureGlyphCached()` 설계

이 커밋은 `FontClient::CreateBitmap()`, compressed glyph buffer, atlas size policy를 다루므로 별도 작업으로 둔다.

### 3. Prototype TextVisualizer geometry-only update

전제:

- glyph cache references가 안정적으로 유지된다.

범위:

- same glyph sequence / same atlas split에서 existing `VertexBuffer::SetData()` 사용
- actor / renderer / geometry reuse
- fallback 유지

## 11. Validation plan

추가 UTC 후보:

| 테스트 | 목적 |
|---|---|
| empty glyph cache state | 초기 `HasGlyphCache() == false`, count 0 |
| render cache miss false no refs | cache miss 시 refs가 남지 않음 |
| render after warmup acquires refs | 기존 renderer warmup 이후 success 가능하면 entry count 확인 |
| clear releases refs | `Clear()` 후 cache entry count 0 |
| render failure rollback smoke | 중간 실패 후 old cache 유지 / temporary refs rollback |
| repeated same glyph sequence | cache entry count가 불필요하게 증가하지 않음 |

측정 가능성:

- `AtlasGlyphManager::GetMetrics()`는 verbose glyph ref count 문자열을 제공한다.
- 단, test에서 특정 glyph ref count를 강하게 assert하면 font / glyph cache 환경에 의존할 수 있다.
- 우선은 renderer-local cache entry count와 no-crash smoke 중심으로 검증한다.
- ref count string은 필요 시 debug validation으로만 사용한다.

## 12. 결론

현재 `TextVisualizerGlyphRenderer` MVP는 `TextVisualizer` 전용 renderer가 atlas mesh actor를 만들 수 있음을 보여준 proof다.

하지만 active path 전환 전에는 glyph ownership이 필요하다. 기존 `Text::AtlasRenderer`는 `CacheGlyph()` / `RemoveText()` / `mTextCache`로 이 문제를 private하게 해결하고 있으며, 새 renderer는 같은 개념의 ownership을 자체적으로 가져야 한다.

정리:

- current MVP는 rendering feasibility proof다.
- active path 연결 전 glyph cache lifecycle이 먼저다.
- geometry-only update는 glyph cache references가 안정된 뒤에 가능하다.
- 기존 `AtlasRenderer` 직접 수정 전, `TextVisualizerGlyphRenderer` isolated cache lifecycle을 먼저 만든다.

다음 추천 작업:

```text
Add TextVisualizerGlyphRenderer glyph cache references
```

## 13. 진행: glyph cache references

`TextVisualizerGlyphRenderer`에 already-cached glyph ownership 기반을 추가했다.

추가된 구조:

- `GlyphCacheEntry`
  - `fontId`
  - `glyphIndex`
  - `AtlasGlyphManager::GlyphStyle`
  - `atlasId`
  - `imageId`
- `mGlyphCacheEntries`
- `mGlyphCacheSignature`
- `mHasGlyphCacheSignature`

추가된 lifecycle:

- `Render()`가 새 glyph sequence signature를 계산한다.
- 기존 signature와 같고 cache entries가 있으면 glyph reference acquire / release를 생략한다.
- signature가 다르면 already-cached glyph에 대해 `AtlasGlyphManager::AdjustReferenceCount(..., +1)`로 temporary refs를 acquire한다.
- cache miss는 여전히 `false`를 반환한다.
- mesh build 또는 actor creation 실패 시 temporary refs를 rollback한다.
- render 성공 시 old refs를 release하고 temporary entries를 commit한다.
- `Clear()`는 mesh / output actor lifecycle과 함께 owned glyph refs를 release한다.

이번 단계에서 의도적으로 하지 않은 것:

- cache miss handling
- `FontClient::CreateBitmap()`
- `AtlasGlyphManager::Add()`
- glyph bitmap 생성
- geometry-only update
- active `AtlasRendererBridge` path 연결

의미:

- current MVP는 더 이상 “남이 보유한 cached glyph slot을 읽기만 하는 smoke path”에 머물지 않고, 성공한 render에 대해서는 glyph refs를 보유할 수 있다.
- 같은 glyph sequence render에서는 ref count churn을 피할 수 있는 기반이 생겼다.
- cache miss가 있으면 여전히 fallback이 필요하므로 active path 연결은 아직 하지 않는다.

다음 후보:

```text
Design TextVisualizer glyph cache miss handling
```

## 16. 진행: lightweight renderer benchmark diagnostics

flag ON 실험을 관찰하기 위한 lightweight renderer diagnostics를 보강했다.

추가된 bridge getter:

- `GetLightweightGeometryOnlyUpdateCount()`
- `GetLightweightFullMeshRebuildCount()`
- `GetLightweightGlyphCacheEntryCount()`
- `GetLightweightMeshRecordCount()`
- `GetLightweightMeshTopologySignature()`
- `HasLightweightMeshTopologySignature()`

`TextVisualizerImpl::LogRenderDiagnostics()`에는 diagnostics flag가 켜진 경우에만 다음 항목을 추가로 출력한다.

- lightweight attempts
- lightweight successes
- lightweight fallbacks
- lightweight full mesh rebuilds
- lightweight geometry-only updates
- lightweight glyph cache entry count
- lightweight mesh record count
- lightweight topology signature state

기본 정책:

- `ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER` 기본값은 계속 `false`다.
- `ENABLE_TEXT_VISUALIZER_RENDER_DIAGNOSTICS` 기본값도 계속 `false`다.
- 기본 build에서는 기존 `Text::AtlasRenderer` path와 runtime logging behavior가 그대로 유지된다.
- performance sample status에는 표시하지 않는다. 해당 counters는 internal bridge state라 public API 없이 sample에서 접근하지 않는다.

local benchmark 절차:

1. `atlas-renderer-bridge.cpp`에서 `ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER`를 임시로 `true`로 바꾼다.
2. `text-visualizer-impl.cpp`에서 `ENABLE_TEXT_VISUALIZER_RENDER_DIAGNOSTICS`를 임시로 `true`로 바꾼다.
3. `text-visualizer-performance.example`을 실행한다.
4. log에서 `attempts`, `successes`, `fallbacks`, `fullRebuilds`, `geometryOnly`, `glyphCacheEntries`, `meshRecords`를 본다.
5. 실험 후 flag 변경은 되돌리고 커밋하지 않는다.

주의:

- 이 단계는 correctness fix가 아니다.
- 일부 glyph가 잘못 보이는 현상은 별도 glyph/atlas correctness diagnostics로 다룬다.
- cache miss handling은 여전히 없다.

## 17. 진행: fallback reason diagnostics

로컬에서 `ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER=true`와 `ENABLE_TEXT_VISUALIZER_RENDER_DIAGNOSTICS=true`로 performance sample을 실행했을 때 다음 관찰이 있었다.

```text
lightweight attempts=1144 successes=0 fallbacks=1144 fullRebuilds=0 geometryOnly=0 glyphCacheEntries=0 meshRecords=0
```

해석:

- lightweight renderer가 한 번도 성공하지 않았다.
- full mesh rebuild와 geometry-only update가 모두 0이므로 renderer backend 성능 비교는 아직 이루어진 것이 아니다.
- 현재 성능 차이가 체감되지 않는 것은 lightweight path가 실제 render output을 만들지 못하고 기존 `Text::AtlasRenderer`로 100% fallback하기 때문일 수 있다.

이번 진단에서 추가한 상태:

- failure reason enum
- failure reason별 counters
- last failed placement index
- last failed glyph placement index
- last failed font id
- last failed glyph id
- last failed glyph advance / width / height / bearing

분리해서 볼 실패 원인:

- empty input
- no renderable glyphs
- no renderer position cache
- glyph cache signature failure
- no glyph manager
- glyph placement lookup failure
- glyph info lookup failure
- renderer glyph position lookup failure
- glyph cache miss
- empty mesh
- no output actor
- no texture set

가장 의심되는 원인:

- space / zero-width / non-renderable glyph 하나가 cache miss 또는 empty mesh로 처리되어 전체 `Render()`가 `false`를 반환할 수 있다.
- 이 경우 다음 구현은 glyph cache miss handling이 아니라 먼저 non-renderable glyph skip policy일 수 있다.
- 반대로 실제 visible glyph가 cache miss라면 `CacheGlyph` equivalent 또는 helper extraction 설계가 필요하다.

diagnostics log에서 볼 항목:

- `failureCacheMiss`
- `failureEmptyMesh`
- `failurePlacement`
- `failureGlyphInfo`
- `failurePosition`
- `failureNoTexture`
- `lastFailureReason`
- `lastFailedPlacementIndex`
- `lastFailedGlyphIndex`
- `lastFailedFontId`
- `lastFailedGlyphId`
- `lastFailedAdvance`
- `lastFailedWidth`
- `lastFailedHeight`

주의:

- 기본 flag는 계속 `false`다.
- 이 단계는 cache miss handling이 아니다.
- `FontClient::CreateBitmap()`과 `AtlasGlyphManager::Add()`는 여전히 사용하지 않는다.
- glyph correctness fix도 아직 하지 않는다.

다음 후보:

```text
Skip TextVisualizerGlyphRenderer non-renderable glyphs
```

또는 로그 결과가 visible glyph cache miss라면:

```text
Design TextVisualizer glyph cache miss handling
```

## 18. 진행: skip non-renderable glyphs

fallback reason diagnostics로 다음 실패가 관찰됐다.

```text
main TextVisualizer:
attempts=1711 successes=0 fallbacks=1711
failure cacheMiss=1
lastGlyphId=3
lastAdvance=5.00
lastWidth=0.00
lastHeight=0.00
```

해석:

- `lastWidth=0`, `lastHeight=0`, `lastAdvance=5`인 glyph는 space 같은 advance-only glyph로 보인다.
- 이 glyph는 화면에 mesh로 그릴 대상이 아니므로 atlas에 없어도 정상이다.
- 이전 prototype은 이 glyph에도 `AtlasGlyphManager::IsCached()`를 호출했고, cache miss를 전체 `Render()` 실패로 처리했다.

변경 정책:

- glyph cache signature는 기존처럼 모든 glyph identity를 포함한다.
- glyph sequence identity에는 space도 포함되어야 하므로 `"ab"`와 `"a b"`는 다른 signature가 된다.
- `width == 0 && height == 0`인 glyph는 non-renderable로 보고 atlas lookup / ref acquire / mesh generation 대상에서 제외한다.
- `width > 0 || height > 0`인 visible glyph cache miss는 여전히 fallback한다.
- cache miss handling / glyph bitmap 생성 / atlas add는 여전히 구현하지 않았다.

의미:

- advance-only glyph 하나 때문에 lightweight renderer 전체가 fallback하는 false negative를 줄인다.
- 다음 flag ON 실험에서는 cache miss가 visible glyph에서 나는지, 아니면 renderer가 mesh를 실제로 만들 수 있는지 분리해서 볼 수 있다.

다음 후보:

```text
Enable lightweight renderer flag locally and inspect fallback diagnostics
```

또는 already-cached 환경을 전제로:

```text
Prototype TextVisualizer geometry-only mesh update
```

## 14. 진행: geometry-only mesh update prototype

`TextVisualizerGlyphRenderer`에 같은 glyph sequence / 같은 mesh topology에서 기존 mesh actor / renderer / geometry를 유지하고 vertex data만 갱신하는 prototype path를 추가했다.

추가된 상태:

- `mMeshTopologySignature`
- `mHasMeshTopologySignature`
- `mGeometryOnlyUpdateCount`
- `mFullMeshRebuildCount`

geometry-only update 조건:

- glyph cache signature가 기존 render와 같다.
- mesh topology signature가 기존 render와 같다.
- atlas id sequence가 같다.
- mesh record count가 같다.
- 각 mesh record의 actor / renderer / geometry / vertex buffer handle이 유효하다.
- vertex count와 index count가 기존 mesh와 같다.
- mesh actor가 현재 output actor 아래에 붙어 있다.

동작:

- 첫 render는 기존처럼 atlas id별 mesh actor / renderer / geometry / vertex buffer를 생성한다.
- 다음 render에서 glyph sequence와 topology가 같으면 `VertexBuffer::SetData()`로 새 vertex data만 반영한다.
- 이 path는 vertex color도 vertex data에 포함하므로 text color가 바뀌어도 topology가 같으면 geometry-only update가 가능하다.
- topology가 다르거나 handle이 유효하지 않으면 full mesh rebuild로 fallback한다.
- cache miss handling은 여전히 없다.
- active `AtlasRendererBridge` path에는 연결하지 않았다.

의미:

- Phase 2 primary target인 layout-dynamic workload에서 renderer object churn을 줄일 수 있는 최소 proof가 생겼다.
- glyph cache references와 결합되면서, 같은 glyph sequence에서는 ref count churn 없이 position/color vertex data만 갱신하는 방향을 확인했다.

남은 한계:

- cache miss는 여전히 `false`다.
- glyph bitmap 생성 / atlas add lifecycle은 아직 없다.
- active path 연결 전에는 fallback policy와 failure 시 old output 유지 정책을 다시 정해야 한다.
- topology가 같아도 atlas page split이 바뀌면 full rebuild가 필요하다.

다음 후보:

```text
Design TextVisualizer glyph cache miss handling
```

또는 active path 전환 준비를 위해:

```text
Integrate TextVisualizerGlyphRenderer behind internal fallback flag
```

## 15. 진행: optional lightweight renderer bridge path

`AtlasRendererBridge`에 `TextVisualizerGlyphRenderer`를 optional internal path로 연결했다.

정책:

- compile-time flag `ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER`의 기본값은 `false`다.
- 기본 build에서는 기존 `Text::AtlasRenderer` path만 사용하므로 public behavior는 바뀌지 않는다.
- flag를 켠 경우 bridge는 lightweight renderer를 먼저 시도한다.
- lightweight render 또는 attach가 실패하면 기존 `Text::AtlasRenderer` path로 fallback한다.
- cache miss handling은 아직 없으므로 fallback은 필수다.
- active benchmarking은 flag를 로컬에서 켠 뒤 별도 비교로 진행한다.

추가된 bridge 상태:

- `TextVisualizerGlyphRenderer mGlyphRenderer`
- `mLightweightRenderAttemptCount`
- `mLightweightRenderSuccessCount`
- `mLightweightRenderFallbackCount`

추가된 renderer API:

- `TextVisualizerGlyphRenderer::Render(const AtlasViewAdapter& adapter)`

의미:

- Phase 2 prototype을 active bridge lifecycle 근처에 배치했지만 기본값을 꺼 두어 안정성을 유지했다.
- 다음 단계에서 flag를 켜고 performance sample을 비교할 수 있는 최소 연결점이 생겼다.

남은 한계:

- cache miss handling은 여전히 없다.
- flag true 상태의 성능/시각 검증은 별도 작업이다.
- production 전환 전에는 failure 시 old output 유지, fallback 조건, render dirty clear 정책을 다시 확인해야 한다.

다음 후보:

```text
Enable lightweight renderer flag locally and benchmark performance sample
```

또는:

```text
Design TextVisualizer glyph cache miss handling
```

## 19. 진행: preserve lightweight renderer state across adapter updates

non-renderable glyph skip 이후 flag ON 로컬 실험에서 다음 로그가 관찰됐다.

```text
attempts=563
successes=562
fallbacks=1
fullRebuilds=1
geometryOnly=0
glyphCacheEntries=3137
meshRecords=2
outputSize=(0,0,0)
firstChildSize=(1354,970,0)
```

해석:

- lightweight renderer는 대부분 성공하므로 space / advance-only glyph fallback 문제는 크게 줄었다.
- 하지만 `geometryOnly=0`이므로 mesh state가 render 사이에서 유지되지 않았다.
- 원인 후보는 `AtlasRendererBridge::SetAdapter(non-null)`이 layout update마다 `mGlyphRenderer.Clear()`를 호출해 glyph cache / mesh topology / mesh records를 모두 지우는 것이었다.
- 또한 output actor size가 `(0,0,0)`이라 child mesh actor의 transform 기준이 불안정할 수 있었다.

변경 정책:

- `SetAdapter(nullptr)`와 `AtlasRendererBridge::Clear()`에서는 기존처럼 `TextVisualizerGlyphRenderer::Clear()`를 호출한다.
- `SetAdapter(non-null)`에서는 renderer state를 clear하지 않는다.
- adapter가 바뀌어도 `TextVisualizerGlyphRenderer::Render()`가 glyph cache signature와 mesh topology signature로 full rebuild 또는 geometry-only update를 판단한다.
- text / font / fontSize 변경으로 glyph sequence가 바뀌면 signature mismatch로 old refs release + full rebuild가 가능하다.
- `RenderAdapter()`는 output actor size를 adapter control size로 매 render마다 동기화한다.

기대:

- flag ON 재실험에서 첫 render는 full rebuild, 이후 같은 glyph sequence / topology render는 geometry-only update가 증가해야 한다.
- diagnostics log의 `outputSize`가 더 이상 `(0,0,0)`에 머물지 않아야 한다.

남은 확인:

- glyph correctness 문제는 별도 diagnostics가 필요하다.
- cache miss handling은 여전히 없다.
- default lightweight flag는 계속 `false`다.

## 20. 진행: fix lightweight renderer mesh actor bounds

flag ON 로컬 실험에서 state 보존 이후 다음 로그가 관찰됐다.

```text
attempts=483
successes=482
fallbacks=1
fullRebuilds=1
geometryOnly=481
glyphCacheEntries=3137
meshRecords=2
renderHostSize=(1354,970,0)
outputSize=(1354,970,0)
firstChildSize=(1354,970,0)
```

기존 `Text::AtlasRenderer` fallback path에서는 같은 상황에서 output / first child height가 약 `1152`까지 커졌다.

해석:

- lightweight renderer는 output actor와 mesh actor size를 adapter control size로 사용했다.
- 실제 layout / text content bounds는 control height보다 클 수 있다.
- actor bounds가 content보다 작으면 culling / clipping / transform 기준 차이로 일부 glyph가 보이지 않을 수 있다.

변경 정책:

- render host는 기존처럼 control size를 유지한다.
- `TextVisualizerGlyphRenderer`의 output actor와 mesh actor size는 layout size를 우선 사용한다.
- layout size가 0인 축은 control size로 fallback한다.
- layout size와 control size가 모두 유효하면 각 축별 `max(layoutSize, controlSize)`를 사용한다.
- geometry-only update path에서도 mesh actor size를 같은 actor size로 갱신한다.

의미:

- control보다 layout content가 큰 경우에도 lightweight renderer actor bounds가 content를 감쌀 수 있다.
- default lightweight flag와 diagnostics flag는 계속 `false`다.

남은 확인:

- flag ON 재실험에서 output / first child size가 layout content bounds에 맞게 커지는지 확인해야 한다.
- glyph correctness 문제가 bounds 문제만으로 해결되는지는 별도 확인이 필요하다.
- cache miss handling은 여전히 없다.

## 21. 진행: stabilize optional lightweight renderer experiment

`ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER=true`와
`ENABLE_TEXT_VISUALIZER_RENDER_DIAGNOSTICS=true` 로컬 실험에서 성능은 좋아졌지만,
`text-breaker.example`에서 `4` 키로 brick renderer를 반복 전환할 때 일부 glyph가 번쩍이거나 사라지는 현상이 관찰됐다.

이번 안정화는 lightweight renderer를 production default로 켜는 작업이 아니라, flag ON 실험이 더 정확한 비교가 되도록 prototype path의 명확한 차이를 줄이는 작업이다.

### flag 적용 전 / 후 차이

flag OFF 기본 경로:

- `AtlasRendererBridge`는 기존 `Text::AtlasRenderer::Render()`만 호출한다.
- render dirty마다 기존 text renderer가 view interface를 통해 glyph / position 배열을 받고, atlas mesh actor / renderer / geometry를 generic path로 다시 구성한다.
- glyph cache miss 처리는 기존 `Text::AtlasRenderer::Impl::CacheGlyph()`가 담당한다.
- 안정성은 기존 text stack과 같지만, moving exclusion처럼 glyph sequence는 같고 위치만 바뀌는 경우에도 full render update 비용이 남는다.

flag ON + lightweight 허용 경로:

- `AtlasRendererBridge`가 먼저 `TextVisualizerGlyphRenderer`를 시도한다.
- renderer가 성공하면 기존 `Text::AtlasRenderer::Render()`를 호출하지 않는다.
- 첫 성공 render는 lightweight renderer가 atlas id별 mesh actor를 만든다.
- 같은 glyph cache signature와 같은 mesh topology이면 이후 render는 actor / renderer / geometry를 유지하고 `VertexBuffer::SetData()`로 vertex data만 갱신한다.
- cache miss, invalid mesh, unsupported 상태에서는 기존 `Text::AtlasRenderer` path로 fallback한다.
- cache miss handling은 아직 없으므로 이미 atlas에 올라간 glyph만 lightweight renderer가 직접 처리한다.

성능적으로는 flag ON 경로가 layout-dynamic workload에서 유리하다.
특히 text / font / glyph sequence가 안정적이고 exclusion / layout / glyph positions만 바뀌는 경우, full actor / renderer / geometry rebuild를 피하고 vertex buffer update 중심으로 움직일 수 있다.
실제 로컬 실험에서도 position/exclusion 변화가 더 부드럽게 보였다.

반대로 content-dynamic workload에서는 이 경로가 아직 primary target이 아니다.
짧은 fragment, status text, brick text처럼 glyph sequence가 자주 바뀌거나 no-exclusion인 TextVisualizer는 기존 renderer path가 더 안전하다.

### lightweight renderer 사용 조건

기본 compile-time flag는 계속 `false`다.

flag를 로컬에서 `true`로 바꿔도, bridge가 무조건 lightweight renderer를 쓰지는 않는다.
`TextVisualizerImpl`은 현재 exclusion regions가 있는 경우에만 internal allow flag를 켠다.

즉 현재 lightweight renderer path는 다음 조건에서만 시도된다.

- `ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER == true`
- 해당 `TextVisualizer`에 non-empty exclusion regions가 있음
- adapter에 renderable glyphs와 renderer glyph position cache가 있음

이 정책의 이유:

- Phase 2 primary target은 layout-dynamic text다.
- exclusion이 있는 TextVisualizer는 moving bounds / glyph position update가 자주 발생하는 대표 workload다.
- no-exclusion text, short-lived effect text, status text, brick TextVisualizer는 existing `Text::AtlasRenderer` fallback을 사용해 안정성을 우선한다.

### 확인된 문제와 패치

1. advance-only glyph fallback

- 최초 flag ON 실험에서는 attempts는 많지만 successes가 0이었다.
- last failed glyph는 `advance=5`, `width=0`, `height=0`인 space 계열 glyph였다.
- non-renderable glyph는 atlas lookup / reference acquire / mesh generation에서 제외했다.
- glyph cache signature에는 계속 포함해 glyph sequence identity는 유지했다.

2. adapter update마다 renderer state reset

- non-renderable skip 후 success는 대부분 발생했지만 geometry-only count가 0이었다.
- `SetAdapter(non-null)`에서 `TextVisualizerGlyphRenderer::Clear()`를 호출해 mesh / cache / topology state를 매 layout update마다 잃고 있었다.
- non-null adapter update에서는 renderer state를 유지하고, `Render()` 내부 signature / topology 판단이 full rebuild 또는 geometry-only update를 결정하도록 바꿨다.

3. output actor size 0 / bounds mismatch

- output actor size가 `(0,0,0)` 또는 control size에 묶이는 문제가 있었다.
- output / mesh actor size는 layout size와 control size의 axis-wise max를 사용하도록 했다.
- render host는 기존 control size 정책을 유지한다.

4. glyph mesh coordinate origin mismatch

- 기존 `Text::AtlasRenderer`는 glyph position을 actor-center local coordinate로 변환한 뒤 atlas mesh를 만든다.
- lightweight renderer는 layout top-left coordinate를 그대로 써서 glyph가 우하단으로 밀려 보였다.
- `GenerateMeshData()` 전에 `round(position.x)`를 적용하고 `halfActorSize`를 빼 기존 renderer와 같은 coordinate convention으로 맞췄다.

5. geometry-only texture set stale 가능성

- geometry-only update는 기존 renderer / geometry를 유지하고 vertex buffer만 갱신했다.
- breaker처럼 다른 text controls가 atlas를 계속 건드리는 상황에서는 atlas texture set handle이 바뀔 수 있으므로, geometry-only update에서도 `renderer.SetTextures(glyphManager.GetTextures(atlasId))`를 다시 호출한다.

6. fallback 순간 output blank 가능성

- lightweight render 실패 시 기존 lightweight output을 즉시 detach하면 fallback renderer가 새 output을 준비하기 전 빈 프레임이 생길 수 있었다.
- fallback 시 즉시 detach하지 않고, fallback render가 성공한 뒤 기존 bridge output 교체 로직이 이전 output을 정리하도록 했다.

7. shader uniform `uOffset` 누락

- 기존 `Text::AtlasRenderer::CreateMeshActor()`는 mesh actor에 `uOffset` property를 `Vector2::ZERO`로 등록한다.
- atlas shader vertex stage는 `aPosition + uOffset`을 사용한다.
- lightweight renderer mesh actor에는 `uOffset`이 없어 일부 환경에서 undefined/unstable uniform 상태가 glyph flicker 또는 missing으로 보일 수 있었다.
- lightweight mesh actor도 `uOffset`을 명시적으로 등록해 기존 renderer와 맞췄다.

### 현재 남은 한계

- cache miss handling은 아직 없다.
- `FontClient::CreateBitmap()` / `AtlasGlyphManager::Add()` equivalent는 구현하지 않았다.
- default flag는 계속 `false`라 기본 build behavior는 기존 renderer와 같다.
- flag ON 실험에서 안정성이 좋아졌지만, production 전환 전에는 glyph cache miss handling과 fallback policy를 별도 커밋으로 다뤄야 한다.
