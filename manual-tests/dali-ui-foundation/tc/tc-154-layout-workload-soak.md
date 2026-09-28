# 154. Layout: Workload soak

이 문서는 [tc-154-layout-workload-soak.cpp](tc-154-layout-workload-soak.cpp)의 실행 계약입니다. [suite 검증 기준](../layout-validation-coverage.md)을 먼저 읽으십시오. 이 TC는 모든 scenario에서 `LR_EXTERNAL_REQUIRED`를 출력합니다. 앱 내부 결과가 모두 맞아도 최종 verdict는 `EXTERNAL_REQUIRED`이며, 아래 실제 재진입·cleanup·메모리 관측까지 끝나기 전에는 서버의 최종 PASS를 기록하지 마십시오.

## 진입마다 실행할 전체 geometry

모든 step ID는 `run`입니다. 한 진입에서 다음 7개 scenario를 순서대로 전부 실행하십시오.

| 순서 | scenario | required_checks | 독립 기대값 |
|---|---|---:|---|
| 1 | `stack` | 771 | 256 cycle 각각 128개 leaf의 rect `(10*i,0,8,16)` + 최종 렌더 집계 3행 |
| 2 | `flex` | 771 | 위와 동일 |
| 3 | `grid` | 771 | 위와 동일 |
| 4 | `absolute` | 771 | 위와 동일 |
| 5 | `depth-8` | 34 | root Measure `(24,32)`, 아래 child 식 |
| 6 | `depth-64` | 258 | root Measure `(136,144)`, 아래 child 식 |
| 7 | `depth-192` | 770 | root Measure `(392,400)`, 아래 child 식 |

flat scenario는 workload를 화면의 stage에 부착하고 `ProcessLayouts()`로 settle한 뒤, cycle `c=0..255`마다 index `(37*c)%128`인 leaf의 Actor x를 `-123.25`, width를 `1`로 손상시키고 형제 leaf의 폭을 바꿔 controller pass(`ProcessLayouts()`)를 실행합니다. 즉시 **모든** leaf의 parent-local Actor event property를 독립 식과 대조하고, 마지막 cycle 뒤 새 `REFRESH_ONCE` render task 완료를 거친 current scene geometry로 `rendered.visited/mismatches/maximum_error` 3행을 검사합니다. 각 cycle의 `cycle.c.visited=128`, `cycle.c.mismatches=0`, `cycle.c.maximum_error<=0.001` 3행을 보존하십시오. `mismatches`는 x/y/width/height 각각의 불일치 수이며, maximum_error는 전체 좌표의 최대 절대오차입니다. 모든 leaf를 읽은 집계이며 leaf별 raw 512행을 출력하는 방식은 아닙니다. 16 cycle마다 root의 Measure/Arrange를 명시적으로 invalidate하고 pass를 실행합니다. 마지막 cycle이나 일부 leaf만 검사해서는 안 됩니다.

깊이 D는 padding=1인 Stack D개와 최하위 8×16 leaf이며 root를 stage에 부착합니다. root의 측정 크기는 `(8+2*D,16+2*D)`이고, 최하위부터 index `i=0..D-1`인 child의 parent-local 렌더 rect는 `(1,1,8+2*i,16+2*i)`입니다. root의 2행과 모든 child의 렌더 4행을 읽으십시오. 좌표 허용오차는 절대값 `0.001`, 개수/ID는 exact입니다. NaN/Inf, 행 누락, stale run/action, unexpected exception, crash, timeout, cleanup error는 FAIL입니다. 깊이(depth)와 flat scenario 모두 새 render task 완료 뒤 current scene geometry를 검사합니다. flat의 개별 cycle 검사만 event-side입니다. 화면 밖 노드의 current geometry를 읽는 것은 OS Window에 그 픽셀이 보였다는 증거가 아닙니다.

## 실제 TC 재진입 10회

