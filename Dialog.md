# Dialog

`Dialog` is a three-section container component in `dali-ui-components`.
It provides header, body, and footer slots. `AlertDialog` builds on top of it
for common title/message/action-button dialogs, and `DialogContainer` presents
dialog content above a scrim.

Dialogs are usually shown modally through [Navigator](Navigator.md).

## Reusable styles

For an interactive comparison and modal lifecycle checks, see the registered
[Dialog manual scenario](../manual-tests/dali-ui-components/tc/tc-dialog-basics.md).

All three types accept an immutable typed style: `Dialog::New(DialogStyle)`,
`AlertDialog::New(AlertDialogStyle)`, and `DialogContainer::New(DialogContainerStyle)`.
`New()` uses the matching `Default()`. Each style supports `DefaultKey()`,
`DefaultPreset()`, `Default()`, `Configure()`, and a move-only `Builder`.
`Build()` consumes the builder. Passing an empty style to `New(style)` asserts.

```cpp
auto appearance = DialogStyle::Default().Configure()
  .SetBackgroundColor(UiColor::SURFACE)
  .SetPadding(Insets(24.0f, 16.0f))
  .SetSpacing(12.0f).Build();
auto actions = TextButtonStyle::Default().Configure()
  .SetTextColor(UiColor::PRIMARY).Build();
auto style = AlertDialogStyle::Default().Configure()
  .SetDialogStyle(appearance).SetActionButtonStyle(actions)
  .SetTitleFontSize(24.0f).Build();
auto alert = AlertDialog::New(style);
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");
alert.AddActionButton("Cancel");

auto container = DialogContainer::New(DialogContainerStyle::Default().Configure()
  .SetScrimColor(UiColor(0x000000u, 0.6f)).Build());
container.SetModalContent(alert);
```

Register custom default creators with `config.StyleSheet().SetStyle(TypeStyle::DefaultKey(), creator)`
before applying the Components config. A creator must use `DefaultPreset()` or a
builder, not recursively call its own `Default()`.

`DialogStyle` owns the dialog's requested size, background, overall padding,
section spacing, corners, shadows, borders, and blur. The supplied header, body,
and footer Views own their padding and appearance. Configure each View directly
before attaching it; Dialog does not override those values when attaching,
replacing, or reordering sections. Section layout and alignment remain managed
by Dialog.

```cpp
auto header = Label::New("Custom header");
header.SetPadding(Insets(8.0f, 4.0f));
dialog.SetHeaderView(header);
```

`AlertDialogStyle` composes a `DialogStyle` and `TextButtonStyle`, and adds title/message
font/color and action-row height/spacing. Empty nested styles select the documented
defaults at creation. An explicit action style is used without Alert's legacy color/font
overrides. The implicit action style retains configured TextButton geometry/effects
and the existing Alert color/font defaults. Existing generated labels retain their
identity and custom font/color on text updates; clearing text removes that section.

`DialogContainerStyle` affects only its generated scrim. Replacing the scrim preserves
the supplied view's color/effect. The default scrim remains clickable but does not take
keyboard or touch focus.

Both blur-radius setters accept finite values in `[0, 4096]`; zero disables blur,
and fractional parts are truncated when creating the effect. Larger values assert
to prevent unsafe conversion and excessive renderer-side radius reduction.

`Dialog` and `AlertDialog` expose the accessibility Dialog role. `DialogContainer`
temporarily marks its content modal, preserves its role, and restores the prior
explicit modal flag on removal, transfer, or destruction. Reinstalling the same
handle is a no-op. This is distinct from the implicit modal state of a Dialog role.
Showing notifications follow actual accessible visibility, including scene removal,
ancestor visibility, world alpha, and culling. Applications still own focus placement,
focus restoration, accessible names, dismissal callbacks, and keyboard policy.
State UTCs are not evidence of actual screen-reader speech or D-Bus delivery.

These APIs preserve the existing default appearance; they do not silently apply OneUI
pixel values. `UiColor` tokens remain tokens. Styles apply at construction, not as a
live replacement of subsequent instance overrides. See the [buildable sample](../samples/dialog/README.md).

---

## Basic Setup

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

Application application = Application::New(&argc, &argv);
Components::UiConfig::New().Apply();
```

---

## Dialog

`Dialog` has three optional sections:

| Section | API | Typical content |
|---|---|---|
| Header | `SetHeaderView()` | Title, icon, or heading content |
| Body | `SetBodyView()` | Message, form, list, or custom content |
| Footer | `SetFooterView()` | Action buttons or status content |

```cpp
Dialog dialog = Dialog::New();
dialog.SetBackgroundColor(UiColor(0xFFFFFFu));
dialog.SetSpacing(12.0f);
dialog.SetLayoutAlignment(LayoutAlignment::FILL);

