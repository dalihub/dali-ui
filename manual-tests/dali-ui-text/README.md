# DALi UI Text Manual Tests

사람과 agent가 실제 화면에서 같은 테스트를 실행하는 앱이다.
현재 `Text Layout`, `Ellipsis`, `Marquee` 순서로 제공하며, `manual-tests/dali-ui-foundation`과 같은
registry / `OnEnter()` / `OnExit()` 구조를 사용한다.

테스트와 공용 검사 모듈은 DALi **Public API만** 사용한다. Internal / integration / extension /
devel API나 UTC mock으로 제품 내부 상태를 검사하지 않는다. Emoji Sequence internal 검증은
이 앱에 포함하지 않는다. 앱 실행에 필요한 `Application`이 adaptor devel API로 이동하여,
launcher의 `main.cpp`에서 해당 헤더를 직접 include하는 경우만 예외로 허용한다.
매 빌드에서 앱 소스의 비공개 API include/namespace 사용을 검사한다. 이는 실수를 막는
소스 검사이며, 제품 내부 상태 검증은 앱이 아닌 별도의 UTC에만 둔다.

## 빌드

저장소 루트 `dali-ui/`에서 실행한다. 현재 소스와 호환되는 DALi core, adaptor,
ui-foundation library와 public development headers, libpng 1.6 development package가 필요하다.
Tizen에서는 `Application` 헤더를 제공하는 `dali2-adaptor-integration-devel`도 필요하며,
spec의 BuildRequires에 명시되어 있다. 별도의 integration library를 링크하는 것은 아니다.

