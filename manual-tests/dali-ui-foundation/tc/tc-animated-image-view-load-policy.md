# AnimatedImageView: LoadPolicy / ReleasePolicy / SyncLoading

SetLoadPolicy / GetLoadPolicy, SetReleasePolicy / GetReleasePolicy, SetSynchronousLoading / IsSynchronousLoading 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: LoadPolicy / ReleasePolicy / Sync / View 상태
- 버튼 행 1: Load IMMEDIATE / Load ATTACHED
- 버튼 행 2: Release DETACHED / Release DESTROYED / Release NEVER
- 버튼 행 3: Sync ON / Sync OFF
- 버튼 행 4: Remove View / Re-Add View / Play

## 테스트 1: ReleasePolicy NEVER

1. [Release: NEVER] 버튼을 탭한다
2. [Remove View] 버튼을 탭한다
3. **기대 결과**: 애니메이션이 화면에서 사라짐
4. [Re-Add View] 버튼을 탭한다
5. **기대 결과**: 캐시에서 즉시 복원됨

## 테스트 2: ReleasePolicy DETACHED

1. [Release: DETACHED] 버튼을 탭한다
2. [Remove View] -> [Re-Add View] 버튼을 탭한다
3. **기대 결과**: 리로드가 발생함

## 테스트 3: SynchronousLoading

1. [Sync ON] 버튼을 탭한다
2. **기대 결과**: 즉시 로딩됨

## 통과 기준

- NEVER 정책: Re-Add 시 캐시된 텍스처를 재사용하여야 한다
- DETACHED 정책: Re-Add 시 리로드가 발생하여야 한다
- Sync ON 시 즉시 로딩되어야 한다