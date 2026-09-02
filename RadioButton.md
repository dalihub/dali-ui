[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/RadioButton-(kr).md)

# DALi UI Components - RadioButton

`RadioButton` is an icon-only, single-selection control. Selecting one member of a group
automatically deselects the previous member. Clicking the selected member again is a no-op;
an application can still clear selection explicitly.

`RadioButton` inherits its selection, signal, click, focus, and grouping APIs from
`GroupSelectableView`.

## Sample

![RadioButton sample showing unselected and selected states, grouping, animation modes, a non-square icon, and a labelled row](./assets/radio-button/radiobutton.gif)

The sample demonstrates the normal outer-ring-only unselected state, selected center fill,
parent-auto and named-group arbitration, selection animation modes, a 48 x 32 custom icon,
and a labelled radio row. The green status text reports the most recent selection.

---

## Core behavior

| Action | Result |
|---|---|
| Click an unselected radio | Selects it and deselects the previous member of its group |
| Click the selected radio again | No change |
| `SetSelected(true)` | Selects the radio programmatically |
| `SetSelected(false)` | Deselects the radio programmatically |
| `SelectionGroup::ClearSelection()` | Leaves the group with no selected member |

A pointer or key gesture never empties a group. Use a programmatic API when an empty group is
part of the application flow.

---

## Setup and basic use

Applications using `dali-ui-components` must apply `Components::UiConfig` once before
`MainLoop()`. Do not apply foundation `UiConfig` separately.

```cpp
#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

using namespace Dali;
using namespace Dali::Ui;

int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  Components::UiConfig::New().Apply();

  MyController controller(application);
  application.MainLoop();
  return 0;
}
```

Create an icon-only radio and provide an accessibility name:

```cpp
RadioButton radio = RadioButton::New();
radio.SetRequestedWidth(52.0f);
radio.SetRequestedHeight(52.0f);
radio.SetAccessibilityName("Wi-Fi");
parent.Add(radio);
```

The default style uses a 36 x 36 icon, 8 logical pixels of padding on each side,
`UiColor::OUTLINE` when deselected, `UiColor::PRIMARY` when selected, and a round state
effect. Colors resolve through the current theme.

---

## Grouping

### Parent auto-group

RadioButtons with no group name automatically share the group of their direct `View` parent
while on-scene.

```cpp
StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);

RadioButton compact = RadioButton::New();
RadioButton normal  = RadioButton::New();
RadioButton large   = RadioButton::New();

row.Add(compact);
row.Add(normal);
row.Add(large);
root.Add(row);

SelectionGroup sizeGroup = SelectionGroup::Find(row);
```

Only direct children participate in this parent group. If each radio is placed in a separate
wrapper, the wrappers are different parents and the radios are not automatically grouped.
Use a named group for that layout.

### Named group across different parents

```cpp
RadioButton cardOption = RadioButton::New();
RadioButton cashOption = RadioButton::New();

cardOption.SetGroupName("payment-method");
cashOption.SetGroupName("payment-method");

cardWrapper.Add(cardOption);
cashWrapper.Add(cashOption);

SelectionGroup paymentGroup = SelectionGroup::Find("payment-method");
```

A non-empty group name takes precedence over parent auto-grouping. Clear the name with
`SetGroupName("")` to return to parent grouping when the radio is on-scene under a `View`.

### Observe and clear a group

```cpp
void SettingsPage::OnSelectedMemberChanged(View previous,
                                           View current,
                                           InputEvent event)
{
  if(current)
  {
    // current is the newly selected member.
  }
}

paymentGroup.SelectedMemberChangedSignal().Connect(
  this, &SettingsPage::OnSelectedMemberChanged);

View selected = paymentGroup.GetSelectedMember(); // Empty when no member is selected.
paymentGroup.ClearSelection();
```

`GetGroup()` is also available on a radio, but a parent-auto group is bound only after the
member is on-scene. `SelectionGroup::Find(parent)` is convenient when the parent is already
known.

---

## Selection state and signals

