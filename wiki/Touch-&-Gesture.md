[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Touch-&-Gesture-(kr))

# Touch & Gesture Behavior and Code Implementation Guide


## Table of Contents

1. [Scope](#scope)
2. [TouchEvent](#touchevent)
3. [Blocking Touch Propagation](#blocking-touch-propagation)
4. [InterceptTouchEvent](#intercepttouchevent)
5. [HoverEvent](#hoverevent)
6. [Gesture Detection by HandleEvent](#gesture-detection-by-handleevent)
7. [Gesture Propagation](#gesture-propagation)
8. [Nested Scrolling with an Overlay](#nested-scrolling-with-an-overlay)
9. [Per-Device Gesture Recognition Options](#per-device-gesture-recognition-options)
10. [Example 1: Basic Gesture Recognition](#example-1-basic-gesture-recognition)
11. [Example 2: Gesture Handling Using InterceptTouchEvent](#example-2-gesture-handling-using-intercepttouchevent)
12. [Summary](#summary)
13. [Touch Boundary Diagnostics](#touch-boundary-diagnostics)
14. [Source References](#source-references)

---

## Scope

This document describes **GEOMETRY input hit testing**, which DALi UI enables by default when the
application is created. TouchEvent and HoverEvent both use front-to-back coordinate candidates, but their
lifetimes differ: TouchEvent fixes candidates at the initial DOWN and may select a stream owner, while
HoverEvent hit-tests every input and keeps the actually visited candidate prefix active. Setting
`GeometryHittestEnabled` to `false` selects the legacy PARENT hit-test path. Select this setting during
application startup and do not change it after input event processing has started.

This document uses the following terms:

- **device**: the device ID that identifies a point in a `TouchEvent`, normally for the lifetime of one pointer
- **initial routing group**: the ID of the frontmost eligible coordinate candidate requiring PINCH or ROTATION; if none exists, the initial hit Actor ID is used. Devices with the same group ID join the same stream.
- **stream**: the TouchEvent delivery unit that owns the devices, candidates, recipients, and owner state for one initial routing group
- **coordinate candidate**: a front-to-back TouchEvent candidate selected by the geometry hit test at the initial DOWN coordinate
- **owner**: the first Actor to consume a TouchEvent and handle delivery until stream termination or interception on the observation path
- **recipient**: an Actor whose TouchEvent callback has actually been invoked at least once for the stream
- **interceptor**: an Actor that returns `true` from an InterceptTouchEvent callback on the ancestor path and restricts the previous delivery range
- **ancestor path**: the actual parent-child path from the root toward a target

Devices and streams are related as follows:

```text
device 4 ─┐
          ├─ same initial routing group ID ──> Stream 1
device 7 ─┘

device 9 ─── different routing group ID ──> Stream 2

Each Stream
├─ coordinate candidates
├─ ACTIVE/TERMINATED recipients
├─ captured Touch propagation boundaries
└─ owner / interceptor

Stream 1 and Stream 2 remain separate even if they later select the same owner.
```

The main stream lifetime phases and ownership selection are shown below. Interception is separate arbitration
that changes the TouchEvent delivery range rather than being a phase itself.

```text
initial DOWN
   │
   ▼
UNOWNED ── TouchEvent true ──> OWNED
   │                            │
   │ TouchEvent false           │ later TouchEvents go only to the owner
   │ actual receivers are ACTIVE │
   └─ retry on the next event   │
                                │
UNOWNED or OWNED ── last UP / forced cancellation ──> FINISHING ──> FINISHED

InterceptTouchEvent true
   └─ restricts normal Touch delivery to the interceptor→root path
      └─ the first TouchEvent consumer on that path becomes the owner
```

---

## TouchEvent

The initial DOWN coordinate is geometry hit-tested. Before interception, normal TouchEvents in a stream
without an owner follow the coordinate candidates from front to back, independently of their parent-child
relationships.

The normal TouchEvent delivery range depends on the current stream state:

- no owner and not intercepted: initial-DOWN coordinate candidates, from front to back
- owner selected: only the current owner
- intercepted but no new owner: the fixed interceptor-to-root path, subject to the captured Touch propagation boundary

A boundary limits ordinary delivery without selecting an owner; see
[Blocking Touch Propagation](#blocking-touch-propagation).

### Consumption Within One View

In GEOMETRY mode, an Actor's intrinsic Touch handling and its connected Touch callbacks are combined with
logical OR; all connected callbacks run. A callback returning `false` cannot cancel consumption elsewhere
on the same View. For example, an enabled InteractiveTrait with click or long-press handling can consume
DOWN even if an application callback returns `false`. The examples below assume no other intrinsic handler
or callback consumes the event.

### Handling Touch Events

A View receives touch events via `TouchEventSignal`. Note that the callback receives an `Actor` (not a `View`), so you need to downcast it if you want to use View-specific APIs.

Using a member function:

```cpp
class MyController : public ConnectionTracker
{
public:
  void SetupTouchHandler(View view)
  {
    view.TouchEventSignal().Connect(this, &MyController::OnTouched);
  }

private:
  bool OnTouched(Actor actor, const TouchEvent& touch)
  {
    // Downcast Actor to View
    View view = View::DownCast(actor);

    if(touch.GetState(0) == PointState::DOWN)
    {
      // Handle touch down
      return true;  // consumed — this View becomes the owner
    }
    return false;  // keep an existing owner; otherwise continue to the next candidate
  }
};
```

The same can be written concisely with a lambda:

```cpp
view.TouchEventSignal().Connect(&tracker, [](Actor actor, const TouchEvent& touch) -> bool {
  View view = View::DownCast(actor);

  if(touch.GetState(0) == PointState::DOWN)
  {
    // Handle touch down
    return true;  // consumed
  }
  return false;  // keep an existing owner; otherwise continue to the next candidate
});
```

> [!NOTE]
> While there is no owner, the first Actor to return `true` becomes the stable owner. Returning `false`
> continues delivery to the next coordinate candidate, but the Actor remains an active recipient and may
> receive later events and consume them. When another Actor becomes the owner, each previous active recipient
> receives exactly one `INTERRUPTED` and then leaves the stream.

<br/>

### Event Propagation Method

When four Views overlap on screen as shown below, the DOWN coordinate determines the geometry hit-test
result. The drawing represents screen overlap; it does not imply a parent-child relationship.

```
┌─────────────────────────────────────┐
│            Blue View                │
│  ┌───────────────────────────────┐  │
│  │        Yellow View            │  │
│  │  ┌─────────────────────────┐  │  │
│  │  │       Red View          │  │  │
│  │  │  ┌─────────────────┐    │  │  │
│  │  │  │   Orange View   │    │  │  │
│  │  │  └─────────────────┘    │  │  │
│  │  └─────────────────────────┘  │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

- For example, if Red, Yellow, and Blue are candidates at the coordinate, delivery proceeds Red → Yellow → Blue.
- When Red returns `false`, the same event proceeds to Yellow. If no Actor consumes, Red remains eligible
  to receive the next Motion event.

### Consume Behavior

The first Actor to return `true` from a TouchEvent becomes the owner.

When Yellow returns `true`, subsequent TouchEvents are delivered to Yellow. Returning `false` from a later
owner callback does not release ownership. An Actor on the path to the initial hit Actor can still intercept,
including the owner itself or a descendant on that path.

### Event Propagation Example

The sequence will be as follows:

```
Red View TouchDown return false
Yellow View TouchDown return true
Red View Interrupted
Yellow View TouchMotion return true
Yellow View TouchMotion return true
Yellow View TouchFinished return true
```

Red already received the same DOWN and is therefore an active recipient. It receives `INTERRUPTED`
immediately after Yellow becomes the owner. If several recipients received this or an earlier event, every
previous active recipient other than the new owner receives exactly one `INTERRUPTED`. An Actor that received
only an intercept callback is tracked separately as an intercept observer. Owner selection alone does not
close this observation. When interception succeeds or the stream ends, remaining active intercept observers
receive cleanup INTERRUPTED through InterceptTouchEvent, so their detectors must also handle cancellation.

```text
Red TouchEvent false   ──> Red = ACTIVE recipient
Yellow TouchEvent true ──> Yellow = ACTIVE recipient + owner
                            Red receives INTERRUPTED and becomes TERMINATED
```

### Touch Event Propagation Characteristics

- The coordinate-candidate order created on the initial DOWN is independent of parent-child relationships.
- Until an owner is selected, the initial candidates may continue competing on later events. Actors newly
  encountered at a Motion coordinate are not added to the stream.
- The owner remains selected even when the pointer moves outside the Actor's bounds.
- GEOMETRY Touch does not emit `PointState::LEAVE` when the pointer leaves an Actor. HoverEvent continues
  to use `PointState::LEAVE`.

### Parent Area and Touch Events

A child's hit-test area is not automatically restricted to its parent's bounds. A child outside the parent
may still be a normal TouchEvent coordinate candidate if it is hittable at that coordinate. The child is
excluded outside the parent only when the parent uses `CLIP_CHILDREN` or `CLIP_TO_BOUNDING_BOX`.

#### Example: When Red is the parent and Orange is the child

- At a coordinate where Red and Orange overlap, both may be normal TouchEvent coordinate candidates.
- Without clipping, Orange can receive a normal TouchEvent even at a coordinate outside Red.
- Red can receive InterceptTouchEvent because it is Orange's ancestor, even if Red is not a coordinate candidate.
- If Red does not intercept, its normal TouchEvent callback is not invoked unless Red is also a coordinate candidate.
- If Red intercepts, previous active recipients other than Red receive `INTERRUPTED`. If Red has a Touch
  callback, normal TouchEvent delivery begins at Red and proceeds toward the root. Red becomes the owner only
  if its Touch callback returns `true`.

```
          Blue
        /      \
   Yellow      Red
                 |
              Orange
```


---

## Blocking Touch Propagation

Use `Actor::SetTouchPropagationBlocked(true)` when an overlay must stop ordinary Touch from reaching
the background while allowing foreground children to keep recognizing gestures. `View` inherits this API.
The getter is `IsTouchPropagationBlocked()`; the default is `false`. This requires a Core version
providing the API (`@SINCE_2_5.46`), and only affects GEOMETRY Touch.

This setting controls **whether Touch continues to candidates behind this Actor**. An Actor's hit area
determines whether it is a candidate at a coordinate; blocking propagation changes neither its size nor its
hit area. The Actor can receive Touch and return `false`, while still preventing delivery to candidates
behind it. Below, a *propagation boundary* means the first Actor with this setting enabled.

```cpp
View overlay = View::New();
overlay.SetTouchPropagationBlocked(true);

// Optional: the boundary also works without a Touch callback.
overlay.TouchEventSignal().Connect(&tracker, [](Actor, const TouchEvent&)
{
  return false; // Observe without claiming the stream.
});
```

The boundary remains hit-test eligible without a Touch callback, subject to visibility, sensitivity and
hit-area checks. It is included in the candidate route; connected callbacks can still receive Touch.
Ordinary delivery stops after the first boundary in front-to-back order. A boundary is based on geometry
candidates, so it may also be a sibling behind the foreground content.

```text
Foreground item → Horizontal list → Vertical scroll → Overlay boundary
                                                        │
                                                        └─ stop ordinary Touch
                                                           Background is excluded
```

This diagram assumes those Actors are candidates at the DOWN coordinate. A parent boundary does not
automatically cover children protruding outside its hit area; size or clip the overlay appropriately.

| Operation | Result |
|---|---|
| Enable the boundary, with no callback or a `false` callback | Stop ordinary propagation without selecting an owner |
| A foreground View or the boundary actually consumes Touch | Select the consumer as owner and send `INTERRUPTED` to other active Touch recipients |
| A child consumes before reaching the boundary | The boundary has no special right to keep observing; if already active, it can receive `INTERRUPTED` |
| Disable, hide or remove the boundary during the stream | Do not reopen this stream to excluded background candidates; apply changes to a new stream |
| An ancestor outside the boundary explicitly intercepts | Allow the takeover; this API does not block interception |
| Hover, Wheel or PARENT Touch | Unaffected by this API |

Boundary Actors are captured at the first DOWN. That scope survives an internal restart within the same
routed stream. If a foreground sibling later intercepts, ordinary delivery must still stay in the initial
allowed scope: its parent route cannot reopen a background or outer boundary excluded at DOWN.
Cancellation notifications are not blocked by a boundary. An Actor that never received Touch does not need
a synthetic `INTERRUPTED` just because it is behind the boundary.

An unconditional `true` Touch callback on the overlay selects it as owner and interrupts active foreground
recipients. Remove that consuming callback when adopting the boundary API. Adding a `false` callback cannot
undo another callback's or an InteractiveTrait's consumption. Hover and Wheel need their own input policy;
do not install dummy gesture detectors merely to block background Touch.

---

## InterceptTouchEvent

InterceptTouchEvent follows an actual parent-child path, not the flat list of geometrically overlapping Actors.

### Event Propagation Method

- Until interception succeeds, traversal follows root → initial hit Actor, including the hit Actor.
- Selecting a Touch owner does not shorten this path. The owner itself and descendants below it can
  receive InterceptTouchEvent if they lie on this path and meet dispatch conditions.
- If the initial hit leaves the Scene while an established owner remains, traversal falls back to
  root → owner, including the owner.
- A sibling outside the initial hit path is not visited. If a sibling behind the hit becomes the Touch
  owner, interception still follows the initial hit branch, not that sibling's branch.

Ordinary Touch follows coordinate candidates before interception and the interceptor-to-root path after
interception. Captured propagation boundaries limit ordinary delivery, but do not disable intercept traversal.

### Event Propagation Example

The following example assumes that Blue → Yellow → Red → Orange is an actual parent-child hierarchy.

```
┌─────────────────────────────────────┐
│            Blue View                │
│  ┌───────────────────────────────┐  │
│  │        Yellow View            │  │
│  │  ┌─────────────────────────┐  │  │
│  │  │       Red View          │  │  │
│  │  │  ┌─────────────────┐    │  │  │
│  │  │  │   Orange View   │    │  │  │
│  │  │  └─────────────────┘    │  │  │
│  │  └─────────────────────────┘  │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

- If Orange is the initial hit, Blue → Yellow → Red → Orange remains the intercept path even when Red
  becomes the Touch owner. Red and Orange remain eligible for intercept callbacks.
- Ordinary Touch goes only to Red after owner selection; intercept observations can continue below Red.
- If every intercept callback returns `false`, normal TouchEvent delivery continues.

### Consume Behavior

Returning `true` from InterceptTouchEvent does not automatically make the interceptor the owner. It fixes the
normal TouchEvent delivery range to the `root → interceptor` hierarchy path. Ordinary delivery walks
from interceptor toward root, subject to captured propagation boundaries. An outside ancestor may explicitly
intercept; enabling a boundary does not disable interception.

When Yellow intercepts a later event from an existing owner, processing continues in this order:

1. The existing owner and previous ACTIVE recipients other than Yellow receive exactly one `INTERRUPTED`.
2. The current normal TouchEvent is delivered from Yellow to Yellow's parents and then toward the root.
3. The first Actor on this restricted path to return `true` becomes the new owner.
4. If no Actor consumes, the stream remains without an owner and later events reuse the same restricted path.
5. After a stream has been successfully intercepted, InterceptTouchEvent callbacks are not traversed again.

Therefore, Yellow becomes the owner in the common case where it has a Touch callback and that callback returns
`true`, but the InterceptTouchEvent result alone does not establish ownership.

```text
actual hierarchy: Root → Blue → Yellow → Red(owner) → Orange(initial hit)

intercept traversal: Root → Blue → Yellow(true)   [stops because Yellow returns true]
previous delivery:   Red → INTERRUPTED
normal Touch:        Yellow → Blue → Root         [first Actor returning true is the new owner]
```

### Event Propagation Example

The sequence will be as follows:

```
Blue View InterceptTouchDown return false
Yellow View InterceptTouchDown return true
Yellow View TouchDown return true
Yellow View TouchMotion return true
Yellow View TouchMotion return true
Yellow View TouchFinished return true
```

### Sample: Using InterceptTouchEvent

Blue and Yellow are actual ancestors of Red, and both have InterceptTouchEvent callbacks. Yellow consumes
the InterceptTouch.

**Behavior Process:**
1. Blue and Yellow receive InterceptTouch in root-to-target order.
2. When Yellow consumes it, traversal toward descendants and delivery to the existing target stop.
3. If Red was already an active normal TouchEvent recipient, Red first receives `INTERRUPTED`.
4. Normal TouchEvent delivery for the same event proceeds from Yellow toward Blue.
5. When Yellow's Touch callback returns `true`, as in this example, Yellow becomes the owner and handles the
   rest of the stream.

---

## HoverEvent

GEOMETRY HoverEvent hit-tests every input and visits the hoverable results from front to back. An Actor's
full dispatch result combines its intrinsic `OnHoverEvent()` result and every callback connected to the
same Actor with logical OR. If that result is `false`, delivery continues to the next geometry candidate;
if it is `true`, the candidate walk stops for the current input. Consumption does not capture future hover
inputs because the next input is hit-tested and evaluated again.

The Actors actually visited before the walk stops form the active-target prefix. A `false` Actor remains
active, continues to receive later `MOTION` inputs while hit, and may coexist with unrelated active siblings.

### Event States

- A new candidate encountered during `MOTION` receives synthetic `STARTED` and then the same `MOTION` input.
- An existing active candidate receives `MOTION`.
- After current candidates are dispatched, every previous active target not in the new visited prefix receives `LEAVE`.
- A hidden, insensitive, disabled, disconnected, removed, or reparented active target receives `INTERRUPTED` once.
- `FINISHED` and explicit `INTERRUPTED` are delivered to every active target regardless of consumption, then
  the active list is cleared.

GEOMETRY hover guarantees `STARTED` and `LEAVE` for every active target and does not consult
`LEAVE_REQUIRED`. PARENT hover and touch retain the property-controlled behavior.

### Event Propagation Example

Assume A and B are unrelated siblings, B is in front, and their bounds partially overlap:

```text
A-only area       A/B overlap       B-only area
┌──────── A ──────────┐
          ┌──────── B ──────────┐
```

Assume B returns `false` and A returns `true`. The trace is:

```text
A-only -> overlap : B STARTED -> B MOTION -> A MOTION
inside overlap    : B MOTION -> A MOTION
overlap -> B-only : B MOTION -> A LEAVE
B-only -> overlap : B MOTION -> A STARTED -> A MOTION
```

A and B are both active in the overlap because B's `false` result allows A to be visited. If B changes to
`true`, B receives the current event, the walk stops, and the previously active A receives `LEAVE` after B's
dispatch. If B later returns `false`, A re-enters with `STARTED` followed by `MOTION`.

An actual parent is not a separate bubbling destination in GEOMETRY mode. It is visited only at its position
in the flat hit list, after front-most child and sibling subtrees:

```text
B callback false : B -> A -> shared Parent -> Root (until consumed)
B callback true  : B (stop for this input)
```

Only hoverable Actors present in the geometry results participate. The callback's Actor argument identifies
the current receiver, while `HoverEvent::GetHitActor()` and local position continue to identify the primary,
front-most hit Actor (B in the overlap), even in A or Parent callbacks.

Plain `View` has no intrinsic hover consumption, so a raw callback returning `false` demonstrates sibling
fall-through. `InteractiveView`, or a `View` with `InteractiveTrait`, intrinsically handles hover and returns
`true` for its lifecycle states. Its full dispatch therefore normally stops at the front-most interactive
View even when an additional raw callback returns `false`.

### Consume Behavior

- Returning `false` continues to the next front-to-back geometry candidate, which can be an unrelated sibling.
- Returning `true` stops the current candidate walk; it does not create a future hover owner or capture.
- Every connected callback on one Actor still runs. Their results and intrinsic handling are OR-combined before
  deciding whether to continue to the next candidate.

---

## Gesture Detection by HandleEvent

There are two ways to use a GestureDetector in GEOMETRY mode:

1. `GestureDetector::Attach(view)` connects the detector to the View's TouchEvent.
2. Calling `GestureDetector::HandleEvent()` from a Touch callback enables custom arbitration, including interception.

Do not combine both methods for the same detector because doing so may feed the same event twice. The
following example does not call `Attach()` and manually feeds the detector from a Touch callback.

### Basic Usage

```cpp
// Create PanGestureDetector and connect its signal. Do not call Attach(view).
mPanGestureDetector = PanGestureDetector::New();
mPanGestureDetector.DetectedSignal().Connect(this, &YourClass::OnPan);

// Call HandleEvent in TouchEvent callback
bool YourClass::OnTouched(Actor actor, const TouchEvent& touch)
{
  // This may return false before the gesture is recognized.
  // A View whose callback ran remains an ACTIVE recipient while there is no owner.
  return mPanGestureDetector.HandleEvent(actor, touch);
}

void YourClass::OnPan(Actor /*actor*/, const PanGesture& /*pan*/)
{
  // Update the UI according to the Pan state.
}
```

### How It Works

1. **Receive TouchEvent**: Call `HandleEvent` on the desired GestureDetector, passing the View that should
   recognize the gesture and the TouchEvent.

2. **Analysis while UNOWNED**: Before the detector recognizes a gesture, `HandleEvent()` may return `false`.
   A View whose Touch callback ran remains an ACTIVE recipient and can continue receiving TouchEvents.

3. **Ownership selection**: `HandleEvent()` may return `true` after recognition. The View becomes the owner
   when its Touch callback returns that result, or the application handling decision, as `true`. Previous active recipients receive `INTERRUPTED` and clean up their recognizers.

4. **Owner retention**: A later `false` result from `HandleEvent()` does not release ownership. The View keeps
   receiving TouchEvents until stream termination or interception on the observation path.

---

### Recognition and Touch Ownership Are Separate Decisions

`HandleEvent()` advances a recognizer. If scrolling depends on direction, remaining scroll range or an edit
mode, return the application's decision from the Touch callback rather than always returning the detector's
recognition result. Keep feeding UP/INTERRUPTED so pending recognizers and pressed states are cleared.

A LongPress can be recognized by a timer between Touch callbacks. Its signal alone does not return `true`
from TouchEvent and therefore does not automatically establish a Touch owner at that instant.
`GestureDetector::CancelAllOtherGestureDetectors()` is a separate, explicit recognizer cancellation API.
Currently it cancels the other registered detectors in GestureEventProcessor; it is not scoped to one
Touch owner, Actor subtree or pointer stream. Do not use it indiscriminately when independent multi-touch
interactions must remain active.

---

## Gesture Propagation

A gesture signal itself does not propagate to another View. Before an owner is selected, however, several
Touch candidates at the coordinate may receive the same stream and advance their own GestureDetectors. This
is **gesture arbitration between touch candidates**, not gesture-signal propagation.

### Limitations of Gesture Recognition

- Gesture recognition requires **continuous touch events**.
- While there is no owner, a candidate that returned `false` on DOWN may recognize a gesture on Motion and
  become a late consumer.
- The first View to consume becomes the owner; other active recipients receive `INTERRUPTED`.
- After ownership is selected, normal TouchEvents no longer continue to other coordinate candidates.
- An Actor on the intercept observation path can intercept a later event, including the owner itself or
  a descendant below it. A new Touch consumer on the restricted path can then become the owner.

---

## Nested Scrolling with an Overlay

```text
Overlay (Touch propagation boundary)
└─ Vertical scroll                         ↑ ↓
   ├─ Horizontal row A                     ← →
   │  ├─ Item A1: Tap / LongPress
   │  └─ Item A2: Tap / LongPress
   └─ Horizontal row B                     ← →
      ├─ Item B1: Tap / LongPress
      └─ Item B2: Tap / LongPress
```

For custom scroll Views that feed Pan from ordinary Touch callbacks, let all required candidates receive
DOWN without consuming it. A direction-constrained detector or the application can then decide who handles
the movement:

```cpp
outerPan.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);
rowAPan.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
rowBPan.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
```

| Input | Decision before ownership |
|---|---|
| DOWN | Candidates observe; the overlay returns `false` and stops background propagation with its boundary |
| Horizontal drag | The row's Touch callback consumes after deciding it can scroll; other active Touch recipients are interrupted |
| Vertical drag | The row remains unconsumed; the vertical scroll recognizes and consumes |
| The row cannot scroll in that direction | Keep returning `false` before ownership so the next allowed candidate can decide |

**NUI also needed application logic to decide whether to propagate Pan and set `e.Handled`.**
For custom GEOMETRY Touch handling, that decision is expressed through the Touch consume result.
The boundary supplies the separate background-blocking policy. Once a View owns the stream, returning
`false` at a scroll edge does not hand the same drag to a parent; late takeover requires an eligible
interceptor or an explicit application coordination policy.

This is a Core routing pattern, not an assertion that the built-in `Dali::Ui::ScrollView` uses only normal
Touch callbacks. The built-in ScrollView feeds Pan through InterceptTouchEvent, then consumes ordinary
Touch after takeover. A child InteractiveTrait may consume DOWN; the parent's intercept observation can
still recognize scrolling. Avoid feeding the same event in both Intercept and Touch, and configure nested
scroll directions and priorities so an outer interceptor does not take an inner row's horizontal drag.
See [ScrollView](ScrollView.md) and [child-drag coordination](#coordinating-scrollview-with-a-child-drag).

Two rows can scroll independently when their fingers start in different routing groups. A PINCH/ROTATION
candidate inside the boundary can intentionally group both points. Keep separate detector state for
independent interactions; a shared detector or global cancellation policy can still couple them.

---

## Per-Device Gesture Recognition Options

The same gesture detector can recognize gestures differently depending on which input device
started them — a mouse, a touchscreen finger, a TV pointing remote, or one specific named device.
This is independent of the propagation mode above: it is resolved once per gesture sequence, from
the device that pressed down, whether that touch reached the detector through `Attach()` or through
`HandleEvent()`.

### GestureDeviceSelector

`Dali::GestureDeviceSelector` picks which devices a set of options or thresholds applies to:

```cpp
GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE);
GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON);
GestureDeviceSelector::ByDeviceName("Pointing Device"); // exact match, case-sensitive
```

When several selectors registered on the same detector match a gesture, the most specific one
wins: device name, then class and subclass, then class alone.

### Two layers

- **Detector-local options** — `PanGestureDetector`, `TapGestureDetector` and
  `LongPressGestureDetector` accept a complete `Options` snapshot per selector. `PinchGestureDetector`
  and `RotationGestureDetector` have no detector-local settings at all, even for a single global
  profile, so there is nothing to register here for them.

  ```cpp
  PanGestureDetector pan = PanGestureDetector::New();

  // Mouse: only start the pan for a roughly horizontal drag.
  PanGestureDetector::Options mouseOptions = pan.GetDefaultOptions();
  mouseOptions.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
  pan.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), mouseOptions);

  pan.Attach(view);
  ```

  `GetDeviceOptions(selector, options)` and `ClearDeviceOptions(selector)` complete the surface.

- **Application-wide `Dali::GestureThresholds`** — covers all five gesture types (pan, tap, long
  press, pinch, rotation). These are the recognition thresholds that used to be a single value for
  the whole application. Registering a per-device threshold reaches every detector in the process,
  including ones a dali-ui component creates internally (`ScrollView`, `InteractiveTrait`,
  `InputEditor`), not only detectors the application attaches itself.

  ```cpp
  // Mouse: needs a longer drag before a pan starts than the touch default.
  GestureThresholds::PanThresholds mouseThresholds = GestureThresholds::GetDefaultPanThresholds();
  mouseThresholds.SetMinimumDistance(40);
  GestureThresholds::SetPanThresholds(GestureDeviceSelector::ByDeviceClass(Device::Class::MOUSE), mouseThresholds);
  ```

| Gesture | Detector-local `Options` | `GestureThresholds` |
|---|---|---|
| Pan | touches range, motion age, angles | minimum distance, minimum pan events |
| Tap | taps range, receive-all-tap-events | multi-tap interval, holding time, motion distance |
| Long Press | touches range | minimum holding time |
| Pinch | — (no detector-local settings exist) | minimum distance, minimum touch events (before/after start) |
| Rotation | — (no detector-local settings exist) | minimum touch events (before/after start) |

### Relationship to UiConfig

[UiConfig](Configuration.md)'s gesture setters (`SetLongPressGestureMinimumHoldingTime()` and similar)
set the single global value that `GestureThresholds` falls back to when no device selector matches;
they must be called before `Apply()`. `GestureThresholds` is a separate API and can be set or
cleared at any time, including after `Apply()` — a matching selector always wins over the
`UiConfig`/default value, regardless of call order.

For a complete, runnable example, see the
[device-gesture-options sample](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/device-gesture-options).

---

## Example 1: Basic Gesture Recognition

### Scenario Description

- Orange, Red, and Yellow overlap at the same coordinate in front-to-back order.
- Orange's Touch callback returns `false`.
- Red's Touch callback returns the result of `TapGestureDetector::HandleEvent()`.
- Yellow's Touch callback returns the result of `LongPressGestureDetector::HandleEvent()`.
- Neither detector uses `Attach()`; each is fed manually from its Touch callback.

### Event Flow

```
┌─────────────────────────────────────┐
│            Blue View                │
│  ┌───────────────────────────────┐  │
│  │        Yellow View            │  │
│  │  (LongPressGestureDetector)   │  │
│  │  ┌──────────────────────────┐ │  │
│  │  │       Red View           │ │  │
│  │  │  (TapGestureDetector)    │ │  │
│  │  │  ┌─────────────────┐     │ │  │
│  │  │  │   Orange View   │     │ │  │
│  │  │  └─────────────────┘     │ │  │
│  │  └──────────────────────────┘ │  │
│  └───────────────────────────────┘  │
└─────────────────────────────────────┘
```

### Detailed Behavior

1. When Orange returns `false` on DOWN, Red and Yellow may also receive the same TouchEvent in order.

2. Even if `HandleEvent()` on Red and Yellow still returns `false`, both remain ACTIVE recipients for later
   events while there is no owner.

3. When either detector first returns `true`, its View becomes the owner. Every previous active recipient
   other than the new owner receives `INTERRUPTED`.

### RedView TouchEvent Callback

```cpp
bool RedTouched(Actor actor, const TouchEvent& touch)
{
  return mTapGestureDetector.HandleEvent(actor, touch);
}

bool YellowTouched(Actor actor, const TouchEvent& touch)
{
  return mLongPressGestureDetector.HandleEvent(actor, touch);
}
```

### Result

- If Red does not consume DOWN in advance, Yellow can receive later TouchEvents and analyze a LongPress.
- If Red's Tap selects ownership first, previous active recipients, including Yellow, are terminated.
- If Yellow's LongPress selects ownership first during a delivered TouchEvent, Red and Orange receive
  `INTERRUPTED`, and Yellow handles the rest of the stream.
- If Red must be exclusive from DOWN, its callback should return `true` regardless of the `HandleEvent()`
  result. Yellow then receives no later TouchEvent for that stream.

### Key Point

> Calling `HandleEvent()` does not by itself select an owner. In GEOMETRY mode, ownership is selected when
> a callback actually returns `true`; until then, several coordinate candidates can analyze the gesture.

---

## Example 2: Gesture Handling Using InterceptTouchEvent

### Scenario

OrangeView should receive LongPressGesture, while **YellowView, an actual ancestor of OrangeView**, should
recognize PanGesture.

- After a LongPress on OrangeView, crossing the pan threshold causes YellowView to intercept the stream and
  cancel OrangeView's LongPress.
- If YellowView is merely an overlapping sibling behind OrangeView, its InterceptTouchEvent is not called.

```text
YellowView (parent, Pan)
└── OrangeView (child, LongPress)
```

### Implementation Code

```cpp
class LongPanGestureTest : public ConnectionTracker
{
public:
  void Setup(View yellow, View orange)
  {
    mYellowView = yellow;
    mOrangeView = orange;
    mYellowView.Add(mOrangeView); // Establish the hierarchy required for interception.

    mOrangeView.TouchEventSignal().Connect(this, &LongPanGestureTest::OnOrangeTouched);

    // Feed both detectors manually; do not call Attach().
    mLongPressGestureDetector = LongPressGestureDetector::New();
    mLongPressGestureDetector.DetectedSignal().Connect(this, &LongPanGestureTest::OnLongPress);

    mPanGestureDetector = PanGestureDetector::New();
    mPanGestureDetector.DetectedSignal().Connect(this, &LongPanGestureTest::OnPan);

    mYellowView.InterceptTouchEventSignal().Connect(this, &LongPanGestureTest::OnYellowInterceptTouch);
    mYellowView.TouchEventSignal().Connect(this, &LongPanGestureTest::OnYellowTouched);

    mIsDetected      = false;
    mJustIntercepted = false;
  }

private:
  bool OnOrangeTouched(Actor actor, const TouchEvent& touch)
  {
    mLongPressGestureDetector.HandleEvent(actor, touch);
    return true; // Keep Orange as the owner from DOWN.
  }

  void OnLongPress(Actor /*actor*/, const LongPressGesture& /*gesture*/)
  {
    // Update the Orange UI according to the LongPress state.
  }

  void OnPan(Actor /*actor*/, const PanGesture& pan)
  {
    if(pan.GetState() == GestureState::FINISHED || pan.GetState() == GestureState::CANCELLED)
    {
      mIsDetected = false;
      mYellowView.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::YELLOW);
    }
    else
    {
      mIsDetected = true;
      mYellowView.SetProperty(Actor::Property::COLOR_MULTIPLIER, Color::YELLOW * 0.7f);
    }
  }

  // Yellow is an ancestor of the Orange owner, so it can observe later events.
  // On the event that recognizes Pan, restrict normal Touch delivery to Yellow→root.
  bool OnYellowInterceptTouch(Actor actor, const TouchEvent& touch)
  {
    const bool wasDetected = mIsDetected;
    mPanGestureDetector.HandleEvent(actor, touch);
    mJustIntercepted = !wasDetected && mIsDetected;
    return mIsDetected;
  }

  bool OnYellowTouched(Actor actor, const TouchEvent& touch)
  {
    // The event that begins interception was already fed in the callback above.
    if(mJustIntercepted)
    {
      mJustIntercepted = false;
      return true; // Normal Touch consumption makes Yellow the owner.
    }

    mPanGestureDetector.HandleEvent(actor, touch);
    return true;
  }

private:
  View                     mOrangeView;
  View                     mYellowView;
  LongPressGestureDetector mLongPressGestureDetector;
  PanGestureDetector       mPanGestureDetector;
  bool                     mIsDetected;
  bool                     mJustIntercepted;
};
```

### Behavior Process

```
┌─────────────────────────────────────────────────────────────────┐
│  1. LongPress on OrangeView → LongPressGesture occurs           │
│                                                                 │
│  2. Touch movement starts → ancestor YellowView observes the    │
│     event along the intercept path                              │
│                                                                 │
│  3. Movement exceeds threshold → PanGesture recognized          │
│     - YellowView.InterceptTouchEvent return true (consume)      │
│     - OrangeView receives Interrupted                           │
│     - LongPressGesture ends                                     │
│     - YellowView.TouchEvent return true → Yellow becomes owner  │
│                                                                 │
│  4. Subsequent touches → YellowView receives PanGesture         │
└─────────────────────────────────────────────────────────────────┘
```

### Detailed Description

1. **Long-pressing OrangeView**: LongPressGesture occurs.

2. **Moving touch in this state**: After the threshold is crossed, YellowView returns `true` from
   `InterceptTouchEvent`. Because OrangeView is an active recipient, it receives exactly one `Interrupted`
   and cleans up its LongPressGesture.

3. **When Yellow's Touch callback returns `true` for the same event**, YellowView becomes the new owner and
   receives later PanGesture input. If only the Intercept callback returns `true` and no normal Touch callback
   consumes, Yellow's ownership is not established.

### Role of InterceptTouchEvent

`InterceptTouchEvent` lets an Actor on the initial hit observation path restrict normal Touch delivery
to itself and the path toward the root, subject to propagation boundaries:

- **return false**: Deliver touch event to child view (default behavior)
- **return true**: Terminate previous active recipients other than the interceptor with `INTERRUPTED`, then
  begin normal Touch delivery from the interceptor toward the root

The first Actor to return `true` during that normal Touch delivery becomes the new owner. The interceptor
usually becomes the owner by consuming its own Touch callback, but that is a separate consume decision.

A sibling outside the initial hit path cannot observe through interception. To let such a sibling compete,
keep normal Touch unconsumed until it can decide. Once another Actor owns the stream, ordinary late
consumption from an excluded sibling is no longer available.

This allows flexible control of touch events in complex gesture scenarios.

---

## Summary

| Situation | Behavior |
|------|------|
| TouchEvent `true` while there is no owner | The first consumer becomes the stable owner |
| TouchEvent `false` while there is no owner | Delivery continues to the next coordinate candidate; late consumption remains possible |
| Another Actor becomes owner | Every previous active recipient except the new owner receives exactly one `INTERRUPTED` |
| Owner returns `false` on a later TouchEvent | Ownership remains and the owner continues receiving the stream |
| InterceptTouchEvent `true` | An Actor on the observation path restricts ordinary delivery to interceptor→root, subject to propagation boundaries |
| TouchEvent `true` after interception | The first consumer on the restricted path becomes the new owner |
| InterceptTouchEvent outside the initial hit path | Not called while the initial hit remains on Scene |
| Touch propagation boundary enabled | Ordinary delivery stops at the boundary without selecting an owner; cancellation and interception remain available |

### Stream Termination, Multi-touch, and TouchEvent Data

- `UP` detaches its device from the stream; the last active device's `UP` ends the stream.
- A raw `INTERRUPTED` is a global cancellation delivered to every active geometry stream without first
  splitting the event by point route. In a stream without a fixed interceptor, intercept callbacks run from
  the root before terminal TouchEvents are delivered to ACTIVE recipients. An intercept result cannot suppress
  termination or create a new owner.
- If a new DOWN arrives from a device ID already attached to an active stream, the entire old stream is
  cancelled first. Every ACTIVE recipient in that stream receives exactly one `INTERRUPTED`, and only then
  does hit testing and routing begin for the new DOWN.
- If a tracked primary target or owner becomes insensitive or unhittable, that ACTIVE recipient is
  terminated with `INTERRUPTED` during processing. Removing an established owner ends its stream. Removing
  a different initial hit after ownership is established preserves the owner's stream, render task and
  physical UP; interception falls back to the owner. Without an established owner, disconnecting the tracked
  target terminates the remaining recipients.
- A different non-owner ACTIVE recipient becoming temporarily ineligible does not transfer ownership. It is
  excluded from normal TouchEvent delivery while ineligible and remains subject to terminal cleanup when a
  new owner is selected or the stream ends.
- The first `Actor` argument of a Touch callback is the actual recipient.
- `TouchEvent::GetHitActor(pointIndex)` remains the stream's initial geometry hit Actor, not the callback
  recipient. Use `GetHitActor(0)` for the first point.
- Every point's local position is transformed into the actual callback recipient's coordinate system.
- An Actor with `DISPATCH_TOUCH_MOTION=false` still receives an event that mixes Motion with `UP` or
  `INTERRUPTED`, so a terminal point is not lost.
- A PINCH/ROTATION candidate can group points starting on different child Actors into one stream. Without
  such a candidate, grouping uses the initial hit Actor ID. Boundary filtering precedes this decision, so
  a background PINCH/ROTATION detector beyond the boundary cannot merge foreground streams.
  The grouping requirement comes from detectors attached to the Actor; manually calling `HandleEvent()`
  alone does not register that Actor as a grouping candidate.
- Different initial routing groups remain independent even if they later select the same Touch owner.
  One pointer's UP does not terminate the remaining pointers in its stream.

```text
Without a PINCH/ROTATION grouping candidate:
device 4 DOWN on Actor A ─┐
                         ├─ Stream 1 (initial routing group = Actor A ID)
device 7 DOWN on Actor A ─┘

device 9 DOWN on Actor B ─── Stream 2 (initial routing group = Actor B ID)

Stream 1 owner = Parent P
Stream 2 owner = Parent P    ← streams remain independent even with the same owner
```

#### Scene TouchEventSignal

`Scene::TouchEventSignal()` is emitted once for a raw physical boundary, independently of whether an Actor
callback consumes an event.

| Raw device-state transition | Scene signal |
|------|------|
| Zero active devices → one or more | Emit the first DOWN event once |
| Motion, intermediate device addition, or device termination while another remains active | Do not emit |
| One or more active devices → zero | Emit the last UP event once |
| Raw `INTERRUPTED` | Emit once and clear all active-device state |

Splitting one raw event across several geometry streams does not increase the Scene signal count. All Actor
callbacks for those streams run before the Scene callback.

### Coordinating ScrollView with a Child Drag

`ScrollView` uses `InterceptTouchEvent` and `PanGestureDetector::HandleEvent()`
internally. When its pan threshold is reached it consumes the touch sequence,
which is correct for normal scrolling but conflicts with a child-owned
drag-and-drop gesture.

Suspend only pan scrolling while the child drag is active:

```cpp
void OnDragStarted(View, DragAndDropDetector)
{
  scrollView.SetPanScrollEnabled(false);
}

void OnDragEnded(View, DragAndDropDetector)
{
  scrollView.SetPanScrollEnabled(true);
}
```

While disabled, `ScrollView` does not intercept the child touch sequence and
does not move content from pan displacement. Programmatic APIs such as
`ScrollTo()` remain available, so an application can still implement
drag-and-drop edge auto-scroll.

Returning `true` from the child touch callback is not sufficient to prevent a
parent `ScrollView` from intercepting later motion events. The parent
interception policy must remain disabled for the active child drag.

For complete source/target registration, custom preview, target acceptance, and
edge auto-scroll examples, see
[In-Scene Drag and Drop](https://github.sec.samsung.net/NUI/dali-ui/wiki/In-Scene-Drag-and-Drop).

---

## Touch Boundary Diagnostics

When ordinary delivery stops at a captured boundary without a consumer, Core emits a diagnostic such as:

```text
Touch propagation stopped by SetTouchPropagationBlocked(true): boundaryId(4), name(Overlay), deviceId(2), state(UP)
```

Each changed point is reported with its own device ID and state. DOWN/UP use ordinary release logging;
MOTION uses the debug Touch processor filter. STATIONARY points are omitted. Thus `[STATIONARY, MOTION]`
does not flood ordinary logs, and `[MOTION, UP]` still reports the second point's UP with the filter off.

```sh
LOG_TOUCH_PROCESSOR=1 ./your-app  # Include MOTION diagnostics in a debug-enabled Core build.
```

An earlier consumer or a cancellation/disconnection stop does not produce this boundary-stop diagnostic.
It reports the boundary captured for the stream, even if the current setting was later disabled.

## Source References

This description was checked against the local implementation on 2026-10-29:

- `dali-core/dali/internal/event/events/geometry-touch-stream-router.cpp`: grouping, point routes and Scene signal boundaries.
- `dali-core/dali/internal/event/events/geometry-touch-event-processor.cpp`: owner/intercept paths, cancellation, lifetime and logging.
- `dali-core/dali/internal/event/events/geometry-touch-propagation.h`: captured ordinary-Touch boundaries.
- `dali-core/dali/internal/event/events/gesture-detector-impl.cpp` and `gesture-event-processor.cpp`: HandleEvent and explicit detector cancellation.
- `dali-core/automated-tests/src/dali/utc-Dali-GeoTouchStream.cpp`: regression scenarios for boundaries, interception, removal and independent/grouped pointers.
- `dali-ui-foundation/integration-api/scroll-view-impl.cpp` and `internal/views/view/interactive-trait-impl.cpp`: built-in ScrollView and InteractiveTrait input handling.

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
