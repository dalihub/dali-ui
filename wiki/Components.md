[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components-%28kr%29)

## Components

#### Layouts
`dali-ui-foundation`
name | desc. | sample
-- | -- | --
[AbsoluteLayout](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1AbsoluteLayout.html) | It positions its children at explicit coordinates. | [samples/absolutelayout](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/absolutelayout)
[StackLayout](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1StackLayout.html) | Simple layout container that stacks its children either horizontally or vertically with optional spacing between them. | [samples/stacklayout](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/stacklayout)
[GridLayout](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1GridLayout.html) | Arranges its children in a grid of rows and columns. | [samples/gridlayout](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/gridlayout)
[FlexLayout](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1FlexLayout.html) | Provides a flexible way to lay out children in rows or columns, with support for wrapping, alignment, and flexible sizing. | [samples/flexlayout](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/flexlayout)


<br/>

#### Basics
`dali-ui-foundation`
name | desc. | sample
-- | -- | --
[View](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1View.html) | A basic unit arranged by layouts. It also serves as a visual container, provides various render effects, and offers basic accessibility functionality. | -
[Label](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1Label.html) | A non-editable `View` that displays text. | [samples/text](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text)
[InputField](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1InputField.html) | A single-line editable text. | [samples/text](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text)
[InputEditor](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1InputEditor.html) | A multi-line editable text view. | [samples/text](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/text)
[ImageView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1ImageView.html) | A `View` for displaying an image resource. | [samples/image-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/image-view)
[AnimatedImageView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1AnimatedImageView.html) | A `View` for displaying animated image resources such as GIF files. | -
[LottieAnimationView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1LottieAnimationView.html) | A `View` for displaying a Lottie vector animation. | -
[ScrollView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1ScrollView.html) | A scrollable container that displays content larger than its viewport. | [samples/scrollview](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/scrollview)
[InteractiveView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1InteractiveView.html) | A focusable and clickable `View` base for interactive controls. | -
[SelectableView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1SelectableView.html) | An `InteractiveView` with selected state support. | -
[GroupSelectableView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1GroupSelectableView.html) | A `SelectableView` that participates in single-selection groups. | [samples/group-selectable-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/group-selectable-view)
WidgetView | (~10.2026) | -
[WebView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1WebView.html) | A `View` for displaying web content (HTML, web pages). Supports navigation, JavaScript integration, and various web engine features. | [samples/web-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/web-view)
[CanvasView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1CanvasView.html) | A `View` for displaying ThorVG-backed vector drawables. | [samples/canvas-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/canvas-view)
[VideoView](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1VideoView.html) | A `View` for displaying video from an externally created Tizen MMPlayer/ESPlayer session. | [samples/video-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/video-view)
RecyclerView | (~10.2026) | -

<br/>

#### Components

`dali-ui-components`

Apply `Dali::Ui::Components::UiConfig` before creating components or resolving their default styles. Styled controls support `New()` with the configured default and `New(style)` with an explicit typed style; content-specific overloads are also available. Use `Default().Configure()` to customize an existing style without modifying the original.

