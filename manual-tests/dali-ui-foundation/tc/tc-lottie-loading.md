# Lottie: ReleasePolicy / SyncLoading / PreMult / Placeholder

SetReleasePolicy / GetReleasePolicy, SetSynchronousLoading / IsSynchronousLoading, SetPreMultipliedAlpha / IsPreMultipliedAlpha, SetPlaceholderUrl / GetPlaceholderUrl, GetLoadingStatus 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: ReleasePolicy / Sync / PreMult / Placeholder / LoadingStatus
- 버튼 행 1: Release NEVER / Release DETACHED
- 버튼 행 2: Remove View / Re-Add View
- 버튼 행 3: Sync ON / Sync OFF
- 버튼 행 4: Set Placeholder / Clear Placeholder / Reload URL
- 버튼 행 5: PreMult ON / PreMult OFF

## 테스트 1: ReleasePolicy

1. [Release: NEVER] 버튼을 탭한다
2. [Remove View] -> [Re-Add View] 버튼을 탭한다
3. **기대 결과**: 캐시에서 즉시 표시 (리로드 없음)
4. [Release: DETACHED] 버튼을 탭한다
5. [Remove View] -> [Re-Add View] 버튼을 탭한다
6. **기대 결과**: 리로드 발생

## 테스트 2: Placeholder

1. [Set Placeholder] 버튼을 탭한다
2. [Reload URL] 버튼을 탭한다
3. **기대 결과**: 로딩 중 플레이스홀더 표시
4. [Clear Placeholder] 버튼을 탭한다
5. **기대 결과**: GetPlaceholderUrl이 빈 문자열 반환

## 테스트 3: SynchronousLoading

1. [Sync ON] 버튼을 탭한다
2. [Reload URL] 버튼을 탭한다
3. **기대 결과**: 즉시 로딩

## 통과 기준

- NEVER 정책: Re-Add 시 리로드 없이 즉시 표시되어야 한다
- Placeholder 설정 후 Reload 시 로딩 중 플레이스홀더가 표시되어야 한다
- Sync ON 시 즉시 로딩되어야 한다