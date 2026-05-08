# Lottie: PlaceholderUrl

LottieAnimationView의 PlaceholderUrl 설정/해제 및 로딩 중 표시 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x160)
- 상태 라벨: Placeholder 상태 정보
- 버튼 행: Set Placeholder / Reload Lottie / Clear URL / Clear Holder

## 테스트 1: Placeholder 설정 및 로딩 중 표시

1. [Set Placeholder] 버튼을 탭한다
2. **기대 결과**: 상태 라벨에 "Placeholder: placeholder_image.png" 표시
3. [Reload Lottie] 버튼을 탭한다
4. **기대 결과**: Lottie 로딩 중 placeholder 이미지가 즉시 표시됨
5. **기대 결과**: ResourceReady 시그널 발생 시 placeholder가 사라지고 Lottie 애니메이션이 표시됨
6. 상태 라벨에 "ResourceReady — placeholder removed" 표시

## 테스트 2: URL 클리어 시 Placeholder 표시

1. [Set Placeholder] 버튼으로 placeholder를 설정한다
2. [Clear URL] 버튼을 탭한다
3. **기대 결과**: URL이 클리어되고 placeholder가 표시됨 (설정된 경우)
4. 상태 라벨에 "URL cleared — placeholder visible (if set)" 표시

## 테스트 3: Placeholder 클리어

1. [Clear Holder] 버튼을 탭한다
2. **기대 결과**: Placeholder URL이 제거됨
3. 상태 라벨에 "Placeholder: (none)" 표시
4. URL이 없는 상태에서는 아무것도 표시되지 않음

## 통과 기준

- SetPlaceholderUrl 후 GetPlaceholderUrl이 동일한 값을 반환해야 한다
- Lottie 로딩 중 placeholder가 표시되어야 한다
- ResourceReady 발생 시 placeholder가 사라져야 한다
- URL 클리어 시 placeholder가 표시되어야 한다 (설정된 경우)