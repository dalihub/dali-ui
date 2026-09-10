# Ellipsis

실제 렌더링 픽셀로 overflow 전환, 원상복귀와 Sync/Async 일치를 자동 검증한다.
시스템 폰트를 사용하며 다른 장치에서 만든 기준 이미지는 사용하지 않는다.

## 화면 구성

- 목록의 `Ellipsis` 항목 (`text-ellipsis`)
- 상단 전체 폭 버튼 `Run auto: Sync + Async [V]` ↔ `Cancel auto [V]` (`text-ellipsis-run`)
- 현재 case와 단계 (`text-ellipsis-progress`)
- 최종 PASS/FAIL/ERROR (`text-ellipsis-status`)
- Case 설명 (`text-ellipsis-scenario`), 미리보기 스크롤 (`text-ellipsis-preview`), 실제 검사 영역 (`text-ellipsis-fixture`)
- 요약, 실패 내역, Report와 PNG 경로 (`text-ellipsis-details`, 필요 시 아래 영역 스크롤)
- 아래 body 스크롤 (`text-ellipsis-body`)의 옵션 영역 (`text-ellipsis-controls`): 폭·높이, sync/async, overflow,
  방향·정렬, multiline·MaximumLines·wrap, padding·font size·render scale·문장 변경
- 현재 public measurement (`text-ellipsis-measurements`)
- 설정·실행 상태 카드 (`text-ellipsis-settings`, `text-ellipsis-runtime`)

Text Layout과 같은 배경·버튼·상태 색상을 사용하며 넓은 화면은 좌우 배치다.
자동 검증 중에는 캡처가 clipping의 영향을 받지 않도록 미리보기를 스크롤 밖에 고정한다.

수동 옵션은 [공통 조작 표](../README.md#수동-조작)를 참고한다.
아래 agent 절차에서는 **수동 옵션을 바꾸지 않고** 전체 자동 검증만 실행한다.

## 테스트 절차

1. 앱 목록에서 `Ellipsis`에 진입한다.
2. `Run auto: Sync + Async [V]`를 한 번 누른다. 키보드가 있으면 V로 대체한다.
3. **기대 결과**: 버튼이 `Cancel auto [V]`로 바뀌고 보라색 `AUTO VERIFY RUNNING`, 증가하는 case 번호와 live check 결과가 표시된다.
4. 다른 조작이나 창 크기 변경 없이 완료를 기다린다. 보통 수 분 이내이며 타깃에 따라 달라진다.
5. **기대 결과**: `AUTO VERIFY PASS`, `54 cases`, `0 failed cases`가 표시되고 status accessibility value가 `PASS`이다.
6. 최종 화면과 Report 로그를 저장한다. 실패하면 로그에 나온 case의 `expected`, `actual`, `diff` PNG도 함께 저장한다.
7. **기대 결과**: 로그 마지막에 다음 SUMMARY와 PASS가 있다.

```text
[TEXT_ELLIPSIS][SUMMARY] cases=54 syncCases=27 asyncCases=27 checks=... fail=0 failedCases=0
[TEXT_ELLIPSIS][AUTO][PASS]
```

## 통과 기준

- 캡처 좌표·색상·배경 calibration이 PASS이고 Sync 27개 / Async 27개 모두 완료된다.
- 최종 화면과 로그가 PASS이며 `fail=0`, `failedCases=0`이다.
- ERROR, 취소, timeout, 로그가 없는 경우는 PASS가 아니다. 마지막 case와 오류를 기록한다.
- 현재 구현에서 발견되는 실패를 임의로 제외하거나 허용 오차를 늘려 PASS로 만들지 않는다.

| Case 구성 | 검사 |
|---|---|
| LTR/RTL START·CENTER·END, padding | CLIP→ELLIPSIS / ELLIPSIS→CLIP을 각 모드로 새로 만든 Label과 비교, 복귀 후 처음 이미지와 일치 |
| Arabic/Hebrew, LTR→RTL·RTL→LTR, 숫자·punctuation·isolate | 동일 조건의 상태 전환과 Sync/Async 픽셀 일치 |
| Combining/ligature 문자열, emoji ZWJ·modifier·flag | 같은 입력의 경로 일치와 상태 복귀 |
| WORD/CHARACTER, 한글, 명시적 개행, MaximumLines 1/2/3 | public line count의 상한, overflow 전환·복귀 |
| MaximumLines=0 + 낮은 height | 줄 수 제한이 없어도 viewport 높이로 생략되는 조건 |
| 정확히 2줄 / trailing newline | 보이는 두 줄은 유지되고, 잘리는 빈 마지막 줄 때문에 새 ellipsis가 생기지 않는지 |
| 짧은 문장, natural width −4/+4px | 실제 font measurement로 overflow 경계를 설정하여 생략 필요/불필요 구분 |
| 좁은 폭 + 큰 font, RenderScale=2 | 추가 크기에서도 전환·복귀; async 전용 RenderScale은 같은 배율 내 복귀 검사 |

이 항목은 관계 기반 렌더링 회귀 검사다. 정확히 어떤 grapheme이 남아야 하는지,
모든 BiDi 시각 순서와 shaping이 정답인지를 독립적으로 증명하는 검사는 아니다.
새 Label 대조군은 같은 시스템 폰트·크기·렌더링 경로를 사용한다. Overflow가 있다는 이유만으로
픽셀 변경을 강제하지 않으며, 동적 변경 결과가 새 Label과 다른 경우 실패한다.
검사 대상의 `GetLineCount()`는 전환 검사 후 호출하여 measurement가 layout 재계산을 유발해 결함을 가리지 않도록 한다.
Report는 실행별로 분리되며, 지정한 누적 로그의 `[REPORT][BEGIN]`에서 개별 경로를 찾을 수 있다.
폰트가 fixture의 overflow 전제를 만족하지 않거나 캡처할 수 없으면 ERROR다.
장치 간 golden image는 사용하지 않으며, 같은 장치의 경로 간 차이도 임의로 무시하지 않는다.
단, RenderScale은 async 전용이므로 해당 case는 sync/async의 서로 다른 rasterization 결과에
픽셀 완전 일치를 요구하지 않는다. 모든 나머지 case는 경로 일치를 검사한다.
