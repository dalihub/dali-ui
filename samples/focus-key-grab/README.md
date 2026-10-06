# Focus / Key Grab Probe

메인윈도우가 active인 상태에서 비활성 서브윈도우의 독립 포커스와 실제 키그랩
전달을 비교하는 샘플입니다. 변경된 dali-ui의 `SetIndependentFocusEnabled()`와
윈도우별 조회·해제·변경 신호를 사용합니다. 일반 View만 사용하며 IME는 대상이 아닙니다.

## 초기 상태와 조작

메인의 네이티브 focus-in을 기다린 뒤 `main.view`를 포커스합니다. 서브는 Show 전에
`SetAcceptFocus(false)`를 적용하고 Show/Raise합니다. Tizen에서는 방향키 4개와
Return/KP_Enter를 EXCLUSIVE grab하고 결과를 각각 로그로 남깁니다.
독립 옵션의 초기값은 **꺼짐**이며, 표시가 확인되면 `sub.view`에 포커스를 요청합니다.
이때 `true`는 요청 저장을 포함하므로 실제 상태·조회·신호를 함께 확인합니다.

각 윈도우에는 두 개의 focusable View가 있습니다. `Right`/`Down`은 `.view`에서
`.next`로, `Left`/`Up`은 반대로 이동합니다. 키를 합성하거나 직접 View에 전달하지 않습니다.

| 키 | 동작 |
| --- | --- |
| 1 / Menu / m | 서브 Show/Raise, grab 적용, 표시 확인 후 sub.view 포커스 요청 |
| 2 | grab 해제 및 서브 숨김. 메인 포커스에 새 요청을 하지 않아 유지 여부를 관찰 |
| 3 / 4 | main.view / sub.view에 명시적 포커스 요청 |
| 5 | grab 사용 설정 토글 |
| 6 | 모든 테스트 View의 키 소비 false/true 토글. 초기값 false |
| 7 | 서브 독립 포커스 옵션 토글. 켜기만 하면 저장된 대상은 포커스되지 않음 |
| 8 | ClearFocus(subWindow) |
| 9 | 서브 Show/Raise 및 grab 적용만 수행. 포커스 요청 없음 |
| 0 | 전체 상태 스냅샷 |
| Back / Escape | 종료 및 grab 해제 |

제어 명령은 Intercept에서 UP을 기록한 뒤 이벤트 전달 완료 후 타이머로 실행합니다.
제어키는 grab 대상이 아니므로 일반적으로 메인윈도우로 전달됩니다.

## 실기기 절차와 기대 결과

1. 시작 로그에서 `main.active=1 sub.active=0 sub.accept=0 sub.visible=1`과 테스트할
   `GrabKey ... result=1`을 확인합니다. 아니면 요구한 네이티브 구성이 성립하지 않은 것입니다.
2. 기본 옵션에서 `actual=main.view main.actual=main.view sub.actual=none`,
   `main.view.focused=1 sub.view.focused=0`을 확인합니다. grab된 키는
   sub.intercept/sub.window에서 보이며 두 윈도우의 테스트 View에는 전달되지 않습니다.
3. **7 → 0**: `independent=1`이어도 `sub.actual=none`입니다.
   **4 → 0**: 전역과 메인은 main.view, 서브는 sub.view입니다.
   두 `.view.focused`가 1이고 `main.view.ViewFocus=0` 및 전역 main→sub 신호가 없어야 합니다.
4. grab된 Right를 누릅니다. sub.view → sub.root → sub.window 순으로 기록된 뒤
   서브 내부 탐색으로 sub.next가 포커스됩니다. `WindowFocus[sub]`만 변경되고
   메인 상태와 전역 대상은 유지됩니다. 콜백 등록 순서에 따라 Window 신호 관찰과
   탐색 신호의 로그 순서는 달라질 수 있습니다.
5. **6**으로 소비를 켜고 Left를 누릅니다. sub.next에서 `consumed=1`로 끝나며
   부모/Window 키 전달 및 방향키 탐색은 중단됩니다. 6으로 소비를 끕니다.
6. **5**로 grab을 해제하고 같은 방향키를 누릅니다. 메인윈도우와 그 실제 포커스에
   전달·탐색이 발생하고 서브 포커스는 유지됩니다. 다시 5로 grab합니다.
7. **2 → 9 → 0**: 숨김은 서브만 해제하고 재표시만으로 복원하지 않습니다.
   **4**로 명시적으로 복원합니다. **7**로 옵션을 끄면 서브만 해제됩니다.
8. **7 → 4 → 8**로 윈도우별 명시적 해제도 확인합니다. 종료 시 grab을 해제합니다.

