# Text Input

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Text-Input-(kr))

## Overview

`InputField` and `InputEditor` are editable text Views. They support user interaction such as text input, editing, and selection.

| Component | Purpose | Main Difference |
|---|---|---|
| `InputField` | Single-line input | Supports single-line input and password mode |
| `InputEditor` | Multi-line input | Supports multi-line input, line wrap, line height, auto grow, and line count |

<br/>

## Basic Usage

InputField example:

~~~cpp
InputField field = InputField::New();
field.SetPlaceholder("Enter text");
field.SetFontSize(22.0f);
~~~

InputEditor example:

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetPlaceholder("Enter long text");
editor.SetLineWrapMode(Text::LineWrapMode::WORD);
editor.SetRequestedHeight(120.0f);
~~~

<br/>

## Text and Placeholder

Common APIs:

| API | Description |
|---|---|
| `SetText()` | Set text |
| `GetText()` | Get text |
| `SetPlaceholder()` | Set placeholder text |
| `GetPlaceholder()` | Get placeholder text |
| `SetPlaceholderColor()` | Set placeholder color |
| `SetShowPlaceholderOnFocus()` | Set whether to show the placeholder when focused |

~~~cpp
InputField field = InputField::New();
field.SetText("Initial text");
field.SetPlaceholder("Search");
field.SetPlaceholderColor(UiColor::OUTLINE);
~~~

<br/>

## Translatable Placeholder

Use `SetTranslatablePlaceholder()` to bind a localized string to the placeholder.

~~~cpp
InputField field = InputField::New();
field.SetTranslatablePlaceholder("IDS_SEARCH_PLACEHOLDER");

InputEditor editor = InputEditor::New();
editor.SetTranslatablePlaceholder("IDS_COMMENT_PLACEHOLDER");
~~~

With an explicit domain:

~~~cpp
InputField field = InputField::New();
field.SetTranslatablePlaceholder("IDS_SEARCH_PLACEHOLDER", "myapp");
~~~

> [!NOTE]
> Text value localization can be handled directly with `SetBindingResource()` according to the application state.