```cpp
radio.SetSelected(true);
bool selected = radio.IsSelected();

void SettingsPage::OnRadioSelectionChanged(View view,
                                           bool selected,
                                           InputEvent event)
{
  if(selected)
  {
    // Apply the option represented by view.
  }
}

radio.SelectionChangedSignal().Connect(
  this, &SettingsPage::OnRadioSelectionChanged);
```

`InputEvent` identifies the originating input. API-driven state changes use a programmatic
event. Selecting a member can emit a deselection for the previous member before the new
member's selected notification, so handlers should use the supplied `selected` value rather
than assume every callback means selection.

---

## Selection animation

```cpp
radio.SetSelectionAnimationMode(SelectionAnimationMode::AUTO);
```

| Mode | Behavior |
|---|---|
| `AUTO` | Animates user-initiated changes; programmatic changes snap |
| `ENABLED` | Animates both user and programmatic changes |
| `DISABLED` | Always snaps to the target state |

An off-scene or invisible radio always snaps, regardless of the requested mode. This avoids
starting transitions that cannot be observed and ensures the first visible frame matches the
logical selection state.

---

## Styling

Use the current themed style as the base, change only the required values, and pass the built
style at construction time:

```cpp
RadioButtonStyle style = RadioButtonStyle::Default()
                           .Configure()
                           .SetIconWidth(48.0f)
                           .SetIconHeight(32.0f)
                           .SetPadding(Insets(6, 6, 4, 4))
                           .SetIconColor(UiColor::OUTLINE)
                           .SetSelectedIconColor(UiColor::PRIMARY)
                           .Build();

RadioButton radio = RadioButton::New(style);
```

Icon width and height are independent. `SetIconWidth()` and `SetIconHeight()` are also
available on the `RadioButton` for runtime changes. A non-positive or non-finite value is
normalized to zero, meaning that dimension is not explicitly requested:

- An unset width follows the effective icon height.
- An unset height follows a definite content height, or the style minimum height when
  wrapping content.

Use `RadioButtonStyle::DefaultPreset()` only when the built-in values are required regardless
of a theme override. Use `Default()` for normal application UI.

---

## Custom Lottie icon

An icon generator is invoked once for every `RadioButton` created from the style. It must
return an initialized `SelectableImageInterface`, and `GetView()` on that handle must also
return an initialized `View`. Create a new selectable image and drawing View on every
invocation; sharing one live image or View between controls is not supported.

```cpp
#include <dali-ui-foundation/public-api/types/selectable-lottie-color-binding.h>
#include <dali-ui-foundation/public-api/types/selectable-lottie-image.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>

SelectableImageInterface MakePaymentRadioIcon()
{
  using Binding     = SelectableLottieColorBinding;
  using ColorPolicy = Binding::ColorPolicy;
  using FrameRange  = SelectableLottieImage::FrameRange;

  SelectableLottieColorBindings bindings;
  bindings.PushBack(Binding("payment_radio.inner_fill.color",
                            LottieAnimation::VectorProperty::FILL_COLOR,
                            ColorPolicy::ALWAYS_SELECTED));
  bindings.PushBack(Binding("payment_radio.outline.color",
                            LottieAnimation::VectorProperty::STROKE_COLOR,
                            ColorPolicy::SELECTED_IN_FRAME_RANGE,
                            FrameRange(7, 26)));

  SelectableLottieImage image("/opt/usr/share/my-app/payment-radio.json",
                              FrameRange(0, 19),
                              FrameRange(20, 37));
  return SelectableLottieAnimationView::New(image, bindings);
}

RadioButtonStyle customStyle = RadioButtonStyle::Default()
                                 .Configure()
                                 .SetIconGenerator(
                                   RadioButtonStyle::IconGenerator::New(
                                     &MakePaymentRadioIcon))
                                 .Build();
```

Frame ranges, key paths, and color policies are properties of the Lottie asset; do not copy
the example values to an unrelated asset. A stateless free function is the preferred
generator target because `Ui::Callback` is move-only and style copies share the generator.

