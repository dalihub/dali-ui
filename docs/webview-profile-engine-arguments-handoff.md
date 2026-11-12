# WebView 생성 API 대체 설계 및 구현 인계

작성일: 2026-11-11 (Asia/Seoul)

## 1. 목적과 문서 사용 방법

`WebView::New(uint32_t argc, char** argv)`의 deprecation에 따라 엔진 시작 설정과 뷰 생성을 분리한다. 기존 오버로드에 섞여 있던 incognito 선택은 프로필 생성으로 옮긴다.

이 문서는 후속 작업자가 이전 대화를 읽지 않고 구현을 진행하기 위한 임시 설계·인계 문서다. 아래의 확정 정책을 다시 미결 사항으로 제시하지 않는다. 내부 함수 이름과 파일 분할은 기존 코드에 맞춰 정하되, 공개 동작·오류 정책·소유권을 임의로 변경하지 않는다.

**구현과 필요한 검증을 완료하고, 영구적으로 필요한 설명을 API 주석·샘플·테스트 등에 반영한 뒤 이 문서 자체를 삭제한다.** 구현이 부분 완료이거나 검증이 남았다면 문서를 삭제하지 말고 진행 상태와 남은 작업을 갱신한다. 문서만 작성했다는 이유로 삭제하거나 작업 완료로 보고하지 않는다.

### 현재 상태 (2026-11-12 검증·리뷰 후 갱신)

