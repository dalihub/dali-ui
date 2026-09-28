# Layout validation 도구 판정 계약

이 도구는 TC를 실행하지 않습니다. 서버의 실행 에이전트는 TC별 MD에 따라 앱을 조작하고 원본 stdout/stderr, 종료 상태, artifact hash를 보존한 뒤 이 도구로 판정합니다. JSON 출력과 process exit code를 함께 확인하십시오. 화면에 표시된 PASS만으로 최종 PASS를 결정하지 마십시오.

## 실행 전 계약 검사

저장소 root에서 다음 명령을 실행하십시오.

```sh
python3 manual-tests/dali-ui-foundation/layout-validation-tools.py audit
python3 manual-tests/dali-ui-foundation/layout-validation-tools.py self-test
```

각 명령의 exit code가 0이고 `status`가 각각 `inventory-valid`, `self-test-pass`여야 합니다. `audit`은 TC/MD, MD의 scenario/action/check 수, 문서가 인용한 manifest SHA-256을 검사합니다. 이 결과는 실제 앱의 실행 성공을 뜻하지 않습니다.

Candidate 결과에 맞춰 mandatory check 수나 pin을 자동으로 재생성하지 마십시오. 계약 변경은 source와 독립된 expected 및 검증 범위를 검토한 뒤 이루어져야 하며, manifest pin은 실행 전에 서버 설정에 고정해야 합니다.

## 관측 실패와 strict 검증

`observer.timeout`, `render.settle-timeout`, `render.frame-timeout` 또는 `render.unsupported-parent` 이후 필수 관측이 누락되거나 행 수가 맞지 않는 경우에도 strict 검증은 `invalid-evidence`를 유지합니다. 원본 관측 실패 행과 `error`를 함께 보존하고 frame/layout 관측 실패로 보고하십시오. 재실행했다면 첫 실패 기록과 새 실행의 identity를 모두 보존해야 하며, timeout을 무시하거나 허용 오차를 넓혀 성공으로 바꾸지 마십시오.

## Frame fence와 `LR_FRAME`

`AfterFrame`은 window에 있는 1x1 sentinel actor를 source로 하는 새 `REFRESH_ONCE` render task의 완료를 기다립니다. framebuffer가 없으므로 pixel readback도 GPU sync도 없고, 개별 시도마다 만료되는 별도 timeout도 없습니다. task는 event-side 쓰기가 모두 큐에 들어간 뒤에 생성되므로, 완료는 update 한 번과 render 한 번이 그 쓰기를 모두 소비했다는 뜻입니다. OS window의 presentation이나 compositor 출력이 아니라 fresh scene state의 증거입니다. 전체 fence의 유일한 한도는 `FRAME_DEADLINE_MS`(4000 ms)이며, 이는 fence 이후의 scene-graph settle watchdog인 `DEFAULT_SETTLE_MS`와 독립된 상수입니다.

정상 완료는 아무 기록도 남기지 않습니다. 비정상 완료는 `render.frame-timeout` 실패 행과 그 action의 `protocol.required_checks` 실패 행을 만들고, 동시에 stdout과 stderr에 기계가 읽을 수 있는 `LR_FRAME` 레코드를 한 줄 남깁니다.

```
LR_FRAME {"tc":"TC152","pid":123,"run":1,"scenario":"…","step":"…","action_seq":15,"result":"timeout","reason":"…","attempts":3,"elapsed_ms":4001.2,"deadline_ms":4000}
```

`result`는 `timeout`, `disconnected`, `predicate-exception` 중 하나입니다. `LR_FRAME`은 행이 아니므로 어떤 계약 수치도 바꾸지 않지만, `check-log`는 이 레코드를 검사 대상으로 삼습니다. 형식이 어긋나면 `malformed frame record`로 `invalid-evidence`입니다.

레코드는 **귀속(attribution)될 수 있어야** 증거입니다. `action_seq`는 계약이 선언한 action이어야 하고(`frame record for an undeclared action`), 같은 run에서 이미 시작된 action이어야 하며(`frame record before its action`), `scenario`/`step`이 그 action과 같아야 합니다(`frame record scope mismatch`). 레코드는 stdout과 stderr에 함께 나가므로 **파싱 후 동일한**(키 순서와 공백은 무관) 중복만 같은 레코드로 받아들이고, 값이 달라진 중복은 `changed frame record`입니다. 레코드와 행은 한 사실의 양면이라 어느 쪽도 홀로 설 수 없습니다. 레코드만 있고 행이 없으면 `frame failure record without its render.frame-timeout row`, 행만 있고 레코드가 없으면 `render.frame-timeout row without its frame record`이며 둘 다 `invalid-evidence`입니다. 행 수가 맞지 않는 action에 frame 레코드가 있으면 그 원인을 메시지에 함께 표시합니다.

frame 레코드가 하나라도 있으면 실행은 PASS가 아닙니다. `check-log`의 status는 `observations-valid-with-failures`가 되고 CLI 종료 코드는 0이 아닙니다. `check-log` 결과의 `frame_failures`에 TC별로 `action_seq`, `scenario`, `step`, `result`, `reason`, `attempts`, `elapsed_ms`가 남으므로 이 값을 그대로 보고하십시오.

`LR_FRAME`이 나왔다면 deadline을 늘리거나 재시도를 추가해 통과시키지 마십시오. fence가 만료됐다는 것은 그 action이 관측하려던 frame이 실제로 오지 않았다는 뜻이며, 원인을 규명해 보고해야 합니다.
