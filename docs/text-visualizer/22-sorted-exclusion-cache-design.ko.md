# TextVisualizer Sorted Exclusion Cache Design

## 1. 목적

현재 `LayoutGlyphs()` / `LayoutPlaceholder()`는 layout pass 시작 시 exclusion regions를 y 기준으로 정렬한다.

moving bounds 상황에서는 exclusion이 자주 바뀌지만, 같은 exclusion version 안에서도 다음 상황에서는 sorted cache를 재사용할 가능성이 있다.

- `OnMeasure()`와 `OnRelayout()`이 같은 exclusion set을 사용한다.
- measure cache miss 이후 같은 exclusion set으로 여러 width를 측정한다.
- 향후 partial relayout / available interval cache가 같은 exclusion set을 여러 번 조회한다.

이 문서는 sorted exclusion cache를 `TextVisualizerImpl`로 올릴지, `LayoutEngine` internal type으로 둘지 설계한다.

## 2. 현재 구조

현재 `dali-ui-foundation/internal/text/text-visualizer/layout-engine.cpp`의 exclusion 관련 구조는 다음과 같다.

| 항목 | 현재 위치 | 공개 범위 | 역할 |
|---|---|---|---|
| `SortedExclusionRegion` | `layout-engine.cpp` anonymous namespace | cpp-local | `Rect<float>`와 `top` / `bottom` cache |
| `SortedExclusionRegions` | `layout-engine.cpp` anonymous namespace | cpp-local | `std::vector<SortedExclusionRegion>` alias |
| `BuildSortedExclusionRegions()` | `layout-engine.cpp` anonymous namespace | cpp-local | `Dali::Vector<Rect<float>>`를 y-sort cache로 변환 |
| `BuildAvailableIntervalsFromSorted()` | `layout-engine.cpp` anonymous namespace | cpp-local | sorted regions를 line y band로 scan |
| `LayoutGlyphs()` | `LayoutEngine` public static internal API | header exposed | glyph placement layout |
| `LayoutPlaceholder()` | `LayoutEngine` public static internal API | header exposed | placeholder cluster layout |
| `BuildAvailableIntervals()` | `LayoutEngine` public static internal API | header exposed | test/helper용 vector 기반 interval build |

현재 `TextVisualizerImpl`이 볼 수 있는 것은 `Dali::Vector<Rect<float>> mExclusionRegions`뿐이다. sorted representation은 `LayoutEngine` 안에서 layout pass마다 생성되고 사라진다.

## 3. 현재 비용

exclusion count를 `N`이라고 하면 현재 비용은 다음과 같다.

- 매 layout pass마다 `BuildSortedExclusionRegions()`에서 `O(N log N)` sort
- 각 line마다 sorted list를 y-overlap 기준으로 scan
- scan 이후 line-local blocked interval x-sort / merge

최근 measured layout cache로 `OnMeasure()` 직후 같은 width의 `OnRelayout()`은 layout 계산 자체를 재사용할 수 있다. 하지만 cache miss path에서는 여전히 layout pass마다 sort가 발생한다.

performance sample에서는 orb exclusion을 ellipse bands로 세분화한다. orb 5개, band 11개면 orb만 55개 region이 생기고, 여기에 title / drop cap / fixed / overlay exclusions가 추가될 수 있다. region 수가 커질수록 sort 위치와 cache boundary가 중요해진다.

## 4. 후보 설계

### A. LayoutEngine 내부 유지

현재 구조를 유지한다.

장점:

- API가 가장 단순하다.
- `LayoutEngine`이 self-contained 상태를 유지한다.
- stale cache risk가 없다.
- existing UTC와 helper 사용이 그대로 유지된다.

단점:

- sort cache 재사용이 불가능하다.
- `TextVisualizerImpl`이 exclusion 변경 여부를 알고 있어도 sorted cache를 보유할 수 없다.
- 향후 partial relayout / interval cache를 넣을 때 다시 type 경계를 바꿔야 할 가능성이 높다.

### B. SortedExclusionRegions type을 header로 이동

`SortedExclusionRegion` / `SortedExclusionRegions`를 `layout-types.h` 또는 새 header로 이동한다.

예상 header:

```text
dali-ui-foundation/internal/text/text-visualizer/exclusion-types.h
```

예상 type:

```cpp
struct SortedExclusionRegion
{
  Rect<float> rect;
  float top{0.0f};
  float bottom{0.0f};
};

using SortedExclusionRegions = std::vector<SortedExclusionRegion>;
```

`LayoutEngine`에 overload를 추가한다.

```cpp
LayoutGlyphs(preparedText, width, lineHeight, sortedExclusionRegions, result)
LayoutPlaceholder(preparedText, width, lineHeight, sortedExclusionRegions, result)
BuildAvailableIntervals(layoutWidth, lineY, lineHeight, sortedExclusionRegions)
```

기존 vector API는 유지하고 내부에서 sorted cache를 만든다.

장점:

- `TextVisualizerImpl`이 sorted cache를 member로 보유할 수 있다.
- `OnMeasure()` / `OnRelayout()` / 향후 partial relayout에서 같은 sorted cache를 재사용할 수 있다.
- 구현 diff가 비교적 작다.

