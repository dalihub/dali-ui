# 25. ImageView: PlaceholderUrl

SetPlaceholderUrl / GetPlaceholderUrl 동작과 placeholder 표시를 확인한다.

로컬 파일은 로딩이 관측보다 빨리 끝나 "로딩 중 잠시 표시"는 붙잡을 수 없다. 대신 이 화면은
**관측 가능한 안정 상태**로 같은 경로를 검증한다: [Switch to Missing]이 실패하는 로드를
만들고, **실패한 뒤에** placeholder를 설정하면 등록 가드(본 이미지가 READY면 안 건다)를
통과해 **placeholder가 계속 떠 있다** (실측 2026-08-26: FAILED 프레임 대비 99.72% 변화).
순서가 중요하다. 기존 dali-toolkit 호환 동작에 따라 READY 이미지가 이미 있으면 URL을
바꾸거나 `Reload()`를 호출해도 placeholder를 다시 표시하지 않는다. 이때 placeholder는
설정값으로 유지되지만 현재 이미지를 교체하는 로딩 과정에는 다시 등록되지 않는다.

## 화면 구성

- 중앙: 이미지 프리뷰 (200x200) — 접근성 이름 `ImagePreview`
- 상태 라벨: Placeholder / GetPlaceholderUrl 값 / ResourceReadySignal 카운트 (진짜 getter·카운터)
- 버튼 행 1: Set Placeholder / Clear Placeholder
- 버튼 행 2: Reload / Switch to B / Switch to A / Switch to Missing

## 테스트 1: 경로 왕복

1. [Set Placeholder] 버튼을 탭한다
2. **기대 결과**: `GetPlaceholderUrl:`이 설정한 경로를 그대로 반환 (경로의 `//`는 패키징
   스펙의 뒤 슬래시 산물로 기능 문제 아님 — 문자열 그대로가 기준)
3. [Clear Placeholder] 버튼을 탭한다
4. **기대 결과**: `GetPlaceholderUrl: (empty)`

## 테스트 2: 실패 상태에서 placeholder 표시 (순서 주의)

1. [Clear Placeholder] 상태에서 [Switch to Missing] 버튼을 탭한다
2. **기대 결과**: 로드 실패 (신호 카운터 +1 — 실패도 신호를 쏜다), 프리뷰는 빈 상태
3. **그 상태에서** [Set Placeholder] 버튼을 탭한다
4. **기대 결과**: **placeholder 이미지가 표시되고 유지된다** (실패 프레임과 확연히 다름)
5. [Switch to A] 버튼을 탭한다
6. **기대 결과**: A 로드 완료 후 placeholder가 걷히고 실제 이미지 표시

## 테스트 3: 대조군 — 해제 후엔 안 보인다

1. [Clear Placeholder] → [Switch to Missing] 버튼을 탭한다
2. **기대 결과**: 프리뷰가 빈 실패 상태 그대로 (2번 테스트 2단계와 동일 프레임 —
   실측 0px 차이)

## 테스트 4: READY 이미지 교체·Reload 호환 동작

1. A 이미지가 READY인 상태에서 [Set Placeholder] 버튼을 탭한다
2. [Switch to Missing] 또는 [Reload] 버튼을 탭한다
3. **기대 결과**: 설정된 placeholder를 다시 표시하지 않는다
4. **기대 결과**: `GetPlaceholderUrl()`은 설정한 경로를 계속 반환한다

## 통과 기준

- SetPlaceholderUrl/GetPlaceholderUrl 경로 왕복이 성립해야 한다 (Clear 후 빈 문자열)
- 본 이미지가 READY가 아닌 동안 설정된 placeholder가 표시·유지되어야 한다
- 정상 로드 완료 시 placeholder가 실제 이미지로 교체되어야 한다
- placeholder를 해제하면 실패 상태에서 아무것도 표시되지 않아야 한다 (대조군)
- READY 이미지가 이미 있으면 URL 변경과 `Reload()` 중 placeholder를 다시 표시하지 않아야 한다
- URL 변경과 `Reload()` 후에도 설정된 placeholder 경로는 유지되어야 한다
