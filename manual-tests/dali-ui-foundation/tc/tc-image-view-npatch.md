# ImageView N-Patch Border

NPatchBorder 크기 조절 및 BorderOnly 렌더링 동작을 확인한다.

## 화면 구성

- 중앙: N-Patch 이미지 프리뷰 (회색 배경 위)
- 상태 라벨: Size / Border / BorderOnly 상태
- 버튼 행 1: 100x40 / 200x60 / 300x80 / 400x120
- 버튼 행 2: BorderOnly ON / BorderOnly OFF
- 버튼 행 3: Border Auto / Border (10) / Border (20,5)
- 버튼 행 4: button-up-1.9.png / tooltip.9.png

## 테스트 1: 크기 변경 시 Border 유지

1. [300x80] 상태에서 보더 영역이 고정되어 있는지 확인한다
2. [100x40] 버튼을 탭한다
3. **기대 결과**: 중앙 영역만 늘어나고 보더는 고정됨
4. [400x120] 버튼을 탭한다
5. **기대 결과**: 동일하게 보더 고정, 중앙 영역 확장

## 테스트 2: BorderOnly 모드

1. [BorderOnly ON] 버튼을 탭한다
2. **기대 결과**: 중앙 영역이 투명해지고 보더만 렌더링됨
3. [BorderOnly OFF] 버튼을 탭한다
4. **기대 결과**: 전체 이미지가 다시 표시됨

## 테스트 3: 수동 Border 설정

1. [Border: (10)] 버튼을 탭한다
2. **기대 결과**: 보더 크기가 10px로 설정됨
3. [Border: (20,5)] 버튼을 탭한다
4. **기대 결과**: 보더 크기가 가로 20, 세로 5로 설정됨
5. [Border: Auto] 버튼을 탭한다
6. **기대 결과**: 9-patch 이미지에서 자동 감지된 보더 값 복원

## 통과 기준

- 크기 변경 시 보더 영역은 고정되고 중앙만 늘어나야 한다
- BorderOnly ON 시 중앙이 투명해야 한다
- GetNPatchBorder / IsNPatchBorderOnly 반환값이 설정한 값과 일치해야 한다