단점:

- `std::vector` 기반 type이 header에 노출된다.
- cache invalidation 책임이 caller로 일부 이동한다.
- sorted cache build helper를 어디에 둘지 결정해야 한다.

### C. ExclusionLayoutCache class 추가

internal-only `ExclusionLayoutCache` class를 추가한다.

예상 책임:

```cpp
class ExclusionLayoutCache
{
public:
  void Clear();
  void SetRegions(const Dali::Vector<Rect<float>>& regions);
  const SortedExclusionRegions& GetSortedRegions() const;
  uint64_t GetVersion() const;
  bool Empty() const;
};
```

장점:

- cache / invalidation 책임이 명확하다.
- `TextVisualizerImpl`이 raw sorted vector를 직접 만지지 않아도 된다.
- 향후 static / dynamic split, y bucket, affected range cache로 확장하기 쉽다.
- `SetExclusionRegions()` exact compare / same-count in-place update와 역할을 분리할 수 있다.

단점:

- 새 abstraction이 추가된다.
- 단순 sort reuse치고는 구현 범위가 커질 수 있다.
- `LayoutEngine`이 cache class를 받을지, sorted vector만 받을지 API 경계를 정해야 한다.

### D. LayoutEngine callback/cache provider

`LayoutEngine`이 callback 또는 provider interface를 통해 line interval을 가져오게 하는 설계다.

장점:

- 향후 available interval cache / y bucket / partial relayout까지 확장 가능하다.

단점:

- 현재 단계에서는 과하다.
- layout policy와 cache policy가 강하게 섞일 수 있다.
- word wrap / line y progression 디버깅이 어려워진다.

이번 단계에서는 보류한다.

## 5. 추천 방향

장기적으로는 C, 즉 `ExclusionLayoutCache`가 가장 낫다.

이유:

- sorted cache는 단순 vector가 아니라 “exclusion regions의 derived layout data”다.
- 향후 static / dynamic split, y bucket, interval cache, affected y range cache까지 같은 축으로 확장될 가능성이 높다.
- cache stale risk를 줄이려면 cache 소유권과 version을 한 곳에 묶는 편이 좋다.

단기 구현은 C의 작은 형태를 추천한다.

추천 1차 구현:

1. `internal/text/text-visualizer/exclusion-layout-cache.h/cpp` 추가
2. `SortedExclusionRegion` / `SortedExclusionRegions`를 이 header로 이동
3. `ExclusionLayoutCache::SetRegions()`에서 sort cache rebuild
4. `LayoutEngine`에 sorted cache overload 추가
5. 기존 vector API는 유지하고 내부에서 temporary `ExclusionLayoutCache` 또는 helper를 사용
6. `TextVisualizerImpl`이 `mExclusionLayoutCache`를 member로 보유
7. `SetExclusionRegions()` / `ClearExclusionRegions()`에서 cache update / clear

이렇게 하면 public API 없이 `TextVisualizerImpl`만 optimized path를 사용할 수 있다.

## 6. Invalidation 정책

sorted exclusion cache는 rect set에만 의존한다.

| 변화 | sorted exclusion cache |
|---|---|
| `SetExclusionRegions()` changed | rebuild |
| `SetExclusionRegions()` same values | keep |
| same-count in-place update | rebuild |
| `ClearExclusionRegions()` | clear |
| text 변경 | keep |
| font family 변경 | keep |
| font size 변경 | keep |
| line height 변경 | keep |
| text color 변경 | keep |
| layout width / size 변경 | keep |

주의할 점:

- cache rebuild는 `AreExclusionRegionsEqual()`이 false인 경우에만 수행한다.
- `UpdateStoredExclusionRegions()`가 same-count in-place update를 수행해도 sorted cache는 stale이므로 반드시 rebuild해야 한다.
- measured layout cache는 layout result cache이므로 exclusion 변경 시 clear한다. sorted exclusion cache는 exclusion 변경 시 rebuild한다.

## 7. API 영향

public API 영향은 없다.

internal API 영향:

- `LayoutEngine::LayoutGlyphs()` / `LayoutPlaceholder()` vector overload는 유지한다.
- sorted cache overload가 추가된다.
- `BuildAvailableIntervals()` vector overload는 유지한다.
- test helper는 기존 vector API를 계속 사용할 수 있다.

파일 영향 후보:

- `internal/text/text-visualizer/exclusion-layout-cache.h`
- `internal/text/text-visualizer/exclusion-layout-cache.cpp`
- `internal/text/text-visualizer/layout-engine.h`
- `internal/text/text-visualizer/layout-engine.cpp`
- `integration-api/text-visualizer-impl.h`
- `integration-api/text-visualizer-impl.cpp`
- `automated-tests/src/dali-ui-foundation/utc-Dali-TextVisualizer.cpp`

## 8. 성능 기대

기대 효과:

- 같은 exclusion set으로 여러 layout 계산이 발생할 때 `O(N log N)` sort를 한 번만 수행한다.
- measured layout cache miss path에서도 sorted cache는 재사용할 수 있다.
- moving bounds에서는 매 tick exclusion set이 바뀌므로 sort 횟수 자체는 tick마다 필요할 수 있지만, sort 비용을 layout pass 밖의 update 단계로 이동할 수 있다.
- 향후 partial relayout / interval cache의 기반이 된다.

제한:

- exclusion이 매 tick 바뀌면 sorted cache rebuild는 여전히 매 tick 발생한다.
- line-local blocked interval sort / merge는 그대로 남는다.
- word wrap range scan과 full relayout은 별도 병목이다.

## 9. 위험

주요 위험은 다음과 같다.

- stale sorted cache로 인한 잘못된 text avoidance
- `LayoutEngine` overload 증가로 테스트 경로가 나뉨
- `std::vector` / `Dali::Vector` 혼합 사용에 따른 ownership 혼동
- sort 위치가 `SetExclusionRegions()`로 이동하면서 profiling 해석이 바뀜
- future static / dynamic split과 중복 설계 가능성
- placeholder / glyph layout 양쪽에서 같은 sorted cache를 정확히 쓰는지 검증 필요

위험 완화:

- vector overload를 유지해 fallback path 보존
- sorted cache와 vector path가 같은 layout signature를 만드는 UTC 추가
- exclusion changed / cleared invalidation UTC 추가
- `ExclusionLayoutCache`에 version을 둬 debugging 가능성을 열어둠

## 10. 다음 구현 후보

추천 커밋:

```text
Add TextVisualizer ExclusionLayoutCache
```

권장 범위:

1. `ExclusionLayoutCache` type/header 추가
2. `LayoutEngine` sorted overload 추가
3. 기존 vector overload 유지
4. `TextVisualizerImpl`만 sorted cache path 사용
5. UTC:
   - vector path와 sorted cache path layout signature 동일
   - `SetExclusionRegions()` changed 시 cache rebuild smoke
   - `ClearExclusionRegions()` clear smoke
   - placeholder path sorted cache smoke

이번 설계의 결론은 “바로 `layout-types.h`에 raw sorted vector를 노출하기보다, 작은 internal `ExclusionLayoutCache`로 cache ownership을 먼저 세우는 것”이다.

## 11. 진행: ExclusionLayoutCache

이번 구현에서 설계안 C의 작은 형태를 적용했다.

추가된 구조:

- `internal/text/text-visualizer/exclusion-layout-cache.h`
- `internal/text/text-visualizer/exclusion-layout-cache.cpp`
- `SortedExclusionRegion`
- `SortedExclusionRegions`
- `ExclusionLayoutCache`

`ExclusionLayoutCache::SetRegions()`는 raw `Dali::Vector<Rect<float>>`를 받아 `top` / `bottom` 값을 미리 계산하고 y 기준으로 정렬한다. `Clear()`와 `SetRegions()`는 version을 증가시켜 cache 변경 추적이 가능하게 했다.

`LayoutEngine`에는 다음 sorted cache overload를 추가했다.

- `BuildAvailableIntervals(..., const ExclusionLayoutCache&)`
- `LayoutPlaceholder(..., const ExclusionLayoutCache&, LayoutResult&)`
- `LayoutGlyphs(..., const ExclusionLayoutCache&, LayoutResult&)`

기존 vector 기반 API는 유지했다. 기존 API는 compatibility/test path로 남기고, 내부에서 temporary `ExclusionLayoutCache`를 만들어 새 overload로 위임한다. 따라서 기존 caller의 behavior는 유지된다.

`TextVisualizerImpl`은 `mExclusionLayoutCache`를 member로 보유한다.

적용 정책:

- `SetExclusionRegions()`에서 exact compare가 false인 경우 `mExclusionRegions`를 갱신한 뒤 `mExclusionLayoutCache.SetRegions(mExclusionRegions)`를 호출한다.
- `ClearExclusionRegions()`는 raw regions와 sorted cache를 함께 clear한다.
- `UpdateLayout()`은 raw `mExclusionRegions` 대신 cached sorted exclusions를 사용하는 `LayoutEngine` overload를 호출한다.
- text / font / fontSize / lineHeight / color / layout size 변경은 sorted exclusion cache를 clear하지 않는다.
- measured layout cache는 기존처럼 exclusion 변경 시 clear된다.

검증 방향:

- `ExclusionLayoutCache` empty / set / clear UTC
- unsorted input이 top 기준으로 정렬되는지 확인
- vector path와 sorted cache path의 available intervals가 같은지 확인
- `LayoutGlyphs()` vector path와 cache path의 `LayoutResult::CalculateSignature()`가 같은지 확인
- `LayoutPlaceholder()` vector path와 cache path의 signature가 같은지 확인

남은 한계:

- moving bounds처럼 exclusion set이 매 tick 바뀌면 cache rebuild 자체는 계속 발생한다.
- line-local blocked interval sort / merge는 여전히 layout 중 수행된다.
- static / dynamic core split, y bucket cache, available interval cache는 다음 단계 후보로 남긴다.