---

## Radio with a text label

`RadioButton` is intentionally icon-only. For a fully clickable icon-and-label row, make an
outer `GroupSelectableView` the selection, grouping, focus, and accessibility owner. Treat
the child `RadioButton` only as a visual indicator.

```cpp
GroupSelectableView SettingsPage::MakeRadioRow(const Dali::String& text)
{
  GroupSelectableView row = GroupSelectableView::New();
  row.AttachLayoutManager(
    Dali::MakeUnique<StackLayoutManager>(StackOrientation::HORIZONTAL, 8.0f));
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(56.0f);
  row.SetAccessibilityRole(Accessibility::Role::RADIO_BUTTON);
  row.SetGroupName("settings-options");
  row.SetAccessibilityName(text);

  RadioButton indicator = RadioButton::New();
  indicator.SetRequestedWidth(52.0f);
  indicator.SetRequestedHeight(52.0f);
  indicator.SetClickable(false);
  indicator.SetSensitive(false);
  indicator.SetFocusable(false);
  indicator.SetAccessibilityHidden(true);
  indicator.SetSelectionAnimationMode(SelectionAnimationMode::ENABLED);

  Label label = Label::New(text);
  label.SetAccessibilityHidden(true);
  label.SetLayoutParams(
    StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

  row.Add(indicator);
  row.Add(label);
  row.SelectionChangedSignal().Connect(
    this, [indicator](View, bool selected, InputEvent) mutable
    {
      indicator.SetSelected(selected);
    });
  return row;
}
```

`SetClickable(false)` only disables the child's click action; it does not remove the child
from hit testing. `SetSensitive(false)` is what allows a pointer over the indicator to target
the outer row. Hiding both children from accessibility ensures that the row is exposed as one
radio option rather than duplicate nodes.

---

## Accessibility

- Give every icon-only radio a meaningful `SetAccessibilityName()`.
- `RadioButton` supplies the `RADIO_BUTTON` role and checked state.
- For a labelled row, expose the outer row as `RADIO_BUTTON`, give it the label text as its
  name, and hide the indicator and label children from accessibility.
- `RadioButton` does not inject a touch-specific default description. Set an explicit
  accessibility description only when the option needs additional, screen-specific context.
- Keep the outer row focusable and enabled so keyboard and screen-reader activation follow
  the same selection path as pointer input.

---

## Troubleshooting

### Radios in separate cards do not exclude each other

Parent auto-grouping applies only to direct children of the same `View`. Assign the same
non-empty group name when wrappers or different parent containers are involved.

### Programmatic changes do not animate

This is expected in `AUTO`. Use `SelectionAnimationMode::ENABLED` when a visible
programmatic transition is required.

---

## Key APIs

| Type | API | Purpose |
|---|---|---|
| `RadioButton` | `New()`, `New(style)` | Creates an icon-only radio |
| `RadioButton` | `SetSelected()`, `IsSelected()` | Controls and reads selection |
| `RadioButton` | `SelectionChangedSignal()` | Observes this member's state |
| `RadioButton` | `SetGroupName()`, `GetGroup()` | Configures or obtains grouping |
| `RadioButton` | `SetSelectionAnimationMode()` | Chooses transition policy |
| `RadioButton` | `SetIconWidth()`, `SetIconHeight()` | Changes requested icon size at runtime |
| `SelectionGroup` | `Find()`, `GetSelectedMember()` | Obtains and queries a group |
| `SelectionGroup` | `ClearSelection()` | Explicitly empties a group |
| `RadioButtonStyle` | `Default().Configure()` | Derives an immutable themed style |

---

## See also

- [RadioButton API reference](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1RadioButton.html)
- [RadioButton sample](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/radio-button)
- [RadioButton manual test](https://github.sec.samsung.net/NUI/dali-ui/blob/devel/manual-tests/dali-ui-components/tc/tc-radio-button-basics.cpp)
- [GroupSelectableView sample](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/group-selectable-view)
- [Configuration](Configuration.md)
- [Components](Components.md)
