# ImageView: LoadPolicy / ReleasePolicy

LoadPolicy 및 ReleasePolicy 설정에 따라 이미지 로딩/해제 동작을 확인한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (180x180)
- 상태 라벨: LoadPolicy / ReleasePolicy / View 상태 / ResourceReadySignal 카운트
- 버튼 행 1: Load IMMEDIATE / Load ATTACHED
- 버튼 행 2: Release DETACHED / Release DESTROYED / Release NEVER
- 버튼 행 3: Remove View / Re-Add View

## 테스트 1: ReleasePolicy NEVER

1. [Release: NEVER] 버튼을 탭한다
2. [Remove View] 버튼을 탭한다
3. **기대 결과**: 이미지가 화면에서 사라짐
4. [Re-Add View] 버튼을 탭한다
5. **기대 결과**: 이미지가 캐시에서 즉시 복원됨 (ResourceReadySignal 카운트 증가 없음)

## 테스트 2: ReleasePolicy DETACHED

1. [Release: DETACHED] 버튼을 탭한다
2. [Remove View] 버튼을 탭한다
3. [Re-Add View] 버튼을 탭한다
4. **기대 결과**: 이미지가 다시 로딩됨 (ResourceReadySignal 카운트 증가)

## 통과 기준

- NEVER 정책: Re-Add 시 캐시에서 즉시 표시, ResourceReadySignal 카운트 미증가
- DETACHED/DESTROYED 정책: Re-Add 시 리로드 발생, ResourceReadySignal 카운트 증가