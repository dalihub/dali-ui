[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Ui-Scale-(Kr))

# Ui Scale

dali-ui provides three independent scaling mechanisms that together produce the final rendered size of any view:

| Mechanism | When applied | Who sets it |
|---|---|---|
| **Static scale** (`ScalingFactor`, `_spx`, `_sdp`) | At startup via `UiConfig::Apply()`, frozen at runtime | App / framework developer |
| **Dynamic scale** (`UiScale`, `UiScalePolicy`) | At runtime, propagated through the view tree on every layout pass | System / OS accessibility settings |
| **Per-window scale** (`LayoutScaleManager::SetWindowScale()`) | At runtime, to every view in one window | App / framework developer |

This document covers the **dynamic scale** system (`UiScale`) and the **per-window scale**, which is an independent multiplier on top of it.

<br/>

## How It Works

Every view in the tree carries an **effective scale** — a single `float` multiplier that is applied to all layout measurements (sizes, margins, padding, positions) during the Measure and Arrange passes.

The **UI scale** (also called the system scale) is the process-wide scale held by `LayoutScaleManager` and set with `LayoutScaleManager::SetUiScale()`, usually by an OS or accessibility handler. `LayoutScaleManager::GetUiScale()` returns that stored value; a view's own effective scale is `ViewImpl::GetEffectiveLayoutScale()`.

The effective scale is the product of two independent terms:

```
effectiveScale = windowScale × uiScale
```

- `uiScale` is the term `UiScalePolicy` selects: the UI scale held by `LayoutScaleManager`, or `1.0` for a view that opts out.
- `windowScale` is the scale set for the view's `Window`, or `1.0` when none is set.
- While the global master switch is off (`SetScalable(false)`), both terms are `1.0`; see *Global master switch* below.

The `uiScale` term of a view is determined by:
1. Walking up the parent chain to find the nearest ancestor (or root) that defines a concrete scale.
2. Roots inherit their scale from `LayoutScaleManager`.

The window scale is applied **once per inheritance chain**, at the chain's terminal view (an `ENABLED` view, a `DISABLED` view, or an inheriting root). Views that inherit receive the already-multiplied value, so the window scale never compounds down the tree.

```
LayoutScaleManager (system scale 1.5) + window scale 2.0
  └── RootLayout             effectiveScale = 3.0  (2.0 × 1.5, inherited from LayoutScaleManager)
        ├── StackLayout       effectiveScale = 3.0  (inherited)
        │     ├── Label       effectiveScale = 3.0  (inherited)
        │     └── Button      effectiveScale = 2.0  (DISABLED — opts out of the system scale only)
        └── Badge             effectiveScale = 3.0  (ENABLED — always tracks system)
```

When `LayoutScaleManager::SetUiScale()` is called, every registered layout root recomputes its effective scale and triggers a full re-layout automatically. A window scale change does the same for the roots of that one window. A view that is not on the scene (never added, or removed) belongs to no registered root and is not re-laid out, but it caches no effective scale either: its `GetEffectiveLayoutScale()` and `View::Measure()` follow a new UI scale or switch state at once, and it is laid out at the current scale when it is added to a window.

<br/>

## LayoutScaleManager

`LayoutScaleManager` is a process-wide singleton that holds the current system scale value and the scale of each window. It is not a `LayoutManager` and lays nothing out itself; the layout pass applies what it holds.

```cpp
#include <dali-ui-foundation/public-api/configuration/layout-scale-manager.h>

// Read the current system scale
float scale = LayoutScaleManager::Get().GetUiScale();  // default: 1.0

// Update the system scale (typically called by an OS/vconf handler)
LayoutScaleManager::Get().SetUiScale(1.5f);
```

Calling `SetUiScale()` does three things atomically:
1. Updates the stored scale value.
2. Resets the effective scale cache of every registered layout root and its entire subtree.
3. Calls `InvalidateMeasure()` on each root, scheduling a full re-layout on the next frame.

> **Note:** `SetUiScale()` accepts only positive, finite and normal values (`>= FLT_MIN`). `NaN`, infinities, zero, negatives and subnormals are ignored, leave the current UI scale in place, and log an error.

<br/>

### Global master switch

`LayoutScaleManager` also exposes a process-wide **master switch** that turns dynamic scaling on or off for the whole application.

```cpp
bool enabled = LayoutScaleManager::Get().IsScalable();  // default: true

LayoutScaleManager::Get().SetScalable(false);  // every view's effective scale collapses to 1.0
LayoutScaleManager::Get().SetScalable(true);   // restores the stored scales
```

While scaling is disabled, every view's effective scale resolves to `1.0` regardless of its `UiScalePolicy` (`ENABLED`, `INHERIT`, or `DISABLED`) or its position in the tree: the `uiScale` term and the window scale are both `1.0`. The value set via `SetUiScale()` and every window's scale set via `SetWindowScale()` are preserved while disabled and re-applied the moment scaling is re-enabled. Flipping the switch invalidates all registered layout roots and triggers a full re-layout, exactly like `SetUiScale()`.

