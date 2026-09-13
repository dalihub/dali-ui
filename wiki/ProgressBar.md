[한국어](https://github.sec.samsung.net/NUI/dali-ui/wiki/ProgressBar-%28kr%29) · [Components](https://github.sec.samsung.net/NUI/dali-ui/wiki/Components)

# ProgressBar

`Dali::Ui::ProgressBar` displays horizontal progress with a rounded track and trail (the filled portion). Use determinate mode when the amount completed is known, or indeterminate mode while waiting for an unknown amount of work. It is a non-interactive `ProgressView`, not a slider; the application supplies progress updates.

## Setup and first bar

Apply `Dali::Ui::Components::UiConfig` once during application initialization, before creating components or resolving default styles. It includes foundation configuration; do not apply a second foundation `UiConfig`. Include and link the components library in your existing DALi UI application.

The following content-building function is intended to be called after application initialization has started. Add its returned View to your application's content container. If your application already applies the configuration, omit the `Apply()` line.

```cpp
#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

using namespace Dali;
using namespace Dali::Ui;

View CreateProgressContent()
{
  Components::UiConfig::New().Apply();

  StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
  root.SetRequestedWidth(MATCH_PARENT);
  root.SetRequestedHeight(WRAP_CONTENT);
  root.SetSpacing(12.0f);

  ProgressBar basic = ProgressBar::New();
  basic.SetValue(0.25f); // Default range is 0..1.
  basic.SetAccessibilityName("Download progress");
  root.Add(basic);

  ProgressBarStyle style = ProgressBarStyle::Default().Configure()
    .SetPadding(Insets(8.0f, 8.0f, 12.0f, 12.0f))
    .SetTrackThickness(14.0f)
    .SetTrackColor(UiColor(0xD5D9E0u))
    .SetTrailColor(UiColor(0x6750A4u))
    .SetDividerColor(UiColor(0xFFFFFFu, 0.8f))
    .Build();

  ProgressBar custom = ProgressBar::New(0.0f, 100.0f, style);
  custom.SetDividerStepCount(4); // Three visual dividers; no value snapping.
  custom.SetValue(50.0f);
  custom.SetAccessibilityName("Installation progress");
  root.Add(custom);
  return root;
}
```

`ProgressBar()` alone creates an empty handle. Use `New()`, `New(style)`, `New(minimum, maximum)`, or `New(minimum, maximum, style)`. Explicit styles must be initialized. A new bar starts at its range minimum.

## Immutable styles and defaults

`ProgressBarStyle::Default()` resolves the configured `DefaultKey()` entry, falling back to `DefaultPreset()`. Both require applied configuration. `DefaultPreset()` is the cached built-in style; `Builder()` starts from built-in values, not a configured override.

`Configure()` copies a style into a mutable, move-only builder. Its five setters configure padding, thickness, track color, trail color, and divider color. `Build()` consumes the builder once and returns an immutable style. For a named builder, use `std::move(builder).Build()` with `<utility>`. Neither a derived style nor a bar's runtime setters mutate the original style.

| Setting | Built-in/default value |
| --- | --- |
| Range / initial value | `0..1` / `0`; explicit ranges start at their minimum |
| Determinate / reversed | `true` / `false` |
| Value / divider step count | `0` / `0` |
| Requested width / height | `MATCH_PARENT` / `WRAP_CONTENT` |
| Style padding (`start, end, top, bottom`) | `Insets(0, 0, 10, 10)` |
| Track thickness | `10` logical units |
| Track / trail / divider color | `UiColor(0x1C1C1Eu, 0.1f)` / `UiColor::PRIMARY` / `UiColor(0xFFFFFFu, 0.6f)` |

With the built-in style, natural height is 30 logical units before effective scaling. Natural width only includes horizontal padding: provide a bounded parent width or an explicit requested width. The track is vertically centered inside the padded content and its thickness is limited by available height.

For an application-wide default, register a style creator with `config.StyleSheet().SetStyle(ProgressBarStyle::DefaultKey(), creator)` **before** `config.Apply()`. See the [configuration API](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/components-ui-config.h) and the default-key override test linked below.

## Runtime API groups

| Responsibility | API |
| --- | --- |
| Value and range, inherited from `ProgressView` | `SetValue()` / `GetValue()`, `SetRange()` / `GetRange(minimum, maximum)` |
| Value snapping | `SetValueStepCount()` / `GetValueStepCount()` |
| Mode | `SetDeterminate()` / `IsDeterminate()` |
| Appearance | `SetTrackThickness()` / `GetTrackThickness()`, `SetTrackColor()` / `GetTrackColor()`, `SetTrailColor()` / `GetTrailColor()` |
| Visual segmentation and direction | `SetDividerStepCount()` / `GetDividerStepCount()`, `SetReversed()` / `IsReversed()`, inherited `SetLayoutDirection()` |
| Overlay geometry | `GetTrailEndPosition()`, `GetDividerCount()`, `GetDividerPosition(index)` |

Style padding and divider color initialize the bar at construction. There is no ProgressBar-specific runtime divider-color setter; the inherited `View::SetPadding()` remains available for layout padding.

Values and range endpoints must be finite, with `maximum > minimum`. Finite values outside the range are clamped. Value step count `0` means continuous values; positive `N` gives `N` equal intervals and `N + 1` valid values. Nearest-step snapping rounds a midpoint toward the higher step. Changing the range or step count re-coerces the current value.

Divider step count is independent: `0` or `1` produces no dividers, and `N > 1` produces `N - 1`. Both step counts must be non-negative. Thickness must be finite and positive; style insets must be finite and non-negative. Invalid inputs and out-of-range divider indices trigger DALi assertions, not an error return.

## Modes and direction

Call `bar.SetDeterminate(false)` for indeterminate progress and `bar.SetDeterminate(true)` to restore the current value's fill. Range, value, and snapping still work while indeterminate. The determinate trail and built-in dividers are hidden; four animated bars run inside the rounded track when it is on scene with nonzero track dimensions. Animation is cleared off scene and restored on reconnection as needed.

Reversal changes determinate fill origin, not divider indexing. Effective RTL and reversal combine as follows:

| Layout direction | `reversed = false` | `reversed = true` | Divider index order |
| --- | --- | --- | --- |
| LTR | Left to right | Right to left | Left to right |
| RTL | Right to left | Left to right | Right to left |

Do not treat `SetReversed()` as an indeterminate-animation direction API.

## Signals, tooltips, and divider labels

`ValueChangedSignal()` sends `(ProgressView, float)` after committing the effective value and renderer state. `DeterminateChangedSignal()` sends `(ProgressView, bool)` after committing the mode. Repeating an unchanged effective value or mode does not emit its signal; a range-only change need not emit `ValueChangedSignal()`.

`TrailEndPositionChangedSignal()` and `DividersChangedSignal()` send only the `ProgressBar`. Their local snapshots are committed before notification, and changes in an event turn are coalesced. They report changed geometry, not general state changes: mode changes, scene attachment, world movement, colors, or layouts with unchanged local results do not themselves notify geometry listeners.

The component does not create tooltips or divider labels. To synchronize application-owned overlays:

1. Connect geometry callbacks with a `ConnectionTracker` whose lifetime covers the overlay, then immediately read the getters for initial synchronization.
2. Read `GetTrailEndPosition()` to position the tooltip. It is a mathematical endpoint in top-left-origin, ProgressBar-local visual coordinates, including RTL/reversal. At very small nonzero values the rounded trail can extend beyond that endpoint.
3. On every divider signal, re-read `GetDividerCount()` and all positions with `index < count`; rebuild or fully synchronize the label collection.
4. Convert these local coordinates into the overlay parent's coordinates. Parent/world movement requires application-side synchronization if the overlay is elsewhere.
5. Update text from value changes and hide/show your own overlays from mode changes. Geometry signals are not a substitute for `DeterminateChangedSignal()`.

Snapshots remain valid before first arrange, at zero size, off scene, and while indeterminate. In indeterminate mode the endpoint still represents the stored value, not an animated bar's moving edge. See the [complete overlay example](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.cpp).

## Accessibility

The inherited role is `PROGRESS_BAR`; internal track, trail, divider, and animation Views are accessibility-hidden. Give the bar a meaningful `SetAccessibilityName()`. Determinate progress exposes the rounded normalized percentage (for example, value `24.6` in `0..100` becomes `25%`); indeterminate progress has no default value text. A nonempty `SetAccessibilityValue()` overrides either mode, so the application must maintain or clear that custom text.

This is a read-only indicator: increment/decrement actions are unsupported. Default value-change notifications follow determinate ratio changes and mode transitions when no explicit accessibility value is set. This does not guarantee automatic speech or a busy announcement; those depend on the accessibility client and application policy. Application-created overlay labels need their own accessibility policy.

## Sources and verification examples

- Public APIs: [ProgressBar](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/progress-bar.h), [ProgressBarStyle](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/progress-bar-style.h), [ProgressView](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-foundation/public-api/views/progress-view.h).
- Implementation: [bar layout, rendering, and geometry](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/internal/progress-bar-impl.cpp), [style defaults and validation](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-components/public-api/styles/progress-bar-style.cpp), [value coercion and accessibility](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/dali-ui-foundation/extension-api/progress-view-impl.cpp).
- Automated tests: [ProgressBar UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-components/utc-Dali-ProgressBar.cpp) covers styles/default override, geometry, zero-size/off-scene behavior, and mode lifecycle; [ProgressView UTC](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/automated-tests/src/dali-ui-foundation/utc-Dali-ProgressView.cpp) covers clamping, snapping, signals, and accessible values.
- Manual verification: [scenario](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.md) and [sample source](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-progress-bar-basics.cpp) exercise default/custom styles, values, RTL/reversal, overlay synchronization, and repeated mode switching. These are verification instructions, not evidence of a run on your target device.
