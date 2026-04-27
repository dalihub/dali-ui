# ImageView: FittingMode

4가지 FittingMode(FIT_KEEP / FILL / OVER_FIT / CENTER)의 시각적 차이를 확인한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (280x160, 가로형, 빨간색 테두리)
- 상태 라벨: 현재 FittingMode 및 GetFittingMode 일치 여부
- 버튼 행: FIT_KEEP / FILL / OVER_FIT / CENTER

## 테스트 1: FittingMode 변경

1. [FIT_KEEP] (기본값) 상태를 확인한다
2. **기대 결과**: 가로세로 비율 유지, 레터박스 패딩 표시
3. [FILL] 버튼을 탭한다
4. **기대 결과**: 이미지가 박스를 채우도록 늘어남, 가로세로 비율 무시
5. [OVER_FIT] 버튼을 탭한다
6. **기대 결과**: 가로세로 비율 유지, 이미지가 박스 밖으로 잘림
7. [CENTER] 버튼을 탭한다
8. **기대 결과**: 원본 크기로 중앙 정렬, 잘리거나 패딩 발생 가능

## 통과 기준

- 각 모드마다 시각적으로 구분되는 결과가 나타나야 한다
- GetFittingMode() 반환값이 설정한 값과 일치해야 한다