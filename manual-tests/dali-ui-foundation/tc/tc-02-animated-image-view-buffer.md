# 02. AnimatedImageView: BatchSize / CacheSize

SetBatchSize / GetBatchSize, SetCacheSize / GetCacheSize 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: Batch / Cache / Frame 정보
- 버튼 행 1: Load URLs (dog 8) / Play / Stop
- 버튼 행 2: Batch:1 / Batch:2 / Batch:4
- 버튼 행 3: Cache:1 / Cache:3 / Cache:8

> **참고**: 값을 바꾸는 setter는 비주얼을 다시 만들며, 재생 상태는 그 리빌드를 넘어가지
> 못한다(프레임워크 동작 — AnimatedImageView 헤더에 문서화됨). 그래서 아래 절차는 setter를
> 누른 뒤 [Play]를 다시 누른다.

## 테스트 1: URL 배열 로딩

1. [Load URLs (dog 8)] 버튼을 탭한다
2. **기대 결과**: 8프레임 PNG 시퀀스가 로딩되고 재생됨 (Frame 카운터가 굴러감)

## 테스트 2: BatchSize 변경

1. [Batch:1] 버튼을 탭한 뒤 [Play]를 탭한다
2. **기대 결과**: 라벨의 Batch 값이 1로 갱신되고 재생이 재개됨
3. [Batch:4] 버튼을 탭한 뒤 [Play]를 탭한다
4. **기대 결과**: 라벨의 Batch 값이 4로 갱신되고 재생이 재개됨
5. GetBatchSize 반환값이 라벨에 표시되는지 확인

> 프리패치 크기에 따른 부드러움 차이는 참고 사항이다 — 화면에서 판정할 수 있는 명제가
> 아니므로 기대 결과가 아니다.

## 테스트 3: CacheSize 변경

1. [Cache:1] 버튼을 탭한 뒤 [Play]를 탭한다
2. **기대 결과**: 라벨의 Cache 값이 1로 갱신됨
3. [Cache:8] 버튼을 탭한 뒤 [Play]를 탭한다
4. **기대 결과**: 라벨의 Cache 값이 8로 갱신됨
5. GetCacheSize 반환값이 라벨에 표시되는지 확인

## 통과 기준

- Set 호출 후 Get 반환값이 즉시 일치해야 한다 (라벨은 버튼 핸들러에서 동기로 갱신된다)
- CacheSize / BatchSize 설정값이 반영되어야 한다 — 캐시에 실제로 몇 프레임이 올라와 있는지는
  화면에서 관측할 수 없는 프로파일러 사안이라 이 TC의 기준이 아니다