1. 대상 app의 PID, `/proc/PID/stat` starttime, 실제 app/library build-ID와 revision, build type, 해상도·scale·locale를 기록하십시오. 동일 app process를 유지하고 다른 TC를 실행하지 마십시오. 로그를 수동적으로 파일에 기록하십시오. 10회 진입은 모두 같은 app process에서 수행하고, 결과를 별도 디렉터리에 보존하십시오.
2. launcher에서 `154. Layout`을 검색하고 진입하십시오. 자동 생성되는 `LR_CASE_CONTRACT`의 새 `(tc=TC154,pid,run)`을 기록하십시오. 이 진입을 `entry=1`로 지정하고, contract부터 Back의 END까지를 독립 파일 `entry-01.log`에 보존하십시오. 파일의 byte 시작/끝 offset도 원본 전체 로그와 연결하십시오. 각 진입에서 자동 생성되는 run을 사용하고 `Reset run`으로 재진입을 대신하지 마십시오.
3. `TC154.Run scenario`를 누르십시오. `LR_ACTION`의 scenario, step=`run`, action_seq, required_checks를 표와 대조하십시오. flat scenario는 마지막 render settle까지 비동기로 끝나며, 일반 실행에서는 120초 이내 `LR_READY`가 없으면 timeout FAIL입니다. 완료 시 해당 step의 전체 `LR_CHECK`가 stdout에 자동 출력되며 HUD에는 요약과 실패 행만 표시됩니다. 같은 행의 중복 출력은 검사 수를 늘리지 않습니다.
4. 결과를 확인한 뒤 `TC154.Next scenario`를 한 번 누르고 다음 scenario에서 3번을 반복하십시오. 각 진입의 필수 합계는 **7 scenario, 7 step, 4,146 assertion**입니다. 마지막 `LR_RESULT`는 `completed_scenarios=required_scenarios=7`, `completed_steps=7`, `checks=4146`, `failures=0`, `verdict=EXTERNAL_REQUIRED`여야 합니다. 이전 scenario 실패를 마지막 화면의 PASS로 덮지 마십시오. 그 전의 `FAILING`/`INCOMPLETE`는 종결 verdict가 아니므로 실행을 중단하지 마십시오. 완료 후 `< Back`으로 나가 `LR_CASE_END`의 `completed_scenarios`가 `required_scenarios`와 같은지 확인하십시오.
5. 마지막 결과를 보존한 후 `< Back`을 눌러 launcher로 나가십시오. Escape로 app를 종료하지 마십시오. 같은 `(tc,pid,run)`의 **`LR_CASE_END`**에서 `reason=exit`, `cleanup_passed=true`, `completed_scenarios=7`, `completed_steps=7`, `failures=0`을 반드시 확인하십시오. counts/failures는 마지막 `LR_RESULT`와 같아야 합니다. END 누락, 다른 run의 END, `reason=reset`, `LR_CLEANUP_ERROR`는 정상 종료 증거가 아닙니다.
6. END 후 다음 절의 RSS/PSS를 저장하십시오. launcher에서 아무 입력도 하지 않고 최소 5초 기다린 뒤, 1초 간격으로 3회 관측합니다. 세 관측이 끝나기 전에 다음 TC에 진입하지 마십시오. app 재시작, 강제 GC/allocator trim, cache 삭제, 창 크기·scale 변경은 금지합니다.
7. 같은 launcher에서 다시 `154. Layout`을 검색해 이 TC에 진입하여 2–6번을 **entry 10까지** 반복하십시오. 각 entry는 새로운 run ID를 가져야 하고 PID/starttime은 같아야 합니다. 1–2회는 warm-up이며 geometry/cleanup 실패가 면제되지 않습니다. 3–10회의 종료 후 값을 비교하십시오. 내부 subtree 생성 반복이나 Reset 10회는 이 계약을 충족하지 않습니다. 10개 로그 모두 각각 검사하고 최신 run만 검사하는 하나의 합친 로그로 대체하지 마십시오.

