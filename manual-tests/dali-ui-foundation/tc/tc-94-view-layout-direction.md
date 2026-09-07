# 94. View Layout: Layout Direction

LayoutManager가 없는 plain View에서도 RTL mirroring과 standalone 예외가 적용되는지 확인한다.

## 화면 구성

- margin이 있는 빨강·초록·파랑 일반 자식
- 좌상단 standalone `Change LayoutDirection` 버튼

## 테스트 절차

1. 버튼으로 RTL로 전환한다.
2. **기대 결과**: 일반 자식의 좌우 위치가 부모 content bounds를 기준으로 반전되고
   Y·크기·margin은 유지된다. 버튼은 움직이지 않는다.
3. LTR로 복귀한다.
4. **기대 결과**: 원래 위치가 복원된다.

## 통과 기준

- plain View direct child mirroring이 정확히 한 번 적용되고 standalone 자식은 제외된다.