See also: [Localization & Multilingual UI](https://github.sec.samsung.net/NUI/dali-ui/wiki/Localization-&-Multilingual-UI)

<br/>

## Input Method Context

`InputMethodContext` is the platform input panel / keyboard context used by `InputField` or `InputEditor`. Applications can use this context to request input panel visibility changes, configure input panel layout, return key type, text prediction, and query the input panel state or area.

Applications normally do not create `InputMethodContext` directly. Instead, they obtain it from `InputField` or `InputEditor`.

~~~cpp
InputField field = InputField::New();

InputMethodContext context = field.GetInputMethodContext();
~~~

It can be used in the same way with `InputEditor`.

~~~cpp
InputEditor editor = InputEditor::New();

InputMethodContext context = editor.GetInputMethodContext();
~~~

Representative features are listed below.

| API | Description |
|---|---|
| `ShowInputPanel()` | Request to show the input panel |
| `HideInputPanel()` | Request to hide the input panel |
| `SetInputPanelLayout()` | Set the input panel layout |
| `SetInputPanelReturnKeyType()` | Set the return key type |
| `SetTextPredictionEnabled()` | Enable or disable text prediction |
| `GetInputPanelState()` | Get the current input panel state |
| `GetInputPanelArea()` | Get the current input panel area |
| `StatusChangedSignal()` | Signal emitted when the input panel state changes |
| `ResizedSignal()` | Signal emitted when the input panel area changes |

Showing and hiding the input panel are platform requests, and the actual state change may occur asynchronously.

~~~cpp
InputField field = InputField::New();

InputMethodContext context = field.GetInputMethodContext();
context.ShowInputPanel();
context.HideInputPanel();
~~~

Input panel layout, return key type, and text prediction can also be configured through the context.

~~~cpp
InputField field = InputField::New();

InputMethodContext context = field.GetInputMethodContext();
context.SetInputPanelLayout(InputMethod::PanelLayout::NUMBER);
context.SetInputPanelReturnKeyType(InputMethod::ReturnKeyType::DONE);
context.SetTextPredictionEnabled(false);
~~~

The input panel state and area can be queried when needed.

~~~cpp
InputMethodContext::State state = context.GetInputPanelState();
BoundsInteger area = context.GetInputPanelArea();
~~~

Signals can be connected when the input panel state or area needs to be observed.

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

See also: [text-input-method-context-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/text/text-input-method-context-example.cpp)

<br/>

## Text Style

Common style APIs:

| API | Description |
|---|---|
| `SetTextColor()` | Text color |
| `SetPlaceholderColor()` | Placeholder color |
| `SetFontFamily()` | Font family |
| `SetFontSize()` | Font size |
| `SetFontWeight()` | Font weight |
| `SetFontWidth()` | Font width |
| `SetFontSlant()` | Font slant |
| `SetTextBackgroundColor()` | Text background color |
| `SetUnderline()` | Underline style |
| `SetShadow()` | Shadow style |
| `SetOutline()` | Outline style |
| `SetLineThrough()` | Line-through style |
| `SetFontVariation()` | Variable font axis |

~~~cpp
InputField field = InputField::New();
field.SetTextColor(UiColor::PRIMARY);
field.SetPlaceholderColor(UiColor::OUTLINE);
field.SetFontSize(22.0f);
~~~

<br/>

## Cursor and Selection

Common cursor and selection APIs:

| API | Description |
|---|---|
| `SetCursorWidth()` | Cursor width in pixels |
| `SetCursorColor()` | Cursor color |
| `SetCursorBlinkEnabled()` | Enable cursor blinking |
| `SetCursorBlinkInterval()` | Cursor blink interval in seconds |
| `SetCursorPosition()` | Cursor position as a character index |
| `SetSelectionEnabled()` | Enable text selection |
| `SetSelectionColor()` | Selection highlight color |
| `SetTextHandleEnabled()` | Enable text handles |
| `SetTextHandleColor()` | Text handle color |
| `SelectText(start, end)` | Select a range |
| `SelectWholeText()` | Select all text |
| `ClearSelection()` | Clear selection |
| `GetSelectedText()` | Get selected text |
| `GetSelectedTextStart()` | Selection start index |
| `GetSelectedTextEnd()` | Selection end index |

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

`InputFilter` is a filter applied to inserted input. It uses the `std::regex` ECMAScript grammar.

Main rules:
- **allow pattern**: If set, input that does not match the pattern is rejected.
- **deny pattern**: If set, input that matches the pattern is rejected.
- **both set**: Input must satisfy the allow pattern and must not match the deny pattern.

> [!NOTE]
> InputFilter is applied to user input, IME commit, and paste. It is not applied to text set directly with `SetText()`.

~~~cpp
Text::InputFilter inputFilter;
inputFilter.SetAllowPattern("[\\d]");
inputFilter.SetDenyPattern("[0-5]");

InputField field = InputField::New();
field.SetInputFilter(inputFilter);
~~~

The example above allows only digits, but rejects 0 to 5, so only 6 to 9 can be entered.

The rejection reason can be checked from `InputRejectedSignal`:

~~~cpp
field.InputRejectedSignal().Connect(
  [](View view, Text::InputFilter::RejectReason reason)
  {
    // reason: NOT_ALLOWED or DENIED
  });
~~~

Clear the filter:

~~~cpp
field.ClearInputFilter();
~~~

<br/>

## Password Mode

This feature is only for `InputField`.

| Mode | Description |
|---|---|
| `PasswordMode::NONE` | No password masking |
| `PasswordMode::HIDE_ALL` | Show all characters as the mask character |
| `PasswordMode::REVEAL_LAST_CHARACTER` | Show the last entered character for a certain duration, then mask it |

~~~cpp
InputField password = InputField::New();
password.SetPasswordMode(Text::PasswordMode::REVEAL_LAST_CHARACTER);
password.SetPasswordMaskCharacter(0x2022);   // '•' (U+2022)
password.SetPasswordRevealDuration(1000);    // 1000ms
~~~

> [!NOTE]
> The argument of `SetPasswordMaskCharacter()` is a Unicode code point (`uint32_t`). Common values are `0x2A` (`*`), `0x2022` (`•`), and `0x25CF` (`●`).

<br/>

## Editable

Use `SetEditable()` to control whether the text can be edited.

~~~cpp
field.SetEditable(false);  // read-only
~~~

<br/>

## Typing Style

Typing style is the input style applied at the current cursor position or selection.

| API | Description |
|---|---|
| `SetTypingTextColor()` | Input text color |
| `SetTypingFontFamily()` | Input font family |
| `SetTypingFontSize()` | Input font size |
| `SetTypingFontWeight()` | Input font weight |
| `SetTypingFontWidth()` | Input font width |
| `SetTypingFontSlant()` | Input font slant |

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetTypingTextColor(UiColor::PRIMARY);
editor.SetTypingFontSize(24.0f);
~~~

If there is a selection, the style is applied to the selected range. If the selection is collapsed, it is applied to subsequent input.

> [!NOTE]
> `TypingStyleChangedSignal` is not a signal that simply mirrors every `SetTyping*()` call. It is emitted when the resolved typing style changes based on the current cursor or selection. For example, it may also be emitted after cursor movement or selection changes.

See also: [text-typing-style-example.cpp](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text/text-typing-style-example.cpp)

<br/>

## InputEditor-only Features

### Multi-line / Line Wrap

InputEditor supports multi-line input by default.

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetLineWrapMode(Text::LineWrapMode::WORD);
editor.SetLineHeight(1.5f);
editor.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
~~~

### Auto Grow

Auto grow invalidates the measured size for a `WRAP_CONTENT` dimension when the text changes.

~~~cpp
InputEditor editor = InputEditor::New();
editor.SetAutoGrowEnabled(true);
editor.SetRequestedHeight(WRAP_CONTENT);
~~~

> [!NOTE]
> Fixed size or `MATCH_PARENT` dimensions keep the normal layout behavior. Auto grow only affects `WRAP_CONTENT`.

### Line Count

~~~cpp
int lineCount = editor.GetLineCount();        // based on the current layout width
int lineCount2 = editor.GetLineCount(300.0f); // based on the specified width
~~~

> [!NOTE]
> If the width has not been resolved by layout yet, the line count may not be accurate.

<br/>

## Signals

Common signals for `InputField` and `InputEditor`.

| Signal | Signature | Description |
|---|---|---|
| `TextChangedSignal()` | `void(View)` | When text changes |
| `MaximumLengthReachedSignal()` | `void(View)` | When the maximum length is reached |
| `InputRejectedSignal()` | `void(View, InputFilter::RejectReason)` | When input is rejected by the input filter |
| `CursorPositionChangedSignal()` | `void(View, uint32_t)` | When the cursor position changes |
| `SelectionStartedSignal()` | `void(View)` | When selection starts |
| `SelectionChangedSignal()` | `void(View, uint32_t, uint32_t)` | When the selection range changes |
| `SelectionClearedSignal()` | `void(View)` | When selection is cleared |
| `TypingStyleChangedSignal()` | `void(View, TypingStyle::Mask)` | When the resolved typing style changes |

~~~cpp
field.TextChangedSignal().Connect(
  [](View view)
  {
    // text changed
  });
~~~

<br/>

## Samples

| Feature | Sample |
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
