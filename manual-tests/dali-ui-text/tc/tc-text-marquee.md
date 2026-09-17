# Marquee

실제 픽셀 이동과 Public API 상태를 함께 사용해 Marquee를 자동 검증한다.
설정한 LoopCount를 다시 읽는 대신 연속 캡처에서 움직임과 cycle 시작을 관찰한다.

## 화면 구성

- 목록의 `Marquee` 항목 (`text-marquee`)
- 상단 전체 폭 버튼 `Run auto: Sync + Async [V]` ↔ `Cancel auto [V]` (`text-marquee-run`)
- 현재 case와 단계 (`text-marquee-progress`)
- 최종 PASS/FAIL/ERROR (`text-marquee-status`)
- Case 설명 (`text-marquee-scenario`), 미리보기 스크롤 (`text-marquee-preview`), 실제 검사 영역 (`text-marquee-fixture`)
- 요약, 실패 내역, Report와 PNG 경로 (`text-marquee-details`, 필요 시 아래 영역 스크롤)
- 아래 body 스크롤 (`text-marquee-body`)의 옵션 영역 (`text-marquee-controls`): 공통 text/layout 옵션과 Start/Stop,
  orientation·stop mode·trigger policy·loop count·speed·gap·delay
- 현재 public measurement와 RUNNING/STOPPED (`text-marquee-measurements`)
- 설정·실행 상태 카드 (`text-marquee-settings`, `text-marquee-runtime`)

Text Layout과 같은 배경·버튼·상태 색상을 사용하며 넓은 화면은 좌우 배치다.
자동 검증 중에는 캡처가 clipping의 영향을 받지 않도록 미리보기를 스크롤 밖에 고정한다.