## RSS/PSS의 정확한 관측

실행 서버에서 Python 3와 대상 `/proc/PID/smaps_rollup` 읽기 권한이 필요합니다. 없으면 `resident_memory=NOT_EXECUTED`로 기록하고 external 검증을 완료하지 마십시오. 다른 앱의 PID를 탐색하거나 관측하지 마십시오. 다음 변수는 앞 절에서 **실제로 기록한** 값으로 지정하십시오. `TC154_EXE`는 실행 app의 절대 경로이며 `TC154_ENTRY_LOG`는 해당 entry의 END까지 저장한 로그입니다. `TC154_OUT`은 서버가 마련한 결과 디렉터리입니다. 출력은 entry별 JSONL 파일로 보존하십시오.

```sh
python3 - "$TC154_PID" "$TC154_RUN" "$TC154_ENTRY" "$TC154_ENTRY_LOG" "$TC154_EXE" <<'PY' > "$TC154_OUT/entry-$TC154_ENTRY-memory.jsonl"
import json, pathlib, sys, time
pid, run, entry = map(int, sys.argv[1:4])
log, executable = pathlib.Path(sys.argv[4]), pathlib.Path(sys.argv[5]).resolve()
proc = pathlib.Path('/proc') / str(pid)
ends = [json.loads(line.split(' ', 1)[1]) for line in log.read_text().splitlines()
        if line.startswith('LR_CASE_END ')]
end = next(e for e in reversed(ends)
           if (e['tc'], e['pid'], e['run']) == ('TC154', pid, run))
assert end['reason'] == 'exit' and end['cleanup_passed'] is True
assert (end['completed_scenarios'], end['completed_steps'], end['failures']) == (7, 7, 0)
assert (proc / 'exe').resolve() == executable
starttime = (proc / 'stat').read_text().rsplit(')', 1)[1].split()[19]
keys = ('Rss', 'Pss', 'Private_Clean', 'Private_Dirty', 'SwapPss')
time.sleep(5)
for sample in range(3):
    assert (proc / 'exe').resolve() == executable
    assert (proc / 'stat').read_text().rsplit(')', 1)[1].split()[19] == starttime
    raw = (proc / 'smaps_rollup').read_text()
    values = {}
    for line in raw.splitlines():
        fields = line.split()
        if fields and fields[0].rstrip(':') in keys:
            assert len(fields) == 3 and fields[2] == 'kB'
            values[fields[0][:-1]] = int(fields[1])
    assert set(values) == set(keys)
    assert (proc / 'stat').read_text().rsplit(')', 1)[1].split()[19] == starttime
    print(json.dumps(dict(tc='TC154', pid=pid, run=run, entry=entry,
          process_start_ticks=starttime, sample=sample, monotonic_ns=time.monotonic_ns(),
          unit='KiB', values=values, raw_smaps_rollup=raw)), flush=True)
    if sample != 2:
        time.sleep(1)
PY
```