> **Note:** Calling `SetUiScale()` or `SetWindowScale()` while scaling is disabled stores the new value but does not re-layout; the deferred value takes effect on the next `SetScalable(true)`.

> **Note:** `GetUiScale()` and `GetWindowScale()` report the stored values even while scaling is disabled; they are what `SetScalable(true)` re-applies, not the scale views are currently laid out at.

<br/>

### Per-window scale

Every `Window` can carry a **window scale** that multiplies the `uiScale` term of every view in it.

```cpp
Window window = application.GetWindow();

float windowScale = LayoutScaleManager::Get().GetWindowScale(window);  // default: 1.0

LayoutScaleManager::Get().SetWindowScale(window, 1.25f);  // 25% larger content
LayoutScaleManager::Get().SetWindowScale(window, 1.0f);   // back to the default
```

The window scale is **independent** of `UiScalePolicy`, but not of the master switch:

- It applies to every view in the window, including views whose policy is `UiScalePolicy::DISABLED`.
- Like the system scale, it is turned off by `SetScalable(false)` and re-applied by `SetScalable(true)`; a value set while scaling is disabled takes effect on re-enable.
- It is scoped to one window: setting it invalidates only the layout roots currently in that window, and other windows are untouched.

It does **not** change the window's own pixel size — it changes how much logical content fits in it. A `MATCH_PARENT` root still fills the window exactly, while the content inside it is laid out at the effective scale, so raising the window scale makes content bigger and fits *less* of it on screen.

Passing `1.0` is the reset: it restores the default and drops the window's record. There is no separate reset method.

Destroying a window needs no reset. A destroyed window's record is dropped automatically, and it never applies to a later window.

The window scale applies to a view only while the view is in that window. A view removed from its window has no window term, together with its whole subtree, and so does a view whose window is destroyed while the application still holds it: until it is added to a window again, `GetEffectiveLayoutScale()` and `View::Measure()` use the current UI-scale term alone (they follow `SetUiScale()` and `SetScalable()` while it is detached). Adding it to a window applies that window's scale again.

> **Note:** `SetWindowScale()` takes the same values as `SetUiScale()`: positive, finite and normal (`>= FLT_MIN`). Anything else — `NaN`, infinities, zero, negatives, subnormals — and an empty `Window` are ignored, leave the current window scale in place, and log an error. The layout divides by the effective scale, which is why the bounds are this strict.

<br/>

### UiScaleManager (deprecated)

`UiScaleManager` (`ui-scale-manager.h`) is the earlier name of this class. It is deprecated and kept for applications built against earlier releases; new code uses `LayoutScaleManager`. Both classes share one state, so a value set through either one is read back through both and the last call wins. They are separate handles: `LayoutScaleManager::DownCast()` of a `UiScaleManager` handle, and the reverse, returns an empty handle.

| UiScaleManager (deprecated) | LayoutScaleManager |
|---|---|
| `UiScaleManager::Get()` | `LayoutScaleManager::Get()` |
| `GetScale()` | `GetUiScale()` |
| `SetScale(float)` | `SetUiScale(float)` |
| `IsScalable()` | `IsScalable()` |
| `SetScalable(bool)` | `SetScalable(bool)` |
| `RegisterLayoutRoot(View)`, `UnregisterLayoutRoot(View)` | None; the library registers layout roots itself |
| None | `GetWindowScale(Window)`, `SetWindowScale(Window, float)` |

`ViewImpl::GetEffectiveScale()` is deprecated along with it: use `ViewImpl::GetEffectiveLayoutScale()`, which returns the same value.

<br/>

## UiScalePolicy

Each `View` can override how it participates in the scale propagation via `SetUiScalePolicy()`.

```cpp
#include <dali-ui-foundation/public-api/configuration/ui-scale-policy.h>

view.SetUiScalePolicy(UiScalePolicy::INHERIT);   // default
view.SetUiScalePolicy(UiScalePolicy::ENABLED);
view.SetUiScalePolicy(UiScalePolicy::DISABLED);
```

| Policy | `uiScale` term | Description |
|---|---|---|
| `INHERIT` | From parent (or `LayoutScaleManager` at root) | Default. Inherits scale through the tree. Most views should use this. |
| `ENABLED` | Always equals `LayoutScaleManager::GetUiScale()` | Always tracks the system scale, regardless of what the parent's policy is. |
| `DISABLED` | Always `1.0` | Opts out of the system scale. Descendant `INHERIT` views also receive `1.0` for this term. |

> **Note:** The column above is the `uiScale` **term**, not the resulting effective scale. The policy does not select the window scale, which multiplies whichever term the policy chose — so under a window scale of `2.0` a `DISABLED` view's effective scale is `2.0`, not `1.0`.

<br/>

### INHERIT (default)

The view inherits the effective scale from its parent. Since all views default to `INHERIT`, the system scale set on `LayoutScaleManager` naturally flows down the entire tree without any extra configuration.

```cpp
// No explicit policy needed — INHERIT is the default.
Label label = Label::New("Hello");
// label.SetUiScalePolicy(UiScalePolicy::INHERIT); // implicit
```

