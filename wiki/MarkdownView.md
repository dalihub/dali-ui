[English](https://github.sec.samsung.net/NUI/dali-ui/wiki/MarkdownView) | [한국어](https://github.sec.samsung.net/NUI/dali-ui/wiki/MarkdownView-%28kr%29) | [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)

# MarkdownView

`Dali::Ui::MarkdownView` displays Markdown as native UI views, including headings, lists, quotes, code blocks, and tables. Use it for formatted help, messages, or documents with vertically growing content. It is a `View`, not a text editor or a browser; task-list checkboxes are interactive, but links and images currently use a text-only presentation.

## Setup and a complete example

Include the components and foundation headers and link your application against `dali-ui-components` and its foundation dependencies. Apply `Components::UiConfig` once before `MainLoop()`; it already includes foundation configuration, so do not apply a separate foundation `UiConfig`.

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

The [configuration API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/components-ui-config.h) defines the components setup contract. See [ScrollView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ScrollView) for scrolling behavior.

## Content, updates, and task lists

| API | Behavior |
|---|---|
| `MarkdownView::New()` | Creates a view using the configured default style. |
| `MarkdownView::New(style)` | Creates a view using a `MarkdownViewStyle`. |
| `SetMarkdown(source)` | Replaces the complete Markdown source. |
| `GetMarkdown()` | Returns the current source, including checkbox changes. |
| `Clear()` | Removes the document content and clears its source. |
| `MarkdownView::ToPlainText(source)` | Converts source to `Dali::String` without creating a view. |
| `MarkdownView::DownCast(handle)` | Obtains a MarkdownView handle from a compatible `BaseHandle`. |

For streaming output, accumulate the received UTF-8 text and pass the complete accumulated source to `SetMarkdown()` on each update. There is no append API. Identical source is a no-op; changed source is parsed again and existing rendered components are reconciled where possible. Batch updates appropriately for your document size; this is not a virtualized document API.

Task syntax supports `- [ ] todo`, `- [x] done`, and `- [X] done`, including ordered and nested lists. Activating a task checkbox updates its source marker to `x` or a space, so read `GetMarkdown()` when saving user changes. Replacing the source with an older copy also replaces those changes.

MarkdownView declares **no dedicated signals**: there is no `MarkdownChangedSignal()`, task-change signal, or link-activation signal on this class. Internally, task controls use `CheckBox::SelectionChangedSignal()`. Rendered children are implementation details, not a stable task-control API; do not depend on their indices for application logic.

This helper can be added alongside the complete example:

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

`ToPlainText()` removes emphasis and heading markers, omits unordered bullets, retains ordered numbering and task markers, and appends link/image URLs in brackets. Tables become newline-separated cell text; code blocks retain code and their language label. It is not a Markdown round-trip serializer. Unlike display parsing, conversion excludes HTML markup: inline HTML text can remain, while a raw HTML block is omitted.

## Immutable styling

Build a style before creating the view. `Configure()` copies an existing style into a builder; `Build()` produces a new style without changing the original. The builder is move-only and `Build()` is rvalue-qualified, so use a chained temporary as below or `std::move(builder).Build()` for a named builder.

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

Call this helper after configuration has been applied. Style options cover body/heading/code font families, six heading sizes, code/title sizes, text and decoration colors, and task icon generation/colors. Font sizes must be finite and greater than zero. `UiColor` theme tokens retain their identity and resolve through the theme; literal colors in the example are fixed choices.

There is no public `MarkdownView::SetStyle()`. To change typography/style settings after creation, create a replacement view with the new style and transfer its source. Application-wide defaults can be registered through `config.StyleSheet().SetStyle(MarkdownViewStyle::DefaultKey(), factory)` before `config.Apply()`; `DefaultPreset()` is the built-in style, while `Default()` resolves the configured default.

## Width, layout, and supported syntax

Prefer fixed or `MATCH_PARENT` width with `WRAP_CONTENT` height. A `WRAP_CONTENT` width consumes the full bounded measurement width; with an unbounded width it remains content-based. `SetMaximumWidth()` or a bounded parent makes wrapping predictable. Fixed widths still obey size constraints. MarkdownView does not scroll itself: use a vertical ScrollView as above. Code blocks explicitly use left-to-right layout even in an RTL document.

| Syntax / feature | Current behavior |
|---|---|
| `#` through `######`, Setext headings | Heading typography; Setext uses `===` or `---` underlines. |
| Paragraphs, `*emphasis*`, `**strong**`, `~~strike~~`, inline backticks | Styled text; escapes and HTML character entities are parsed. |
| Ordered/unordered lists, nesting, `>` quotes, thematic breaks | Native layouts and decorations; task items add checkboxes. |
| Fenced / indented code | Code presentation; a fence language label is displayed, not syntax highlighting. |
| Pipe tables with delimiter/alignment rows | Native table layout with left/center/right cell alignment. |
| `[text](url)` and `<https://example.com>` | Display `text [url]`; not interactive hyperlinks. Bare-URL autolink extensions are not enabled. |
| `![alt](url)` | Display `alt [url]` for both inline and standalone images; no image loading in the current presentation. |
| Raw HTML | HTML rendering is disabled; tags are treated as text in the display parser. |

The parser enables tables, strikethrough, and task lists; do not assume all GitHub Markdown extensions are supported. Math rendering, diagram rendering, and executable code blocks are not provided. There is no public parser-options or image/link-presentation switch.

## Accessibility and references

Task controls are real CheckBoxes with the `CHECK_BOX` accessibility role. Their accessible name is populated from the task text, and the adjacent task text view is hidden from accessibility to avoid duplicate content. The implementation does not assign a Markdown-specific document role or heading/table semantic roles; do not assume browser-like document navigation or clickable-link announcements. Validate reading order and task interaction on your target accessibility stack. See [Accessibility](https://github.sec.samsung.net/NUI/dali-ui/wiki/Accessibility).

- [Public MarkdownView API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/markdown/markdown-view.h) and [style API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/markdown-view-style.h).
- [Parser and rendering implementation](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/dali-ui-components/internal/markdown).
- [Markdown sample](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/markdown): document cases, streaming chunks, direction, scaling, and plain-text output. Follow the components configuration above when adapting it.
- [UTC coverage](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-MarkdownView.cpp): width constraints, style configuration, task-source updates, text-only URLs/images, reconciliation, and plain-text conversion.

[Back to Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)
