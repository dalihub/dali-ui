# dali-ui Manual Tests

각 모듈 개발자가 독립적으로 작성·실행할 수 있는 수동 테스트 앱 모음입니다.
앱을 실행하면 등록된 TC 목록이 표시되며, TC를 선택해 진입하고 Back 키로 목록으로 돌아올 수 있습니다.

## 디렉터리 구조

```
manual-tests/
├── CMakeLists.txt
├── README.md
└── dali-ui-foundation/          # foundation 모듈 전용 테스트 앱
    ├── CMakeLists.txt
    ├── manual-test-case.h       # TC 인터페이스 및 등록 매크로 (공개 인터페이스)
    ├── main.cpp                 # 런처 앱 (수정 불필요)
    ├── bin/                     # 빌드 후 실행 파일 생성 위치
    └── tc/                      # TC 소스 파일 디렉터리
        └── tc-hello-world.cpp   # 예시 TC
```

모듈이 추가될 경우 동일한 구조로 `manual-tests/<module-name>/` 디렉터리를 추가하면
최상위 CMakeLists.txt가 자동으로 인식합니다.

---

## 빌드 방법

### 1. 전체 빌드 (manual-tests 루트에서)

```bash
cd manual-tests
cmake .
make install -j
```

빌드 완료 후 실행 파일:

```
manual-tests/dali-ui-foundation/bin/manual-test-dali-ui-foundation
```

### 조작

| 동작 | 설명 |
|---|---|
| TC 항목 탭 | 해당 TC로 진입 |
| `< Back` 탭 | TC 목록으로 돌아가기 |
| 하드웨어 Back 키 | TC 내에서: 목록으로 / 목록에서: 앱 종료 |
| Escape 키 | 앱 종료 |

---

## TC 추가 방법

### 1. TC 파일 생성

`manual-tests/dali-ui-foundation/tc/` 디렉터리에 `tc-<기능명>.cpp` 파일을 추가합니다.
CMakeLists.txt는 `tc/*.cpp`를 자동으로 빌드에 포함하므로 **빌드 파일 수정이 필요 없습니다.**

동일 디렉터리에 **`tc-<기능명>.md`** 파일을 함께 작성하여 테스트 시나리오를 기록합니다.
화면 구성, 조작 순서, 기대 결과, 통과 기준을 포함합니다.

### 2. TC 클래스 작성

```cpp
#include "manual-test-case.h"

using namespace Dali;
using namespace Dali::Ui;

class TcMyFeature : public ManualTest::TestCase
{
public:
  Dali::String GetName() const override
  {
    return "My Feature";
  }

  Dali::String GetDescription() const override
  {
    return "내 기능이 정상 동작하는지 확인";
  }

  void OnEnter(View contentArea) override
  {
    // contentArea는 수직 StackLayout (MATCH_PARENT x 나머지 화면 전체)
    // 여기에 테스트할 UI를 추가합니다
    contentArea.Add(
      Label::New("My Feature is working!")
        .SetFontSize(30.0f)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetHorizontalTextAlignment(Text::Alignment::CENTER)
        .SetVerticalTextAlignment(Text::Alignment::CENTER));
  }

  // 필요한 경우 OnExit()을 오버라이드해 정리 작업을 수행합니다
  // (타이머 해제, 시그널 연결 해제 등)
  void OnExit() override
  {
    // cleanup
  }
};

REGISTER_MANUAL_TEST(TcMyFeature)
```

### 3. TestCase 인터페이스 요약

| 메서드 | 필수 | 설명 |
|---|---|---|
| `GetName()` | ✅ | TC 목록에 표시될 이름 |
| `GetDescription()` | ✅ | TC 목록에 표시될 설명 (한 줄) |
| `OnEnter(View contentArea)` | ✅ | TC 진입 시 호출. `contentArea`에 UI를 추가합니다 |
| `OnExit()` | ❌ | TC 이탈 시 호출. 기본 구현은 아무것도 하지 않음 |

### `contentArea` 특성

- 타입: `StackLayout` (수직 방향)
- 너비: `MATCH_PARENT` (화면 전체 너비)
- 높이: 헤더를 제외한 나머지 화면 전체 (`weight = 1`)
- `contentArea.Add(view)` 로 자식 뷰를 추가합니다

---

## 파일 네이밍 규칙

| 항목 | 규칙 | 예시 |
|---|---|---|
| TC 파일명 | `tc-<기능명>.cpp` (소문자, 하이픈) | `tc-scroll-view.cpp` |
| TC 시나리오 | `tc-<기능명>.md` (TC 파일과 동일 이름) | `tc-scroll-view.md` |
| TC 클래스명 | `Tc<기능명>` (PascalCase) | `TcScrollView` |
