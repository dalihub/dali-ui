# Text Input

## Overview

`InputField`와 `InputEditor`는 editable text View입니다. 텍스트 입력, 편집, 선택 등 사용자 상호작용을 지원합니다.

| Component | 용도 | 주요 차이 |
|---|---|---|
| `InputField` | 한 줄 입력 | single-line, password mode 지원 |
| `InputEditor` | 여러 줄 입력 | multi-line, line wrap, line height, auto grow, line count |

<br/>

## Basic Usage

InputField 예:

~~~cpp
InputField field = InputField::New();
field.SetPlaceholder("Enter text");
field.SetFontSize(22.0f);
~~~

InputEditor 예:

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetPlaceholder("Enter long text");
editor.SetLineWrapMode(Text::LineWrapMode::WORD);
editor.SetRequestedHeight(120.0f);
~~~

<br/>

## Text and Placeholder

공통 API:

| API | 설명 |
|---|---|
| `SetText()` | 텍스트 설정 |
| `GetText()` | 텍스트 조회 |
| `SetPlaceholder()` | placeholder 텍스트 설정 |
| `GetPlaceholder()` | placeholder 텍스트 조회 |
| `SetPlaceholderColor()` | placeholder 색상 설정 |
| `SetShowPlaceholderOnFocus()` | focus 시 placeholder 표시 여부 |

~~~cpp
InputField field = InputField::New();
field.SetText("Initial text");
field.SetPlaceholder("Search");
field.SetPlaceholderColor(UiColor::OUTLINE);
~~~

<br/>

## Translatable Placeholder

`SetTranslatablePlaceholder()`를 사용하여 placeholder에 다국어 문자열을 바인딩할 수 있습니다.

~~~cpp
InputField field = InputField::New();
field.SetTranslatablePlaceholder("IDS_SEARCH_PLACEHOLDER");

InputEditor editor = InputEditor::New();
editor.SetTranslatablePlaceholder("IDS_COMMENT_PLACEHOLDER");
~~~

explicit domain 지정:

~~~cpp
InputField field = InputField::New();
field.SetTranslatablePlaceholder("IDS_SEARCH_PLACEHOLDER", "myapp");
~~~

> [!NOTE]
> 텍스트 값의 localization은 앱 상태에 맞게 `SetBindingResource()`로 직접 처리할 수 있습니다.

자세한 내용: [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI-(kr))

<br/>

## Input Method Context

`InputMethodContext`는 `InputField` 또는 `InputEditor`가 사용하는 platform input panel / keyboard context입니다. 앱은 이 context를 통해 input panel 표시 요청, input panel layout, return key type, text prediction 등 입력 패널 관련 옵션을 설정하고, input panel 상태나 영역을 조회할 수 있습니다.

`InputMethodContext`는 앱이 직접 생성하지 않고, `InputField` 또는 `InputEditor`에서 가져와 사용합니다.

~~~cpp
InputField field = InputField::New();

InputMethodContext context = field.GetInputMethodContext();
~~~

`InputEditor`에서도 동일하게 사용할 수 있습니다.

~~~cpp
InputEditor editor = InputEditor::New();

InputMethodContext context = editor.GetInputMethodContext();
~~~

대표적인 기능은 다음과 같습니다.

| API | 설명 |
|---|---|
| `ShowInputPanel()` | input panel 표시 요청 |
| `HideInputPanel()` | input panel 숨김 요청 |
| `SetInputPanelLayout()` | input panel layout 설정 |
| `SetInputPanelReturnKeyType()` | return key type 설정 |
| `SetTextPredictionEnabled()` | text prediction 활성화 여부 설정 |
| `GetInputPanelState()` | 현재 input panel 상태 조회 |
| `GetInputPanelArea()` | 현재 input panel 영역 조회 |
| `StatusChangedSignal()` | input panel 상태 변경 signal |
| `ResizedSignal()` | input panel 영역 변경 signal |

input panel 표시와 숨김은 platform에 요청하는 동작이며, 실제 상태 변경은 비동기로 발생할 수 있습니다.

~~~cpp
InputField field = InputField::New();

InputMethodContext context = field.GetInputMethodContext();
context.ShowInputPanel();
context.HideInputPanel();
~~~

input panel layout, return key type, text prediction도 context를 통해 설정할 수 있습니다.

~~~cpp
InputField field = InputField::New();

InputMethodContext context = field.GetInputMethodContext();
context.SetInputPanelLayout(InputMethod::PanelLayout::NUMBER);
context.SetInputPanelReturnKeyType(InputMethod::ReturnKeyType::DONE);
context.SetTextPredictionEnabled(false);
~~~

input panel 상태나 영역이 필요한 경우 직접 조회할 수 있습니다.

