# 52. AbsoluteLayout: Layout Direction

AbsoluteLayout의 RTL mirroring과 STANDALONE 예외를 확인한다.

## 화면 구성

- 기본 AbsoluteLayout 세 사각형
- 좌상단에 고정된 200x50 `Change LayoutDirection` 버튼

## 테스트 절차

1. 초기 LTR 배치를 확인한 뒤 버튼을 누른다.
2. **기대 결과**: 일반 자식은 좌우 반전되고 standalone 버튼은 좌상단에 유지된다.
3. 버튼을 다시 누른다.
4. **기대 결과**: 모든 일반 자식이 원래 LTR 위치로 돌아온다.

## 통과 기준

- 반복 토글 시 일반 자식만 정확히 mirror된다.