명령 오류·권한 거부·process 변경·3행 미만 출력은 `NOT_EXECUTED`이며 0 KiB로 채우지 마십시오. `Rss`와 `Pss`는 전체 process의 resident/proportional memory이고 Layout 소유 heap 크기가 아닙니다. `Private_Clean+Private_Dirty`, `SwapPss`도 따로 기록하여 shared page 배분이나 swap에 의한 변화를 숨기지 마십시오. 필드 정의는 [proc 문서](https://docs.kernel.org/filesystems/proc.html)를 참고하십시오.

각 entry에서 3개 값의 median, min, max를 필드별로 계산하십시오. entry 2의 median을 B로 하여 entry 3–10 각각 `delta=median(entry)-B`, `spread=max-min`을 표로 보고하십시오. 또한 인접 entry의 delta와 최대값을 남기십시오. `Rss` 또는 `Pss`의 양의 delta가 보고 단위 1 KiB 이상이면 **성장 원인 조사 대상**입니다. spread가 증가량보다 크더라도 무시하거나 임의 퍼센트로 통과시키지 마십시오. 아래 allocation stack과 대조하여 원인이 분리될 때까지 `EXTERNAL_REQUIRED`입니다. RSS/PSS 증가 자체를 leak FAIL로 단정하지 않으며, 증가가 없다는 사실만으로 leak PASS를 내리지도 마십시오.

## 종료 시점의 allocation stack 관측

별도 **allocation-profile** 실행에서 같은 artifact·입력·10회 진입을 반복하십시오. 이 실행의 RSS와 시간은 일반 실행의 값과 비교하지 마십시오. 이미 설치된 Valgrind의 Massif와 `vgdb`, app/library의 build-ID에 대응하는 debug symbol이 필요합니다. 아래 방법은 외부 profiler를 사용하며 product에 allocator hook을 추가하지 않습니다. 도구 미설치, app 실행 불가, 통신 실패, 필요한 stack의 symbol 누락은 `allocation_profile=NOT_EXECUTED` 또는 `INCOMPLETE`이며 최종 PASS가 아닙니다. 자동으로 도구를 설치하거나 시스템 ptrace 설정을 바꾸지 마십시오.

1. `command -v valgrind`, `command -v vgdb`, `valgrind --version`을 저장하십시오. 서버의 기존 display/session bus, font, library path 환경을 그대로 사용하여 다음 명령으로 **검증 app만** 시작하십시오. `TC154_OUT`은 절대 경로이며 공백 없는 결과 디렉터리입니다. app에 필요한 인자는 기존 실행과 똑같이 추가하십시오.

   ```sh
   valgrind --tool=massif --time-unit=ms --stacks=no --threshold=0 --depth=64 \
     --vgdb=yes --massif-out-file="$TC154_OUT/massif.%p.out" \
     "$TC154_EXE" > "$TC154_OUT/allocation-profile.log" 2>&1
   ```

2. allocation-profile 로그의 `LR_CASE_CONTRACT.pid`를 `TC154_PROFILE_PID`로 기록하십시오. 앞 절의 7개 scenario 전체를 10회 진입하여 실행하고, 각 entry의 geometry·`LR_CASE_END`를 모두 보존하십시오. 이 allocation-profile 실행에서는 일반 실행의 120초 대신 사전에 고정한 600초/action 한도를 사용하십시오. 도구 자체의 실행 실패와 제품 assertion FAIL을 구분하되 어느 쪽도 PASS로 처리하지 마십시오.
3. 각 Back의 정상 END 후 launcher에서 5초 기다리십시오. 다음 진입 **전에** 아래 명령으로 그 시점의 live allocation과 전체 stack tree를 저장하십시오. `TC154_ENTRY`는 1–10이며 별도 파일을 사용하십시오. app 종료 후의 최종 heap만 읽으면 Back 이후 잔류를 놓치므로 대체할 수 없습니다.

   ```sh
   vgdb --pid="$TC154_PROFILE_PID" detailed_snapshot \
     "$TC154_OUT/entry-$TC154_ENTRY-massif.out" \
     > "$TC154_OUT/entry-$TC154_ENTRY-vgdb.log" 2>&1
   ```

   명령 전후의 서버 monotonic timestamp와 `(entry,pid,run,END)`를 연결하십시오. 출력 파일의 `heap_tree=detailed`, `mem_heap_B`, `mem_heap_extra_B`, stack별 byte 수를 읽고 원문을 보존하십시오. `--threshold=0`과 depth=64로도 관련 stack이 잘리거나 `???`로 끝나면 해당 원인은 미확인입니다. Massif의 `detailed_snapshot` 계약은 [공식 문서](https://valgrind.org/docs/manual/ms-manual.html#ms-manual.monitor-commands)에 정의되어 있습니다.
4. entry 2와 entry 3–10의 **동일 allocation stack별 live bytes**를 비교하십시오. 총량 감소가 특정 stack의 증가를 상쇄하도록 합산 판정하지 마십시오. TC fixture/결과 저장소/Layout buffer의 종료 후 추가 잔류가 없어야 하며, 기존 launcher/registry의 고정 소유분은 첫 baseline 및 실제 소유·해제 경로의 근거와 함께 구분하십시오. `mem_heap_extra_B`는 allocator 부가 공간이며 Layout 소유 byte로 이름 붙이지 마십시오. Massif가 보지 않는 GPU/native allocation은 이 allocation-profile 실행이 검증했다고 주장하지 마십시오.
5. 반복될수록 같은 stack에 해제되지 않은 추가 객체/byte가 남으면 allocation 성장 FAIL로 재현을 보존하십시오. 단순 RSS 증가이고 live heap은 복귀한 경우 allocator 보관·mapping·외부 resource 등 실제 원인을 분리하십시오. bounded cache라는 설명만으로 면제하지 말고 owner, 고정 상한, 해당 stack의 plateau, 재진입에 따른 추가 잔류 없음의 증거를 붙이십시오. 원인을 특정하지 못하거나 원래 증가가 profiler 실행에서 사라져 설명되지 않으면 `INCOMPLETE`, external 미완료입니다.

## LLM 최종 판정과 증거 묶음

각 일반 실행 entry 로그에 대해, 실행 전에 별도로 검토·고정한 contract manifest와 **신뢰 저장소의 SHA-256 pin**을 사용하십시오. candidate 로그에서 contract나 pin을 새로 만들지 마십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오.

```sh
python3 layout-validation-tools.py check-log "$TC154_ENTRY_LOG" --tc TC154 \
  --profile release --manifest "$TC154_MANIFEST" \
  --manifest-sha256 "$TC154_MANIFEST_SHA256" --external
```

`--external`은 external 검증을 완료시키는 옵션이 아닙니다. 정상 내부 geometry/cleanup 증거에서도 `external_verification_pending=true`, exit status=1이 예상됩니다. `invalid`/exit status=2나 assertion 실패를 정상 external 대기로 혼동하지 마십시오. 10개 로그 각각의 검사를 저장하십시오.

서버 결과에는 build type/artifact fingerprint, 10개 `(entry,pid,starttime,run)` 연결표, 각 entry의 7/7/4146 결과, END/cleanup, RSS/PSS 30개 raw snapshot과 비교표, allocation-profile 10개 raw snapshot과 stack별 비교표, 최초 실패/미완료 사유를 포함하십시오. 앱의 `EXTERNAL_REQUIRED` 원문을 PASS로 고치지 마십시오. 서버의 별도 `soak_verdict`만 다음 기준으로 판정하십시오.

- **FAIL**: geometry/계약/cleanup 오류, 대상 process의 예상치 못한 종료, 반복 allocation 잔류 증가가 재현됩니다.
- **NOT_EXECUTED / INCOMPLETE**: 10회 실제 진입이나 필수 관측이 빠졌거나, 필요한 profiler/권한/symbol이 없거나, 증가 원인이 미해결입니다. `soak_verdict=PASS`를 기록하지 마십시오.
- **PASS (명시한 10회·관측 범위)**: 모든 entry의 내부 assertion과 cleanup이 맞고, 일반 RSS/PSS 및 별도 allocation stack 증거가 모두 있으며, post-warm-up의 추가 잔류가 없거나 관측된 증가가 근거 있는 고정 소유분으로 설명됩니다. 이는 유한한 재진입 범위에서의 regression 미검출이며 모든 종류의 leak 부재나 시간 성능의 증명이 아닙니다.

실패한 run을 Reset하거나 재시작하여 삭제하지 마십시오. 재현용 재실행은 별도 series로 보존하십시오. 마지막 entry도 Back 및 END를 수집하고, profiler 종료와 검증 app의 정상 종료를 수행한 뒤 로그를 보존하십시오.

## 고정 실행 계약과 종료 증거

각 step 실행(`Run step`/`Run scenario`/`Run all`) 후 같은 action의 `LR_READY`를 stdout에서 수동적으로 기다리십시오. timeout 내 도착하지 않으면 FAIL이며, 이 완료 통지만으로 PASS를 판정하지 마십시오. 완료 뒤에 결과를 읽으십시오.

아래 목록은 검토된 manifest의 정확한 ID와 필수 행 수입니다. 설명의 축약 이름보다 이 목록을 우선하십시오. 각 step의 전체 stdout 행은 완료 시 자동 출력되므로 그대로 보존하십시오. stdout와 stderr를 함께 수집하십시오.

Profile `release`: 7 scenario, 7 action, 4146 checks. 이 표에 없는 scenario는 컴파일된 계약에 선언되지 않으며 실행·판정 대상이 아닙니다.

| Scenario ID | 순서대로 실행할 action ID : required_checks |
|---|---|
| `stack` | `run` : 771 |
| `flex` | `run` : 771 |
| `grid` | `run` : 771 |
| `absolute` | `run` : 771 |
| `depth-8` | `run` : 34 |
| `depth-64` | `run` : 258 |
| `depth-192` | `run` : 770 |

마지막 결과 수집 후 `< Back`으로 나가 같은 `(tc,pid,run)`의 `LR_CASE_END`를 수집하십시오. `reason=exit`, `cleanup_passed=true`이고 `completed_scenarios == required_scenarios`이며 모든 count(failures 포함)가 마지막 `LR_RESULT`와 같아야 합니다. PASS는 그 완료 조건과 failures=0이 함께 성립할 때만입니다. `layout-validation-coverage.md`에 알려진 제품 결함 행이 문서화된 TC는 failures>0으로 종결되며, `check-log`가 `observations-valid-with-failures`로 그 행을 **정확히** 나열해야 합니다. 그 밖의 실패 행이나 `invalid-evidence`는 회귀 또는 무효 증거이며 PASS가 아닙니다. END 누락·cleanup 오류도 PASS가 아닙니다.

서버가 사전 고정한 manifest와 SHA256으로 `python3 layout-validation-tools.py check-log LOG --tc TC154 --profile release --manifest MANIFEST --manifest-sha256 PIN`을 실행하십시오. `--profile`에는 manifest의 유일한 profile인 `release`를 지정하십시오. `PIN`을 candidate 출력에서 즉석 생성하지 마십시오. 계약이 다르면 별도 검토가 필요합니다. `status`가 `observations-valid`면 실패 행이 없고, `observations-valid-with-failures`면 증거는 유효하며 `failing_rows`에 나열된 행만 실패했다는 뜻이고(exit 1), `invalid-evidence`면 증거 자체가 무효이므로 재실행해야 합니다(exit 2). `observer.timeout`은 layout 완료 관측 실패, `render.frame-timeout`은 새 render task 또는 입력 반영 완료 관측 실패, `render.settle-timeout`은 current geometry 반영 관측 실패로 분류하십시오. 이 과정에서 필수 관측 행이 누락되거나 실제 행 수가 계약과 달라지면 strict 검증 결과는 `invalid-evidence`(exit 2)입니다. 해당 observer 행, `protocol.required_checks`, 도구의 `error`, 원본 stdout/stderr를 함께 보존하십시오. 재실행은 새 run 증거로 별도 기록하고 첫 실패를 삭제하지 마십시오. timeout, 필수 행 누락, `LR_READY` 또는 END 누락을 **어떤 경우에도 PASS로 처리하지 마십시오**. 외부 검증이 필요한 TC의 `--external` 결과는 pending이며 exit 1입니다. 해당 MD의 외부 증거가 없으면 전체 PASS가 아닙니다.
