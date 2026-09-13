# 54. Custom Layout: Layout Direction

custom Arrange callback 결과에 framework RTL mirroring이 적용되는지 확인한다.

## 화면 구성

- 대각선으로 배치된 세 사각형
- 좌상단에 고정된 `Change LayoutDirection` 버튼

## 테스트 절차

1. 버튼을 눌러 RTL로 전환한다.
2. **기대 결과**: callback이 만든 대각선 배치 전체가 수평 반전되고 standalone 버튼은 움직이지 않는다.
3. 다시 눌러 LTR로 복귀한다.
4. **기대 결과**: 원래 대각선 배치가 복원된다.

## 통과 기준

- custom callback이 RTL을 직접 처리하지 않아도 framework mirroring이 한 번만 적용된다.
