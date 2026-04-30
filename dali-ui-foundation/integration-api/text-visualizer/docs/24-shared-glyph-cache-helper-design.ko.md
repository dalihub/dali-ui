# TextVisualizer Shared Glyph Cache Helper Design

## 1. 목적

이 문서는 `TextVisualizerGlyphRenderer`의 cache miss handling을 구현하기 전에, 기존 `Text::AtlasRenderer`와 glyph cache behavior를 어떻게 공유할지 정리하는 설계 문서다.

현재 `TextVisualizerGlyphRenderer`는 already-cached glyph에 대해서는 다음까지 확인했다.

- atlas id별 mesh actor 생성
- glyph reference acquire / release / rollback
- 같은 glyph cache signature에서 ref count churn 회피
- 같은 glyph cache signature와 mesh topology에서 geometry-only `VertexBuffer::SetData()` update
- `AtlasRendererBridge` internal flag 뒤 optional bridge path 연결
- fallback을 통한 기존 `Text::AtlasRenderer` path 유지

하지만 visible glyph cache miss가 발생하면 아직 lightweight renderer는 `false`를 반환하고 기존 renderer로 fallback한다. production 수준의 optional active path로 가려면 다음 책임이 필요하다.

- glyph bitmap generation
- compressed glyph buffer handling
- `PixelData::New(..., PixelData::FREE)` ownership 처리
- atlas block size / atlas growth policy
- `AtlasGlyphManager::Add()`
- ref count acquire / rollback
- 기존 `AtlasRenderer::Impl::CacheGlyph()`와 behavior drift 방지

핵심 질문은 단순하다.

> `TextVisualizerGlyphRenderer`가 glyph cache miss를 자체 구현할 것인가, 아니면 기존 `Text::AtlasRenderer`의 cache glyph behavior를 shared internal helper로 추출해 함께 쓸 것인가?

## 2. 기존 AtlasRenderer Glyph Cache Lifecycle

`dali-ui-foundation/internal/text/rendering/atlas/text-atlas-renderer.cpp` 기준으로 기존 renderer의 glyph cache lifecycle은 `AtlasRenderer::Impl` private 구현 안에 있다.

### CacheGlyph 역할

`AtlasRenderer::Impl::CacheGlyph()`는 다음 단계를 수행한다.

| 단계 | 현재 동작 | 비고 |
|---|---|---|
| atlas size policy 조회 | `FontClient::GetDefaultTextAtlasSize()`, `GetMaximumTextAtlasSize()` | glyph bitmap 생성 전 기본/최대 atlas 크기 확보 |
| cache lookup | `AtlasGlyphManager::IsCached(fontId, glyphIndex, style, slot)` | font id / glyph index / style key 기준 |
| cache hit | `AtlasGlyphManager::AdjustReferenceCount(..., +1)` | 같은 glyph occurrence를 새 render가 소유 |
| cache miss | glyph bitmap 생성으로 진입 | visible glyph만 의미 있음 |
| bitmap 생성 | `FontClient::CreateBitmap()` | italic / bold / outline 인자 포함 |
| color glyph 확인 | `FontClient::IsColorGlyph()` | color glyph는 desired width / height 전달 |
| compressed buffer 처리 | `GlyphBufferData::Decompress()` 후 owned buffer로 교체 | `NO_COMPRESSION`이 아니거나 ownership이 없으면 새 buffer 할당 |
| pixel data 생성 | `PixelData::New(buffer, size, width, height, format, PixelData::FREE)` | `PixelData`가 buffer 해제를 맡음 |
| block size 갱신 | glyph bitmap width / height로 needed block size 확장 | `MaxBlockSize` 사용 |
| atlas growth | default atlas size를 maximum까지 2배씩 확장 가능 | `DOUBLE_PIXEL_PADDING` 여유 고려 |
| atlas size 설정 | `AtlasGlyphManager::SetNewAtlasSize()` | 다음 atlas add의 block / atlas size hint |
| atlas add | `AtlasGlyphManager::Add(glyph, style, bitmap, slot)` | miss path의 initial reference count가 됨 |

중요한 차이는 cache hit과 cache miss의 ref count 시작점이다.

