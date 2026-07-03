# DALi UI Foundation - ScrollView

[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/ScrollView-(kr))

`ScrollView` is a scrollable container that displays content larger than its visible viewport. It supports pan gesture-based navigation, fling (inertia) scrolling, configurable scroll directions, and scroll bar visibility control.

---

## Table of Contents

1. [Basic Setup](#1-basic-setup)
2. [Adding Content and Layout](#2-adding-content-and-layout)
3. [Scroll Direction](#3-scroll-direction)
4. [Key Properties and Methods](#4-key-properties-and-methods)
   - [Scroll Position](#41-scroll-position)
   - [Fling Behavior](#42-fling-behavior)
   - [Over-Scroll Mode](#43-over-scroll-mode)
   - [Edge Effect](#44-edge-effect)
   - [Scroll Bar Visibility](#45-scroll-bar-visibility)
   - [Scroll State Query](#46-scroll-state-query)
   - [Focus and Key Scrolling](#47-focus-and-key-scrolling)
5. [Programmatic Scrolling](#5-programmatic-scrolling)
6. [Events (Signals)](#6-events-signals)
7. [Configuration Style](#7-configuration-style)
8. [Default Values](#8-default-values)
9. [Troubleshooting](#9-troubleshooting)
10. [Important Notes](#10-important-notes)

---

## 1. Basic Setup

Create a `ScrollView` using the static factory method `New()` and set its viewport size. Always set the content view before using scroll APIs.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali::Ui;

// Create a ScrollView with a 400x300 viewport
ScrollView scrollView = ScrollView::New();
scrollView.SetRequestedWidth(400.0f);
scrollView.SetRequestedHeight(300.0f);

// Create content larger than the viewport
View content = View::New();
content.SetRequestedWidth(400.0f);
content.SetRequestedHeight(2000.0f);

// Set the content — this is required before any scroll API is called
scrollView.SetContent(content);

// Add to the scene
window.Add(scrollView);
```

> **Important:** `SetContent()` must be called before using `SetScrollPosition()`, `ScrollTo()`, `ScrollToX()`, or `ScrollToY()`. Calling scroll APIs without content will result in a crash.

---

## 2. Adding Content and Layout

`ScrollView` holds exactly **one content view**. The content view acts as the scrollable canvas. Any view hierarchy can be built inside the content view.

### Setting content

```cpp
View content = View::New();
content.SetRequestedWidth(800.0f);
content.SetRequestedHeight(800.0f);

scrollView.SetContent(content);
```

### Building a nested layout inside content

Lay out child views inside the content view using any layout:

```cpp
View content = View::New();
content.SetRequestedWidth(400.0f);
content.SetRequestedHeight(3000.0f);

StackLayout layout = StackLayout::New();
layout.SetDirection(StackLayout::Direction::Vertical);
layout.SetSpacing(8.0f);
content.SetLayout(layout);

for(int i = 0; i < 20; ++i)
{
    View item = View::New();
    item.SetRequestedWidth(400.0f);
    item.SetRequestedHeight(140.0f);
    content.Add(item);
}

scrollView.SetContent(content);
```

---

## 3. Scroll Direction

The scroll direction controls which axis(es) the user can pan and which scroll APIs are effective.

```cpp
// Vertical scrolling only (default)
scrollView.SetScrollDirection(ScrollDirection::Vertical);

// Horizontal scrolling only
scrollView.SetScrollDirection(ScrollDirection::Horizontal);

// Both axes
scrollView.SetScrollDirection(ScrollDirection::Both);

// Query current direction
ScrollDirection dir = scrollView.GetScrollDirection();
```

| Value | Behavior |
|---|---|
| `ScrollDirection::Vertical` | Pan and fling only on the Y axis. Default. |
| `ScrollDirection::Horizontal` | Pan and fling only on the X axis. |
| `ScrollDirection::Both` | Full 2D scrolling. |

When `Vertical` or `Horizontal` is set and the gesture has a dominant component in the opposite axis, the off-axis movement is suppressed internally.

---

## 4. Key Properties and Methods

### 4.1 Scroll Position

The scroll position represents how far the content has been scrolled from its origin, measured in pixels. `(0, 0)` is the top-left (start) of the content.

```cpp
// Get current scroll position
Vector2 pos = scrollView.GetScrollPosition();

// Set scroll position immediately (no animation)
// Content must be set before calling this
scrollView.SetScrollPosition(Vector2(0.0f, 500.0f));
```

> `SetScrollPosition()` is equivalent to `ScrollTo(position, false)` — it applies the position immediately without animation.

---

### 4.2 Fling Behavior

Fling (inertia) scrolling is triggered when the user releases a pan gesture with velocity. The following properties control the physics of fling scrolling.

#### MaxFlingDistance

The maximum distance the content can travel in a single fling gesture.

```cpp
// Default: 6000.0f pixels
scrollView.SetMaxFlingDistance(3000.0f);

float dist = scrollView.GetMaxFlingDistance();
```

#### MinimumFlingDuration / MaximumFlingDuration

The animation duration is proportional to the fling distance, clamped between these values (in milliseconds).

```cpp
// Defaults: min=1000ms, max=2000ms
scrollView.SetMinimumFlingDuration(500);
scrollView.SetMaximumFlingDuration(2500);

int minDur = scrollView.GetMinimumFlingDuration();
int maxDur = scrollView.GetMaximumFlingDuration();
```

#### FlingSensitivity

A multiplier applied to the gesture velocity when converting to fling distance. Increasing this makes fling gestures travel farther.

```cpp
// Default: 1.0f
scrollView.SetFlingSensitivity(1.5f);

float sensitivity = scrollView.GetFlingSensitivity();
```

#### DecelerationRate

Controls how quickly the fling animation decelerates. This value is used in the velocity-to-movement conversion formula. Values closer to `1.0` mean slower deceleration (longer glide).

```cpp
// Default: 0.998f
scrollView.SetDecelerationRate(0.995f);

float rate = scrollView.GetDecelerationRate();
```

---

### 4.3 Over-Scroll Mode

Controls whether the content can be scrolled beyond its boundary (rubber-band / edge effect).

```cpp
// Default: ContentScrolls — over-scroll only when content is larger than viewport
scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);

// Never allow over-scrolling
scrollView.SetOverScrollMode(OverScrollMode::Never);

// Always allow over-scrolling
scrollView.SetOverScrollMode(OverScrollMode::Always);

OverScrollMode mode = scrollView.GetOverScrollMode();
```

| Value | Behavior |
|---|---|
| `OverScrollMode::Never` | Content stops exactly at the boundary. |
| `OverScrollMode::Always` | Over-scroll is always enabled. |
| `OverScrollMode::ContentScrolls` | Over-scroll only when content exceeds viewport. Default. |

---

### 4.4 Edge Effect

`ScrollView` can use an `EdgeEffect` to provide feedback when content is dragged or flung past a scroll boundary.

The built-in `BounceEdgeEffect` moves the content past the boundary and springs it back.

```cpp
#include <dali-ui-foundation/public-api/views/scroll/bounce-edge-effect.h>

// Vertical ScrollView: start = top, end = bottom
BounceEdgeEffect startEffect = BounceEdgeEffect::New(ScrollDirection::Vertical);
startEffect.SetPullResistance(0.35f);
startEffect.SetBounceDuration(0.35f);

BounceEdgeEffect endEffect = BounceEdgeEffect::New(ScrollDirection::Vertical);
endEffect.SetPullResistance(0.35f);
endEffect.SetBounceDuration(0.35f);

scrollView.SetStartEdgeEffect(startEffect);
scrollView.SetEndEdgeEffect(endEffect);
scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
```

For horizontal scrolling, create the effects with `ScrollDirection::Horizontal`.

```cpp
BounceEdgeEffect leftEffect = BounceEdgeEffect::New(ScrollDirection::Horizontal);
BounceEdgeEffect rightEffect = BounceEdgeEffect::New(ScrollDirection::Horizontal);

scrollView.SetStartEdgeEffect(leftEffect);
scrollView.SetEndEdgeEffect(rightEffect);
```

To remove an edge effect, pass an uninitialized `EdgeEffect`.

```cpp
scrollView.SetStartEdgeEffect(EdgeEffect());
scrollView.SetEndEdgeEffect(EdgeEffect());
```

| Method | Meaning |
|---|---|
| `SetStartEdgeEffect(effect)` | Sets the effect for the start boundary. Top for vertical scrolling, left for horizontal scrolling. |
| `SetEndEdgeEffect(effect)` | Sets the effect for the end boundary. Bottom for vertical scrolling, right for horizontal scrolling. |
| `GetStartEdgeEffect()` | Returns the current start-boundary effect. |
| `GetEndEdgeEffect()` | Returns the current end-boundary effect. |

> `ScrollView` sets the effect source to its content view when content is available, so application code normally only needs to create the effect and assign it to the scroll view.
>
> `OverScrollMode` controls whether scrolling can go past the boundary. `EdgeEffect` controls the visual feedback shown at that boundary. If `OverScrollMode::Never` is set, boundary feedback is not expected even when an edge effect is assigned.

---

### 4.5 Scroll Bar Visibility

Controls when vertical and horizontal scroll bars are shown.

```cpp
// Vertical scroll bar
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);    // default
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Always);
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);

// Horizontal scroll bar
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Auto);  // default
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Always);
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);

ScrollBarVisibility vVis = scrollView.GetVerticalScrollBarVisibility();
ScrollBarVisibility hVis = scrollView.GetHorizontalScrollBarVisibility();
```

| Value | Behavior |
|---|---|
| `ScrollBarVisibility::Auto` | Shown only while scrolling is in progress. Default. |
| `ScrollBarVisibility::Always` | Always visible. |
| `ScrollBarVisibility::Never` | Never shown. |

---

### 4.6 Scroll State Query

```cpp
bool scrolling = scrollView.IsScrolling();
```

Returns `true` between `ScrollStartedSignal` and `ScrollFinishedSignal` emissions — i.e., during both drag and fling animation.

---

### 4.7 Focus and Key Scrolling

`ScrollView` can keep focused content visible during keyboard or remote-control navigation.

The basic rule is:

- When a descendant of the content view receives focus, `ScrollView` can automatically scroll that child into view.
- The target alignment is controlled by `SetFocusScrollToPosition()`.
- A small extra offset can be applied with `SetFocusScrollPeek()` so the focused item is not placed exactly on the viewport edge.
- Optional key scrolling lets arrow keys scroll by steps before focus jumps to far-away items.

```cpp
// Enabled by default. Disable if the app wants to manage focus scrolling itself.
scrollView.SetScrollOnFocus(true);

// Default: MakeVisible. Other options are Start, Center, and End.
scrollView.SetFocusScrollToPosition(ScrollToPosition::MakeVisible);

// Add a 24px peek offset when MakeVisible scrolls to an edge.
scrollView.SetFocusScrollPeek(24.0f);
```

`SetFocusScrollPeek()` is applied only for `ScrollToPosition::MakeVisible`. When the focused item is outside the viewport, `ScrollView` first chooses the nearest edge that makes the item visible, then scrolls a little farther in the same direction by the peek distance. The final value is still clamped to the valid scroll range.

For key-based step scrolling:

```cpp
scrollView.SetKeyScrollEnabled(true);
scrollView.SetKeyScrollStep(200.0f);
```

When key scrolling is enabled, arrow navigation follows this principle:

- If the next focusable child is close enough, focus moves to it.
- If the next focusable child is farther than the key scroll step, `ScrollView` scrolls by one step and keeps focus where it is.
- `PAGE_UP` and `PAGE_DOWN` scroll by the viewport size and focus the best item in the destination viewport.
- At a scroll boundary with no next item, focus can leave the `ScrollView`; if an edge effect is configured, boundary feedback is triggered.

---

## 5. Programmatic Scrolling

All scroll methods accept an optional `animation` parameter (default `true`). When `false`, the position is applied immediately.

### Scroll to a position

```cpp
// Animated (default)
scrollView.ScrollTo(Vector2(0.0f, 300.0f));

// Immediate (no animation)
scrollView.ScrollTo(Vector2(0.0f, 300.0f), false);
```

### Scroll on a single axis

```cpp
// Scroll to X = 200 (horizontal)
scrollView.ScrollToX(200.0f);
scrollView.ScrollToX(200.0f, false);  // immediate

// Scroll to Y = 500 (vertical)
scrollView.ScrollToY(500.0f);
scrollView.ScrollToY(500.0f, false);  // immediate
```

### Scroll to make a child view visible

`ScrollTo(View child, ...)` scrolls the minimum amount needed to bring the specified child into the viewport.

```cpp
// Scroll to make childView visible (animated, default)
scrollView.ScrollTo(childView);

// Scroll to make childView visible (immediate)
scrollView.ScrollTo(childView, false);
```

The `ScrollToPosition` parameter gives additional control over where the child lands:

```cpp
// Scroll so childView aligns to the start of the viewport
scrollView.ScrollTo(childView, true, ScrollToPosition::Start);

// Scroll so childView is centered in the viewport
scrollView.ScrollTo(childView, true, ScrollToPosition::Center);

// Scroll so childView aligns to the end of the viewport
scrollView.ScrollTo(childView, true, ScrollToPosition::End);

// Scroll only enough to make childView visible (default)
scrollView.ScrollTo(childView, true, ScrollToPosition::MakeVisible);
```

| `ScrollToPosition` | Behavior |
|---|---|
| `MakeVisible` | Minimum scroll to bring the child into view. Default. |
| `Start` | Child aligns to the top/left of the viewport. |
| `Center` | Child is centered in the viewport. |
| `End` | Child aligns to the bottom/right of the viewport. |

> **Note:** `ScrollTo(View child, ...)` requires that `child` is a descendant of the content view.

---

## 6. Events (Signals)

`ScrollView` emits six signals covering scroll and drag lifecycle events. Connect them using a class that inherits from `ConnectionTracker`.

### Signal types

| Signal | Signature | Emitted when |
|---|---|---|
| `ScrollStartedSignal` | `void(ScrollView)` | Scrolling begins (drag or fling start) |
| `ScrollingSignal` | `void(ScrollView)` | Each frame while scrolling |
| `ScrollFinishedSignal` | `void(ScrollView)` | Scrolling ends (drag released or fling completes) |
| `DragStartedSignal` | `void(ScrollView)` | Pan threshold crossed, drag begins |
| `DraggingSignal` | `void(ScrollView, float deltaX, float deltaY)` | Each pan gesture update while dragging |
| `DragFinishedSignal` | `void(ScrollView)` | Pan gesture released |

### Connecting signals

Your callback class must inherit from `ConnectionTracker`:

```cpp
class MyScrollHandler : public Dali::ConnectionTracker
{
public:
    void OnScrollStarted(ScrollView scrollView)
    {
        // scrollView.GetScrollPosition() returns the position at scroll start
    }

    void OnScrolling(ScrollView scrollView)
    {
        Vector2 pos = scrollView.GetScrollPosition();
        // Update UI based on current scroll position
    }

    void OnScrollFinished(ScrollView scrollView)
    {
        // Scrolling has fully stopped
    }

    void OnDragStarted(ScrollView scrollView)
    {
        // User began dragging
    }

    void OnDragging(ScrollView scrollView, float deltaX, float deltaY)
    {
        // deltaX, deltaY: pixels moved since last event
        // Use for parallax effects or header collapse animations
    }

    void OnDragFinished(ScrollView scrollView)
    {
        // User lifted their finger; fling animation may follow
    }
};
```

```cpp
MyScrollHandler handler;

scrollView.ScrollStartedSignal().Connect(&handler, &MyScrollHandler::OnScrollStarted);
scrollView.ScrollingSignal().Connect(&handler, &MyScrollHandler::OnScrolling);
scrollView.ScrollFinishedSignal().Connect(&handler, &MyScrollHandler::OnScrollFinished);
scrollView.DragStartedSignal().Connect(&handler, &MyScrollHandler::OnDragStarted);
scrollView.DraggingSignal().Connect(&handler, &MyScrollHandler::OnDragging);
scrollView.DragFinishedSignal().Connect(&handler, &MyScrollHandler::OnDragFinished);
```

### Signal timing relationship

```
User touches screen
        │
        ▼
  [pan threshold: 5px]
        │
        ▼
  DragStartedSignal ──► ScrollStartedSignal
        │
        ▼  (each frame)
  DraggingSignal ──────► ScrollingSignal
        │
        ▼
  User lifts finger
        │
        ▼
  DragFinishedSignal
        │
        ├─ velocity sufficient?
        │         Yes ──► fling animation ──► ScrollingSignal (each frame)
        │                                           │
        │                                           ▼
        │                                   ScrollFinishedSignal
        │         No  ──► ScrollFinishedSignal
```

> `ScrollStartedSignal` fires only once per scroll session. `ScrollFinishedSignal` fires when all motion (including the fling animation) has stopped.

### Practical example: Invisible header

```cpp
void OnDragging(ScrollView scrollView, float deltaX, float deltaY)
{
    float scrollY = scrollView.GetScrollPosition().y;

    // Collapse header when scrolled down more than 100px
    float headerOpacity = std::max(0.0f, 1.0f - scrollY / 100.0f);
    mHeader.SetOpacity(headerOpacity);
}
```

### Practical example: scroll-to-top button

```cpp
void OnScrollFinished(ScrollView scrollView)
{
    // Show scroll-to-top button only when not at the top
    bool atTop = scrollView.GetScrollPosition().y < 10.0f;
    mScrollToTopButton.SetVisible(!atTop);
}

void OnButtonClicked()
{
    mScrollView.ScrollTo(Vector2(0.0f, 0.0f));  // animated scroll to top
}
```

---

## 7. Configuration Style

`ScrollView` setters do not use method chaining. Configure the handle with ordinary sequential setter calls.

```cpp
ScrollView scrollView = ScrollView::New();

scrollView.SetScrollDirection(ScrollDirection::Vertical);
scrollView.SetMaxFlingDistance(4000.0f);
scrollView.SetMinimumFlingDuration(800);
scrollView.SetMaximumFlingDuration(2000);
scrollView.SetFlingSensitivity(1.2f);
scrollView.SetDecelerationRate(0.997f);
scrollView.SetOverScrollMode(OverScrollMode::ContentScrolls);
scrollView.SetVerticalScrollBarVisibility(ScrollBarVisibility::Auto);
scrollView.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
scrollView.SetContent(content);
```

This is the recommended style for `View`, `ScrollView`, and other handle classes whose APIs may be inherited or extended.

---

## 8. Default Values

| Property | Default |
|---|---|
| `ScrollDirection` | `Vertical` |
| `MaxFlingDistance` | `6000.0f` px |
| `MinimumFlingDuration` | `1000` ms |
| `MaximumFlingDuration` | `2000` ms |
| `FlingSensitivity` | `1.0f` |
| `DecelerationRate` | `0.998f` |
| `OverScrollMode` | `ContentScrolls` |
| `VerticalScrollBarVisibility` | `Auto` |
| `HorizontalScrollBarVisibility` | `Auto` |
| Pan threshold | `5.0f` px (internal, not configurable) |

---

## 9. Troubleshooting

| Symptom | Things to check |
|---|---|
| Content does not scroll | Check that `SetContent()` was called, the content size is larger than the viewport, and `SetScrollDirection()` allows movement on the intended axis. |
| Programmatic `ScrollTo()` stays at `(0, 0)` | The scroll view may not have been laid out yet, or the content may not be larger than the viewport. Call scroll APIs after content and viewport sizes are resolved. |
| Scroll bars do not appear | Check `SetVerticalScrollBarVisibility()` / `SetHorizontalScrollBarVisibility()`, content size, viewport size, and whether scrolling is actually possible on that axis. |
| Edge feedback does not appear | Check that `SetStartEdgeEffect()` / `SetEndEdgeEffect()` were assigned, `OverScrollMode` is not `Never`, and the user is dragging or flinging into a boundary. |
| Focused child does not scroll into view | Check `SetScrollOnFocus(true)`, whether the focused view is a descendant of the content view, and whether the child is keyboard focusable. |
| Arrow keys move focus too far instead of gradually scrolling | Enable key scrolling with `SetKeyScrollEnabled(true)` and tune `SetKeyScrollStep()`. |

---

## 10. Important Notes

- **Content must be set first.** Calling `ScrollTo`, `ScrollToX`, `ScrollToY`, or `SetScrollPosition` before `SetContent()` will result in a crash because the implementation reads content actor properties directly.

- **One content view only.** `ScrollView` manages a single content view. To display a list, build the list structure inside the content view using a layout.

- **Viewport size matters.** Scrollable area is computed as `contentSize - viewportSize`. If the scroll view has not been laid out yet (e.g., in a test environment without a stage), the viewport size is `0` and all scroll positions will be clamped to `(0, 0)`.

- **Scroll bar rendering.** The `ScrollBarVisibility` properties store the setting but actual scroll bar rendering depends on the platform's theme and the concrete implementation of the scroll bar actor. Verify behavior in your target environment.

- **Interactive children and drag interception.** Buttons, sliders, and other interactive children can still receive tap/click input. When pointer movement passes the pan threshold, `ScrollView` may intercept the gesture and switch to scrolling. Design child interactions so tap and drag are visually distinct.

- **`IsScrolling()` covers both drag and fling.** The flag is set to `true` at `ScrollStartedSignal` and cleared at `ScrollFinishedSignal`, so it remains `true` during the entire fling animation phase after the user lifts their finger.

- **`DragFinishedSignal` precedes `ScrollFinishedSignal`.** After the user releases the screen, `DragFinishedSignal` fires first. If a fling animation follows, `ScrollFinishedSignal` fires only after the animation completes.

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