~~~cpp
InputMethodContext::State state = context.GetInputPanelState();
BoundsInteger area = context.GetInputPanelArea();
~~~

input panel 상태 변경이나 영역 변경을 관찰해야 하는 경우 signal을 연결할 수 있습니다.

~~~cpp
context.StatusChangedSignal().Connect(
  [](InputMethodContext context, InputMethodContext::State state)
  {
    // input panel state changed
  });

context.ResizedSignal().Connect(
  [](InputMethodContext context)
  {
    BoundsInteger area = context.GetInputPanelArea();
    // input panel area changed
  });
~~~

참고 샘플: [text-input-method-context-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/text/text-input-method-context-example.cpp)

<br/>

## Text Style

공통 style API입니다.

| API | 설명 |
|---|---|
| `SetTextColor()` | 텍스트 색상 |
| `SetPlaceholderColor()` | placeholder 색상 |
| `SetFontFamily()` | font family |
| `SetFontSize()` | font size |
| `SetFontWeight()` | font weight |
| `SetFontWidth()` | font width |
| `SetFontSlant()` | font slant |
| `SetTextBackgroundColor()` | 텍스트 배경 색상 |
| `SetUnderline()` | underline style |
| `SetShadow()` | shadow style |
| `SetOutline()` | outline style |
| `SetLineThrough()` | line-through style |
| `SetFontVariation()` | variable font axis |

~~~cpp
InputField field = InputField::New();
field.SetTextColor(UiColor::PRIMARY);
field.SetPlaceholderColor(UiColor::OUTLINE);
field.SetFontSize(22.0f);
~~~

<br/>

## Cursor and Selection

공통 cursor/selection API입니다.

| API | 설명 |
|---|---|
| `SetCursorWidth()` | cursor 너비 (pixel) |
| `SetCursorColor()` | cursor 색상 |
| `SetCursorBlinkEnabled()` | cursor 깜빡임 활성화 |
| `SetCursorBlinkInterval()` | cursor 깜빡임 주기 (초) |
| `SetCursorPosition()` | cursor 위치 (문자 인덱스) |
| `SetSelectionEnabled()` | 텍스트 선택 활성화 |
| `SetSelectionColor()` | 선택 영역 하이라이트 색상 |
| `SetTextHandleEnabled()` | text handle 활성화 |
| `SetTextHandleColor()` | text handle 색상 |
| `SelectText(start, end)` | 범위 선택 |
| `SelectWholeText()` | 전체 선택 |
| `ClearSelection()` | 선택 해제 |
| `GetSelectedText()` | 선택된 텍스트 조회 |
| `GetSelectedTextStart()` | 선택 시작 인덱스 |
| `GetSelectedTextEnd()` | 선택 끝 인덱스 |

~~~cpp
InputField field = InputField::New();
field.SetCursorColor(UiColor::PRIMARY);
field.SetSelectionEnabled(true);
field.SetSelectionColor(UiColor(0x99CCFF));
field.SetTextHandleEnabled(true);
field.SetTextHandleColor(UiColor(0x000080));
~~~

<br/>

## InputFilter

`InputFilter`는 삽입되는 입력에 적용되는 필터입니다. `std::regex` ECMAScript 문법을 사용합니다.

핵심 정책:
- **allow pattern**: 설정 시 패턴에 매치되지 않는 입력은 거부
- **deny pattern**: 설정 시 패턴에 매치되는 입력은 거부
- **둘 다 설정**: allow를 만족하고 deny에 걸리지 않아야 통과

> [!NOTE]
> InputFilter는 user input, IME commit, paste에 적용됩니다. `SetText()`로 직접 설정하는 텍스트에는 적용되지 않습니다.

~~~cpp
Text::InputFilter inputFilter;
inputFilter.SetAllowPattern("[\\d]");
inputFilter.SetDenyPattern("[0-5]");

InputField field = InputField::New();
field.SetInputFilter(inputFilter);
~~~

위 예시는 숫자만 허용하되 0-5는 거부하므로, 6-9만 입력 가능합니다.

거부 사유는 `InputRejectedSignal`에서 확인할 수 있습니다:

~~~cpp
field.InputRejectedSignal().Connect(
  [](View view, Text::InputFilter::RejectReason reason)
  {
    // reason: NOT_ALLOWED 또는 DENIED
  });
~~~

필터 해제:

~~~cpp
field.ClearInputFilter();
~~~

<br/>

## Password Mode

`InputField` 전용 기능입니다.

| Mode | 설명 |
|---|---|
| `PasswordMode::NONE` | password masking 없음 |
| `PasswordMode::HIDE_ALL` | 모든 문자를 mask character로 표시 |
| `PasswordMode::REVEAL_LAST_CHARACTER` | 마지막 입력 문자를 일정 시간 표시 후 mask 처리 |

