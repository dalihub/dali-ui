# ProgressBar: Values, Geometry and Modes

기본 및 custom style의 외형, 값 변경, local geometry 기반 overlay 동기화,
reverse/RTL 방향, determinate/indeterminate 전환을 확인한다.

## 화면 구성

- `Default style at 25%`: 기본 style을 사용하며 range `0..1`, value `0.25`인
  ProgressBar.
- `Custom style with public tooltip/divider synchronization`: range `0..100`,
  초기 value `50`, padding `8/8/12/12`, track 두께 `14`, 회색 track과 보라색
  trail을 사용하는 ProgressBar.
- custom ProgressBar의 divider step count는 `4`다. track 내부의 25%, 50%,
  75% 위치에 divider 세 개가 있고, public divider geometry로 배치한 `1`, `2`,
  `3` 라벨이 각 divider 아래에 표시된다.
- trail 끝의 `50%` tooltip은 `GetTrailEndPosition()`이 반환한 ProgressBar-local
  좌표로 배치된다.
- 상태 라벨은 현재 value, determinate 상태, reverse 상태, layout direction을
  `value 50%, determinate, normal, LTR` 형식으로 표시한다.
- 값 버튼: `0%`, `25%`, `50%`, `100%`.
- 모드 버튼: `Determinate / Indeterminate`, `Reverse`, `RTL / LTR`.

## 테스트 1: 초기 외형과 geometry overlay

1. TC에 진입한다.
2. **기대 결과**: 기본 ProgressBar는 기본 style의 둥근 track과 trail로 25%를
   표시한다.
3. custom ProgressBar를 확인한다.
4. **기대 결과**: 회색의 둥근 track 안에서 보라색 trail이 50%까지 채워지고,
   trail 끝에 `50%` tooltip이 표시된다.
5. divider와 라벨을 확인한다.
6. **기대 결과**: track의 25%, 50%, 75% 지점에 divider가 하나씩 표시되고,
   `1`, `2`, `3` 라벨이 각각의 divider 아래에 정렬된다. 초기 layout이 끝난 뒤
   라벨이나 tooltip이 좌상단의 초기 위치에 남아 있지 않아야 한다.

## 테스트 2: 값과 trail 끝 동기화

1. `0%`, `25%`, `50%`, `100%` 버튼을 순서대로 탭한다.
2. 각 탭에서 다음 결과를 확인한다.

   | 버튼 | 기대 결과 |
   |---|---|
   | `0%` | trail의 수학적 끝이 track 시작점으로 이동하고 tooltip과 상태 라벨이 `0%`를 표시한다. |
   | `25%` | trail 끝과 tooltip이 track 25% 지점으로 이동하고 상태 라벨이 `25%`를 표시한다. |
   | `50%` | trail 끝과 tooltip이 track 중앙으로 이동하고 상태 라벨이 `50%`를 표시한다. |
   | `100%` | trail이 track 끝까지 채워지고 tooltip과 상태 라벨이 `100%`를 표시한다. |

3. 각 값에서 divider와 `1`, `2`, `3` 라벨 위치도 확인한다.
4. **기대 결과**: value 변경은 divider 집합과 위치를 바꾸지 않는다. tooltip만
   현재 trail 끝을 따라간다.

## 테스트 3: Reverse와 RTL 방향 조합

1. `50%`를 탭해 trail 길이를 절반으로 만든다.
2. `Reverse`와 `RTL / LTR` 버튼을 사용해 아래 네 조합을 차례로 만든다.

   | 상태 라벨 | trail 진행 방향 | divider 순서 |
   |---|---|---|
   | `normal, LTR` | 왼쪽에서 오른쪽 | 왼쪽부터 `1`, `2`, `3` |
   | `reversed, LTR` | 오른쪽에서 왼쪽 | 왼쪽부터 `1`, `2`, `3` 유지 |
   | `normal, RTL` | 오른쪽에서 왼쪽 | 오른쪽부터 `1`, `2`, `3`으로 mirror |
   | `reversed, RTL` | 왼쪽에서 오른쪽 | 오른쪽부터 `1`, `2`, `3`으로 mirror 유지 |

3. **기대 결과**: tooltip은 모든 조합에서 보라색 trail의 실제 끝을 따라간다.
   Reverse만 바꿀 때 divider와 divider 라벨의 위치 또는 순서는 바뀌지 않고,
   RTL을 바꿀 때만 좌우로 mirror된다.

## 테스트 4: Determinate와 Indeterminate 반복 전환

1. `Determinate / Indeterminate` 버튼을 탭한다.
2. **기대 결과**: 상태 라벨이 `indeterminate`로 바뀌고 determinate trail,
   divider, tooltip, `1`/`2`/`3` 라벨이 숨겨진다. 같은 track 안에서 보라색 bar
   네 개로 구성된 indeterminate animation이 반복된다. 움직이는 bar는 track의
   사각 bounds만 따르지 않고 왼쪽과 오른쪽의 둥근 끝 모양에 맞춰 잘린다.
3. indeterminate 상태에서 값 버튼 하나를 탭한다.
4. **기대 결과**: 상태 라벨의 value는 선택한 값으로 바뀌지만 tooltip과 divider
   라벨은 계속 숨겨져 있다.
5. `Determinate / Indeterminate` 버튼을 다시 탭한다.
6. **기대 결과**: 상태 라벨이 `determinate`로 돌아오고 animation이 제거된다.
   현재 value에 해당하는 trail과 tooltip, divider 세 개와 라벨 세 개가 다시
   표시되며 중복된 bar 또는 라벨이 남지 않는다.
7. 위 전환을 두 번 더 반복한다.
8. **기대 결과**: 전환 횟수와 관계없이 animation, trail, divider와 overlay가
   한 세트만 유지되고 현재 상태와 일치한다.

## 테스트 5: Layout 변경 후 전체 재동기화

1. 창 크기 변경이 가능한 환경에서는 custom ProgressBar의 가로 폭이 달라지도록
   창을 줄였다가 늘린다.
2. **기대 결과**: tooltip은 새 trail 끝을 따라 이동하고 `1`, `2`, `3` 라벨은
   새 track 폭의 25%, 50%, 75% 지점에 다시 정렬된다.
3. 크기 변경 후 테스트 2와 테스트 3의 값 및 방향 조작을 반복한다.
4. **기대 결과**: geometry signal 이후 getter로 현재 전체 상태를 다시 읽으므로
   누락되거나 중복된 divider 라벨, 이전 위치에 남은 tooltip이 없다.

## 통과 기준

- 기본 style ProgressBar가 25%, custom style ProgressBar가 초기 50%로 표시된다.
- 값 버튼마다 trail, tooltip, value 상태 라벨이 같은 값을 나타낸다.
- divider 세 개와 라벨 세 개가 25%, 50%, 75% 위치에 대응하며 value나 Reverse
  변경으로 재배열되지 않는다.
- trail 방향은 `RTL XOR reversed`가 참일 때 오른쪽에서 시작하고, divider는
  RTL에서만 좌우 mirror된다.
- indeterminate에서는 determinate trail/divider/overlay가 숨겨지고 네 bar
  animation만 보인다. 움직이는 bar는 track 양쪽의 둥근 끝 밖으로 노출되지 않으며,
  determinate 복귀 시 현재 value의 한 세트로 복원된다.
- 최초 layout과 후속 layout 변경 뒤에도 tooltip과 divider 라벨이 public local
  geometry와 일치한다.
