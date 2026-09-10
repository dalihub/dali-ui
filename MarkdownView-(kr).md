[English](https://github.sec.samsung.net/NUI/dali-ui/wiki/MarkdownView) | [한국어](https://github.sec.samsung.net/NUI/dali-ui/wiki/MarkdownView-%28kr%29) | [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)

# MarkdownView

`Dali::Ui::MarkdownView`는 제목, 목록, 인용문, 코드 블록, 표 등 Markdown 콘텐츠를 네이티브 UI 뷰로 표시합니다. 서식이 있는 도움말, 메시지, 세로로 길어지는 문서에 적합합니다. 텍스트 편집기나 브라우저가 아닌 `View`이며, 작업 목록 체크박스는 조작할 수 있지만 링크와 이미지는 현재 텍스트로만 표시합니다.

## 설정과 완전한 예제

components 및 foundation 헤더를 포함하고 애플리케이션을 `dali-ui-components`와 필요한 foundation 라이브러리에 연결합니다. `MainLoop()` 전에 `Components::UiConfig`를 한 번 적용합니다. foundation 설정도 포함하므로 foundation `UiConfig`를 별도로 적용하지 않습니다.

```cpp
#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali/public-api/adaptor-framework/application.h>

using namespace Dali;
using namespace Dali::Ui;

class MarkdownExample : public ConnectionTracker
{
public:
  explicit MarkdownExample(Application& application)
  {
    application.InitSignal().Connect(this, &MarkdownExample::Create);
  }

  void Create(Application application)
  {
    ScrollView scroll = ScrollView::New();
    scroll.SetRequestedWidth(MATCH_PARENT);
    scroll.SetRequestedHeight(MATCH_PARENT);
    scroll.SetScrollDirection(ScrollDirection::Vertical);

    MarkdownView document = MarkdownView::New();
    document.SetRequestedWidth(MATCH_PARENT);
    document.SetRequestedHeight(WRAP_CONTENT);
    document.SetMarkdown("# Getting started\n\n"
                         "Read **carefully**, then try `Build()`.\n\n"
                         "- [ ] Read the guide\n- [x] Open the sample\n\n"
                         "> Keep the document width bounded.\n\n"
                         "[Documentation](https://example.com)");
    scroll.SetContent(document);
    application.GetWindow().Add(scroll);
  }
};

int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  Components::UiConfig::New().Apply();
  MarkdownExample controller(application);
  application.MainLoop();
  return 0;
}
```

components 초기화 규칙은 [설정 API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/components-ui-config.h)에 정의되어 있습니다. 스크롤 동작은 [ScrollView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ScrollView-%28kr%29)를 참고하세요.

## 콘텐츠, 업데이트, 작업 목록

| API | 동작 |
|---|---|
| `MarkdownView::New()` | 설정된 기본 스타일로 뷰를 생성합니다. |
| `MarkdownView::New(style)` | `MarkdownViewStyle`을 지정하여 생성합니다. |
| `SetMarkdown(source)` | 전체 Markdown 원문을 교체합니다. |
| `GetMarkdown()` | 체크박스 변경을 포함한 현재 원문을 반환합니다. |
| `Clear()` | 문서 콘텐츠를 제거하고 원문을 비웁니다. |
| `MarkdownView::ToPlainText(source)` | 뷰를 생성하지 않고 원문을 `Dali::String` 일반 텍스트로 변환합니다. |
| `MarkdownView::DownCast(handle)` | 호환되는 `BaseHandle`에서 MarkdownView 핸들을 얻습니다. |

스트리밍 출력을 표시하려면 수신한 UTF-8 텍스트를 누적하고 업데이트마다 누적된 전체 원문을 `SetMarkdown()`에 전달합니다. 추가 전용 API는 없습니다. 동일한 원문은 처리하지 않으며, 변경된 원문은 다시 파싱한 후 가능한 경우 기존 렌더링 컴포넌트를 재사용합니다. 문서 크기에 맞춰 업데이트를 묶어서 처리하세요. 문서 가상화 API는 아닙니다.

작업 문법은 `- [ ] todo`, `- [x] done`, `- [X] done`을 지원하며 순서 있는 목록과 중첩 목록에서도 사용할 수 있습니다. 체크박스를 활성화하면 원문의 표시 문자가 `x` 또는 공백으로 변경됩니다. 사용자 변경을 저장할 때는 `GetMarkdown()`을 읽으세요. 이전 원문으로 다시 설정하면 사용자 변경도 해당 원문으로 교체됩니다.

MarkdownView에는 **전용 시그널이 없습니다**. 이 클래스는 `MarkdownChangedSignal()`, 작업 변경 시그널, 링크 활성화 시그널을 제공하지 않습니다. 내부 작업 컨트롤은 `CheckBox::SelectionChangedSignal()`을 사용합니다. 렌더링된 자식 뷰는 안정적인 작업 제어 API가 아닌 구현 세부 사항이므로 애플리케이션 로직에서 자식 인덱스에 의존하지 마세요.

다음 함수는 위의 완전한 예제와 함께 사용할 수 있습니다.

```cpp
Dali::String ReplaceAndReadPlainText(MarkdownView document)
{
  document.SetMarkdown("# Checklist\n\n- [x] Done\n\n[Guide](guide.html)");
  Dali::String source = document.GetMarkdown();
  Dali::String plain = MarkdownView::ToPlainText(source);
  // Result: "Checklist\n[x] Done\nGuide [guide.html]"
  document.Clear();
  return plain;
}
```

`ToPlainText()`는 강조·제목 문법과 순서 없는 목록의 글머리표를 제거하고, 순서 있는 목록 번호와 작업 표시를 유지하며, 링크·이미지 URL을 대괄호로 덧붙입니다. 표는 셀별 줄바꿈 텍스트로 변환되고 코드 블록은 코드와 언어 이름을 유지합니다. Markdown 왕복 직렬화 기능은 아닙니다. 화면 표시용 파싱과 달리 HTML 마크업을 제외하므로 인라인 HTML의 텍스트는 남을 수 있지만 원시 HTML 블록은 생략됩니다.

## 불변 스타일

뷰를 만들기 전에 스타일을 빌드합니다. `Configure()`는 기존 스타일을 빌더로 복사하고 `Build()`는 원본을 변경하지 않는 새 스타일을 만듭니다. 빌더는 이동만 가능하며 `Build()`는 rvalue에서 호출해야 합니다. 아래처럼 임시 빌더를 체이닝하거나, 이름 있는 빌더에는 `std::move(builder).Build()`를 사용합니다.

```cpp
MarkdownView CreateStyledDocument()
{
  MarkdownViewStyle style = MarkdownViewStyle::Default().Configure()
    .SetTextFontSize(20.0f)
    .SetHeading1FontSize(30.0f)
    .SetCodeBlockFontSize(18.0f)
    .SetTextColor(UiColor(0x202124))
    .SetCodeBlockBackgroundColor(UiColor(0xF1F3F4))
    .SetTaskCheckBoxSelectedIconColor(UiColor(0x2962FF))
    .Build();
  MarkdownView document = MarkdownView::New(style);
  document.SetRequestedWidth(480.0f);
  document.SetRequestedHeight(WRAP_CONTENT);
  document.SetMarkdown("# Styled document\n\n- [ ] Review");
  return document;
}
```

이 함수는 설정을 적용한 후 호출합니다. 스타일은 본문·제목·코드 글꼴, 6단계 제목 크기, 코드·코드 제목 크기, 텍스트·장식 색상, 작업 아이콘 생성 및 색상을 지원합니다. 글꼴 크기는 유한한 양수여야 합니다. `UiColor` 테마 토큰은 토큰 식별자를 유지하고 테마에 따라 해석됩니다. 예제의 색상 리터럴은 고정 색상입니다.

공개 `MarkdownView::SetStyle()`은 없습니다. 생성 후 글꼴이나 스타일 설정을 바꾸려면 새 스타일로 대체 뷰를 만들고 원문을 전달합니다. 애플리케이션 기본 스타일은 `config.Apply()` 전에 `config.StyleSheet().SetStyle(MarkdownViewStyle::DefaultKey(), factory)`로 등록할 수 있습니다. `DefaultPreset()`은 내장 스타일이고 `Default()`는 설정된 기본 스타일을 조회합니다.

## 너비, 레이아웃, 지원 문법

고정 너비 또는 `MATCH_PARENT` 너비와 `WRAP_CONTENT` 높이를 권장합니다. 너비가 `WRAP_CONTENT`이면 유한한 측정 너비 전체를 사용하고, 너비가 무제한이면 콘텐츠 기반 크기를 사용합니다. 예측 가능한 줄바꿈을 위해 `SetMaximumWidth()` 또는 너비가 제한된 부모를 사용하세요. 고정 너비에도 크기 제약이 적용됩니다. MarkdownView 자체는 스크롤하지 않으므로 위 예제처럼 세로 ScrollView를 사용합니다. 코드 블록은 RTL 문서에서도 명시적으로 왼쪽에서 오른쪽으로 배치됩니다.

| 문법 / 기능 | 현재 동작 |
|---|---|
| `#`부터 `######`, Setext 제목 | 제목 서식을 적용합니다. Setext는 `===` 또는 `---` 밑줄을 사용합니다. |
| 문단, `*emphasis*`, `**strong**`, `~~strike~~`, 인라인 백틱 | 스타일 텍스트를 표시하고 이스케이프와 HTML 문자 엔티티를 파싱합니다. |
| 순서 있는/없는 목록, 중첩, `>` 인용문, 구분선 | 네이티브 레이아웃과 장식을 사용하며 작업 항목에는 체크박스를 추가합니다. |
| 펜스 / 들여쓰기 코드 | 코드 형식으로 표시합니다. 펜스 언어 이름은 표시용이며 구문 강조는 하지 않습니다. |
| 구분·정렬 행이 있는 파이프 표 | 왼쪽·가운데·오른쪽 셀 정렬을 지원하는 네이티브 표 레이아웃입니다. |
| `[text](url)`과 `<https://example.com>` | `text [url]`로 표시하며 클릭 가능한 링크가 아닙니다. 일반 URL 자동 링크 확장은 활성화되어 있지 않습니다. |
| `![alt](url)` | 인라인·독립 이미지 모두 `alt [url]`로 표시합니다. 현재 표시 방식에서는 이미지를 로드하지 않습니다. |
| 원시 HTML | HTML 렌더링은 비활성화되어 있으며 화면 표시 파서는 태그를 텍스트로 취급합니다. |

파서는 표, 취소선, 작업 목록 확장을 활성화합니다. 모든 GitHub Markdown 확장을 지원한다고 가정하지 마세요. 수식 렌더링, 다이어그램 렌더링, 코드 블록 실행은 제공하지 않습니다. 파서 옵션이나 이미지·링크 표시 방식을 전환하는 공개 API도 없습니다.

## 접근성과 참고 자료

작업 컨트롤은 접근성 역할이 `CHECK_BOX`인 실제 CheckBox입니다. 작업 텍스트를 접근성 이름으로 설정하고, 중복 콘텐츠를 피하기 위해 옆의 작업 텍스트 뷰는 접근성 트리에서 숨깁니다. 구현에서는 Markdown 전용 문서 역할이나 제목·표의 의미론적 역할을 지정하지 않습니다. 브라우저처럼 문서를 탐색하거나 클릭 가능한 링크로 안내한다고 가정하지 마세요. 대상 플랫폼의 접근성 환경에서 읽기 순서와 작업 조작을 검증하세요. [접근성 가이드](https://github.sec.samsung.net/NUI/dali-ui/wiki/Accessibility-%28kr%29)도 참고하세요.

- [MarkdownView 공개 API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/markdown/markdown-view.h)와 [스타일 API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/markdown-view-style.h).
- [파서와 렌더링 구현](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/dali-ui-components/internal/markdown).
- [Markdown 샘플](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/markdown): 문서 사례, 스트리밍 청크, 방향, 배율, 일반 텍스트 출력을 확인할 수 있습니다. 가져다 사용할 때는 위의 components 설정을 따르세요.
- [UTC 테스트](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-MarkdownView.cpp): 너비 제약, 스타일 설정, 작업 원문 업데이트, 텍스트 URL·이미지 표시, 컴포넌트 재사용, 일반 텍스트 변환을 다룹니다.

[Components로 돌아가기](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)