Label title = Label::New("Network settings");
Label message = Label::New("Choose how this device connects to the network.");

dialog.SetHeaderView(title);
dialog.SetBodyView(message);
```

Section and layout APIs:

```cpp
dialog.SetHeaderView(headerView);
dialog.SetBodyView(bodyView);
dialog.SetFooterView(footerView);

View header = dialog.GetHeaderView();
View body   = dialog.GetBodyView();
View footer = dialog.GetFooterView();

dialog.SetSpacing(8.0f);
dialog.SetLayoutAlignment(LayoutAlignment::CENTER);
```

Passing an empty handle clears a section.

---

## AlertDialog

`AlertDialog` is a convenience dialog for title, message, and action buttons.

```cpp
AlertDialog alert = AlertDialog::New();
alert.SetBackgroundColor(UiColor(0xFFFFFFu));
alert.SetSpacing(8.0f);

alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");
TextButton cancel = alert.AddActionButton("Cancel");
cancel.ClickedSignal().Connect(this, [](View, InputEvent) {
  // Handle cancel.
});
TextButton remove = alert.AddActionButton("Delete");
remove.ClickedSignal().Connect(this, [](View, InputEvent) {
  // Handle delete.
});
```

| API | Description |
|---|---|
| `SetTitle(text)` / `GetTitle()` | Sets or reads the header title. |
| `SetMessage(text)` / `GetMessage()` | Sets or reads the body message. |
| `AddActionButton(text)` | Adds an evenly distributed footer button and returns it for signal connection or customization. |
| `ClearActionButtons()` | Removes all action buttons and clears the footer. |

---

## DialogContainer

`DialogContainer` wraps modal content with a scrim. The scrim fills the
container and can be tapped for dismiss behavior.

```cpp
DialogContainer container = DialogContainer::New();
container.SetModalContent(alert);
```

Scrim APIs:

```cpp
View scrim = View::New();
scrim.SetBackgroundColor(UiColor(0x00000080u));

container.SetScrim(scrim);
View currentScrim = container.GetScrim();
```

Scrim click signal:

```cpp
container.ScrimClickedSignal().Connect(
  this,
  [](DialogContainer container) {
    // Dismiss the container here.
  });
```

When a `DialogContainer` is pushed through `Navigator::PushModal()`,
`Navigator` can dismiss it when the scrim is tapped.

---

## Presenting with Navigator

Use `Navigator::PushModal()` to show a dialog as modal content.

```cpp
AlertDialog alert = AlertDialog::New();
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");

DialogContainer container = DialogContainer::New();
container.SetModalContent(alert);

navigator.PushModal(container);
```

Dismiss it from an action button:

```cpp
TextButton cancel = alert.AddActionButton("Cancel");
cancel.ClickedSignal().Connect(this, [navigator](View, InputEvent) mutable {
  navigator.PopModal();
});

TextButton remove = alert.AddActionButton("Delete");
remove.ClickedSignal().Connect(this, [navigator](View, InputEvent) mutable {
  navigator.PopModal();
});
```

`Navigator::NavigateBack()` also dismisses the top modal item before popping a
normal page.

---

## Layout and Sizing

Dialog content follows its own layout parameters inside `DialogContainer`.
To center a dialog over the scrim, use `AbsoluteLayoutParams` with proportional
positioning.

```cpp
alert.SetLayoutParams(AbsoluteLayoutParams::New()
                        .SetBounds(LayoutRect(0.5f, 0.5f, 600.0f, 340.0f))
                        .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
```

---

## Modal Transition

Dialog transition behavior is controlled by `Navigator`, because dialogs are
usually presented through `PushModal()` and `PopModal()`.

```cpp
navigator.SetModalTransitionAnimationEnabled(false);
navigator.PushModal(container, true);  // Finishes immediately while disabled.
```

Use a modal transition spec when dialogs need a different transition from
normal page navigation:

```cpp
NavigationTransitionSpec modalSpec = NavigationTransitionSpec::New();
modalSpec.SetDuration(0.25f);
modalSpec.EnterSignal().Connect(this, [](Animation& anim, View view) {
  view.SetProperty(Actor::Property::OPACITY, 0.0f);
  anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
});

navigator.SetModalTransitionSpec(modalSpec);
navigator.SetPageModalTransitionSpec(container, modalSpec);
```

See [Navigator](Navigator.md#transition-specification) for the full transition API.

---

## Notes

- Use `Dialog` for custom header, body, and footer layouts.
- Use `AlertDialog` for common title/message/action dialogs.
- Use `DialogContainer` when the dialog should be shown over a scrim.
- Use `Navigator::PushModal()` and `Navigator::PopModal()` for modal dialog
  presentation and modal transition behavior.

<br/>

---

[Back to Components](Components.md)