- cache hit: `AdjustReferenceCount(..., +1)`로 새 owner를 추가한다.
- cache miss: `AtlasGlyphManager::Add()`가 새 glyph record를 만들고 initial count를 만든다.

### TextCacheEntry와 RemoveText

기존 renderer는 render마다 `TextCacheEntry` list를 만든다.

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

`RemoveText()`는 이 entry들을 순회하며 style key를 재구성하고 `AtlasGlyphManager::AdjustReferenceCount(..., -1)`를 호출한다.

기존 renderer의 중요한 commit 순서는 다음과 같다.

```text
new glyph refs acquire
-> RemoveText() old refs release
-> mTextCache.Swap(newTextCache)
```

old refs를 먼저 release하지 않는 이유는, 같은 glyph sequence를 다시 렌더링할 때 atlas glyph가 중간에 제거되는 상황을 피하기 위해서다. `TextVisualizerGlyphRenderer`도 이 순서를 유지해야 한다.

## 3. 현재 TextVisualizerGlyphRenderer Glyph Cache Lifecycle

현재 `TextVisualizerGlyphRenderer`는 다음 상태다.

- `AtlasGlyphManager::IsCached()`가 성공한 glyph만 처리한다.
- `width == 0 && height == 0`인 non-renderable glyph는 atlas lookup / ref acquire / mesh generation에서 제외한다.
- glyph cache signature에는 non-renderable glyph도 포함해 sequence identity를 유지한다.
- same glyph signature이면 existing refs를 유지하고 ref count churn을 피한다.
- signature가 바뀌면 temporary entries에 refs를 acquire한다.
- mesh build 성공 전까지 old entries는 유지한다.
- 실패 시 temporary acquired refs를 rollback한다.
- 성공 시 old refs를 release하고 new entries를 commit한다.
- same glyph signature + same mesh topology이면 actor / renderer / geometry를 유지하고 vertex data만 update한다.
- cache miss는 `GLYPH_CACHE_MISS` failure reason을 기록하고 `false`를 반환한다.
- bridge는 compile-time flag 뒤에서 lightweight renderer를 먼저 시도하고 실패 시 기존 renderer로 fallback한다.
- 기본 flag는 `false`라 production 기본 동작은 기존 renderer path다.
- flag ON에서도 exclusion regions가 없는 short-lived/status/fragment TextVisualizer는 기존 renderer를 우선 사용한다.

즉 현재 renderer는 ownership skeleton과 geometry-only path는 갖췄지만, glyph cache를 새로 채우는 시작점은 없다.

## 4. 문제 정의

현재 fallback은 안전하지만 다음 한계가 있다.

- visible glyph 하나라도 atlas에 없으면 lightweight render 전체가 fallback된다.
- fallback frame에서는 기존 `Text::AtlasRenderer::Render()` full rebuild 비용을 다시 탄다.
- cache miss가 자주 나는 text/font/fontSize 변경 직후에는 geometry-only path 이득이 늦게 나타날 수 있다.
- production optional path로 확대하려면 cache miss handling이 반드시 필요하다.

그렇다고 `TextVisualizerGlyphRenderer` 안에 `CacheGlyph()`를 그대로 복제하면 다음 위험이 생긴다.

- compressed glyph buffer ownership 처리 drift
- color glyph / BGRA atlas 처리 drift
- atlas block size growth policy drift
- outline / bold / italic style key drift
- `AtlasGlyphManager::Add()`와 ref count rollback 순서 drift
- 기존 `Label` / `InputField` / `TextView`와 visual correctness 차이

반대로 기존 `AtlasRenderer::Impl::CacheGlyph()`를 곧바로 외부 API로 노출하면 shared renderer regression 위험이 커진다.

따라서 먼저 helper boundary를 작게 설계하고, 기존 renderer가 helper를 사용해도 behavior가 바뀌지 않는 extraction부터 진행하는 것이 안전하다.

## 5. 설계 후보

### A. TextVisualizerGlyphRenderer local limited CacheGlyph implementation

`TextVisualizerGlyphRenderer`에 제한된 `CacheGlyph()` equivalent를 구현한다.

장점:

- 빠르게 독립 prototype을 완성할 수 있다.
- 기존 `text-atlas-renderer.*`를 건드리지 않는다.
- TextVisualizer scope에 맞춰 single color / no decoration path를 작게 유지할 수 있다.

단점:

- `FontClient::CreateBitmap()` / compressed buffer / `PixelData::FREE` / atlas growth policy가 중복된다.
- existing renderer와 behavior drift 가능성이 높다.
- color glyph / fallback font / style key edge case를 별도로 검증해야 한다.
- 나중에 shared helper로 다시 정리할 가능성이 크다.

판단:

- 실험 속도는 빠르지만, 이미 lightweight geometry-only path 가치가 확인된 상태에서는 production 방향으로 가기엔 유지보수 리스크가 크다.

### B. Extract AtlasRenderer::Impl::CacheGlyph into shared internal helper

`AtlasRenderer::Impl::CacheGlyph()` 책임을 새 internal helper로 추출한다.

예상 파일:

- `dali-ui-foundation/internal/text/rendering/atlas/text-atlas-glyph-cache-helper.h`
- `dali-ui-foundation/internal/text/rendering/atlas/text-atlas-glyph-cache-helper.cpp`

장점:

- 기존 renderer와 TextVisualizer renderer가 cache miss behavior를 공유한다.
- compressed bitmap / atlas growth / style key 정책 drift를 줄인다.
- `TextVisualizerGlyphRenderer`는 helper result와 rollback policy에 집중할 수 있다.

단점:

- `text-atlas-renderer.cpp`를 수정해야 한다.
- 기존 renderer no-behavior-change 검증이 필요하다.
- helper boundary가 너무 크면 기존 `AtlasRenderer::Impl` private state를 많이 노출할 수 있다.

판단:

- 장기 production 방향에 가장 맞다.
- 첫 구현은 기존 `AtlasRenderer`가 helper를 사용하도록 refactor하되, output behavior를 바꾸지 않는 커밋으로 분리해야 한다.

### C. Add helper near AtlasGlyphManager

`AtlasGlyphManager` 근처에 renderer-neutral helper 또는 `EnsureGlyphCached()` 계열 method를 둔다.

장점:

- cache storage owner 근처에 cache ensure 동작을 둘 수 있다.
- future renderer들이 같은 helper를 사용할 수 있다.

단점:

- `AtlasGlyphManager`에 `FontClient` dependency와 bitmap generation policy가 섞일 수 있다.
- manager가 storage뿐 아니라 glyph rasterization policy까지 갖게 되어 책임이 커진다.
- API 영향 범위를 잘라내기 어렵다.

판단:

- manager method로 직접 넣기보다는 별도 helper class가 낫다.
- helper가 `AtlasGlyphManager`와 `FontClient`를 받아 동작하는 형태가 더 안전하다.

### D. Hybrid staged extraction

작게 helper를 설계하고 단계적으로 연결한다.

단계:

1. shared helper interface를 문서화한다.
2. existing `AtlasRenderer::Impl::CacheGlyph()`가 helper를 사용하도록 extraction한다.
3. existing renderer UTC와 visual smoke로 no-behavior-change를 확인한다.
4. `TextVisualizerGlyphRenderer`가 cache miss에서 같은 helper를 사용한다.
5. flag ON benchmark와 correctness를 다시 확인한다.

장점:

- 기존 renderer behavior를 기준점으로 유지한다.
- `TextVisualizerGlyphRenderer` 중복 구현을 피한다.
- production path로 갈 때 correctness 설득력이 크다.

단점:

- A안보다 느리다.
- helper extraction 커밋에서 shared renderer regression 검증이 필요하다.

판단:

- 현재 단계의 추천 방향이다.

## 6. 추천 방향

추천은 **D안: staged shared helper extraction**이다.

이유:

- lightweight renderer의 layout-dynamic 성능 가치는 이미 flag ON 실험에서 확인됐다.
- 남은 핵심 blocker는 performance proof가 아니라 glyph cache correctness다.
- cache miss handling을 빠르게 중복 구현하면 단기 실험은 가능하지만, 기존 renderer와의 visual / cache lifecycle drift를 계속 추적해야 한다.
- existing `Text::AtlasRenderer`가 쓰던 cache glyph behavior를 helper로 먼저 빼면 `TextVisualizerGlyphRenderer`도 같은 policy를 사용할 수 있다.

