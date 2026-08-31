# Lottie: ReleasePolicy / SyncLoading / Placeholder

SetReleasePolicy / GetReleasePolicy, SetSynchronousLoading / IsSynchronousLoading,
SetPlaceholderUrl / GetPlaceholderUrl, GetLoadingStatus 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: `Release: X | Sync: Y | PH: none|SET`
- 신호 라벨: `ResourceReadySignal: N | View: in scene|REMOVED | Load: PREPARING|READY|FAILED`
  — 두 라벨의 필드 전부 진짜 getter다 (`GetReleasePolicy()`, `IsSynchronousLoading()`,
  `GetPlaceholderUrl()`, `GetParent()`, `GetLoadingStatus()`)
- 버튼 행 1: Release NEVER / Release DETACHED / Release DESTROYED
- 버튼 행 2: Sync ON / Sync OFF
- 버튼 행 3: Set Placeholder / Clear Placeholder / Reload URL / Bad URL
- 버튼 행 4: Remove View / Re-Add View

`ResourceReadySignal`은 **실패한 로드에도 발생한다** (실측 — 카운터만으로는 성공/실패를
구분할 수 없고, `Load:` 필드가 그 구분을 준다).

## 테스트 1: ReleasePolicy (캐시 히트 vs 리로드)

1. [Release: NEVER] 버튼을 탭한다
2. [Remove View] → [Re-Add View] 버튼을 탭한다
3. **기대 결과**: ResourceReadySignal 카운터가 **움직이지 않는다** (캐시에서 즉시 표시, 리로드 없음)
4. [Release: DETACHED] 버튼을 탭한다
5. [Remove View] 버튼을 탭한다
6. **기대 결과**: 제거된 동안 `Load: PREPARING` — DETACHED가 리소스를 해제한 것이
   getter로 직접 보인다 (NEVER에서는 같은 상태가 `Load: READY`였다, 실측 2026-08-26)
7. [Re-Add View] 버튼을 탭한다
8. **기대 결과**: 카운터가 **움직이고** `Load: READY` (리로드 발생)

## 테스트 2: GetLoadingStatus (성공/실패 구분)

1. 화면 진입 직후 확인한다
2. **기대 결과**: `Load: READY`
3. [Bad URL] 버튼을 탭한다 (존재하지 않는 경로 로드)
4. **기대 결과**: `Load: FAILED`, 신호 카운터 +1 (실패도 신호를 쏜다)
5. [Reload URL] 버튼을 탭한다
6. **기대 결과**: `Load: READY`, 신호 카운터 +1

## 테스트 3: Placeholder

1. [Set Placeholder] 버튼을 탭한다
2. **기대 결과**: `PH: SET` (GetPlaceholderUrl이 비어 있지 않음)
3. [Clear Placeholder] 버튼을 탭한다
4. **기대 결과**: `PH: none`, 신호 카운터 불변 (placeholder setter는 본 비주얼을 건드리지 않는다)

로딩 중/실패 시의 placeholder **표시**(픽셀)는 이 화면의 대상이 아니다 —
`tc-lottie-fitting-placeholder.md`가 같은 경로를 안정 상태로 검증한다.

## 테스트 4: SynchronousLoading

1. [Sync ON] 버튼을 탭한다
2. **기대 결과**: 탭 직후 한 번 읽은 라벨이 `Sync: ON` (기본값 OFF에서 벗어남)
3. [Reload URL] 버튼을 탭한다
4. **기대 결과**: `Load: READY` — "즉시"라는 지연 시간 주장은 바깥에서 잴 수단이 없어
   값 왕복과 로드 완료까지만 확인한다

## 통과 기준

- NEVER 정책: Re-Add 시 신호 카운터 불변(리로드 없음), DETACHED 정책: 카운터 증가(리로드)
- GetLoadingStatus가 성공(READY)과 실패(FAILED)를 구분해 보고해야 한다
- SetPlaceholderUrl 왕복이 성립하고, placeholder setter가 본 비주얼의 신호를 만들지 않아야 한다
- Sync ON 후 IsSynchronousLoading이 ON을 반환하고 리로드가 READY로 끝나야 한다

> `Release: DESTROYED` 버튼은 화면에 있으나 절차는 **보류** — DESTROYED 정책의 Re-Add
> 동작(3.5의 "다시 안 읽음" 보고)과 함께 결정한 뒤 여기에 절차를 추가한다.
