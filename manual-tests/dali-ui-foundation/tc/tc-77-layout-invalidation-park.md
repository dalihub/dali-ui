# 77. Layout Invalidation: Park

layout pass 안에서 다시 발생한 invalidation이 self-wake하지 않고 다음 외부 이벤트까지 PARK되는지 확인한다.

## 화면 구성

- 현재 arrange 횟수를 표시하는 라벨
- 매 Arrange에서 라벨 text를 바꾸는 파란 box
- 콘솔의 arrange 및 LayoutFinished 로그

## 테스트 절차

1. TC 진입 후 입력하지 않고 CPU와 콘솔을 관찰한다.
2. **기대 결과**: 초기 pass 뒤 앱은 idle 상태가 되고 arrange 로그가 계속 증가하지 않는다.
3. 화면을 한 번씩 터치하며 로그와 라벨을 확인한다.
4. **기대 결과**: 외부 이벤트마다 parked work가 한 차례 처리되고 새 invalidation은 다시 PARK된다.
5. arrange #30 이후 한 번 더 외부 이벤트를 발생시킨다.
6. **기대 결과**: mutation이 멈춘 다음 pass에서 layout이 settle되고 `LayoutFinished`가 출력된다.

## 통과 기준

- idle 중 busy loop가 없고 외부 이벤트 단위로만 진행되며 최종적으로 settle된다.