~~~cpp
InputField password = InputField::New();
password.SetPasswordMode(Text::PasswordMode::REVEAL_LAST_CHARACTER);
password.SetPasswordMaskCharacter(0x2022);   // '•' (U+2022)
password.SetPasswordRevealDuration(1000);    // 1000ms
~~~

> [!NOTE]
> `SetPasswordMaskCharacter()`의 인자는 Unicode code point (`uint32_t`)입니다. 자주 쓰는 값: `0x2A`(`*`), `0x2022`(`•`), `0x25CF`(`●`).

<br/>

## Editable

`SetEditable()`로 편집 가능 여부를 제어합니다.

~~~cpp
field.SetEditable(false);  // 읽기 전용
~~~

<br/>

## Typing Style

Typing style은 현재 cursor 위치 또는 selection에 적용될 입력 스타일입니다.

| API | 설명 |
|---|---|
| `SetTypingTextColor()` | 입력 텍스트 색상 |
| `SetTypingFontFamily()` | 입력 font family |
| `SetTypingFontSize()` | 입력 font size |
| `SetTypingFontWeight()` | 입력 font weight |
| `SetTypingFontWidth()` | 입력 font width |
| `SetTypingFontSlant()` | 입력 font slant |

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetTypingTextColor(UiColor::PRIMARY);
editor.SetTypingFontSize(24.0f);
~~~

선택 영역이 있으면 해당 범위에 스타일이 적용되고, collapsed selection이면 이후 입력에 적용됩니다.

> [!NOTE]
> `TypingStyleChangedSignal`은 모든 `SetTyping*()` 호출을 그대로 mirror하는 signal이 아닙니다. 현재 cursor/selection 기준으로 resolved typing style이 바뀔 때 발생합니다. 예를 들어 cursor 이동이나 selection 변경 후에도 발생할 수 있습니다.

참고 샘플: [text-typing-style-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-typing-style-example.cpp)

<br/>

## InputEditor 전용 기능

### Multi-line / Line Wrap

InputEditor는 기본적으로 multi-line 입력을 지원합니다.

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetLineWrapMode(Text::LineWrapMode::WORD);
editor.SetLineHeight(1.5f);
editor.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
~~~

### Auto Grow

Auto grow는 텍스트 변경 시 `WRAP_CONTENT` dimension에 대해 measured size를 invalidate하는 기능입니다.

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetAutoGrowEnabled(true);
editor.SetRequestedHeight(WRAP_CONTENT);
~~~

> [!NOTE]
> Fixed size 또는 `MATCH_PARENT` dimension은 일반 layout 동작을 유지합니다. Auto grow는 `WRAP_CONTENT`에만 영향을 줍니다.

### Line Count

~~~cpp
int lineCount = editor.GetLineCount();        // 현재 layout width 기준
int lineCount2 = editor.GetLineCount(300.0f); // 지정 width 기준
~~~

> [!NOTE]
> width가 아직 layout resolve되지 않은 상태에서는 line count가 정확하지 않을 수 있습니다.

<br/>

## Signals

`InputField`와 `InputEditor` 공통 signal입니다.

| Signal | 시그니처 | 설명 |
|---|---|---|
| `TextChangedSignal()` | `void(View)` | 텍스트 변경 시 |
| `MaximumLengthReachedSignal()` | `void(View)` | 최대 길이 도달 시 |
| `InputRejectedSignal()` | `void(View, InputFilter::RejectReason)` | input filter에 의해 거부 시 |
| `CursorPositionChangedSignal()` | `void(View, uint32_t)` | cursor 위치 변경 시 |
| `SelectionStartedSignal()` | `void(View)` | 선택 시작 시 |
| `SelectionChangedSignal()` | `void(View, uint32_t, uint32_t)` | 선택 범위 변경 시 |
| `SelectionClearedSignal()` | `void(View)` | 선택 해제 시 |
| `TypingStyleChangedSignal()` | `void(View, TypingStyle::Mask)` | resolved typing style 변경 시 |

~~~cpp
field.TextChangedSignal().Connect(
  [](View view)
  {
    // text changed
  });
~~~

<br/>

## Samples

| 기능 | 샘플 |
|---|---|
| InputField | [text-input-field-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-field-example.cpp) |
| InputEditor | [text-input-editor-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-editor-example.cpp) |
| Typing style | [text-typing-style-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-typing-style-example.cpp) |
| Input method context | [text-input-method-context-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-input-method-context-example.cpp) |
| Localization | [text-localization-po-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-localization-po-example.cpp) |

<br/>

## API Reference

- [InputField API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/)
- [InputEditor API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/)

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#documents)
