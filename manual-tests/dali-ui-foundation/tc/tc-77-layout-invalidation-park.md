# 77. Layout Invalidation: Park

layout pass 안에서 다시 발생한 invalidation이 self-wake하지 않고 다음 외부 이벤트까지 PARK되는지 확인한다.

## 화면 구성

- 현재 arrange 횟수를 표시하는 라벨
- 매 Arrange에서 라벨 text를 바꾸는 파란 box
- 콘솔의 arrange 및 LayoutFinished 로그

## 테스트 절차

1. TC 진입 후 입력하지 않고 CPU와 콘솔을 관찰한다.
2. **기대 결과**: 초기 pass 뒤 앱은 idle 상태가 되고 arrange 로그가 계속 증가하지 않는다. 초기 pass의 `LayoutFinished`도 출력된다.
3. 화면을 한 번씩 터치하며 로그와 라벨을 확인한다. touch-down, touch-up, 포인터(hover) 이동은 각각 별도의 외부 이벤트이므로 한 번의 탭으로 pass가 두 번 진행될 수 있다.
4. **기대 결과**: 외부 이벤트마다 parked work가 한 차례 처리되고 새 invalidation은 다시 PARK된다. 각 pass마다 `LayoutFinished`가 한 번 출력되며(남은 parked work가 있어도 억제되지 않는다), 후속 invalidation을 PARK한 pass의 줄에는 `(label re-invalidation parked)`가 붙는다.
5. `producer stopped mutating` 로그가 나올 때까지 입력을 계속하고, 그 뒤 `LayoutFinished: pass completed after 31 arranges` 줄이 나타날 때까지 입력(터치 또는 포인터 이동)을 계속한다.
6. **기대 결과**: `producer stopped mutating` 줄 뒤에는 먼저 같은 이벤트의 `LayoutFinished: pass completed after 30 arranges (label re-invalidation parked)` 줄이 나온다. 이어서 `LayoutFinished: pass completed after 31 arranges`(ARRANGE_LIMIT + 1) 줄이 suffix 없이 정확히 한 번 출력된다. 이 줄은 arrange #30을 만든 제스처의 touch-up이나 포인터 이동에서 이미 나올 수도 있다. 이 줄 이후에는 입력을 계속해도 arrange 로그와 `LayoutFinished` 로그가 더 이상 출력되지 않고 앱은 idle 상태를 유지한다(layout settle).

## 통과 기준

- idle 중 busy loop가 없고 외부 이벤트 단위로만 진행되며, 매 pass마다 `LayoutFinished`가 출력된다.
- 로그 기준으로 `producer stopped mutating` 이후 `LayoutFinished: pass completed after 31 arranges` 줄이 정확히 한 번 출력되고, 그 이후의 입력에는 arrange 로그도 `LayoutFinished` 로그도 출력되지 않는다(최종 settle).
