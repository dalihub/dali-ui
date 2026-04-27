# AnimatedImageView: BatchSize / CacheSize

SetBatchSize / GetBatchSize, SetCacheSize / GetCacheSize 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: Batch / Cache / Frame 정보
- 버튼 행 1: Load URLs (dog 8) / Play / Stop
- 버튼 행 2: Batch:1 / Batch:2 / Batch:4
- 버튼 행 3: Cache:1 / Cache:3 / Cache:8

## 테스트 1: URL 배열 로딩

1. [Load URLs (dog 8)] 버튼을 탭한다
2. **기대 결과**: 8프레임 PNG 시퀀스가 로딩되고 재생됨

## 테스트 2: BatchSize 변경

1. [Batch:1] 버튼을 탭한다
2. **기대 결과**: 한 번에 1프레임만 프리패치 (부드러움 저하 가능)
3. [Batch:4] 버튼을 탭한다
4. **기대 결과**: 한 번에 4프레임 프리패치 (부드러움 개선)
5. GetBatchSize 반환값이 라벨에 표시되는지 확인

## 테스트 3: CacheSize 변경

1. [Cache:1] 버튼을 탭한다
2. **기대 결과**: 최소 캐시
3. [Cache:8] 버튼을 탭한다
4. **기대 결과**: 모든 프레임이 캐시에 저장됨
5. GetCacheSize 반환값이 라벨에 표시되는지 확인

## 통과 기준

- Set 호출 후 Get 반환값이 즉시 일치해야 한다
- CacheSize > TotalFrame이면 모든 프레임이 메모리에 캐시되어야 한다