수동 옵션은 [공통 조작 표](../README.md#수동-조작)를 참고한다.
아래 agent 절차에서는 **수동 옵션을 바꾸지 않고** 전체 자동 검증만 실행한다.

## 테스트 절차

1. 앱 목록에서 `Marquee`에 진입한다.
2. `Run auto: Sync + Async [V]`를 한 번 누른다. 키보드가 있으면 V로 대체한다.
3. **기대 결과**: 버튼이 `Cancel auto [V]`로 바뀌고 보라색 `AUTO VERIFY RUNNING`, live check 결과가 표시되며 text의 이동·정지가 반복된다.
4. 다른 조작이나 창 크기 변경 없이 완료를 기다린다. 보통 수 분이며 타깃에 따라 달라진다.
5. **기대 결과**: `AUTO VERIFY PASS`, `84 cases`, `0 failed cases`와 status accessibility value `PASS`가 표시된다.
6. 최종 화면과 Report 로그를 저장한다. 실패하면 해당 case의 PNG(`ellipsis`, `marquee-origin`, `moving`, `restored`, `expected/actual/diff`)도 저장한다.
7. **기대 결과**: 로그 마지막에 다음 SUMMARY와 PASS가 있다.

```text
[TEXT_MARQUEE][SUMMARY] cases=84 syncCases=42 asyncCases=42 checks=... fail=0 failedCases=0
[TEXT_MARQUEE][AUTO][PASS]
```

## 통과 기준

- Calibration이 PASS이고 Sync 42개 / Async 42개 모두 완료된다.
- 최종 화면과 로그가 PASS이며 `fail=0`, `failedCases=0`이다.
- ERROR, 취소, timeout, 최종 결과가 없는 실행은 PASS가 아니다.
- 관찰 간격 부족 ERROR가 발생하면 해당 로그를 남긴다. Loop 검사를 생략해 PASS로 처리하지 않는다.

| Case 구성 | 검사 |
|---|---|
| 순수 LTR/RTL (Arabic/Hebrew), CENTER/END, padding | 시작 전 retained text 위치 유지, 실제 이동 방향, IMMEDIATE 정지와 ellipsis 복귀 |
| 혼합 BiDi (RTL+숫자 / LTR+RTL) | 시작·이동 방향·정지·ellipsis 복귀. 시작 시 고정 ROI의 위치 동등 비교는 적용하지 않음 |
| FINISH_LOOP (LTR/RTL/vertical) | Stop 요청 직후 계속 실행하고, 요청 후 실제 이동을 관찰한 뒤 종료 |
| LoopCount 1 / 2 / 3, vertical 2 | 초기 위치에서 출발하는 cycle 수, 실제 이동, 너무 이른 종료 방지, 최종 정지 |
| ON_OVERFLOW + LoopCount 1 / 2, horizontal / vertical | 자동 시작 후 지정된 cycle 수만 실행하고 종료 |
| 무한 반복 | 최소 2 cycle을 관찰한 뒤 명시적으로 정지해야 멈추는지 |
| 정지 후 재시작 | 같은 Label에서 시작→정지→재시작→정지하며 위치와 ellipsis 복구 |
| ON_OVERFLOW + 긴 text | 명시적 Start 없이 자동 시작, 이동, 명시적 Stop |
| ON_OVERFLOW + 짧은 text | 자동 시작하지 않고 원래 화면 유지 |
| ON_OVERFLOW + 문장 변경 | horizontal / vertical, 두 StopMode에서 짧은→긴→짧은 문장 3회 반복; 자동 시작·이동·정지와 원상복귀 |
| ON_OVERFLOW + 폭 변경 | 두 StopMode에서 fit→overflow→fit 3회 반복; 자동 시작·정지와 처음 fit 이미지 복귀 |
| MANUAL + 짧은 text | overflow가 없어도 명시적 Start로 움직이고 Stop으로 복귀 |
| 잘못된 orientation | single-line+vertical / multiline+horizontal에서는 시작하지 않음 |
| Vertical + multiline | 위쪽 이동, IMMEDIATE/FINISH_LOOP, 첫 줄 위치와 ellipsis 복귀 |
| 큰 gap + 1.5초 delay, RenderScale=2 | 다른 반복 설정·배율에서도 실제 반복과 위치 복귀 |
| Lifecycle 16개 조합 | Sync/Async × horizontal/vertical × MANUAL/ON_OVERFLOW × IMMEDIATE/FINISH_LOOP |
| 각 lifecycle의 검증 흐름 | resize 왕복 → Label 숨김·복원 → 부모 숨김·복원 → Stop → idle → 명시적 재시작 → Stop |
| Lifecycle의 추가 검사 | 숨김 중 source 변경, FINISH_LOOP 대기 중 실제 width 변경·복원, 중복 Start, 연속 이동 중 불필요한 Async 완료 signal 증가 없음 |

기능 검사이며 frame 단위의 정확한 animation timing이나 성능 측정은 아니다.
현재 `Capture`는 GLES 경로가 필요하다. PNG 저장/읽기가 불가능하면 ERROR로 종료한다.
delay 동안 초기 위치를 캡처하고, 움직임은 연속 이미지에서 관찰한다. 정확한 px/sec나
모든 frame의 무결성을 보증하지 않으며 ON_OVERFLOW의 문장 교체 case에서는 교체 전후
내용이 다르므로 retained-origin 동등 비교 대신 자동 시작·이동·원문 복귀를 검사한다.
문장/폭의 fit 복귀는 캐시된 정적 결과를 재사용할 수 있으므로 새 완료 signal 횟수를
필수로 요구하지 않는다. 대신 이전 moving/overflow 화면과 구별되는 fit 기준 이미지와
`IsMarqueeRunning()==false`를 함께 요구한다. RenderScale=2는 async 전용이며 sync와
픽셀 완전 일치 대신 같은 경로의 위치 유지·이동·복귀를 검사한다.

혼합 BiDi는 ellipsis가 시각적인 문장 중간에 들어가거나 경계에서 run 배치가 달라질 수 있다.
따라서 시작 전후 모든 잔존 glyph가 같은 위치라는 전제를 두지 않는다. 이 비교만
`NOT_APPLICABLE`로 기록하며 PASS 검사 수에 포함하지 않는다. 정지 후 동일 ELLIPSIS 화면의
복귀 비교는 혼합 BiDi에서도 유지한다. 순수 LTR/RTL의 위치 비교 허용 오차는 늘리지 않는다.

Marquee 시작 전에 overflow 양방향 전환을 새 CLIP/ELLIPSIS Label과 대조한다.
Report는 실행별로 분리되며, 지정한 누적 로그의 `[REPORT][BEGIN]`에서 개별 경로를 찾을 수 있다.

## 타깃 Window iconify/복원

기본 84개 case의 Label/부모 visibility 검사와 **실제 창 최소화**는 별개다.
실제 최소화를 지원하는 타깃에서는 다음 환경 설정으로 lifecycle 16개 조합을 추가 실행한다.
`<binary>`는 설치된 `manual-test-dali-ui-text` 경로로 바꾼다.

```sh
DALI_TEXT_CASE_FILTER=lifecycle DALI_TEXT_TEST_ICONIFY=1 \
  <binary> --test text-marquee --auto-verify --report /tmp/text-marquee-iconify.log
```

1. 시작·resize 검사 후 앱이 Public `Window.Minimize(true)`를 호출한다.
2. Public `VisibilityChangedSignal` 또는 timer에서 `IsMinimized()`가 실제 true인 것을 확인한 뒤 `Minimize(false)`로 복원한다.
3. visible 복귀, Marquee 재시작 위치·실제 이동 및 이후 Stop/재시작까지 완료되어야 PASS다.

최소화 중 앱의 timer가 정지하는 타깃에서는 agent가 타깃의 창 관리 도구로 **같은 앱 창을 복원**한다.
signal과 timer 모두에서 최소화 상태를 앱이 관찰하지 못했다면 외부에서 최소화/복원을 확인했더라도
이 자동 검사는 PASS가 아니다. 외부 timeout을 두고 로그·화면을 보존한다.
관측 가능한 상태에서 8초 내 최소화/복원이 확인되지 않으면 ERROR이며,
실제 최소화를 Label visibility 변경으로 대체하거나 지원되지 않는 환경을 PASS로 계산하지 않는다.
현재 Ubuntu X11 backend의 `Minimize()`는 no-op이므로 기본 실행에서 제외한다.