- UI Draft PR #25와 adaptor/extension Gerrit 변경을 유지하며 P2 두 건의 후속 수정을 완료했다. 동일 라이브러리 alias 통합 및 ECORE/TCORE weak 프로필 추적 정리를 추가했다. 후속 검증과 게시 상태는 아래 기록을 참조한다.
- adaptor: [Gerrit 361247](https://review.tizen.org/gerrit/c/platform/core/uifw/dali-adaptor/+/361247), `8e757fa6263ff45267c9a2a0cbd840300b26cf46` (patchset 2), Change-Id `I83db25b736104d3bbb19c142815471ef71cfef80`.
- extension: [Gerrit 361248](https://review.tizen.org/gerrit/c/platform/core/uifw/dali-extension/+/361248), `f12ae1cc5e5eb8a06b96edfc827a9ede4f53cf5f` (patchset 2), Change-Id `Ia1331a6d99a109cc27655b9857f028246c384a3e`.
- UI: [Draft PR #25](https://github.com/dalihub/dali-ui/pull/25), `rabbitfor/dali-ui:review/webview-engine-arguments` → `dalihub/dali-ui:devel`. UI 변경은 같은 브랜치의 단일 스쿼시 커밋으로 유지한다.
- 사용자의 지시에 따라 이번 작업용 워크트리 3개를 삭제하고 원래 저장소에서 `review/webview-engine-arguments` 브랜치를 체크아웃했다. 구현 파일은 이동 전후 SHA-256을 비교해 보존을 확인했다.
- core/adaptor/extension의 `origin/devel/master`, UI의 `origin/devel`을 fetch하고 최신 기준으로 변경분을 적용했다. 아래 작업 위치 표가 현재 기준이다.
- 최신 core, 변경 adaptor, UI foundation/components의 DALi 3 Debug/coverage 빌드·설치를 완료했다. WebView 관련 UI 테스트 **128/128**, 실제 adaptor 로더를 사용하는 가짜 플러그인 테스트 **12/12**, extension 인자 저장 코드 테스트 **1/1**, ECORE/TCORE 관리자 테스트 **2/2**가 통과했다.
- `New(uint32_t, char**)`를 포함한 기존 생성 심볼과 새 공개 심볼의 export를 `nm -D`로 확인했다. 이관한 샘플과 LWE 공통 플러그인 진입점의 syntax-only 컴파일도 통과했다.
- Tizen `armv7l` GBS에서 ECORE core/adaptor/extension/UI의 DALi3·DALi2 빌드, TCORE adaptor/extension의 DALi3 빌드와 RPM 생성을 완료했다. Ubuntu 공통/백엔드 소스 11개 컴파일 및 기존 EGL renderer의 실제 X11 실행도 통과했다.
- **미완료:** 실기기 동작, Windows 네이티브 빌드, Ubuntu 데스크톱 LWE 최종 링크·페이지 렌더링. 데스크톱 SDK와 Windows 환경이 없고 사내 SDK 서버 연결도 시간 초과였다. 사용자는 이 제약을 확인한 뒤 가능한 검증 결과로 독립 리뷰를 진행하도록 했다. mock 통과를 실제 context 생성·unref·세션 유지·종료 순서의 기기 검증으로 간주하지 않는다.
- 네이티브 검증이 남았으므로 이 임시 문서를 유지한다. 최종 완료 후 API 주석과 `samples/web-view/README.md`의 영구 설명을 확인하고 삭제한다.
- 이슈 #24의 요청 인터셉트, 미디어, WebSDI 등은 구현 범위에 추가하지 않았고, 이슈 완료 처리도 하지 않았다.

### UI 엔진 타입 확정 정책 후속

- 사용자 확정 정책에 따라 `UiConfig` 엔진 설정 fallback을 제거했다. `WebView::SetWebEngineType()`으로 설정한 값 또는 기본값 Chromium만 사용한다.
- 인자 저장·뷰 생성·프로필 생성의 첫 사용에서 타입을 고정한다. 같은 타입 재설정은 허용하며 다른 타입 변경은 실제 초기화 전이라도 assertion으로 거부한다.
- `EnsureEngineType()`은 실제 엔진을 시작하지 않는다. `SetWebEngineArguments()` 이후에도 초기화 전 인자 교체는 가능하다. 인자 오류·백엔드 미제공 실패도 타입 고정을 해제하지 않는다.
- 이 후속은 dali-ui만 변경하며 adaptor/extension 패치셋은 유지한다. 로컬 DALi3 UI build/install 후 Web 관련 테스트 **125/125** 통과(타입 정책 회귀 6개 추가). GBS 및 네이티브 데스크톱 검증은 이번 UI 정책 변경으로 재실행하지 않았으며 아래 결과는 이전 검증 이력이다. 로그: `webview-validation/type-policy/`.
- 독립 리뷰의 “UiConfig=LWE와 하위 API 직접 사용” 항목은 앱의 지원 API 경계와 deprecated 설정 미보장 정책에 따라 확정 결함에서 제외했다. 별도 UI 이름 registry의 만료 이름 누적 P2는 아래 후속에서 수정했다.

### UI 이름 registry 후속 — 수정 완료

- named·기본 프로필의 성공한 등록 경로에서 공통 `RegisterWebProfile()`이 만료 weak 항목을 제거한 뒤 새 항목을 등록한다. 조회는 기존 단일 이름 조회·해당 만료 항목 제거를 유지한다.
- registry는 계속 weak 참조만 보유한다. 마지막 소유자가 사라진 항목은 다음 등록 때 정리하며, 살아 있는 프로필의 중복 이름 거부와 view/cookie 소유권을 유지한다. 공개 API/ABI 변경은 없다.
- 로컬 DALi3 UI foundation/components build/install 후 Web 관련 테스트 **128/128** 통과. 회귀 3개에서 서로 다른 이름 1,000회 생성·해제, registry 크기, 살아 있는 항목, 이름 재사용, 기본 등록 정리 및 view/cookie 소유권을 확인했다.
- 설치된 실제 UI 라이브러리의 registry도 GDB로 검사했다. view/cookie가 각각 보유하는 2개 프로필과 함께 named 등록 1,004회·기본 등록 1회를 거치며 항목 수가 최대 3개를 유지했다. 기본 등록 후에도 정확히 3개였으며 만료 이름 누적은 없었다. 로그·검사 스크립트: `webview-validation/ui-profile-registry/`.
- adaptor/extension 패치셋은 그대로 유지한다. 이번 UI registry 후속에서는 GBS, 실기기, Windows, Ubuntu LWE 최종 링크·페이지 검증을 재실행하지 않았다.

### 독립 리뷰 후속 — P2 두 건 수정 완료

1. **adaptor의 동일 백엔드 alias 처리:** `GetSharedInstance()`가 `dlopen()`의 실제 라이브러리 핸들이 같은 선택값을 하나의 라이브러리 소유 객체로 통합한다. `SelectType()`은 초기화 전후 동일 identity를 허용하며, 다른 백엔드는 초기화 후 계속 거부한다. `-1`을 Chromium으로 고정하지 않는다. Chromium/LWE 각각의 환경변수·기본 symlink, 기본/명시 생성 양방향, 시작 인자 보존, profile/view identity 및 다른 엔진 거부를 8개 새 테스트로 검증했다.
2. **Chromium 기본 프로필 추적 메모리 누적:** ECORE/TCORE의 기본·named 생성이 공통 `TrackProfile()`을 사용해 등록 전에 만료 weak 항목을 제거한다. 강한 registry로 바꾸지 않았으며 살아 있는 프로필과 기존 종료 순서를 유지한다. 실제 관리자 소스를 빌드하는 테스트에서 기본 프로필 1,000회 생성·해제, 살아 있는 named/default 프로필과 함께 반복 생성, 마지막 소유자 해제, view → context → engine 종료 순서를 검증했다. 네이티브 SDK·wrapper·lifecycle 의존성은 테스트 대역이며 실기기 검증을 대신하지 않는다.

수정 전 소스로도 새 테스트를 실행해 alias 8개와 관리자 2개가 각각 실패함을 확인했다. 수정본에서는 모두 통과한다. 공개 API/ABI 변경은 없다.

요청된 TCORE `ClearAllTilesResources()`의 `Deprecated` 주석/no-op과 ECORE 구현은 변경하지 않았다. 기존 PR과 Gerrit Change-Id를 유지한다.

## 2. 작업 위치와 소스 기준

| 저장소 | 현재 위치 / 구현 시작 기준 커밋 |
|---|---|
| dali-ui | `/home/jy/work/dali/dali-ui`, `review/webview-engine-arguments`, `7f2e2fd9` (`origin/devel`, PR 충돌 해결 rebase 기준) |
| dali-core | `/home/jy/work/dali/dali-core`, `review/webview-engine-arguments`, `eb64b58b9` (`origin/devel/master`), 코드 수정 없음 |
| dali-adaptor | `/home/jy/work/dali/dali-adaptor`, `review/webview-engine-arguments`, `69d684693` (`origin/devel/master`) |
| dali-extension | `/home/jy/work/dali/dali-extension`, `review/webview-engine-arguments`, `99d77af` (`origin/devel/master`) |
| 검증 설치 prefix | `/home/jy/work/dali/webview-validation/opt3`, DALi API major 3 |
| Chromium/WV/EWK 조사 소스 | `/home/jy/work/chromium-efl`의 `origin/tizen`, `f5439772622d` (추가 fetch하지 않음) |


Chromium 저장소의 작업 디렉터리는 비어 있고 HEAD는 초기 빈 커밋이다. 엔진 소스는 `git show origin/tizen:<path>`와 `git grep ... origin/tizen -- <path>`로 읽었다. 작업 디렉터리에 파일이 없다는 이유로 소스를 찾을 수 없다고 판단하지 않는다. 위 커밋을 현재 모든 기기에 설치된 엔진 버전으로 간주하지 않는다.

`dali-adaptor`의 기존 `locale-context.h` 사용자 변경은 최신 기준으로 전환하기 전에 별도 stash `95276b7ab78b387bf91ab5c350c647d7ab479274`에 보존했다. 원래 `feature/accessibility-locale` 브랜치는 그대로 유지했다. 작업 이동 백업과 stash 목록은 `/home/jy/work/dali/webview-validation/worktree-transfer/`에 있다. 이 사용자 변경을 WebView 패치에 포함하거나 삭제하지 않는다. UI의 기존 접근성 인계 문서 두 개도 그대로 남겨 두었다.

작업 시작 시 상위 `AGENTS.md`와 저장소별 지침, 상태를 다시 확인한다. 수동 clang-format 금지, 빌드 최대 `-j6`, 테스트 전에 변경 라이브러리 설치 규칙을 준수한다. 이 문서 작성은 AI-SDLC 워크플로 시작 지시가 아니다.

## 3. 확정된 정책

| 주제 | 확정 내용 |
|---|---|
| 엔진 시작 인자 | 프로세스 공용. WebView 인스턴스별 설정이 아님 |
| 새 설정 API | `WebView::SetWebEngineArguments()` |
| 인자 저장 장소 | `dali-extension`; 배열과 문자열을 복사해 보관 |
| 인자 내용 검사 | 입력 형식과 호출 시점 검사. `--incognito`를 특별히 해석하거나 거부하지 않음 |
| 설정 시점 | 최초 엔진 초기화 시작 전. 프로필 생성이 첫 초기화를 유발할 수도 있음 |
| 초기화 전 재호출 | 설정 전체 교체 |
| 초기화 시작 이후 | 명시적 실패 반환. 네이티브 인자 설정 함수를 호출하지 않음 |
| 복사 동작의 문서화 | 일반적인 setter 동작으로 구현. 원본 수정·해제 또는 변경 미반영 설명을 공개 API 문서에 별도로 늘어놓지 않음 |
| incognito 지정 | `WebProfile::New()`의 boolean으로 컨텍스트 생성 시 지정 |
| 프로필로 뷰 생성 | `WebView::New(profile)` |
| bundle 미지정 | 네이티브 생성 함수에 `nullptr`이 아닌 빈 문자열 `""` 전달 |
| 빈 프로필 전달 | `DaliException`을 발생시키는 DALi assertion 사용. 기본 프로필로 대체하지 않음 |
| 중복 이름 | 옵션이 같아도 예외. 기존 프로필 재사용은 `GetProfile(name)` |
| 네이티브 생성 실패 | 오류 로그 + 빈 핸들. 기본 프로필로 대체하지 않음 |
| 이전 인자 오버로드 | `New(argc, argv)`의 선언과 심볼 유지. 전달받은 인자는 전부 무시하고 `New()` 호출 |
| 이전 incognito 해석 | `New(argc, argv)`를 위해 DALi가 수행하던 `--incognito` 문자열 검사 제거 |
| WV/EWK 내부 구현 유무 | 해당 API로 연결하는 DALi 설계를 바꾸거나 연결 작업의 선행 조건으로 삼지 않음. 실제 반환 실패는 그대로 보고 |
| 컨텍스트 수명 | 생성 시 받은 참조 하나를 내부 소유 객체가 인수. 마지막 소유자 정리 시 한 번 unref |

공개 동작에 대한 사용자 결정은 완료된 상태다. 하위 인터페이스의 구체적인 이름, 오류 코드 확장, 빌드 환경 정합성 등 구현 세부 사항을 기존 합의를 다시 묻는 이유로 삼지 않는다. 실제로 합의한 동작을 구현할 수 없는 중대한 근거가 나오면 원인과 영향을 보고한다.

## 4. 공개 API 목표

### 4.1 WebProfile

아래 메서드를 `Dali::Ui::WebProfile`에 추가한다.

```cpp
static WebProfile New(
  const Dali::String& name,
  bool incognito = false);

static WebProfile New(
  const Dali::String& name,
  bool incognito,
  const Dali::String& injectedBundlePath);

static WebProfile GetProfile(const Dali::String& name);

Dali::String GetName() const;
bool IsIncognito() const;
```

- 첫 번째 `New()`는 두 번째 `New()`에 빈 bundle 경로를 전달하는 의미다.
- `name`은 DALi의 조회 키다. 네이티브 컨텍스트 이름이나 저장소 경로로 전달하지 않는다.
- 이름은 대소문자를 구분한다. 빈 이름과 예약 이름 `Default`로 사용자 프로필을 생성하는 것은 오사용 오류로 처리한다.
- 살아 있는 동일 이름의 프로필이 있으면 옵션 비교 없이 예외를 발생시킨다. `GetOrCreate` 의미를 넣지 않는다.
- 이름은 네이티브 컨텍스트 생성에 성공한 후 등록한다. 실패한 이름이 남지 않게 한다.
- `GetProfile()`은 기존 살아 있는 프로필만 조회한다. 조회 실패는 빈 핸들이며 엔진이나 컨텍스트를 생성하지 않는다.
- incognito와 bundle 경로는 생성 시 고정한다. 생성 후 변경 setter를 추가하지 않는다.
- 이름 레지스트리는 약한 참조를 사용한다. 마지막 실제 소유자가 사라진 이름은 다시 사용할 수 있다.
- 비공개 프로필을 유지한 상태에서 모든 뷰가 닫혀도 세션을 유지한다. 비공개 프로필을 완전히 해제한 뒤 다시 생성하면 새 세션이다.
- 일반 프로필 파괴가 영구 저장 데이터 삭제를 의미하지 않는다. 서로 다른 정상 컨텍스트의 영구 저장소까지 완전히 격리된다고 엔진 검증 없이 약속하지 않는다.
- 생성은 DALi 애플리케이션 준비와 `UiConfig::Apply()` 이후 이벤트 스레드에서 수행한다. 뷰보다 먼저 생성할 수 있다는 것이 DALi 실행 환경 없이 생성할 수 있다는 뜻은 아니다.

### 4.2 WebView

```cpp
enum class WebEngineArgumentsResult : uint8_t
{
  SUCCESS,
  INVALID_ARGUMENT,
  ALREADY_INITIALIZED,
  BACKEND_UNAVAILABLE
};

static WebEngineArgumentsResult SetWebEngineArguments(
  uint32_t argc,
  char** argv);

static WebView New(const WebProfile& profile);
```

반환 타입 이름과 열거자 배치는 구현 기준안이다. 앞선 논의의 핵심 결과는 성공/잘못된 입력/초기화 이후 거부였다. 저장을 위해 플러그인 접근이 필요한 구조이므로 로드·필수 기능 확보 실패도 성공으로 숨기지 않도록 `BACKEND_UNAVAILABLE`를 별도 결과로 구분한다. 세부 네이티브 오류는 로그로 남긴다.

`SetWebEngineArguments()` 계약:

- 네이티브 엔진을 시작하지 않고 설정을 저장한다. 플러그인 라이브러리 로딩과 엔진 초기화는 구분한다.
- `SUCCESS`는 설정 보관 성공이며 실제 엔진 초기화 성공이나 모든 엔진 스위치의 지원을 보장하지 않는다.
- `argc > 0`이면 `argv`와 각 유효 인자 포인터가 null이 아니어야 한다. 네이티브 `int argc` 범위도 검사한다.
- 일반 argv 관례에 따라 `argv[0]`은 프로그램 이름이고 나머지가 옵션이다. 호출자가 제공하는 배열은 `argc`로 읽고, 내부 전달 배열에 종단 null을 추가한다.
- `argc == 0`은 저장된 사용자 인자를 비운다. `argv == nullptr`을 허용한다.
- 잘못된 입력이나 실패한 교체로 이전의 정상 설정을 손상시키지 않는다.
- 초기화가 시작된 후에는 동일 값이어도 `ALREADY_INITIALIZED`다. 엔진 초기화 실패 후에도 임의로 다시 설정 가능한 상태로 되돌리지 않는다.
- 문자열 복사와 원본 메모리 변경 여부를 공개 API 문서의 별도 주제로 설명하지 않는다. 문서는 목적·호출 시점·전체 교체·오류 결과에 집중한다.

`New(profile)` 계약:

- 빈 핸들은 `DALI_ASSERT_ALWAYS`와 명확한 메시지로 거부한다. 이 검사는 release 빌드에도 적용되고 `DaliException`을 발생시킨다.
- 지정한 프로필의 컨텍스트로 네이티브 뷰를 생성한다. 실패하면 로그와 빈 `WebView`를 반환한다.
- 뷰는 선택한 프로필을 보유한다. 이후 `SetProfile()`로 변경하는 API는 추가하지 않는다.
- `New(profile, url)` 같은 추가 오버로드는 이번 필수 범위에 넣지 않는다. 필요한 경우 생성 후 기존 URL API를 사용한다.

### 4.3 기존 API 변경

| API | 목표 동작 |
|---|---|
| `WebView::New()` | 기본 프로필로 생성 |
| `WebView::New(url)` | 기본 프로필로 생성 성공 후 URL 로드 |
| `WebView::New(argc, argv)` | 인자를 읽거나 검증하지 않고 `New()`로 위임 |
| `WebView::GetProfile()` | 실제로 뷰가 보유한 프로필 반환 |
| `WebView::SetWebEngineType(type)` | 인자 저장·뷰 생성·프로필 생성의 첫 호출에서 타입 고정; 미설정 시 Chromium, 이후 다른 타입은 assertion |

```cpp
WebView WebView::New(uint32_t /*argc*/, char** /*argv*/)
{
  return New();
}
```

인자 오버로드는 앱의 빌드 오류를 방지하기 위해 남긴다. 기존 동작 보존은 요구하지 않는다. 전달된 인자로 엔진 설정을 덮어쓰거나 incognito를 선택하지 않는다. `SetWebEngineArguments()`에서 별도로 설정한 값은 일반 `New()`와 동일하게 사용한다. 따라서 두 인자 API 사이의 우선순위 정책은 필요 없다.

deprecated 주석은 현재의 "인자가 전달된다/첫 인자가 적용된다" 설명을 지우고, 인자가 무시된다는 사실과 `SetWebEngineArguments()` 및 `WebProfile::New()`로의 이관을 설명하도록 고친다.

`SetWebEngineType()`은 `SetWebEngineArguments()`·`WebView::New()`·`WebProfile::New()`의 첫 사용 전에 호출한다. 첫 사용 시 타입이 확정되며 명시 설정이 없으면 Chromium을 사용한다. 인자 저장이나 백엔드 생성 실패도 타입 확정을 되돌리지 않는다. 이후 다른 타입은 assertion으로 거부하며 같은 타입 재지정은 허용한다. deprecated된 `UiConfig::SetWebEngineType()`의 저장값은 더 이상 엔진 선택에 사용하지 않는다. `UiConfig::Apply()` 선행 조건은 유지한다.

### 4.4 기본 프로필

- 기본 컨텍스트를 처음 사용할 때 `Default` 프로필을 준비한다.
- `GetProfile("Default")`도 조회만 한다. 아직 기본 프로필을 준비하지 않았다면 빈 핸들이다.
- 기본 컨텍스트는 엔진 소유의 빌려온 참조로 취급한다. 사용자 생성 컨텍스트의 소유·unref 규칙을 그대로 적용하지 않는다.
- 명시적인 프로필 생성/뷰 생성 실패를 기본 프로필로 대체하지 않는다.

### 4.5 목표 사용 흐름

아래는 DALi 애플리케이션 준비와 `UiConfig::Apply()`를 마친 이벤트 스레드 함수의 예시다. 반환형은 void인 호출 문맥을 가정한다. 앱은 실제 오류 처리 방식에 맞춰 실패 분기를 작성한다.

```cpp
WebView::SetWebEngineType(WebEngineType::CHROMIUM);
if(WebView::SetWebEngineArguments(argc, argv) != WebEngineArgumentsResult::SUCCESS)
{
  return;
}

auto privateProfile = WebProfile::New("PrivateSession", true);
if(!privateProfile)
{
  return;
}

auto first = WebView::New(privateProfile);
auto second = WebView::New(privateProfile);
if(!first || !second)
{
  return;
}

// 두 뷰는 같은 프로필의 비공개 세션을 사용한다.
// 기존 프로필을 다시 사용할 때는 New()를 반복하지 않는다.
auto sameProfile = WebProfile::GetProfile("PrivateSession");
```

## 5. 라이브러리별 설계

### 5.1 dali-ui-foundation

주요 변경 파일:

- `dali-ui-foundation/public-api/views/web/web-view.h`, `.cpp`
- `dali-ui-foundation/public-api/views/web/web-profile.h`, `.cpp`
- 결과 enum을 둘 web 타입 헤더 및 필요 시 umbrella/install 목록
- `dali-ui-foundation/integration-api/web-view-impl.h`, `.cpp`
- `dali-ui-foundation/integration-api/web-profile-impl.h`, `.cpp`
- `dali-ui-foundation/integration-api/web-cookie-manager-impl.h`, `.cpp`

담당 책임:

1. 공개 API, DALi 핸들 유효성 검사, 프로필 이름·중복 검사.
2. 이름 레지스트리와 프로필 메타데이터 관리.
3. 명시적인 엔진 타입 또는 기본값 Chromium을 선택하고, 인자 저장·프로필/뷰 생성의 첫 사용에서 UI 타입 변경을 금지.
4. 뷰가 생성 시 선택한 프로필을 보유하고 `GetProfile()`에서 반환.
5. context/cookie manager 작업을 해당 프로필의 하위 객체로 전달.

현재 `WebViewImpl::GetProfile()`은 `isIncognito`로 전역 context와 cookie manager를 다시 찾는다. 이름 있는 여러 프로필을 구별할 수 없으므로 이 경로를 실제 프로필 보유 방식으로 바꾼다.

UI의 타입 확정 상태와 adaptor의 실제 엔진 초기화 상태를 분리한다. `EnsureEngineType()`은 타입을 고정하고 선택만 수행하며 실제 엔진 초기화를 호출하지 않는다. 타입이 고정된 뒤에도 실제 초기화 시작 전까지는 인자 저장·교체가 가능하다.

공개 `WebView`, `WebProfile`, `WebCookieManager` 핸들의 크기·기존 vtable은 변경하지 않는다. 상태는 impl에 둔다. 새 공개 메서드는 기존 메서드 뒤에 추가하고 export와 SINCE 규칙을 적용한다. 불필요한 새 public 객체 모델은 추가하지 않는다.

### 5.2 dali-adaptor

주요 변경 영역:

- `dali/devel-api/adaptor-framework/web-engine/web-engine.h`, `.cpp`
- `dali/internal/web-engine/common/web-engine-impl.h`, `.cpp`
- 컨텍스트 소유 핸들 및 내부 런타임의 새 파일
- 플러그인 함수 조회·호출 경계

담당 책임:

1. 프로세스 공용 엔진 상태, 종류 선택, 최초 초기화 진입을 관리.
2. 인자 설정을 선택된 extension 플러그인에 전달. 인자 문자열의 상시 소유자는 extension.
3. 뷰 생성 없이 엔진을 초기화하고 프로필을 만들 수 있는 경로 제공.
4. 네이티브 컨텍스트 소유권을 표현하는 핸들(예: `Dali::WebEngineProfile`) 제공.
5. 소유 핸들이 살아 있는 동안 플러그인 코드와 필요한 엔진 의존성을 유지.
6. 프로필을 지정하는 뷰 생성 경로와 성공/실패 결과 제공.

하위 프로필 핸들의 책임은 context 소유와 context/cookie manager 접근이다. 이름 레지스트리는 UI에 있으므로 adaptor에 이름 관리 기능을 중복 구현하지 않는다. 기존 `WebEngineContext`는 context 작업을 제공하는 인터페이스이므로, raw pointer 하나를 소유 핸들로 착각하지 않는다.

하위에 필요한 연산:

| 연산 | 의미 |
|---|---|
| 인자 설정 | extension에 설정을 저장·교체, 엔진 시작 없음 |
| 초기화 시작 여부 조회/타입 변경 검사 | adaptor는 실제 초기화 상태를 관리; UI 타입 변경은 별도의 첫 사용 확정 상태로 제한 |
| 엔진 초기화 | 저장 인자를 적용한 뒤 엔진 시작, 결과 확인 |
| 기본 프로필 취득 | 빌려온 기본 context의 소유 정책 구분 |
| 프로필 생성 | incognito와 bundle 경로로 새로운 context 획득 |
| 프로필 context/cookie manager 접근 | 해당 프로필의 기존 연산 인터페이스 제공 |
| 프로필로 뷰 생성 | context를 전달하고 네이티브 뷰 성공 여부 반환 |
| 프로필 해제 | context 소유 참조를 한 번 반납 |

adaptor의 내부 로더는 플러그인 로딩과 실제 초기화 상태를 분리하며, 초기화 전 내부 타입 변경 시 저장된 인자를 인계하는 기존 동작을 유지한다. 앱은 이 하위 API를 직접 사용하는 대상이 아니다. UI는 그보다 먼저, 인자 저장·프로필/뷰 생성의 첫 사용 시 타입을 고정한다. 따라서 앱에서 LWE를 사용하려면 `WebView::SetWebEngineType(LWE)`를 해당 API들보다 먼저 호출해야 한다.

### 5.3 dali-extension 공통 플러그인 계약

현재 `CreateWebEnginePlugin`, `GetWebEngineContext` 등을 조회하는 방식과 맞춰 별도 함수 심볼 또는 버전이 있는 추가 함수 테이블로 기능을 제공한다. 기존 `WebEnginePlugin`의 가상 함수 순서를 변경하지 않는다. 새 기능이 없는 플러그인은 명시적으로 실패 처리하고 지원된 것처럼 동작하지 않는다.

다음은 필요한 연산의 내부 명명 예시이며 공개 DALi API 이름의 합의 사항은 아니다.

```text
StoreWebEngineArguments / ReadStoredWebEngineArguments
InitializeWebEngine
CreateWebEngineProfile
GetDefaultWebEngineProfile
DestroyWebEngineProfile
GetWebEngineProfileContext
GetWebEngineProfileCookieManager
CreateWebEngineViewWithProfile
```

플러그인 프로필 토큰은 context, cookie manager 래퍼, 소유 여부 등을 관리하는 불투명 객체다. WV/EWK 핸들을 dali-ui에 노출하지 않는다. 새 토큰의 생성/파괴는 같은 플러그인에서 수행한다.

인자 보관 구현:

- setter 호출 중 포인터 목록과 문자열을 모두 복사한다. 성공 반환 이후 호출자의 버퍼에 의존하지 않는다.
- 입력 검증과 새 저장소 준비를 마친 뒤 기존 설정을 교체한다.
- 네이티브에 전달하는 배열 끝에는 null 포인터를 추가한다.
- 문자열 주소가 안정된 후 포인터 배열을 구성한다. 전달 이후 vector 재할당이나 string 변경으로 주소를 무효화하지 않는다.
- 네이티브가 수정 가능한 argv를 요구하는 경로는 쓰기 가능한 소유 버퍼를 제공한다. 읽기 전용 문자열에 대한 무분별한 const_cast를 피한다.
- 사용자 설정과 TCORE 필수 스위치를 조합하는 작업은 초기화 전 extension에서 수행한다. 반복 교체나 초기화 확인 시 필수 스위치를 계속 누적하지 않는다.
- 원본 설정을 다른 엔진 플러그인으로 인계할 때 TCORE 전용으로 합성한 인자까지 사용자 인자로 옮기지 않는다.

### 5.4 Chromium ECORE/TCORE 백엔드

주요 변경 파일군:

```text
dali-extension/web-engine-chromium/ecore/tizen-web-engine-manager.*
dali-extension/web-engine-chromium/ecore/tizen-web-engine-chromium.*
dali-extension/web-engine-chromium/ecore/tizen-web-engine-context.*
dali-extension/web-engine-chromium/ecore/tizen-web-engine-cookie-manager.*
dali-extension/web-engine-chromium/tcore/의 대응 파일
```

| 기능 | TCORE / WV | ECORE / EWK |
|---|---|---|
| 시작 인자 | `wv_set_arguments()` | `ewk_set_arguments()` |
| 엔진 시작 | `wv_init()` | 필요한 EFL 준비 후 `ewk_init()` |
| 일반 context 생성 | `wv_context_new_with_injected_bundle_path(path)` | `ewk_context_new_with_injected_bundle_path(path)` |
| 비공개 context 생성 | `wv_context_new_with_injected_bundle_path_in_incognito_mode(path)` | `ewk_context_new_with_injected_bundle_path_in_incognito_mode(path)` |
| context로 view 생성 | `wv_view_add_with_context(context)` | `ewk_view_add_with_context(evas, context)` |
| 소유 참조 해제 | `wv_context_unref(context)` | `ewk_context_unref(context)` |

현재 manager 생성자는 엔진을 바로 시작한다. 설정 저장과 플러그인 조회만으로 이 생성자 경로가 실행되지 않도록 초기화를 명시적 연산으로 분리한다. engine init의 반환값도 확인한다.

현재 context/cookie manager는 일반·비공개 두 슬롯에 저장되며, 새 뷰를 만들 때 `SetContext()`가 래퍼를 교체한다. named profile 여러 개를 지원하려면 **context별 소유 객체**로 변경해야 한다. 다른 뷰를 생성했다고 기존 profile/cookie wrapper가 무효화되지 않아야 한다.

`InitWebView(bool)` 중심의 경로를 선택된 프로필 context로 생성하는 경로와 연결한다. context별 콜백과 네이티브 뷰-to-plugin 매핑도 실제 프로필 기준으로 정리한다. 뷰 생성 시 기존의 refresh rate 60 설정 등 context 설정을 매번 덮어쓰는 동작이 있다면 프로필 초기 기본값 설정으로 옮겨 공유 프로필 설정을 훼손하지 않도록 한다.

`--incognito`를 strstr 등으로 검사해 뷰 생성 모드를 고르던 DALi 코드는 대체 경로에서 제거한다. 엔진 인자 전달 자체에 incognito 금지 목록을 넣지 않는다.

### 5.5 LWE 백엔드

- 같은 플러그인 계약에 맞춰 엔진 시작과 뷰 생성을 분리한다. 기존 기본 프로필을 사용하는 `WebView::New()`는 계속 동작해야 한다.
- 현재 LWE 플러그인은 context/cookie manager를 공용 singleton으로 제공하고 `IsIncognito()`는 false다. 이름만 다른 동일 context를 반환해 독립 프로필을 지원하는 것처럼 보이게 하지 않는다.
- 지원하지 않는 named context, incognito, injected bundle 요청은 오류 로그와 실패로 처리한다. 기본 프로필로 대체하지 않는다.
- Tizen의 기존 버전 선택 옵션은 엔진 초기화 전 처리로 이동한다. Windows의 무시되던 인자를 검증된 지원 없이 Chromium과 같은 옵션으로 해석하지 않는다.
- LWE의 독립 context/incognito 엔진 기능 자체를 새로 만드는 것은 이번 범위가 아니다.

## 6. 상태와 소유권

### 6.1 엔진 상태

```text
설정 가능
  -> 초기화 시작: 엔진 종류와 저장 인자 고정
     -> 사용 가능
     -> 초기화 실패
  -> 앱 종료 시 사용 종료
```

- setter와 최초 초기화 진입은 동일한 상태 관리 기준으로 직렬화한다. 주된 API 호출은 DALi 이벤트 스레드에서 수행한다.
- 초기화가 시작된 뒤 setter는 항상 거부한다. 모든 뷰/프로필을 삭제해도 엔진 옵션 변경이나 재시작을 암묵적으로 허용하지 않는다.
- 프로필 생성만으로 초기화될 수 있으므로 "첫 WebView 생성 전"을 문서의 유일한 조건으로 사용하지 않는다.
- `GetProfile(name)` 조회, 빈 핸들 생성, 설정 저장만으로 엔진이 시작되지 않는다.
- 초기화 실패와 context 생성 실패를 구분한다. 엔진은 사용 가능한데 개별 context 생성이 실패한 경우를 엔진 전체 종료로 취급하지 않는다.

### 6.2 참조 관계

```text
앱의 WebProfile 핸들 ----+
WebViewImpl ------------+--> WebProfileImpl --> adaptor 프로필 소유 핸들
WebCookieManagerImpl ---+                          |
                                                  v
                                      extension context 소유 객체
                                                  |
                                                  v
                                        native context 참조 1개

이름 레지스트리 -- weak --> WebProfileImpl
WebProfileImpl -- weak cache --> WebCookieManagerImpl
```

- 공개 핸들 복사는 DALi impl 참조만 증가시킨다. 매 복사마다 native ref를 호출하지 않는다.
- 새 context 생성 시 받은 참조를 인수하고 소유 객체 파괴 시 unref한다. 같은 native pointer에 독립적인 소유 객체를 중복 생성하지 않는다.
- `WebCookieManager`만 남겨도 context를 필요로 하므로 profile을 보유한다. profile이 cookie handle을 강하게 캐시하면 순환 참조가 생긴다. 캐시는 weak로 바꾼다.
- 이름 조회에서도 같은 살아 있는 profile impl을 돌려준다. cookie manager가 profile을 유지하는 동안 이름이 해제됐다고 판단하면 안 된다.
- 기본 context는 borrowed 정책으로 별도 표시한다.
- context callback 해제는 profile 수명에 맞춘다. 마지막 비공개 뷰 파괴만으로 callback을 지우면 profile 단독 보유 요구를 깨뜨린다.
- 정상 실행 중 retained profile/dependency가 필요로 하는 엔진을 조기에 종료하지 않는다.
- 앱 종료에서는 뷰와 callback을 정리하고 소유 context를 반납한 뒤 shutdown한다. 종료 후에도 DALi 핸들이 남을 수 있으므로 소유 객체를 무효 상태로 전환하고 뒤늦은 소멸 시 중복 unref를 방지한다.

## 7. 소스에서 확인한 사실과 검증 한계

### 7.1 기존 인자 전달

- dali-ui와 adaptor는 현재 `New(argc, argv)` 인자를 저장하지 않고 plugin `Create()`로 전달한다.
- ECORE plugin은 `ewk_set_arguments()`로 원본을 전달한다.
- TCORE plugin은 manager의 static `vector<string>`와 포인터 배열에 복사한 뒤 `wv_set_arguments()`로 전달한다.
- Chromium `CommandLineEfl`은 받은 argv 포인터와 별도의 문자열 집합을 보관한다.
- WV의 setter는 이미 초기화된 경우에도 성공을 반환할 수 있고, `CommandLineEfl::Init()` 재호출은 일부 저장 상태를 변경한다. 네이티브 함수가 안전하게 무시할 것이라고 가정하지 말고 DALi에서 늦은 호출을 차단한다.

### 7.2 복사와 EWK 문서

`ewk_main_internal.h`에는 원본 인자와 문자열 복사에 대한 제약 설명이 있다. 조사한 Chromium의 원본 메모리 의존 처리는 프로세스 제목 변경 경로이며, `EwkGlobalData`의 임베딩 초기화는 해당 처리가 있는 `ContentMain()` 대신 `ContentMainRunner::Initialize()`를 직접 호출한다. 현 TCORE plugin도 이미 복사본을 전달한다.

따라서 최종 설계는 복사 보관이다. 사용자에게 argv 수명 유지 책임을 다시 요구하지 않는다. 다만 이 근거는 조사한 revision의 소스 분석이며 모든 기기 버전의 실증 결과가 아니다. 실제 ECORE/TCORE 초기화와 인자 적용을 검증한다.

### 7.3 context 생성과 해제

- WV/EWK의 bundle 경로 생성 함수는 null 포인터를 거부하나 빈 문자열은 통과시킨다.
- EWK의 bundle 없는 내부 생성 경로도 빈 문자열로 `EWebContext`를 생성한다. 빈 경로는 bundle 로딩/전달을 생략한다.
- EWK의 `Create(incognito, path)`는 일반/비공개 모두 `AddRef()` 후 반환한다.
- 조사한 WV 공개 생성 함수의 EWK 경로는 위 함수에 연결된다. WV 내부 `Create(incognito, path)` 역시 `AddRef()` 한다.
- EWK에는 `ewk_context_ref()`와 `ewk_context_unref()`가 있다. 조사한 WV 실제 API에는 `wv_context_unref()`가 있고 `wv_context_ref()`는 없다. 문서의 변환표에만 있는 ref 항목을 구현된 API로 착각하지 않는다.
- native view는 context를 `scoped_refptr`로 보유한다.
- borrowed context를 별도의 소유 객체로 바꾸는 용도로 임의 ref를 호출하는 설계를 하지 않는다. DALi가 만든 context의 소유 impl을 공유한다.

### 7.4 백엔드 미구현

조사한 WV revision의 standalone 경로에서 `wv_context_new_with_injected_bundle_path*()`와 `wv_view_add_with_context()`는 미구현 로그와 null을 반환한다. EWK 연결 경로는 실제 함수 호출이 있다.

사용자는 **이 상태와 관계없이 DALi API를 올바른 WV API에 연결**하라고 결정했다. EWK 경로로 강제 우회하거나, 일반 뷰를 대신 만들거나, incognito를 무시해서 성공시키지 않는다. mock으로 연결·실패 정책을 검증할 수 있지만 실제 기기 성공을 대신 증명하지는 못한다.

### 7.5 주요 근거 경로

Chromium 저장소의 `origin/tizen:` 기준:

```text
tizen_src/wv/integration/public/wv_main.cc
tizen_src/wv/integration/public/wv_main_internal.h
tizen_src/wv/integration/public/wv_context.cc
tizen_src/wv/integration/public/wv_context_internal.h
tizen_src/wv/integration/public/wv_view.cc
tizen_src/wv/integration/public/wv_view_internal.h
tizen_src/wv/integration/private/wv_context_private.cc
tizen_src/wv/integration/private/wv_view_impl.h
tizen_src/ewk/efl_integration/public/ewk_main.cc
tizen_src/ewk/efl_integration/public/ewk_main_internal.h
tizen_src/ewk/efl_integration/public/ewk_context.cc
tizen_src/ewk/efl_integration/public/ewk_context_internal.h
tizen_src/ewk/efl_integration/public/ewk_view.cc
tizen_src/ewk/efl_integration/private/ewk_context_private.cc
tizen_src/ewk/efl_integration/command_line_efl.cc
tizen_src/ewk/efl_integration/ewk_global_data.cc
tizen_src/ewk/efl_integration/eweb_context.cc
tizen_src/ewk/efl_integration/content_browser_client_efl.cc
base/process/set_process_title_linux.cc
content/app/content_main.cc
```

## 8. 구현 순서와 검증

1. 각 저장소 상태와 build prefix/API major를 확인하고 adaptor/extension 작업 영역을 분리한다.
2. adaptor와 extension 사이의 초기화·설정 저장·프로필 소유·뷰 생성 계약을 구현한다. 성공/실패와 소유권을 테스트에서 관찰할 수 있게 한다.
3. Chromium ECORE/TCORE를 대응 API에 연결하고, LWE의 기존 기본 동작과 미지원 기능의 실패 처리를 구현한다.
4. UI의 프로필 레지스트리, 생성 함수, 소유 관계, 엔진 상태를 연결한다.
5. `New(argc, argv)`를 인자 무시 위임으로 변경하고 API 주석과 샘플을 이관한다.
6. 변경 라이브러리를 설치한 뒤 자동 테스트를 빌드·실행한다. 기기 검증 결과와 미실시 범위를 기록한다.
7. 완료 조건을 충족하고 영구 설명을 코드에 반영한 뒤 이 문서를 삭제한다.

### 자동 테스트 대상

| 영역 | 검증 항목 |
|---|---|
| 인자 저장 | 원본 버퍼 변경/해제 후 복사본 사용, 전체 교체, 빈 설정, 잘못된 입력 시 기존 값 유지, 종단 null |
| 엔진 상태 | setter/lookup만으로 init 안 됨, profile이 먼저 init, 시작 이후 설정 거부, 실패한 init 뒤에도 재설정 차단 |
| 종류 변경 | UI 첫 사용 전 변경 허용, 첫 사용 후 다른 타입 거부, 기본 Chromium 및 UiConfig 설정 무시; adaptor 내부 인자 인계와 TCORE 합성 옵션 비전파 유지 |
| 전달 | 설정이 초기화 직전 한 번 전달, TCORE 필수 인자 중복 없음, 새 setter의 incognito 특별 검사 없음 |
| 프로필 | 일반/비공개와 bundle 유무의 올바른 네이티브 함수/인자, 중복 예외, 조회 miss, 실패한 이름 미등록 |
| 뷰 | 빈 profile 예외, 지정 context 전달, 생성 실패 시 빈 view, 실제 profile 반환 |
| 소유권 | profile 복사, 여러 view 공유, 마지막 view 뒤 profile 유지, cookie만 보유, 마지막 해제 시 unref 한 번 |
| 레지스트리 | 불필요한 강한 참조 없음, 실제 소유자 존재 시 중복 거부, 완전 해제 후 이름 재사용 |
| 기본 프로필 | 사용자 프로필 실패 시 fallback 없음, borrowed default에 부당한 unref 없음 |
| 이전 오버로드 | 임의 argc/argv 및 `--incognito`를 읽지 않음, 일반 `New()`와 같은 동작, 새 setter 설정은 유지 |
| 종료 | callbacks/views/context 정리 순서, 살아 남은 핸들 접근 및 소멸에서 UAF/중복 unref 방지 |
| 미지원 | 플러그인 기능 미제공·WV null·LWE 미지원 요청을 성공이나 default로 숨기지 않음 |

테스트 유틸리티의 `ui-web-engine.cpp/.h`는 실제 adaptor 대신 사용하는 mock이다. 호출 횟수, init 순서, 인자 값, native context identity, ownership 해제 횟수를 관찰 가능하게 확장한다. 현재 빈 `WebEngine::Create()` mock만으로 전달 동작이 검증된다고 보고하지 않는다.

주요 UI 테스트 위치는 `automated-tests/src/dali-ui-foundation/utc-Dali-WebView.cpp`이며 프로필 수명 테스트는 적절한 별도 파일로 분리할 수 있다. 프로세스 공용 상태를 테스트하는 경우 기존 테스트 실행기의 프로세스 격리 방식을 확인한다. 실제 제품 API에 테스트용 reset을 추가하지 않는다.

실기기 검증에서는 일반/비공개 혼용, 같은 profile 공유 세션, profile 단독 보유 후 view 재생성, bundle 없는 생성, engine 인자의 실제 효과를 확인한다. 미구현 엔진에서는 기대한 실패 처리만 검증됐음을 명시한다.

### 빌드 환경 주의 (구현 전 조사 이력)

상위 `AGENTS.md`의 빌드·설치·테스트 명령을 기준으로 하며 병렬도는 최대 6이다. 조사 시 다음 불일치가 있었다.

- 환경/워크스페이스 `setenv`: `/home/jy/work/dali/dali-env/opt`
- 기존 build cache의 설치 prefix: `/home/jy/dali-env/opt`
- 확인한 기존 설치에는 `dali2-*.pc`가 있었고, 최신 dali-ui CMake의 기본 API major는 3이었다.

따라서 기존 경로를 그대로 사용하면 최신 소스와 다른 설치본을 링크할 수 있다. 실제 선택한 source/header/library/API major/prefix를 맞춘 뒤 검증한다. 이 문서는 특정 빌드 조합의 성공을 보장하지 않는다. 필요한 권한은 도구의 승인 절차를 이용하며 사용자 변경을 덮어쓰지 않는다.


### 실제 검증 결과와 재현 위치

- core: `webview-validation/core-build`, adaptor: `webview-validation/adaptor-build`, UI: `webview-validation/ui-build`; 각각 최대 `-j6`로 빌드하고 `opt3`에 설치.
- UI 테스트: `automated-tests/build.sh dali-ui-foundation` 후 `execute.sh -p UtcDaliWeb dali-ui-foundation`; 119개 통과. 테스트 실행기의 fork 격리를 사용하며 제품 reset API는 추가하지 않음. D-Bus 소켓 생성 때문에 실행은 도구의 권한 승인 경로를 사용.
- adaptor 테스트: `dali-adaptor/automated-tests/web-engine-runtime`, 빌드 디렉터리 `webview-validation/runtime-tests`; `ctest --test-dir webview-validation/runtime-tests --output-on-failure`로 4개 통과. 실제 로더의 인자 인계, 초기화 실패 후 고정, 토큰 공유/해제, 기능 미제공 실패를 테스트용 플러그인으로 검증.
- extension 저장 테스트: `dali-extension/automated-tests/web-engine-arguments`, 빌드 디렉터리 `webview-validation/arguments-tests`; 1개 통과. 실제 `WebEngineArguments`의 복사, 전체 교체, 잘못된 입력 보존, writable argv/종단 null, TCORE 필수 옵션 중복 방지 및 다른 백엔드 인계 시 합성 옵션 제외를 검증.
- UI 테스트 바이너리의 `ldd`에서 core/adaptor/foundation 모두 `opt3/lib/libdali3-*.so.3`로 해석됨을 확인.
- 이관한 `samples/web-view/web-view-win-example.cpp` 및 LWE 공통 플러그인 진입점의 C++17 syntax-only 컴파일 통과. Windows 실행이나 Starfish 연동 빌드 성공을 의미하지 않음.
- 최신 extension에 Ubuntu LWE 백엔드가 추가돼 있어, Tizen/Windows와 함께 같은 초기화 계약으로 갱신.
- 테스트 로그: `/tmp/webview-ui-tests-run.log`, `/tmp/webview-ui-tests-build.log`, `/tmp/webview-*-build.log`. 네이티브 전체 빌드 실패 근거: `/tmp/webview-extension-configure.log`.

### P2 후속 검증 (2026-11-12)

- 로컬 DALi3 adaptor 및 UI build/install 완료 (`webview-validation/opt3`). UI 테스트 바이너리를 재빌드한 뒤 Web 관련 **119/119** 통과.
- 설치된 adaptor를 사용하는 runtime 테스트 **12/12** 통과 (기존 4개 + alias 8개). extension 인자 저장 테스트 **1/1**, ECORE/TCORE 관리자 테스트 **2/2** 통과.
- 수정 전 adaptor 소스로 별도 라이브러리를 만들어 alias 8개 실패 확인. 수정 전 관리자 소스로 registry 크기 assertion 2개 실패 확인. 원본 저장소와 설치본은 되돌리지 않고 검증 디렉터리에서 비교했다.
- GBS `tizen / armv7l / -j6`: 변경된 adaptor/extension의 ECORE DALi3·DALi2 및 TCORE DALi3 빌드·링크·RPM 생성 모두 성공. 각 구성의 Chromium 플러그인 RPM을 검증용 GBS chroot에 설치한 뒤 로컬 runtime/arguments/profile 회귀 테스트를 다시 실행해 모두 통과했다. 관리자 회귀는 설치된 ARM 플러그인을 실행하는 테스트가 아니라 실제 관리자 소스와 네이티브 의존성 대역을 빌드한 테스트다.
- ECORE/TCORE RPM은 `webview-validation/review-followup/{ecore,tcore}/RPMS`에 분리 보관했다. `target-results.json`에 명령·종료 코드·RPM SHA-256, `results.json`에 최종 커밋·게시 상태·검증 범위를 기록했다.
- **PR 충돌 해결:** `devel`의 `7f2e2fd9588ca8314a065d4dcd3698c28396ce4d` 위로 단일 커밋을 rebase했다. 유일한 충돌은 `web-view-impl.cpp`의 기존 생성 함수에 적용된 `WebViewImplPtr` 소유권 변경과 PR의 프로필 기반 생성 함수 교체였다. 새 `New(profile)`에서도 `WebViewImplPtr`로 즉시 소유하도록 유지하고, 첫 사용 타입 고정 및 프로필 기반 생성 동작을 보존했다. 합의된 정책을 변경할 필요는 없었다. rebase 후 로컬 DALi3 foundation/components build/install 및 Web 회귀 **125/125**가 통과했다. 로그와 보존 확인: `webview-validation/pr25-resolution/`. 이번 rebase에서 GBS/실기기/Windows/Ubuntu LWE 최종 링크·페이지 검증은 재실행하지 않았다.
- 후속 로그·결과: `webview-validation/review-followup/`. 기존 GBS/Ubuntu 결과는 아래의 최초 구현 검증 이력이며, 후속 수정 검증과 구분한다.
- 미실시 범위는 그대로다: 실기기 실행, Windows 네이티브 빌드, Ubuntu 데스크톱 LWE 최종 링크·페이지 렌더링.

### GBS armv7l 검증 (2026-11-12)

- GBS 2.1.0과 `~/.gbs.conf`의 `tizen` 프로필로 사용자 지정 Tizen Base/Unified reference 저장소 사용. 빌드 설정은 Tizen 11이며, 실제 Chromium SDK는 `chromium-efl 1.1.144-1`, LWE SDK는 `lightweight-web-engine 1.5.11-1`.
- 공통 명령: 각 원본 저장소에서 `gbs build -P tizen -A armv7l -B /home/jy/work/dali/webview-validation/gbs-root --include-all --threads 1 --define 'jobs 6' --define '_smp_mflags -j6'`. 커밋하지 않은 구현 변경도 포함.
- ECORE 기본 구성: core → adaptor → extension → UI 모두 성공. 기본 spec의 DALi3 및 DALi2 호환 빌드를 모두 수행했고, Chromium/LWE 플러그인과 UI foundation/components RPM 생성까지 확인.
- 생성된 DALi3 Chromium/LWE 플러그인의 `GetWebEngineProfileApi` export 확인. 실제 `libchromium-ewk.so`의 EWK/WV context 생성·해제 및 context 기반 view 생성 심볼도 확인.
- TCORE 구성: 공통 명령에 `--overwrite --define 'BUILD_TCORE_ENABLE 1' --define '_without_dali2 1'`을 추가해 DALi3 검증. adaptor와 extension 모두 빌드·링크·RPM 생성 성공. Chromium 플러그인의 `GetWebEngineProfileApi` export 및 WV context/view API 참조 확인.
- **특이 사항 — TCORE extension**: 변경 전 `99d77af0a89474575c041b0ee20eee5b1293091c`에도 있던 `ClearAllTilesResources()`의 `wv_view_clear_all_tiles_resources()` 호출이 최초 빌드에서 실패. 현재 SDK에는 이 함수의 헤더 선언과 export 심볼이 모두 없음. 사용자 지시에 따라 TCORE 구현의 호출을 제거하고 함수 본문에 `Deprecated` 주석을 남겨 no-op으로 변경한 뒤 재빌드 성공. adaptor 인터페이스와 ECORE 구현은 유지. 결과 바이너리에 제거한 WV 함수 참조가 없음을 확인.
- ECORE RPM 86개와 로그: `webview-validation/gbs-artifacts/ecore-armv7l`. TCORE adaptor/extension RPM 및 재빌드 로그: `webview-validation/gbs-artifacts/tcore-armv7l` (최초 실패 로그는 `dali-gbs-extension-tcore.before-deprecated.log`). 명령·소스 revision·결과: `webview-validation/gbs-artifacts/results.json`. GBS 작업용 로컬 저장소에는 구성 전환 결과가 섞여 있으므로 구분해 보관한 결과물을 사용.
- 앞선 로컬 SDK 부재는 GBS의 Tizen SDK로 해소됨. GBS 성공은 컴파일·링크·패키징 검증이며 실제 기기 동작 검증은 아님. Windows/Ubuntu LWE 네이티브 빌드도 이번 GBS 범위에 포함되지 않음.

### Windows/Ubuntu 사전 검증 (2026-11-12)

- 사용자 지시에 따라 독립 리뷰를 중지하고 데스크톱 검증을 먼저 수행.
- Ubuntu 정식 CMake 설정에서 로컬 DALi3 core/adaptor 및 EGL/GLES/X11 의존성 확인 성공. 데스크톱 Starfish 헤더/라이브러리가 설치되지 않아 `Starfish SDK not found`로 설정 중단. 로그: `/tmp/dali-lwe-ubuntu-configure.log`.
- 실제 Tizen LWE 1.5.11 공개 헤더를 사용해 Ubuntu 플러그인의 공통/백엔드 소스 11개를 호스트 C++17 컴파일러로 각각 컴파일 성공 (`-Wall -Wextra -Werror`). 이는 데스크톱 SDK와의 최종 링크 또는 LWE 실행 검증을 대신하지 않음.
- 저장소의 기존 Ubuntu EGL renderer 테스트를 로컬 DALi3 라이브러리와 빌드하고 실제 X11 디스플레이에서 실행 성공 (`EGL renderer tests passed`, 종료 코드 0). readback, row orientation, texture upload, resize, shared context, shutdown/reinitialization 검증. Starfish 페이지 렌더링 테스트는 SDK가 없어 미실시.
- Windows용 MSVC/PowerShell/Starfish SDK 및 사용할 Windows 호스트가 현재 환경에서 확인되지 않음. 빌드 스크립트에 지정된 사내 SDK 배포 서버 `github.sec.samsung.net` 연결은 시간 초과. 사용자에게 제약을 알린 뒤 독립 리뷰를 재개했고, 문서 상단의 P2 두 건을 확인.
- 컴파일 명령/로그, renderer 테스트 바이너리/실행 로그: `webview-validation/lwe-ubuntu-native-checks`.

### 남은 검증 항목

1. Windows LWE 네이티브 빌드, Ubuntu 데스크톱 SDK를 사용한 플러그인 최종 링크·페이지 렌더링 테스트 및 각 플랫폼에서 기본 프로필의 기존 동작·미지원 프로필 실패 확인.
2. 기기에서 profile 단독 유지·cookie만 유지·복수 view 공유·bundle 유무·엔진 인자 효과·종료 후 남은 핸들의 안전성 확인. WV standalone의 미구현 API는 실패를 정상적으로 보고하는지 구분해 기록.
3. 검증 결과와 한계를 영구 설명에 반영하고 이 임시 문서 삭제.

## 9. 완료 조건과 삭제 지시

- [x] 세 공개 API군과 관련 메타데이터/조회 기능 구현
- [x] UI/adaptor/extension의 타입·인자·초기화 상태 연결 (로컬 runtime/mock 검증)
- [x] ECORE/TCORE 연결 및 LWE 지원/실패 정책 코드 구현 (ECORE/TCORE Chromium·Tizen LWE GBS 통과, 기기 실행 미검증)
- [ ] 프로필·쿠키 매니저·뷰의 소유권 및 앱 종료 안전성 검증
- [x] `New(argc, argv)` 심볼 유지와 인자 무시 동작 검증
- [x] 공개 API 주석·deprecated 설명·샘플 갱신
- [ ] 적절한 build/install/자동 테스트 완료, 기기 검증 결과와 한계 기록
- [x] 코드 리뷰에 필요한 API 변화·검증·특이 사항 보고 (P2 두 건 후속 수정 및 회귀 검증 완료)
- [ ] 영구 설명을 남긴 뒤 **이 임시 문서 삭제**

GitHub 이슈 전체를 구현 완료로 변경하거나, 요청하지 않은 다른 기능을 추가하지 않는다. 이후 이슈의 추가 수정이 필요하면 현재 본문과 사용자의 수정 범위를 확인한다. 커밋/PR을 작성할 경우 상위 AGENTS.md의 한국어 PR 템플릿 및 AI co-author 규칙을 따른다.
