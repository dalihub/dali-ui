# Lottie: SetDynamicProperty (FILL_COLOR)

SetDynamicProperty로 프레임 단위 동적 색상 변경을 확인한다. 워커 스레드 콜백을 통해 레이어 색상을 실시간으로 변경한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: 현재 Fill 색상 정보
- 버튼 행 1: Play / Stop
- 버튼 행 2: Fill: Red / Fill: Blue / Fill: Green
- 버튼 행 3: Clear Dynamic

## 테스트 1: DynamicProperty 색상 변경

1. [Play] 버튼을 탭한다
2. **기대 결과**: 원래 색상으로 재생됨
3. [Fill: Red] 버튼을 탭한다
4. **기대 결과**: 모든 레이어(keyPath="**")가 빨간색으로 변경됨
5. [Fill: Blue] 버튼을 탭한다
6. **기대 결과**: 파란색으로 변경됨
7. [Fill: Green] 버튼을 탭한다
8. **기대 결과**: 초록색으로 변경됨

## 테스트 2: DynamicProperty 해제

1. [Clear Dynamic] 버튼을 탭한다
2. **기대 결과**: URL 리로드로 동적 속성이 초기화되고 원래 색상 복원

## 통과 기준

- Fill 버튼 클릭 직후 재생 중 애니메이션 색상이 변경되어야 한다
- Clear 후 원래 색상이 복원되어야 한다