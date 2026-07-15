# Navigator

`Navigator` is a page-stack navigation container in `dali-ui-components`.
It manages two stacks:

- **Navigation stack** for normal application pages.
- **Modal stack** for temporary content shown above the current page, such as a `DialogContainer`.

Use `Navigator` when an application needs page push/pop, modal presentation,
back handling, stack queries, and page transition customization in one place.

---

## Basic Setup

```cpp
#include <dali-ui-components/dali-ui-components.h>

using namespace Dali;
using namespace Dali::Ui;

Application application = Application::New(&argc, &argv);
Components::UiConfig::New().Apply();
```

```cpp
Navigator navigator = Navigator::New();
navigator.SetLayoutParams(StackLayoutParams::New()
                            .SetWeight(1.0f)
                            .SetAlignment(LayoutAlignment::FILL));
root.Add(navigator);
```

---

## Navigation Stack

Push a page:

```cpp
View page = View::New();
page.SetBackgroundColor(UiColor(0x1565C0u));

Label title = Label::New("Page 1");
title.SetFontSize(28.0f);
title.SetTextColor(UiColor(0xFFFFFFu));
title.SetRequestedX(24.0f);
title.SetRequestedY(24.0f);
page.Add(title);

navigator.Push(page);
```

Pop the top page:

```cpp
View popped = navigator.Pop();
```

Other stack APIs:

| API | Description |
|---|---|
| `InsertBefore(page, before)` | Inserts a page below an existing page. |
| `Remove(page)` | Removes a page or modal item. |
| `Clear()` | Removes all navigation and modal items. |

---

## Modal Stack

Push modal content above the navigation stack:

```cpp
AlertDialog alert = AlertDialog::New();
alert.SetTitle("Delete item?");
alert.SetMessage("This action cannot be undone.");
alert.SetActionButtons({
  {"Cancel", [navigator]() mutable { navigator.PopModal(); }},
  {"Delete", [navigator]() mutable { navigator.PopModal(); }}
});

DialogContainer container = DialogContainer::New();
container.SetModalContent(alert);

navigator.PushModal(container);
```

Dismiss the top modal item:

```cpp
navigator.PopModal();
```

When a `DialogContainer` is pushed as modal content, tapping the scrim can
dismiss it through `Navigator`.

---

## Back Navigation

`NavigateBack()` handles common back behavior:

1. Dismiss the top modal item if one exists.
2. Otherwise pop the navigation stack.
3. Return `false` if there is nothing to go back to.

```cpp
if(!navigator.NavigateBack())
{
  application.Quit();
}
```

A page can intercept back navigation:

```cpp
navigator.SetBackHandler(editPage, []() {
  if(HasUnsavedChanges())
  {
    ShowDiscardChangesPrompt();
    return true;
  }
  return false;
});
```

---

## Queries

| API | Description |
|---|---|
| `GetCurrentView()` | Returns the top modal item, or the top navigation page if no modal exists. |
| `GetNavigationStackCount()` | Returns the number of navigation pages. |
| `GetModalStackCount()` | Returns the number of modal items. |
| `GetNavigationStackItem(index)` | Returns a navigation page by index, where `0` is the bottom item. |
| `GetModalStackItem(index)` | Returns a modal item by index, where `0` is the bottom item. |

---

## Transition Basics

`Push()`, `Pop()`, `PushModal()`, and `PopModal()` take an `animated` parameter.
Passing `false` makes that single operation finish immediately.

```cpp
navigator.Push(page, false);
navigator.PopModal(false);
```

Page and modal transition animation can also be enabled or disabled
independently:

```cpp
navigator.SetPageTransitionAnimationEnabled(false);   // Affects Push/Pop.
navigator.SetModalTransitionAnimationEnabled(false);  // Affects PushModal/PopModal.
```

---

## Transition Specification

Use `NavigationTransitionSpec` to customize transitions. A spec contains
callbacks for incoming and outgoing views:

| Callback | Used for |
|---|---|
| `enter` | Incoming view for `Push()` or `PushModal()`. |
| `exit` | Outgoing view for `Push()` or `PushModal()`. |
| `popEnter` | Revealed view for `Pop()` or `PopModal()`. |
| `popExit` | Removed view for `Pop()` or `PopModal()`. |
| `snapIncoming` | Restores the incoming view to its final state. |
| `snapOutgoing` | Restores an outgoing view that remains in the stack. |

### Default Page Transition

```cpp
auto spec = std::make_shared<NavigationTransitionSpec>();
spec->duration = 0.25f;
spec->enter = [](Animation& anim, View view) {
  view.SetProperty(Actor::Property::OPACITY, 0.0f);
  anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
};
spec->exit = [](Animation& anim, View view) {
  anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f);
};
spec->snapIncoming = [](View view) {
  view.SetProperty(Actor::Property::OPACITY, 1.0f);
};

navigator.SetTransitionSpec(spec);
```

### Per-page Transition

```cpp
navigator.SetPageTransitionSpec(detailsPage, spec);
navigator.SetPageTransitionSpec(detailsPage, nullptr); // Remove override.
```

### Modal Transition

Modal transitions are separate from page transitions:

```cpp
navigator.SetModalTransitionSpec(modalSpec);
navigator.SetPageModalTransitionSpec(dialogContainer, modalSpec);
```

This lets normal page navigation and modal dialog presentation use different
motion.

---

## Signals

| Signal | Description |
|---|---|
| `PageWillAppearSignal()` | Emitted before a page or modal item becomes visible. |
| `PageDidAppearSignal()` | Emitted after a page or modal item becomes visible. |
| `PageWillDisappearSignal()` | Emitted before a page or modal item is covered or removed. |
| `PageDidDisappearSignal()` | Emitted after a page or modal item is covered or removed. |
| `TransitionFinishedSignal()` | Emitted when the current transition finishes. |

```cpp
navigator.PageDidAppearSignal().Connect(
  this,
  [](Navigator navigator, View page, bool byPop) {
    // Update page state here.
  });
```

---

## Notes

- A view cannot be pushed into both the navigation stack and modal stack at the
  same time.
- Page transition settings and modal transition settings are independent.
- `DialogContainer` is the recommended wrapper for modal dialog content.

<br/>

---

[Back to Components](Components.md)