libpng는 PNG 기반 자동 픽셀 검사용이다. libpng 직접 의존성이 없는 수동 조작 빌드는
`-DTEXT_PIXEL_CHECKS=OFF`로 설정한다. 이때 Text Layout 자동 검증과 모든 수동 버튼은
그대로 사용하며 Ellipsis/Marquee 자동 픽셀 검증은 명확한 ERROR로 종료한다.
전체 기능 빌드는 기본값 `ON`이며, configure에서 PNG 파일 읽기·쓰기 API의 compile/link를 확인한다.
배포판별 지원과 타깃 주의사항은 [libpng 의존성](common/README.md#libpng-의존성)을 참고한다.

### Ubuntu

DALi 환경 설정으로 `DESKTOP_PREFIX`, `PKG_CONFIG_PATH`, runtime library 경로를
준비한 뒤 실행한다.

```sh
cmake -S manual-tests/dali-ui-text -B manual-tests/dali-ui-text/build -DCMAKE_BUILD_TYPE=Release
cmake --build manual-tests/dali-ui-text/build -j4
./manual-tests/dali-ui-text/bin/manual-test-dali-ui-text
```

`manual-tests/` 루트에서 전체 앱을 빌드할 때도 자동으로 발견된다.
창 크기는 지정하지 않으며 플랫폼의 기본 크기와 사용자의 resize를 따른다.
Ubuntu에서 작은 화면을 확인하려면 실행 환경으로 크기를 지정할 수 있다.

```sh
DALI_WINDOW_WIDTH=800 DALI_WINDOW_HEIGHT=480 \
  ./manual-tests/dali-ui-text/bin/manual-test-dali-ui-text
```

### GBS / 타깃

```sh
gbs build -A armv7l --include-all \
  --packaging-dir manual-tests/dali-ui-text/packaging \
  --spec com.samsung.dali.ui-text-manual-test.spec
```

`armv7l`은 예시이며 실제 타깃 architecture에 맞춘다. 타깃 이미지와 일치하는
GBS profile/repository를 사용하고, 현재 public API를 포함한 DALi RPM을 먼저
준비한다. 오래된 platform DALi package로 최신 API 테스트를 빌드하지 않는다.

생성한 `com.samsung.dali.ui-text-manual-test` RPM을 타깃에 설치하면 다음 경로에
앱과 두 ImageSpan 이미지가 설치된다.

```text
/usr/apps/com.samsung.dali.ui-text-manual-test/
├── bin/manual-test-dali-ui-text
└── res/{flag_kr.png,flag_us.png}
```

앱 ID는 `com.samsung.dali.ui-text-manual-test`이다.
타깃의 앱 실행 도구로 실행하거나, compositor/session 환경이 준비된 타깃 shell에서
위 binary를 직접 실행한다. 자동 모드도 실제 rendering을 수행하므로 그래픽 세션이
필요하다. Shell 환경 문제로 앱이 열리지 않으면 앱 실행 도구로 실행한 뒤 UI 절차를 사용한다.

## 실행

| 목적 | 방법 |
|---|---|
| 사람이 테스트 | 목록에서 항목 선택, 화면 버튼 또는 키보드 조작 |
| agent의 UI 실행 | `text-layout` 항목 → `text-layout-V` 클릭 → 최종 status 확인 |
| 항목 직접 열기 | `--test text-layout` |
| Ellipsis 픽셀 검사 | `--test text-ellipsis --auto-verify` |
| Marquee 픽셀 검사 | `--test text-marquee --auto-verify` |
| 자동 검증 후 종료 | `--auto-verify` (기본 항목: `text-layout`) |
| 로그 위치 지정 | `--report /tmp/text-layout.log` |
| 항목 목록 / 도움말 | `--list` / `--help` |

```sh
./manual-tests/dali-ui-text/bin/manual-test-dali-ui-text \
  --test text-layout --auto-verify --report /tmp/text-layout.log
```

GUI의 `Run auto` / `V`와 CLI는 **같은 Sync + Async 검증 목록**을 실행한다.
GUI는 최종 결과를 남긴다. Ellipsis/Marquee는 `Reset [Z]` 또는 다른 수동 버튼으로
수동 조작을 재개할 수 있다. CLI는 완료 후 종료한다.

| Exit code | 의미 |
|---|---|
| 0 | 전체 자동 검증 PASS |
| 1 | 한 개 이상 검증 실패 |
| 2 | 잘못된 항목/옵션, report/PNG I/O, 캡처 또는 검증 환경 오류 |
| 3 | 사용자가 완료 전에 취소/종료 |

일반 GUI 앱 종료의 0은 자동 검증 PASS를 의미하지 않는다.
timeout, signal 종료, 해당 항목의 최종 `[AUTO][PASS]` 없는 로그도 PASS로 판정하지 않는다.

자동 검증 로그는 stdout과 `/tmp/dali-ui-text-<pid>.log`에 기록한다.
`--report`로 변경할 수 있으며 **기존 파일에 누적하므로 다른 항목이나 재실행 결과를 덮어쓰지 않는다**.
각 실행은 별도로 `<지정 경로>.<test-id>.XXXXXX/report.log`에도 저장한다.
화면의 Report는 이 개별 로그이며 `[REPORT][BEGIN]`에서 경로를 찾을 수 있다.
누적 로그의 예전 PASS를 현재 결과로 읽지 않도록, agent는 해당 실행의 개별 로그와 최종 결과를 확인한다.
경로의 부모 디렉터리는 미리 준비한다.

자세한 UI 구성, 자동 검증 범위, 실패 분석과 타깃 agent 절차는
[Text Layout 테스트 시나리오](tc/tc-text-layout.md)를 따른다.

픽셀 검사는 [Ellipsis](tc/tc-text-ellipsis.md), [Marquee](tc/tc-text-marquee.md)를 따른다.
Ellipsis는 Sync 27개 + Async 27개, Marquee는 Sync 42개 + Async 42개를 실행하고,
Report 옆의 고유한 `.images-XXXXXX/` 경로에
PNG를 남긴다. 작은 화면에서도 실행할 수 있지만 검증 중 resize는 ERROR로 처리한다.
자동 CLI 실행은 다른 앱의 키 입력을 가져오지 않도록 window focus를 요청하지 않는다.

재현 범위를 줄이려면 `DALI_TEXT_CASE_FILTER=lifecycle`처럼 case 이름의 부분 문자열을
지정한다. 선택된 case도 Sync/Async 양쪽을 실행하며, 일치하는 case가 없으면 ERROR다.
필터 실행의 PASS는 전체 검증 PASS를 대신하지 않는다. 실제 창 최소화 검증은
[Marquee 타깃 iconify 절차](tc/tc-text-marquee.md#타깃-window-iconify복원)를 따른다.

두 항목은 시스템 폰트를 사용하는 관계 기반 검사다. 모든 BiDi ellipsis의 정확한 잔존
문자나 shaping을 독립적으로 판정하지 않는다. StyledText / Builder 전용 TC는 아직 추가하지 않았다.
공용 모듈의 재사용 방법, 의존성과 한계는 [이미지 검사 모듈](common/README.md)을 참고한다.

## 수동 조작

Ellipsis/Marquee는 Text Layout과 같은 화면 구성을 사용한다.
상단의 전체 폭 Run/Cancel 전환 버튼, 짙은 진행 정보, 색상 상태 배너는 고정되어 있다.
아래에는 case 이동·설명, 회색 미리보기와 빨간 외곽선, 측정값·결과 카드와 옵션을 배치한다.
작은 화면에서는 세로 스크롤하고, 폭 1000px 이상에서는 미리보기와 옵션을 좌우로 배치한다.
설정·측정값·실행 상태는 짙은 카드로, 실패 상세와 파일 경로는 밝은 결과 카드로 표시한다.

자동 검증 중에는 같은 미리보기를 스크롤 밖의 고정 영역으로 옮긴다. ScrollView의 clipping이
픽셀 캡처를 가리지 않게 하기 위한 배치이며, fixture의 크기·텍스트·비교 영역은 바뀌지 않는다.
완료·취소·오류 시 원래 스크롤 배치로 돌아가며 결과 카드와 수동 버튼을 확인할 수 있다.
높이 450px 미만의 자동 화면은 상단 상태 표시를 한 줄씩으로 줄여 미리보기와 조작 공간을 확보한다.
버튼은 clickable state effect를 사용한다. 키보드 없이도 모든 옵션을 조작할 수 있다.
옵션 변경은 같은 Label에 적용하며 Previous/Next/Reset만 새 Label로 초기화한다.
자동 검증 중에는 수동 옵션을 잠가 입력이 검증 조건을 바꾸지 않게 한다.

| 항목 | 버튼 / 단축키 |
|---|---|
| 기본 case 선택 | Previous `B`, Next `N`, Reset `Z` |
| 영역 크기 | Width `-` / `+`, Height `[` / `]` |
| 렌더링·생략 | Sync/Async `A`, ELLIPSIS/CLIP `E` |
| 방향·정렬 | LTR/RTL `D`, START/CENTER/END `H` |
| 줄바꿈 | Multiline `M`, MaximumLines 0~4 `K`, Wrap 4종 `W` |
| 스타일·텍스트 | Padding `P`, Font size `F`, RenderScale 1/2 `U`, 짧은/원래 문장 `Y` |
| Marquee 실행 | Start `R`, Stop `S`, Horizontal/Vertical `O` |
| Marquee 종료·트리거 | IMMEDIATE/FINISH_LOOP `I`, MANUAL/ON_OVERFLOW `T` |
| Marquee 반복·속도 | 무한/1/2/3회 `L`, Speed `C`, Gap `G`, Delay `J` |
| 자동 검증 | 같은 버튼/`V`로 Run ↔ Cancel 전환 (`X`도 Cancel 단축키로 유지) |

현재 크기, NaturalSize, HeightForWidth, LineCount, RUNNING/STOPPED도 표시한다.
상태 문구는 값이 바뀔 때만 갱신한다. Wrap의 hyphenation 결과는 장치의 사전 지원에
영향받으므로 고정 이미지 정답으로 판정하지 않고 수동 옵션으로 제공한다.

## 테스트 추가

`tc/tc-<feature>.cpp`와 같은 이름의 `.md`를 함께 추가한다.
`text-test-case.h`의 `TextManualTest::TestCase`를 상속하고 고정 `GetId()`,
화면 진입/종료, 키 처리 및 자동 실행/완료 조회를 구현한다.
`REGISTER_MANUAL_TEST`로 등록한다. 새 파일 추가 후 CMake configure를 다시 실행한다.
특별한 순서 요구가 없다면 `text-test-case.h`의 `CreateOrderedCases()` ID 목록 맨 뒤에 추가한다.
미등록 ID도 기존 세 항목보다 뒤에 표시하며 translation unit 초기화 순서에 의존하지 않는다.

목록/Back은 launcher가 소유하고 테스트는 전달된 `contentArea`에만 UI를 추가한다.
`OnExit()`에서 timer를 중지하고 signal을 해제한다. 재진입 시 새 TC 객체를 생성하므로
기존 실행 결과나 view handle이 다음 진입에 남지 않는다. 프로세스 내부 DALi cache까지
초기화되는 것은 아니며 성능 측정 환경으로 사용하지 않는다.
