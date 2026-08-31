# 28. ImageView: URL / Reload / ResourceReady

SetResourceUrl / GetResourceUrl, Reload, ResourceReadySignal, GetLoadingStatus 동작을 확인한다.

43개 화면 중 `GetResourceUrl()`과 `GetLoadingStatus()`를 확인하는 곳은 여기뿐이다.
URL 교체·Reload·신호 카운터의 최소 조합이라 회귀가 나면 원인 격리가 가장 쉬운 자리다.
`GetResourceUrl match:` 토큰은 **방금 설정한 URL과의 진짜 비교**다(구 화면은 getter를
자기 자신과 비교해 무조건 OK였다 — 2026-08-26 수정).

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200) — 접근성 이름 `ImagePreview`
- 상태 라벨 1: URL 정보 및 GetResourceUrl 일치 여부 (진짜 비교)
- 상태 라벨 2: LoadingStatus / ResourceReadySignal 카운트 (진짜 getter·카운터)
- 버튼 행: Set URL A / Set URL B / Set URL Missing / Reload

## 테스트 1: URL 변경

(진입 시 이미 A가 떠 있으므로 [Set URL A]로 시작하지 않는다 — 같은 URL은 무동작이라
실측 0픽셀·카운터 불변이고, 그걸 "전환됐다"로 읽으면 거짓 PASS다.)

1. 진입 상태를 확인한다: `SetResourceUrl: A | GetResourceUrl match: OK`, `Status: READY`
2. [Set URL B] 버튼을 탭한다
3. **기대 결과**: 이미지 B로 전환(A 512x512와 B 640x360은 겹칠 수 없음), 라벨
   `SetResourceUrl: B | GetResourceUrl match: OK`, 카운터 +1
4. [Set URL A] 버튼을 탭한다
5. **기대 결과**: 처음 프레임으로 복귀, 카운터 +1

## 테스트 2: Reload

1. [Reload] 버튼을 탭한다
2. **기대 결과**: ResourceReadySignal 카운트가 정확히 1 증가, 그림은 동일(같은 파일)

## 테스트 3: GetLoadingStatus 대조군

READY 단언이 의미를 가지려면 FAILED에 도달하는 경로가 있어야 한다 — `return READY;` 한 줄
구현도 정상 경로에서는 전부 통과한다.

1. [Set URL Missing] 버튼을 탭한다 (존재하지 않는 경로)
2. **기대 결과**: `Status: FAILED`, 카운터 +1 (실패 로드도 신호를 쏜다)
3. [Set URL A] 버튼을 탭한다
4. **기대 결과**: `Status: READY` 복귀

## 통과 기준

- URL 변경 시 렌더가 실제로 전환되어야 한다 (A↔B 상이, A 복귀 시 처음과 동일)
- GetResourceUrl()이 설정한 URL을 반환해야 한다 (`match: OK` — 진짜 비교)
- Reload마다 ResourceReadySignal이 정확히 +1이어야 한다
- GetLoadingStatus가 정상 URL에서 READY, 없는 URL에서 **FAILED**를 반환해야 한다

(`PREPARING` 관측은 로컬 파일로는 느린 로드를 만들 수 없어 참고로 내린다.)
