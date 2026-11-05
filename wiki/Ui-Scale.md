[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Ui-Scale-(Kr))

# Ui Scale

dali-ui provides two independent scaling mechanisms that together produce the final rendered size of any view:

| Mechanism | When applied | Who sets it |
|---|---|---|
| **Static scale** (`ScalingFactor`, `_spx`, `_sdp`) | At startup via `UiConfig::Apply()`, frozen at runtime | App / framework developer |
| **Dynamic scale** (`UiScale`, `UiScalePolicy`) | At runtime, propagated through the view tree on every layout pass | System / OS accessibility settings |

This document covers the **dynamic scale** system (`UiScale`).

<br/>

## How It Works

Every view in the tree carries an **effective scale** — a single `float` multiplier that is applied to all layout measurements (sizes, margins, padding, positions) during the Measure and Arrange passes.

The **UI scale** (also called the system scale) is the process-wide scale held by `LayoutScaleManager` and set with `LayoutScaleManager::SetUiScale()`, usually by an OS or accessibility handler. `LayoutScaleManager::GetUiScale()` returns that stored value; a view's own effective scale is `ViewImpl::GetEffectiveLayoutScale()`.

The effective scale of a view is determined by:
1. Walking up the parent chain to find the nearest ancestor (or root) that defines a concrete scale.
2. Roots inherit their scale from `LayoutScaleManager`.

```
LayoutScaleManager (system scale)
  └── RootLayout             effectiveScale = 1.5  (inherited from LayoutScaleManager)
        ├── StackLayout       effectiveScale = 1.5  (inherited)
        │     ├── Label       effectiveScale = 1.5  (inherited)
        │     └── Button      effectiveScale = 1.0  (DISABLED — opt-out subtree)
        └── Badge             effectiveScale = 1.5  (ENABLED — always tracks system)
```

When `LayoutScaleManager::SetUiScale()` is called, every registered layout root recomputes its effective scale and triggers a full re-layout automatically.

<br/>

## LayoutScaleManager

`LayoutScaleManager` is a process-wide singleton that holds the current system scale value. It is not a `LayoutManager` and lays nothing out itself; the layout pass applies what it holds.

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

LayoutScaleManager::Get().SetScalable(false);  // every view renders unscaled
LayoutScaleManager::Get().SetScalable(true);   // restores the stored scale
```

While scaling is disabled, every view's effective scale resolves to `1.0` regardless of its `UiScalePolicy` (`ENABLED`, `INHERIT`, or `DISABLED`) or its position in the tree. The value set via `SetUiScale()` is preserved while disabled and re-applied the moment scaling is re-enabled. Flipping the switch invalidates all registered layout roots and triggers a full re-layout, exactly like `SetUiScale()`.

> **Note:** Calling `SetUiScale()` while scaling is disabled stores the new value but does not re-layout; the deferred value takes effect on the next `SetScalable(true)`.

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

| Policy | Effective Scale | Description |
|---|---|---|
| `INHERIT` | From parent (or `LayoutScaleManager` at root) | Default. Inherits scale through the tree. Most views should use this. |
| `ENABLED` | Always equals `LayoutScaleManager::GetUiScale()` | Always tracks the system scale, regardless of what the parent's policy is. |
| `DISABLED` | Always `1.0` | Opts out of scaling entirely. Descendant `INHERIT` views also receive `1.0`. |

<br/>

### INHERIT (default)

The view inherits the effective scale from its parent. Since all views default to `INHERIT`, the system scale set on `LayoutScaleManager` naturally flows down the entire tree without any extra configuration.

```cpp
// No explicit policy needed — INHERIT is the default.
Label label = Label::New("Hello");
// label.SetUiScalePolicy(UiScalePolicy::INHERIT); // implicit
```

<br/>

### DISABLED — Freezing a subtree at 1.0

Use `DISABLED` when a subtree should always render at 1:1 scale, regardless of system scale.
Typical use cases: fixed-size HUD elements, overlays that must not be rescaled.

```cpp
// This panel and all its children will always render at natural pixel size.
panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
```

Because children inherit `1.0` from a `DISABLED` ancestor, only the subtree under `panel` is unscaled. Sibling views continue to scale normally.

```
LayoutScaleManager UI scale = 2.0
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
LayoutScaleManager UI scale = 2.0
  └── Panel         effectiveScale = 1.0  (DISABLED)
        ├── Icon    effectiveScale = 1.0  (INHERIT)
        └── Badge   effectiveScale = 2.0  (ENABLED — bypasses DISABLED parent)
```

> **Note:** `ENABLED` is rarely needed for app developers. It is primarily useful for framework-level views that must always track scale regardless of where they are placed in the tree.

<br/>

## Combining Static and Dynamic Scale

The total rendered size of a view is determined by both scaling mechanisms together.

Given:
- `ScalingFactor` = `1.5` (set via `UiConfig::SetUnitScalingFactor()` at startup)
- UI scale = `2.0` (set at runtime with `LayoutScaleManager::SetUiScale()`)
- A view with requested width `100_spx`

The final visual width is:

```
100_spx = 100 × ScalingFactor = 100 × 1.5 = 150 px   (natural units)
visual width = 150 × effectiveScale = 150 × 2.0 = 300 px
```

Static scale (`ScalingFactor`) converts design units to device pixels once at startup.
Dynamic scale (`effectiveScale`) stretches or shrinks the entire layout tree at runtime.

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
| Turn dynamic scaling on/off globally | `LayoutScaleManager::Get().SetScalable(enable)` |
| Query whether scaling is enabled | `LayoutScaleManager::Get().IsScalable()` |
| Opt a subtree out of scaling | `view.SetUiScalePolicy(UiScalePolicy::DISABLED)` |
| Force a view to always track system scale | `view.SetUiScalePolicy(UiScalePolicy::ENABLED)` |
| Reset to default inherited behavior | `view.SetUiScalePolicy(UiScalePolicy::INHERIT)` |
| Query a view's effective scale (framework use) | `viewImpl.GetEffectiveLayoutScale()` |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
