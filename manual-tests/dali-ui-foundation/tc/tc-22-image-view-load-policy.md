# 22. ImageView: LoadPolicy / ReleasePolicy

LoadPolicy 및 ReleasePolicy 설정에 따라 이미지 로딩/해제 동작을 확인한다.

판정은 `ResourceReadySignal` 카운터의 **전후 변화량**이다 — Remove→Re-Add 후 화면에는
어느 정책이든 이미지가 다시 나타나므로, "캐시에서 꺼냈나 다시 읽었나"를 밖에서 알 수 있는
창구는 카운터뿐이다. 절대값은 상황에 따라 다르므로 단언하지 않는다. 정책 버튼 자체도
비주얼을 재생성해 카운터를 +1 올린다는 것을 산수에 넣을 것.

세 정책은 **해제 시점**이 다르다 (API 정의, `image-enumerations.h`):

| 정책 | 해제 시점 | Remove→Re-Add에서 |
|---|---|---|
| DETACHED | 씬에서 뗄 때 | 해제됨 → **다시 읽음** (카운터 +1) |
| DESTROYED | 비주얼이 **파괴**될 때 | 해제 안 됨 → **캐시 재사용** (카운터 그대로) |
| NEVER | 절대 안 함 | 캐시 재사용 (카운터 그대로) |

Remove→Re-Add는 씬에서 떼었다 붙이는 것이지 비주얼 파괴가 아니므로, **DESTROYED가 이
조작에서 NEVER처럼 동작하는 것이 맞다** (2026-08-21 "리로드 안 함"은 결함이 아니라 명세가
두 정책을 잘못 묶은 것 — 2026-08-26 실측 재확인: 진입 1 → DESTROYED 2 → Remove 2 → Re-Add 2).

## 화면 구성

- 중앙: 이미지 프리뷰 (180x180) — 접근성 이름 `ImagePreview`
- 상태 라벨: LoadPolicy / ReleasePolicy / View 상태 / ResourceReadySignal 카운트 (전부 진짜 getter)
- 버튼 행 1: Load IMMEDIATE / Load ATTACHED
- 버튼 행 2: Release DETACHED / Release DESTROYED / Release NEVER
- 버튼 행 3: Remove View / Re-Add View

## 테스트 1: ReleasePolicy NEVER

1. [Release: NEVER] 버튼을 탭한다 (카운터 +1 — 정책 변경의 부수효과)
2. [Remove View] 버튼을 탭한다
3. **기대 결과**: 이미지가 화면에서 사라짐, 카운터 불변
4. [Re-Add View] 버튼을 탭한다
5. **기대 결과**: 캐시에서 즉시 복원 — **카운터 불변**

## 테스트 2: ReleasePolicy DETACHED

1. [Release: DETACHED] 버튼을 탭한다 (카운터 +1)
2. [Remove View] → [Re-Add View] 버튼을 탭한다
3. **기대 결과**: 다시 로딩됨 — **카운터 +1**

## 테스트 3: ReleasePolicy DESTROYED

1. [Release: DESTROYED] 버튼을 탭한다 (카운터 +1)
2. [Remove View] → [Re-Add View] 버튼을 탭한다
3. **기대 결과**: 캐시 재사용 — **카운터 불변** (NEVER와 동일; 뷰 객체가 살아 있어
   비주얼이 파괴되지 않았기 때문)

## 통과 기준

- NEVER: Re-Add 시 카운터 **불변** (캐시 재사용)
- DETACHED: Re-Add 시 카운터 **+1** (리로드)
- DESTROYED: Re-Add 시 카운터 **불변** (해제 시점이 비주얼 파괴이므로)
- 픽셀 보강(정적 화면): Re-Add 후 캡처가 Remove 전과 **같아야** 한다 — 카운터가
  "다시 읽었는지"를, 픽셀이 "결과가 같은지"를 맡아 서로를 보강한다
