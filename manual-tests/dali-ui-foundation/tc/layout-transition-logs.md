# Layout transition을 실행 후 로그로 판정하는 방법

원래 TC 버튼·drag를 수행하면서 stdout(Tizen STDOUT dlog)을 수집하고, 실행 후 AI가 실제 관측값을 기대 결과와 비교한다. 별도 Record/조회 버튼, 표본 버퍼, 전환 정지·seek·수동 tick·자동 PASS/FAIL은 없다. 전환은 원래 timing으로 실행된다.

## 실행 및 수집

1. TC 진입 전부터 stdout/dlog 수집을 시작한다. 사용 중인 실행 도구로 로그를 파일에 보존하고 TC 진입·조작 순서·종료 구간을 기록한다. 수집기가 프로세스의 다른 로그를 함께 저장한다면 `[TC78]`~`[TC82]` 태그로 추출한다. 앱 로그는 외부 파일로 보존해야 하며 앱 내부에 다시 조회할 기록은 없다.
2. TC별 원래 버튼 또는 drag를 수행한다. 일반적인 독립 동작 사이에는 약 1초를 두고 관측 창의 종료와 최종 상태를 확인한다. 중도 취소와 연속 reorder는 해당 TC 절차를 따른다.
3. 실행 후 로그를 읽는다. ONEMAIN PontusM의 첫 트리 읽기가 0.5~0.7초 걸려 0.4초 전환을 놓쳤던 조건에서도, 전환 도중 앱이 남긴 로그를 사후 분석할 수 있다. 전환 중 접근성 트리나 화면을 반복 조회할 필요는 없다.
4. 같은 TC/run/actor ID의 시작·유효 중간·최종 표본을 연결해 판정한다. 중간 표본 누락, 로그 잘림·유실, actor 식별 실패는 **미검증**으로 기록한다. 수집기를 확인한 뒤 다시 실행한다. 최종값만으로 중간 보간을 통과시키지 않는다.
5. 최종 화면의 잔상·중복·SVG 로딩과 실제 gesture·scroll은 기존 화면 확인 절차를 유지한다. 로그는 OS compositor의 실제 표시 완료나 화면 전체의 시각적 정확성을 증명하지 않는다.

## 로그 필드

- `[TC78][ENTER]`와 같은 주 표본은 `run`, `sample`, `t`, `clock`, `kind`, `peers` 및 관찰 대상의 `id/name/parent/x/y/w/h/opacity/scene`을 포함한다. `t`는 해당 probe 시작 이후 경과 시간, `clock`은 프로세스 내 monotonic timestamp이다. raw 진행률이나 정확한 프레임 시각이 아니다.
- `[TCxx][phase][PEER]`는 같은 run/sample의 다른 관찰 대상이다. 주 표본의 `peers` 수와 행 수를 대조해 누락을 확인한다. 좌표는 각 actor의 parent 기준이다. TC81의 card/inner와 ScrollView offset·화면 좌표를 혼동하지 않는다. name이 없어도 ID와 parent로 연결한다.
- `kind=pre-layout`는 조작 직후 첫 표본이다. EXIT는 Remove 직전 값이며 다른 동작은 요청 변경 이후 첫 layout 전 값일 수 있다. 새 ENTER actor는 기본 current 값일 수 있으므로 이를 유효 시작 프레임으로 판정하지 않는다.
- `kind=timer`는 16ms Timer가 요청해 읽은 current 속성이다. 실제 주기나 새 프레임 완료를 보장하지 않으며 같은 값이 반복될 수 있다. `kind=stop`은 후속 조작/이탈 직전 마지막 읽기이며 연결·요청 상태가 이미 바뀌었을 수 있다.
- `[END]`는 약 0.8초 관측 창 종료, `[STOP]`는 새 run 또는 화면 이탈로 관찰이 중단됐다는 뜻이다. 정상 animation 완료 신호나 PASS/FAIL이 아니다. 후속 동작으로 기존 run이 STOP돼도 이전 로그는 외부 수집 파일에 남는다.
- `[TC79][CONTEXT]`는 실제 callback마다 기록한 `clock/id/slot/raw/eased/from/to`이다. ENTER=0, EXIT=1, CHANGE=2이다. ID·slot·clock과 run의 경계를 함께 사용해 연결한다. context는 event thread에서 읽고 geometry는 current 속성이므로 **동일 프레임으로 짝지어 비교하지 않는다**.
- EXIT 후 parent=0·scene=0이면 제거를 확인할 수 있다. 분리된 actor의 current 크기·opacity는 마지막 렌더 값일 수 있으므로 최종 0을 필수 조건으로 요구하지 않는다.

## 독립 기대값

TC79의 SINE_IN_OUT context는 `e=(1-cos(π×raw))/2`를 AI가 계산해 eased와 비교한다. 정확히 raw=0.25/0.5/0.75인 표본을 요구하지 않는다. geometry는 별도로 유효한 중간값의 존재·이동 방향·상대 배치와 최종 목표값을 검사한다.

Spec 방식 TC78/80/81/82의 내부 Animation 진행률은 읽지 않는다. `t/0.4`나 geometry에서 역산한 값을 독립 raw 기대값으로 사용하지 않는다. 기본 허용 오차는 context easing 0.001, opacity 0.01, geometry 1 layout 단위이며 수치는 effective scale=1 기준이다. 실제 scale과 판정 기준을 기록한다. Timer 표본만으로 실제 16ms 동시 완료를 단정하지 않는다.

AI가 항목별 expected/observed/verdict와 이유를 작성한다. 예: `TC78 ENTER: expected 0<h<80, 0<opacity<1인 중간 표본과 최종 80/1; observed run=..., sample=..., id=..., h=..., opacity=...; verdict=PASS/FAIL/미검증`. 원본 로그 파일의 해당 구간과 조작 순서·최종 화면을 보존한다. 정확한 시간별 보간에 대한 UTC를 인용한다면 실제 검증 범위를 확인하며, context snapshot UTC만으로 실제 clock 보간까지 검증했다고 기록하지 않는다.
