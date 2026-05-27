[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Fluent-API-(kr))

# Fluent API
Construct complex UI hierarchies with ease using our declarative method chaining:

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali::UI;

void Create(Application& application) {
  application.GetWindow().Add(
    StackLayout::New(StackOrientation::VERTICAL)
      .SetBackgroundColor(UiColor::PRIMARY)
      .Contents({
        View::New()
          .SetBackgroundColor(UiColor(0xFF00FF))
          .SetRequestedWidth(100_spx)
          .SetRequestedHeight(100_spx)
          .With([](View& v) {
            v.TouchedSignal().Connect([](Actor a, const TouchEvent& t) {
              /* Describes on touch handler */
              return true;
            });
          }),
        View::New()
          .SetBackgroundColor(UiColor(0x0000FF))
          .SetRequestedWidth(100_spx)
          .SetRequestedHeight(100_spx)
          .As(mSecondChild)
      })
  );
}
```
<br/>

## `As` and `With`

`As` and `With` are two utility methods on `View` designed to integrate seamlessly into fluent method chains.

### `As` — Capture a reference

`As` assigns the current view to an external variable without breaking the chain.
This is useful when you need to hold a reference to a view that is created inline inside a declarative UI tree.

```cpp
Label mLabel;

View::New()
  .Children({
    Label::New("Hello")
      .SetTextColor(UiColor::PRIMARY)
      .As(mLabel),   // mLabel now holds this Label
  });

// mLabel can be used later
mLabel.SetText("Updated");
```

### `With` — Inject custom logic

`With` executes a lambda on the current view and then returns it, allowing you to perform
arbitrary setup that cannot be expressed as a single setter call — such as connecting signals —
without interrupting the chain.

```cpp
View::New()
  .SetBackgroundColor(UiColor::PRIMARY)
  .With([](View& v) {
    v.StateChangedSignal().Connect([](View view, StateEvent event) {
      // handle state change
    });
  });
```

> [!NOTE]
> `With` is particularly useful for connecting signals or performing conditional initialization inline.

<br/>

## Extending Fluent API (for Framework Developers)

dali-ui's Fluent API is designed so that method chaining never breaks across class inheritance
boundaries. When developing a new control that inherits from `View`, you need to understand
this mechanism and extend it correctly.

<br/>

### The Problem

In C++, if `View`'s setters return `View&`, calling them on a `Label` instance still returns
`View&` — making it impossible to chain `Label`-specific methods afterward.

```cpp
// ❌ Compile error: SetPositionX() returns View&, so SetText() cannot be chained
Label::New()
  .SetPositionX(10_dp)   // returns View&
  .SetText("hello");     // error: View has no SetText()
```

<br/>

### dali-ui's Solution: Autogen Macros

dali-ui solves this with **generated wrapper macros**.

At build time, `gen-chaining-macro.py` analyzes header files and generates `*.autogen.h` files.
Each generated file defines a `DALI_UI_CHAIN_{CLASS}_METHODS(ChildClass)` macro that re-wraps
every setter from the parent class to return `ChildClass&`.

> [!IMPORTANT]
> The code generation script runs automatically at build time, but it’s strictly exclusive to the dali-ui source.
> To apply it to external libraries, please refer to the guide [here](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/scripts/autogen).

For example, `view.autogen.h` contains:

```cpp
#define DALI_UI_CHAIN_VIEW_METHODS(ChildClass)              \
  ChildClass& SetPositionX(float x)                        \
    { View::SetPositionX(x); return *this; }               \
  ChildClass& SetRequestedWidth(float width)               \
    { View::SetRequestedWidth(width); return *this; }      \
  // ... (all View setters)
```

`Label` places this macro at the end of the class, so that `View` setters always return `Label&`
when called on a `Label` instance:

```cpp
class Label : public View {
public:
  Label& SetText(const Dali::String& text);
  // ...

public:
  DALI_UI_CHAIN_VIEW_METHODS(Label)
};
```

Chaining now works correctly:

```cpp
// ✅ Works
Label::New()
  .SetPositionX(10_dp)   // returns Label& (via macro)
  .SetText("hello");     // OK
```

<br/>

### Propagation Through the Hierarchy

As inheritance deepens, the macros propagate hierarchically.
`DALI_UI_CHAIN_LABEL_METHODS` in `label.autogen.h` includes the `View` macro:

```cpp
#define DALI_UI_CHAIN_LABEL_METHODS(ChildClass)   \
  DALI_UI_CHAIN_VIEW_METHODS(ChildClass)          \  // includes all View setters
  ChildClass& SetText(...) { ... }                \
  ChildClass& SetFontSize(...) { ... }            \
  // ... (all Label setters)
```

A class inheriting from `Label` only needs `DALI_UI_CHAIN_LABEL_METHODS` to chain setters from
both `View` and `Label`.

<br/>

### Adding Fluent API to a New Class

When developing a new control, follow these steps.

**1. Declare setter methods returning `ClassName&` and add annotations**

```cpp
// @CHAIN_START(MyView, View)   ← class name, parent class name
MyView& SetRadius(float radius);
MyView& SetBorderWidth(float width);
// @CHAIN_END
```

Any `ClassName& SetXXX(...)` setter inside the `@CHAIN_START` ~ `@CHAIN_END` block is
collected automatically.

**2. Build — the rest is handled automatically**

The build script performs the following automatically:
- Generates `myview.autogen.h` (containing the `DALI_UI_CHAIN_MYVIEW_METHODS` macro)
- Inserts `#include "myview.autogen.h"` into the header file
- Inserts `DALI_UI_CHAIN_VIEW_METHODS(MyView)` at the end of the class

> **Note**: Autogen does not run in GBS build environments.
> Commit the generated `*.autogen.h` files to the repository after a desktop build.

**3. Classes inheriting from `MyView` use `DALI_UI_CHAIN_MYVIEW_METHODS`**

```cpp
class MySpecialView : public MyView {
public:
  // @CHAIN_START(MySpecialView, MyView)
  MySpecialView& SetShadow(float blur);
  // @CHAIN_END
};
```

<br/>

### `@CHAIN_MANUAL`: Methods That Don't Follow the `SetXXX` Pattern

Auto-collection only recognizes the `ClassName& SetXXX(...)` pattern.
For methods with other naming conventions such as `Enable...` or `As...`, use `@CHAIN_MANUAL`:

```cpp
// @CHAIN_START(MyView, View)
MyView& SetRadius(float radius);   // auto-collected

// @CHAIN_MANUAL
MyView& EnableShadow(bool enable); // not SetXXX — must be marked manually
// @CHAIN_END
```

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
