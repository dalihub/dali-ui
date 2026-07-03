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

The effective scale of a view is determined by:
1. Walking up the parent chain to find the nearest ancestor (or root) that defines a concrete scale.
2. Roots inherit their scale from `UiScaleManager`.

```
UiScaleManager (system scale)
  └── RootLayout             effectiveScale = 1.5  (inherited from UiScaleManager)
        ├── StackLayout       effectiveScale = 1.5  (inherited)
        │     ├── Label       effectiveScale = 1.5  (inherited)
        │     └── Button      effectiveScale = 1.0  (DISABLED — opt-out subtree)
        └── Badge             effectiveScale = 1.5  (ENABLED — always tracks system)
```

When `UiScaleManager::SetScale()` is called, every registered layout root recomputes its effective scale and triggers a full re-layout automatically.

<br/>

## UiScaleManager

`UiScaleManager` is a process-wide singleton that holds the current system scale value.

```cpp
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>

// Read the current system scale
float scale = UiScaleManager::Get().GetScale();  // default: 1.0

// Update the system scale (typically called by an OS/vconf handler)
UiScaleManager::Get().SetScale(1.5f);
```

Calling `SetScale()` does three things atomically:
1. Updates the stored scale value.
2. Resets the effective scale cache of every registered layout root and its entire subtree.
3. Calls `InvalidateMeasure()` on each root, scheduling a full re-layout on the next frame.

> **Note:** `SetScale()` silently ignores `NaN`, zero, and negative values. Passing such a value is a no-op and logs an error.

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
| `INHERIT` | From parent (or `UiScaleManager` at root) | Default. Inherits scale through the tree. Most views should use this. |
| `ENABLED` | Always equals `UiScaleManager::GetScale()` | Always tracks the system scale, regardless of what the parent's policy is. |
| `DISABLED` | Always `1.0` | Opts out of scaling entirely. Descendant `INHERIT` views also receive `1.0`. |

<br/>

### INHERIT (default)

The view inherits the effective scale from its parent. Since all views default to `INHERIT`, the system scale set on `UiScaleManager` naturally flows down the entire tree without any extra configuration.

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
UiScaleManager scale = 2.0
  └── RootLayout         effectiveScale = 2.0
        ├── Content       effectiveScale = 2.0  (INHERIT)
        └── Panel         effectiveScale = 1.0  (DISABLED)
              ├── Icon    effectiveScale = 1.0  (INHERIT from DISABLED parent)
              └── Text    effectiveScale = 1.0  (INHERIT from DISABLED parent)
```

<br/>

### ENABLED — Breaking out of a DISABLED subtree

Use `ENABLED` when a specific view inside a `DISABLED` subtree must still track the system scale. `ENABLED` bypasses parent inheritance and always reads directly from `UiScaleManager`.

```cpp
panel.SetUiScalePolicy(UiScalePolicy::DISABLED);
badge.SetUiScalePolicy(UiScalePolicy::ENABLED);  // scales even inside DISABLED panel
```

```
UiScaleManager scale = 2.0
  └── Panel         effectiveScale = 1.0  (DISABLED)
        ├── Icon    effectiveScale = 1.0  (INHERIT)
        └── Badge   effectiveScale = 2.0  (ENABLED — bypasses DISABLED parent)
```

> **Note:** `ENABLED` is rarely needed for app developers. It is primarily useful for framework-level views that must always track scale regardless of where they are placed in the tree.

<br/>

## Combining Static and Dynamic Scale

The total rendered size of a view is determined by both scaling mechanisms together.

Given:
- `ScalingFactor` = `1.5` (set via `UiConfig::SetScalingFactor()` at startup)
- `UiScaleManager` scale = `2.0` (set at runtime)
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

If your custom view (implemented via `ViewImpl`) caches pixel-unit values, those caches must be invalidated when scale changes. Since `InvalidateMeasure()` is called automatically on the root when `SetScale()` fires, derived views that override `OnMeasure` / `OnArrange` receive fresh constraints and `effectiveScale` on each layout pass — no extra work is required.

For views that cache values outside the layout pass, subscribe to property change notifications or call `GetEffectiveScale()` lazily inside `OnMeasure`.

<br/>

## Summary

| Goal | How |
|---|---|
| Apply system-wide scale change | `UiScaleManager::Get().SetScale(newScale)` |
| Read the current system scale | `UiScaleManager::Get().GetScale()` |
| Opt a subtree out of scaling | `view.SetUiScalePolicy(UiScalePolicy::DISABLED)` |
| Force a view to always track system scale | `view.SetUiScalePolicy(UiScalePolicy::ENABLED)` |
| Reset to default inherited behavior | `view.SetUiScalePolicy(UiScalePolicy::INHERIT)` |
| Query a view's effective scale (framework use) | `viewImpl.GetEffectiveScale()` |

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
