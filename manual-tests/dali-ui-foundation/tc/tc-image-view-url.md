# ImageView: URL / Reload / ResourceReady

SetResourceUrl / GetResourceUrl, Reload, ResourceReadySignal 동작을 확인한다.

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200)
- 상태 라벨 1: URL 정보 및 GetResourceUrl 일치 여부
- 상태 라벨 2: LoadingStatus / ResourceReadySignal 카운트
- 버튼 행: Set URL A / Set URL B / Reload

## 테스트 1: URL 변경

1. [Set URL A] 버튼을 탭한다
2. **기대 결과**: 이미지 A가 표시됨, URL 라벨이 "A"로 업데이트됨
3. [Set URL B] 버튼을 탭한다
4. **기대 결과**: 이미지 B로 전환됨, URL 라벨이 "B"로 업데이트됨
5. GetResourceUrl 반환값이 설정한 URL과 일치하는지 확인

## 테스트 2: Reload

1. [Reload] 버튼을 탭한다
2. **기대 결과**: ResourceReadySignal 카운트가 1 증가함

## 통과 기준

- URL 변경 시 이미지가 즉시 전환되어야 한다
- GetResourceUrl()이 설정한 URL을 반환해야 한다
- Reload 호출 시 ResourceReadySignal이 한 번 더 발생해야 한다
- GetLoadingStatus가 READY 상태를 반환해야 한다