관찰 경로는 Intercept → 실제 키 대상 View → 같은 윈도우의 부모 → 소비되지 않은
Window KeyEventSignal입니다. `FocusKeyGrab` 로그의 `#번호`는 콜백 순서입니다.
같은 key/code/time/DOWN·UP/event.windowId로 사건을 비교합니다. ID가 0이면 전달한
윈도우로 라우팅하고, 유효한 nonzero ID는 전달 윈도우의 nativeId와 일치해야 합니다.
화면은 로그를 줄여 표시하므로 전체 필드는 dlog에서 확인합니다.
위 내용은 **기대 결과**이며 Tizen 실기기 결과는 아직 수집하지 않았습니다.

## 빌드와 설치 버전

새 API가 없는 기존 SDK로는 이 샘플을 빌드할 수 없습니다. 변경된 dali-ui의 foundation,
components 및 development RPM으로 샘플을 빌드하고 같은 라이브러리를 기기에 설치합니다.
core/adaptor도 현재 소스가 요구하는 API를 제공하는 호환 버전이어야 합니다.
기존 `bin/tizen`에 이전 기본 정책 샘플 RPM이 남아 있다면 새 기능 검증에 사용하지 마십시오.

시작 디렉터리는 `/home/puro/workspace/submit/dali-ui`입니다. 기기의 profile과 아키텍처로
치환합니다. 첫 명령은 라이브러리를 빌드하며, 생성된 development RPM을 동일 GBS
로컬 repository/buildroot에 반영한 뒤 두 번째 명령을 실행합니다.

```sh
gbs build -A armv7l --include-all --packaging-dir packaging
gbs build -A armv7l --include-all \
  --packaging-dir samples/focus-key-grab/packaging
```

다른 profile은 `-P <profile>`, 아키텍처는 `-A aarch64` 등으로 지정합니다.
`--include-all`은 아직 커밋하지 않은 변경을 포함합니다. 샘플 spec은 TIZEN=ON으로
빌드하며 실행 파일과 manifest를 설치합니다. manifest에 keygrab privilege를 선언했으며
EXCLUSIVE grab의 성공은 기기의 앱 권한과 compositor 정책에 따라 확인해야 합니다.

개발 기기에 변경된 라이브러리 RPM과 샘플 RPM을 설치한 뒤 실행합니다.
버전·release·revision·빌드 시각·아키텍처를 기록합니다.

```sh
sdb root on
sdb push <sample.rpm> /tmp/focus-key-grab.rpm
sdb shell rpm -Uvh /tmp/focus-key-grab.rpm
sdb shell app_launcher -s com.samsung.dali.focus-key-grab
sdb shell dlogutil DALI:I | rg FocusKeyGrab
```

초기 로그도 남기려면 dlog 수집을 먼저 시작합니다. `<sample.rpm>`을 실제 경로로 바꿉니다.

## Ubuntu 프리뷰

호환 DALi SDK를 `$DESKTOP_PREFIX`에 설치하고 pkg-config/라이브러리 환경을 설정합니다.

```sh
cmake -S samples/focus-key-grab -B /tmp/dali-ui-independent-sample-build \
  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="$DESKTOP_PREFIX"
cmake --build /tmp/dali-ui-independent-sample-build --parallel 4
./samples/focus-key-grab/bin/focus-key-grab.example
```

Ubuntu에서는 네이티브 keygrab을 호출하지 않습니다. X11 키 전달과 두 윈도우 상태를
관찰할 수 있지만 grab 검증은 Tizen에서 수행합니다. 진단 로그와 애플리케이션 설정에
Adaptor integration/devel 헤더를, 키 대상 판정에 foundation extension API를 사용하며
dali-ui 내부 구현을 참조하지 않습니다.

생성한 armv7l RPM은 `bin/tizen/independent-focus-2.1.0-armv7l/`에 있습니다.
앱 2.1.0, foundation/components 2.5.41.11517, core/adaptor 2.5.41 런타임과
SHA256SUMS를 모았습니다. `public_10.0_TV` 프로필에서 빌드했으므로 기기의 프로필과
아키텍처가 일치하는지 확인합니다. 라이브러리와 샘플의 development RPM 및 전체 로그는
`/tmp/dali-ui-independent-gbs/local/repos/public_10.0_TV/armv7l/`에 있습니다.

현재 구현의 빌드·자동 테스트·GBS 결과는
[구현 리뷰](../../../WithAI/dali-ui/multi-window-focus-exception/02-implementation-review.md)에 기록합니다.
이전 기본 정책 버전의 실행 기록은 현재 독립 포커스 기능의 검증 증거로 사용하지 않습니다.