권장 구현 순서:

1. `Add shared Atlas glyph cache helper skeleton`
   - helper type / request / result / release helper skeleton 추가
   - 아직 active path 연결 없음
2. `Extract AtlasRenderer glyph cache helper without behavior change`
   - 기존 `AtlasRenderer::Impl::CacheGlyph()`가 helper를 사용
   - 기존 renderer behavior no-change 검증
3. `Use shared glyph cache helper in TextVisualizerGlyphRenderer`
   - visible glyph cache miss에서 helper로 cache add + ref acquire
   - failure rollback 유지
4. `Benchmark TextVisualizer lightweight renderer cache miss handling`
   - flag ON performance sample / breaker sample 재검증
5. `Decide optional path policy`
   - exclusion-present layout-dynamic TextVisualizer에 한정할지
   - broader TextVisualizer에 확대할지

## 7. Shared Helper Interface 초안

helper는 renderer-neutral internal component로 두는 것이 좋다.

예상 namespace:

```cpp
namespace Dali::Ui::Internal::Text
```

또는 atlas directory convention을 따라:

```cpp
namespace Dali::Ui
```

단, public API가 아니므로 header 경로는 internal 아래에 둔다.

### Request

```cpp
struct GlyphCacheRequest
{
  Text::GlyphInfo                  glyph;
  AtlasGlyphManager::GlyphStyle    style;
  TextAbstraction::FontClient      fontClient;
  AtlasGlyphManager                glyphManager;
  Text::FontId                     lastFontId;
  std::vector<MaxBlockSize>*       blockSizes;
};
```

`MaxBlockSize`는 현재 `text-atlas-renderer.cpp` private type일 가능성이 있으므로, helper extraction 때 다음 중 하나를 선택해야 한다.

| 선택 | 설명 | 판단 |
|---|---|---|
| helper-local block state type 추가 | `AtlasGlyphCacheBlockState` 같은 type을 새 helper에 둠 | 추천 |
| existing private type 노출 | `MaxBlockSize`를 header로 이동 | 가능하지만 이름/책임 재검토 필요 |
| helper가 block size를 내부 계산 | caller가 font grouping state를 주지 않음 | 기존 behavior와 달라질 위험 |

### Result

```cpp
struct GlyphCacheResult
{
  bool                    success{false};
  bool                    wasCacheHit{false};
  bool                    acquiredReference{false};
  AtlasManager::AtlasSlot slot;
  uint32_t                atlasId{0u};
  uint32_t                imageId{0u};
  Pixel::Format           pixelFormat{Pixel::L8};
};
```

정책:

- `success == true`이면 caller는 해당 glyph reference를 하나 소유한다.
- cache hit이면 helper가 `AdjustReferenceCount(..., +1)`를 수행한다.
- cache miss이면 helper가 `AtlasGlyphManager::Add()`를 성공시키고 initial reference를 caller ownership으로 간주한다.
- 실패 시 helper는 ref count를 변경하지 않아야 한다.
- 실패 후 caller rollback 대상이 생기지 않아야 한다.

### Cache Entry

shared helper가 `TextCacheEntry`와 호환되는 entry type을 제공하는 것이 좋다.

```cpp
struct GlyphCacheEntry
{
  Text::FontId                  fontId{0u};
  Text::GlyphIndex              glyphIndex{0u};
  AtlasGlyphManager::GlyphStyle style;
  uint32_t                      imageId{0u};
  uint32_t                      atlasId{0u};
};
```

기존 `AtlasRenderer::Impl::TextCacheEntry`는 `atlasId`를 저장하지 않지만, `TextVisualizerGlyphRenderer`는 mesh topology와 diagnostics에 `atlasId`가 유용하다. shared entry에 `atlasId`를 포함하고 기존 renderer는 필요한 필드만 쓰거나, helper result와 기존 entry를 변환하는 방식이 가능하다.

### Methods

후보 함수:

```cpp
GlyphCacheResult EnsureGlyphCached(const GlyphCacheRequest& request);

void ReleaseGlyphReference(AtlasGlyphManager glyphManager,
                           const GlyphCacheEntry& entry);

void ReleaseGlyphReferences(AtlasGlyphManager glyphManager,
                            std::vector<GlyphCacheEntry>& entries);

AtlasGlyphManager::GlyphStyle BuildGlyphStyle(const Text::GlyphInfo& glyph,
                                               uint16_t outlineWidth);
```

추가로 helper가 block state를 관리한다면:

```cpp
class AtlasGlyphCacheHelper
{
public:
  explicit AtlasGlyphCacheHelper(AtlasGlyphManager glyphManager,
                                 TextAbstraction::FontClient fontClient);

  void ResetBlockState();
  void PrepareBlockStateForGlyphs(const Vector<Text::GlyphInfo>& glyphs);
  GlyphCacheResult EnsureGlyphCached(const Text::GlyphInfo& glyph,
                                     const AtlasGlyphManager::GlyphStyle& style,
                                     Text::FontId lastFontId);
};
```

첫 extraction은 기존 renderer behavior를 보존해야 하므로, `CalculateBlocksSize()` 또는 equivalent block state 준비 흐름을 helper와 어떻게 나눌지 먼저 봐야 한다.

## 8. Invalidation / Rollback Policy

`TextVisualizerGlyphRenderer`의 current policy는 유지한다.

### same signature

```text
same glyph cache signature
-> helper 호출 없음
-> ref count 변경 없음
-> pending mesh build
-> compatible topology면 geometry-only update
```

### signature changed

```text
newEntries = []
for each renderable glyph:
  result = EnsureGlyphCached(...)
  if fail:
    ReleaseGlyphReferences(newEntries)
    keep old entries / old mesh
    return false
  newEntries.push(result.entry)

build pending meshes
if mesh build fail:
  ReleaseGlyphReferences(newEntries)
  keep old entries / fallback
  return false

commit:
  ReleaseGlyphReferences(oldEntries)
  oldEntries = newEntries
  store new signature
```

### fallback output policy

flag ON optional path에서도 fallback은 반드시 유지한다.

- lightweight render failure는 old lightweight mesh를 즉시 blank로 만들지 않는다.
- fallback `Text::AtlasRenderer` render 성공 후 bridge output swap이 old output을 정리한다.
- fallback render도 실패하면 old output 유지 여부는 별도 policy로 둔다.

### old refs release 순서

old refs는 new refs와 mesh build가 성공한 뒤 release한다. 이 순서는 기존 `AtlasRenderer::AddGlyphs()`의 `RemoveText()` / `mTextCache.Swap(newTextCache)` 순서를 따른다.

## 9. Risk

| 리스크 | 설명 | 완화 방향 |
|---|---|---|
| compressed bitmap ownership | `GlyphBufferData`가 ownership이 없거나 compressed인 경우 새 buffer와 free 책임이 복잡함 | helper가 `PixelData::FREE`까지 기존 흐름 그대로 보유 |
| `PixelData::FREE` ownership | `PixelData` 생성 후 `glyphBufferData.isBufferOwned=false` 전환 누락 가능 | helper extraction no-behavior-change test 필요 |
| atlas growth side effect | block size / atlas size hint가 기존과 달라지면 atlas split이 달라질 수 있음 | 기존 block state와 size policy 먼저 helper로 옮김 |
| color glyph / BGRA atlas | emoji/color glyph는 desired size와 shader path가 다름 | cache helper는 pixel format / atlas id를 result에 포함 |
| outline / style key | outline, synthetic bold, italic style key mismatch 가능 | `BuildGlyphStyle()` helper로 통일 |
| ref count leak | render 실패 중 acquired refs rollback 누락 | temporary entries + rollback UTC |
| premature release | old refs를 먼저 release하면 same glyph가 atlas에서 사라질 수 있음 | success commit 후 old release |
| existing renderer regression | helper extraction이 Label/InputField rendering을 바꿀 수 있음 | first extraction commit은 existing renderer no-behavior-change에 집중 |
| duplicate entry shape | 기존 `TextCacheEntry`와 new entry가 갈라질 수 있음 | shared `GlyphCacheEntry` 또는 explicit conversion helper |
| atlas manager responsibility creep | manager에 FontClient policy가 섞일 수 있음 | manager method 대신 separate helper 선호 |

