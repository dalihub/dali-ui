# Lottie: PixelArea

LottieAnimationView의 PixelArea 설정 및 애니메이션 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨: 현재 PixelArea 좌표 정보
- 버튼 행 1: FULL / TOP-L / TOP-R / BOT-L / BOT-R / CENTER (영역 프리셋)
- 버튼 행 2: ANIMATE PIXEL AREA

## 테스트 1: PixelArea 프리셋 영역

1. [FULL] 버튼을 탭한다
2. **기대 결과**: 전체 애니메이션이 표시됨 (x=0, y=0, w=1, h=1)
3. [TOP-L] 버튼을 탭한다
4. **기대 결과**: 좌측 상단 1/4 영역만 표시됨 (x=0, y=0, w=0.5, h=0.5)
5. [TOP-R] 버튼을 탭한다
6. **기대 결과**: 우측 상단 1/4 영역만 표시됨 (x=0.5, y=0, w=0.5, h=0.5)
7. [BOT-L] 버튼을 탭한다
8. **기대 결과**: 좌측 하단 1/4 영역만 표시됨 (x=0, y=0.5, w=0.5, h=0.5)
9. [BOT-R] 버튼을 탭한다
10. **기대 결과**: 우측 하단 1/4 영역만 표시됨 (x=0.5, y=0.5, w=0.5, h=0.5)
11. [CENTER] 버튼을 탭한다
12. **기대 결과**: 중앙 1/4 영역만 표시됨 (x=0.25, y=0.25, w=0.5, h=0.5)

## 테스트 2: PixelArea 애니메이션

1. [ANIMATE PIXEL AREA] 버튼을 탭한다
2. **기대 결과**: 버튼 색상이 변경되고 PixelArea 애니메이션 시작
3. **기대 결과**: 애니메이션이 부드럽게 pan 및 zoom 효과로 진행됨
   - FULL → TOP-L → CENTER → BOT-R → FULL 순서로 순환
4. 상태 라벨에 "PixelArea: animating..." 표시
5. [ANIMATE PIXEL AREA] 버튼을 다시 탭한다
6. **기대 결과**: 애니메이션 정지, 마지막 선택한 프리셋 영역으로 복원

## 통과 기준

- SetPixelArea 후 GetPixelArea가 동일한 값을 반환해야 한다
- 각 프리셋 버튼이 해당 영역만 정확히 표시해야 한다
- PixelArea 애니메이션이 부드럽게 진행되어야 한다
- 애니메이션 정지 시 이전 프리셋 상태로 복원되어야 한다