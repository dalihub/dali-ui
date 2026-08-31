# Lottie: GetContentInfo / GetMarkerInfo

GetContentInfo(레이어 정보)와 GetMarkerInfo(마커 정보) 조회 동작을 확인한다.

리소스는 **checkbox.json** — 마커 2개(`on`: 프레임 0~18, `off`: 프레임 20~38, fr=60,
전체 39프레임)를 가진 파일이다. 마커 없는 리소스로는 `GetMarkerInfo()`가 스텁이어도
"0 items"로 통과하므로, 마커가 있는 리소스여야 통과 기준 2가 실제 검증이 된다.

## 화면 구성

- 중앙: Lottie 애니메이션 프리뷰 (240x240) — 진입 시 자동 재생, 별도 재생 버튼 없음
- 상태 라벨 1: ContentInfo (레이어 수, 첫 번째 레이어 이름)
- 상태 라벨 2: MarkerInfo (마커 수, 첫 마커 이름과 프레임 범위)
- 버튼 행: GetContent Info / GetMarker Info

두 라벨 모두 리소스 로드 완료(ResourceReady) 시 한 번 자동으로 채워지고, 버튼은 같은
getter를 다시 부른다.

## 테스트 1: GetContentInfo

1. [GetContent Info] 버튼을 탭한다
2. **기대 결과**: ContentInfo 라벨에 레이어 수(Count > 0)와 첫 번째 레이어 이름 표시
   (checkbox.json 실측: `4 items | first: "Null 297834"`)

## 테스트 2: GetMarkerInfo

1. [GetMarker Info] 버튼을 탭한다
2. **기대 결과**: MarkerInfo 라벨에 `2 items | first: "on"`과 첫 마커의 프레임 범위 표시
3. **기대 결과**: 표시된 범위가 리소스가 정의한 값과 일치 — `on`은 **[0-18]**

> ⚠ **알려진 컴포넌트 결함 (2026-08-26 실측, dali-adaptor 네이티브 렌더러 경로)**:
> Lottie 스펙에서 마커의 `tm`/`dr`은 **프레임 단위**인데, dali-adaptor의
> `ParseLottieMetadata()`가 이를 **초로 해석해 fr을 곱한다**
> (`vector-animation-renderer-native.cpp`: *"tm and dr are in seconds, need to convert"*).
> 그래서 `on [0-18]`이 **[0-1080]**으로 보고된다 (18 × fr 60). 이 값은
> `SetMinMaxFrameByMarker`가 쓰는 조회와 같은 경로라, 마커 기반 재생 범위도 같이 틀어진다.
> 수정 전까지 테스트 2의 3번 기대 결과는 이 결함의 재현 절차다.

## 통과 기준

- GetContentInfo: 반환된 Property::Map의 Count() > 0, 레이어 이름이 표시되어야 한다
- GetMarkerInfo: 마커 이름과 **리소스가 정의한 프레임 범위**가 표시되어야 한다