## 10. Validation Plan

### helper extraction no-behavior-change

- existing `UtcDaliTextVisualizer*`
- existing atlas renderer 관련 UTC
- Label / InputField text rendering smoke 가능하면 실행
- text atlas metrics / glyph count smoke
- glyph cache hit path
- glyph cache miss path
- repeated same text render
- outline / bold / italic smoke 가능하면 실행
- color glyph / emoji smoke 가능하면 실행

### TextVisualizerGlyphRenderer integration

- cache miss visible glyph가 fallback 없이 helper로 cached 되는지
- helper success 시 glyph cache entry count 증가
- `Clear()`가 refs release
- render failure rollback에서 entry count leak 없음
- repeated same glyph sequence에서 helper 호출 / ref churn 없음
- same glyph sequence + same topology에서 geometry-only count 증가
- cache miss 후 다음 frame geometry-only 전환 가능 여부

### samples

- `text-visualizer-performance.example`
  - flag OFF 기본 behavior 확인
  - flag ON + diagnostics에서 attempt / success / fallback / full rebuild / geometry-only 확인
- `text-breaker.example`
  - `4` brick renderer toggle 반복
  - destroy effect / short-lived text fallback policy 확인
  - flicker / missing glyph 재확인

### diagnostics

flag ON 실험 절차:

1. `ENABLE_TEXT_VISUALIZER_LIGHTWEIGHT_RENDERER=true`로 임시 변경
2. `ENABLE_TEXT_VISUALIZER_RENDER_DIAGNOSTICS=true`로 임시 변경
3. sample 실행
4. 로그에서 다음을 확인
   - attempts
   - successes
   - fallbacks
   - fullRebuilds
   - geometryOnly
   - glyphCacheEntries
   - meshRecords
   - failureCacheMiss
5. 실험 후 flag 변경은 commit하지 않음

## 11. 다음 구현 추천

다음 구현 커밋은 아래 둘 중 하나가 자연스럽다.

### 1순위: Add shared Atlas glyph cache helper skeleton

범위:

- helper header / cpp 추가
- request / result / entry / release helper type 추가
- existing renderer와 lightweight renderer에 아직 연결하지 않음
- UTC로 empty / release no-op / style builder 정도 확인

장점:

- `text-atlas-renderer.cpp` behavior를 바로 건드리지 않고 helper boundary를 코드로 고정할 수 있다.
- 다음 extraction 커밋의 diff를 작게 만들 수 있다.

### 2순위: Extract AtlasRenderer glyph cache helper without behavior change

범위:

- existing `AtlasRenderer::Impl::CacheGlyph()` logic을 helper로 이동
- existing renderer가 helper를 사용
- `TextVisualizerGlyphRenderer`는 아직 cache miss에서 fallback 유지
- 기존 renderer no-behavior-change 검증에 집중

장점:

- 곧바로 behavior sharing 기반을 만든다.

주의:

- `text-atlas-renderer.*`를 수정하므로 UTC / sample smoke를 더 넓게 잡아야 한다.

## 12. 결론

`TextVisualizerGlyphRenderer`의 Phase 2 prototype은 layout-dynamic workload에서 geometry-only update가 유효하다는 점을 보여줬다. 이제 남은 핵심은 renderer geometry가 아니라 glyph cache ownership의 production correctness다.

따라서 다음 단계는 local limited `CacheGlyph()` 중복 구현보다, 기존 `Text::AtlasRenderer`와 cache miss behavior를 공유할 수 있는 internal helper boundary를 만드는 것이다.

추천 방향은 staged shared helper extraction이다.

```text
helper skeleton
-> existing AtlasRenderer no-behavior-change extraction
-> TextVisualizerGlyphRenderer cache miss handling
-> flag ON benchmark / correctness
-> optional path policy decision
```

이 순서가 `Label` / `InputField` regression risk를 제어하면서도, `TextVisualizer` lightweight renderer를 production-grade optional path로 끌어올리는 가장 안전한 경로다.
