# 55. Custom LayoutManager

plain View에 attach한 custom LayoutManager와 manager-owned 상태 invalidation을 확인한다.

## 화면 구성

- `Step 1.0`, `Step 1.5` 버튼
- 50x50 빨강, 100x100 초록, 200x200 파랑의 대각선 배치

## 테스트 절차

1. 초기 Step 1.0 배치를 확인한다.
2. **기대 결과**: 각 자식이 직전 자식 크기만큼 이동해 맞닿는다.
3. `Step 1.5`를 누른다.
4. **기대 결과**: 추가 owner invalidation이 발생해 자식 사이 대각선 간격이 즉시 커진다.
5. `Step 1.0`을 누른다.
6. **기대 결과**: 처음 배치로 돌아온다.

## 통과 기준

- manager setter만 변경해도 새 layout pass가 예약되고 화면이 갱신된다.
