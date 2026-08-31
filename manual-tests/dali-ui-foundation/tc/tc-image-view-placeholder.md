# ImageView: PlaceholderUrl

SetPlaceholderUrl / GetPlaceholderUrl 동작과 placeholder 표시를 확인한다.

로컬 파일은 로딩이 관측보다 빨리 끝나 "로딩 중 잠시 표시"는 붙잡을 수 없다. 대신 이 화면은
**관측 가능한 안정 상태**로 같은 경로를 검증한다: [Switch to Missing]이 실패하는 로드를
만들고, **실패한 뒤에** placeholder를 설정하면 등록 가드(본 이미지가 READY면 안 건다)를
통과해 **placeholder가 계속 떠 있다** (실측 2026-08-26: FAILED 프레임 대비 99.72% 변화).
순서가 중요하다 — 지금 구현에서는 placeholder를 먼저 설정하고 URL을 바꾸면 그 시점의 IMAGE
상태가 아직 옛 이미지의 READY라 가드에 걸려 등록되지 않는다(알려진 순서 결함, 이슈 리포트
대상 — 수정되면 "설정이 먼저여도 보인다"가 판정 기준이 된다).

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

## 통과 기준

- SetPlaceholderUrl/GetPlaceholderUrl 경로 왕복이 성립해야 한다 (Clear 후 빈 문자열)
- 본 이미지가 READY가 아닌 동안 설정된 placeholder가 표시·유지되어야 한다
- 정상 로드 완료 시 placeholder가 실제 이미지로 교체되어야 한다
- placeholder를 해제하면 실패 상태에서 아무것도 표시되지 않아야 한다 (대조군)

> **알려진 결함 (이슈 리포트 대상)**: ① `Reload()`가 placeholder를 다시 걸지 않는다
> (리로드도 로딩이므로 계약상 걸어야 함) ② `SetResourceUrl()`의 placeholder 재등록 판정이
> 새 비주얼을 만들기 **전**에 돌아 옛 이미지의 READY에 걸린다(주석과 코드가 모순).
> 수정되면 테스트 2의 순서 제약이 사라지고 "READY 상태에서 Missing 전환만으로 placeholder가
> 떠야 한다"가 판정 기준으로 승격된다.
