# ImageView: PlaceholderUrl

SetPlaceholderUrl / GetPlaceholderUrl 동작을 확인한다. 로딩 중 플레이스홀더가 표시되고 완료 후 실제 이미지로 교체되는지 확인한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200)
- 상태 라벨: Placeholder / ResourceReadySignal 카운트
- 버튼 행 1: Set Placeholder / Clear Placeholder
- 버튼 행 2: Reload / Switch to B / Switch to A

## 테스트 1: Placeholder 설정 및 로딩

1. [Set Placeholder] 버튼을 탭한다
2. **기대 결과**: GetPlaceholderUrl이 설정한 경로를 반환함
3. [Reload] 버튼을 탭한다
4. **기대 결과**: 로딩 중 플레이스홀더 이미지가 잠시 표시됨, 완료 후 실제 이미지로 교체됨

## 테스트 2: 이미지 전환 시 Placeholder

1. [Set Placeholder] 상태에서 [Switch to B] 버튼을 탭한다
2. **기대 결과**: 로딩 중 플레이스홀더 표시 후 이미지 B로 교체됨

## 테스트 3: Placeholder 해제

1. [Clear Placeholder] 버튼을 탭한다
2. **기대 결과**: GetPlaceholderUrl이 빈 문자열을 반환함

## 통과 기준

- Placeholder 설정 후 Reload 시 로딩 중 플레이스홀더가 표시되어야 한다
- 로딩 완료 후 실제 이미지로 교체되어야 한다
- Clear Placeholder 후 GetPlaceholderUrl이 빈 문자열을 반환해야 한다