# Lottie: GetContentInfo / GetMarkerInfo

GetContentInfo(레이어 정보)와 GetMarkerInfo(마커 정보) 조회 동작을 확인한다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240)
- 상태 라벨 1: ContentInfo (레이어 수, 첫 번째 아이템 이름)
- 상태 라벨 2: MarkerInfo (마커 수)
- 버튼 행: Play / GetContentInfo / GetMarkerInfo

## 테스트 1: GetContentInfo

1. [Play] 버튼을 탭한다
2. [GetContentInfo] 버튼을 탭한다
3. **기대 결과**: ContentInfo 라벨에 레이어 수(Count > 0)와 첫 번째 레이어 이름 표시

## 테스트 2: GetMarkerInfo

1. [GetMarkerInfo] 버튼을 탭한다
2. **기대 결과**: MarkerInfo 라벨에 마커 수 표시 (jolly_walker.json에 마커가 없으면 "0 items" 표시)

## 통과 기준

- GetContentInfo: 반환된 Property::Map의 Count() > 0, 레이어 이름이 표시되어야 한다
- GetMarkerInfo: 마커가 있으면 이름 표시, 없으면 "0 items" 표시