<br/>

### DISABLED — Opting a subtree out of the system scale

Use `DISABLED` when a subtree must not follow the **system** scale.
Typical use cases: fixed-size HUD elements, overlays that must not be rescaled by an accessibility setting.

```cpp
// This panel and all its children ignore the system scale.
panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
```

Because children inherit the `1.0` term from a `DISABLED` ancestor, only the subtree under `panel` stops following the system scale. Sibling views continue to scale normally.

> **Note:** This is not an opt-out of *all* scaling. If the view's `Window` carries a window scale, it still applies, so the subtree renders at the window scale rather than at natural pixel size. No public policy opts out of the window scale as well; only the master switch turns it off, for every view. (The framework itself opts out only the views it manages in physical pixels: the text cursor and handles, and the inline replacement layer.)

```
LayoutScaleManager UI scale = 2.0, no window scale
  └── RootLayout         effectiveScale = 2.0
        ├── Content       effectiveScale = 2.0  (INHERIT)
        └── Panel         effectiveScale = 1.0  (DISABLED)
              ├── Icon    effectiveScale = 1.0  (INHERIT from DISABLED parent)
              └── Text    effectiveScale = 1.0  (INHERIT from DISABLED parent)
```

<br/>

### ENABLED — Breaking out of a DISABLED subtree

Use `ENABLED` when a specific view inside a `DISABLED` subtree must still track the system scale. `ENABLED` bypasses parent inheritance and always reads directly from `LayoutScaleManager`.

```cpp
panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
badge.SetUiScalePolicy(UiScalePolicy::ENABLED);  // scales even inside DISABLED panel
```

```
LayoutScaleManager UI scale = 2.0, no window scale
  └── Panel         effectiveScale = 1.0  (DISABLED)
        ├── Icon    effectiveScale = 1.0  (INHERIT)
        └── Badge   effectiveScale = 2.0  (ENABLED — bypasses DISABLED parent)
```

With a window scale of `1.5` the same tree reads `1.5` / `1.5` / `3.0`: the window scale multiplies each terminal view's own term, and `Icon` inherits the already-multiplied value from `Panel`.

> **Note:** `ENABLED` is rarely needed for app developers. It is primarily useful for framework-level views that must always track scale regardless of where they are placed in the tree.

<br/>

## Combining the Three Scale Mechanisms

The total rendered size of a view is determined by all three scaling mechanisms together.

Given:
- `ScalingFactor` = `1.5` (set via `UiConfig::SetUnitScalingFactor()` at startup)
- UI scale = `2.0` (set at runtime with `LayoutScaleManager::SetUiScale()`)
- Window scale = `1.25` (set at runtime for this view's window)
- A view with requested width `100_spx`

The final visual width is:

```
100_spx = 100 × ScalingFactor = 100 × 1.5 = 150 px   (natural units)
effectiveScale = windowScale × uiScale = 1.25 × 2.0 = 2.5
visual width = 150 × effectiveScale = 150 × 2.5 = 375 px
```

Static scale (`ScalingFactor`) converts design units to device pixels once at startup.
Dynamic scale (`effectiveScale`) stretches or shrinks the entire layout tree at runtime; the window scale is the part of it that applies to one window only.

<br/>

## Reacting to Scale Changes in Custom Views

If your custom view (implemented via `ViewImpl`) caches pixel-unit values, those caches must be invalidated when scale changes. Since `InvalidateMeasure()` is called automatically on the root when `SetUiScale()` fires, derived views that override `OnMeasure` / `OnArrange` receive fresh constraints and `effectiveScale` on each layout pass — no extra work is required.

For views that cache values outside the layout pass, subscribe to property change notifications or call `GetEffectiveLayoutScale()` lazily inside `OnMeasure`.

<br/>

## Summary

| Goal | How |
|---|---|
| Apply system-wide scale change | `LayoutScaleManager::Get().SetUiScale(newScale)` |
| Read the current system scale | `LayoutScaleManager::Get().GetUiScale()` |
| Turn dynamic scaling (window scales included) on/off globally | `LayoutScaleManager::Get().SetScalable(enable)` |
| Query whether scaling is enabled | `LayoutScaleManager::Get().IsScalable()` |
| Apply an additional scale to one window | `LayoutScaleManager::Get().SetWindowScale(window, scale)` |
| Read a window's scale | `LayoutScaleManager::Get().GetWindowScale(window)` |
| Reset a window to the default scale | `LayoutScaleManager::Get().SetWindowScale(window, 1.0f)` |
| Opt a subtree out of the system scale | `view.SetUiScalePolicy(UiScalePolicy::DISABLED)` |
| Force a view to always track system scale | `view.SetUiScalePolicy(UiScalePolicy::ENABLED)` |
| Reset to default inherited behavior | `view.SetUiScalePolicy(UiScalePolicy::INHERIT)` |
| Query a view's effective scale (framework use) | `viewImpl.GetEffectiveLayoutScale()` |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