See [Component creation, styling, and verification](https://github.sec.samsung.net/NUI/dali-ui/wiki/ComponentConsistency) for configuration, style builders, state APIs, and verification scenarios. Component names below link to component usage guides. Sample links use repository URLs so that they also work from the published Wiki.

name | desc. | sample
-- | -- | --
[ChartView](https://github.sec.samsung.net/NUI/dali-ui/wiki/ChartView) | Line, Bar, Pie, Area, Scatter, Gauge charts using ThorVG rendering. Supports zoom/pan gestures, animated data transitions, and real-time data feeds. | [samples/chart-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/chart-view)
[WindowFrame](https://github.sec.samsung.net/NUI/dali-ui/wiki/WindowFrame) | A client-rendered window frame with custom decoration, content layout, controls, move/resize interaction, overlay, and compositor state tracking. | [samples/window-frame](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/window-frame), [samples/desktop-window](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/desktop-window)
[Navigator](https://github.sec.samsung.net/NUI/dali-ui/wiki/Navigator) | Page-stack navigation container with navigation and modal stacks, back handling, stack queries, and page lifecycle signals. | [samples/navigator](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/navigator)
[Dialog](https://github.sec.samsung.net/NUI/dali-ui/wiki/Dialog#dialog-1) | Composes custom header, body, and footer Views. `DialogStyle` controls the initial size, background, overall padding, section spacing, corners, shadow, border, and blur. Supplied section Views retain their own padding and appearance. | [samples/dialog](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/dialog), [manual scenario](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-dialog-basics.md)
[AlertDialog](https://github.sec.samsung.net/NUI/dali-ui/wiki/Dialog#alertdialog) | Convenience Dialog with a title, message, and action buttons. `AlertDialogStyle` combines Dialog styling, typography, and action-button styling; title/message updates preserve the existing Labels and their custom properties. | [samples/dialog](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/dialog), [public API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/dialog/alert-dialog.h)
[DialogContainer](https://github.sec.samsung.net/NUI/dali-ui/wiki/Dialog#dialogcontainer) | Wraps modal content with a scrim and manages modal ownership and accessibility state restoration. `DialogContainerStyle` styles the generated scrim; a custom scrim keeps its appearance. `ScrimClickedSignal()` lets the application handle dismissal. | [samples/dialog](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/dialog), [public API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/dialog/dialog-container.h)
[TextButton](https://github.sec.samsung.net/NUI/dali-ui/wiki/TextButton) | Text-only interactive button with `TextButtonStyle` for typography, colors, padding, and size constraints. Supports mutable text/font properties, click handling, and disabled-state accessibility updates. | [manual scenario](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-text-button-basics.md), [manual source](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-text-button-basics.cpp)
[IconButton](https://github.sec.samsung.net/NUI/dali-ui/wiki/IconButton) | Icon-only interactive button with `IconButtonStyle`. Uses explicit dimensions first, then a valid image natural size, then a fallback of 56 logical units; remeasures after asynchronous image loading. Applications provide a meaningful accessibility name. | [samples/icon-button](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/icon-button), [sample instructions](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/icon-button/README.md)
[CheckBox](https://github.sec.samsung.net/NUI/dali-ui/wiki/CheckBox) | Checked/unchecked control with an optional trailing label and an animated check glyph. `CheckBoxStyle` configures its presentation; `SetSelected()` and `SelectionChangedSignal()` manage selection, with configurable selection animation. | [samples/check-box](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/check-box), [manual scenario](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-check-box-basics.md)
[Switch](https://github.sec.samsung.net/NUI/dali-ui/wiki/Switch) | Binary selectable control with tap, horizontal drag, focused Enter-key operation, selection animation, and `SwitchStyle` customization. | [samples/switch](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/switch)
[RadioButton](https://github.sec.samsung.net/NUI/dali-ui/wiki/RadioButton) | Icon-only single-selection control with parent-auto and named grouping, selection animation modes, accessibility, RTL, and `RadioButtonStyle` customization. | [samples/radio-button](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/radio-button)
[ProgressBar](https://github.sec.samsung.net/NUI/dali-ui/wiki/ProgressBar) | Horizontal determinate or indeterminate progress indicator. Inherits range/value APIs from `ProgressView`, uses `ProgressBarStyle` for its appearance, and exposes trail-end and divider geometry for application-owned tooltip and label placement. | [manual scenario](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.md), [manual source](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.cpp)
ProgressCicle | (~10.2026) | -
Loading | (~10.2026) | -
Slider | (~10.2026) | -
[Toast](https://github.sec.samsung.net/NUI/dali-ui/wiki/Toast) | Non-modal Window overlay for brief feedback with text and an optional icon. `ToastStyle` controls presentation; `Post(window)`, a finite duration, `Dismiss()`, and shown/hidden signals control its lifetime. Does not provide Snackbar action buttons or an expanded state. | [samples/toast](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/toast), [sample instructions](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/toast/README.md)
[MarkdownView](https://github.sec.samsung.net/NUI/dali-ui/wiki/MarkdownView) | Renders Markdown with `MarkdownViewStyle` typography, colors, and task-icon settings. Supports content updates, plain-text conversion, and task selection; use a bounded width for predictable text wrapping. | [samples/markdown](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/markdown), [sample instructions](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/samples/markdown/README.md)
[DummyComponent](https://github.sec.samsung.net/NUI/dali-ui/wiki/DummyComponent) | Compatibility View shell without its own style or interaction policy. Prefer `View` for a general-purpose container. | [usage notes](https://github.sec.samsung.net/NUI/dali-ui/wiki/ComponentConsistency#dummycomponent-is-not-a-feature-control)

For the Dialog family's shared style creation and fallback rules, see [Reusable styles](https://github.sec.samsung.net/NUI/dali-ui/wiki/Dialog#reusable-styles). `Dialog`, `AlertDialog`, and `DialogContainer` have separate responsibilities: content composition, standard alert content, and modal presentation respectively.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
