[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Touch-&-Gesture-(kr))

# Touch & Gesture Behavior and Code Implementation Guide


## Table of Contents
1. [Scope](#scope)
2. [TouchEvent](#touchevent)
3. [InterceptTouchEvent](#intercepttouchevent)
4. [HoverEvent](#hoverevent)
5. [Gesture Detection by HandleEvent](#gesture-detection-by-handleevent)
6. [Gesture Propagation](#gesture-propagation)
7. [Example 1: Basic Gesture Recognition](#example-1-basic-gesture-recognition)
8. [Example 2: Gesture Handling Using InterceptTouchEvent](#example-2-gesture-handling-using-intercepttouchevent)

---

## Scope

This document describes **GEOMETRY touch propagation**, which DALi UI enables by default when the
application is created. Setting `GeometryHittestEnabled` to `false` selects PARENT propagation from
the primary hit Actor toward its parents; that mode does not use the coordinate-candidate and late-consume
behavior described below. Select this setting during application startup and do not change it after input
event processing has started.

This document uses the following terms:

- **device**: the device ID that identifies a point in a `TouchEvent`, normally for the lifetime of one pointer
- **initial routing group**: the Actor ID hit by the initial DOWN; devices starting with the same ID join the same stream
- **stream**: the TouchEvent delivery unit that owns the devices, candidates, recipients, and owner state for one initial routing group
- **coordinate candidate**: a front-to-back TouchEvent candidate selected by the geometry hit test at the initial DOWN coordinate
- **owner**: the first Actor to consume a TouchEvent and handle delivery until stream termination or interception by an ancestor
- **recipient**: an Actor whose TouchEvent callback has actually been invoked at least once for the stream
- **interceptor**: an Actor that returns `true` from an InterceptTouchEvent callback on the ancestor path and restricts the previous delivery range
- **ancestor path**: the actual parent-child path from the root toward a target

Devices and streams are related as follows:

```text
device 4 ─┐
          ├─ same initial hit Actor ID ──> Stream 1
device 7 ─┘

device 9 ─── different initial hit ID ──> Stream 2

Each Stream
├─ coordinate candidates
├─ ACTIVE/TERMINATED recipients
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
- intercepted but no new owner: the fixed interceptor-to-root path

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
owner callback does not release ownership. An ancestor of the owner may still intercept a later event.

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
only an intercept callback, but no normal TouchEvent callback, is not a recipient for this rule.

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

## InterceptTouchEvent

InterceptTouchEvent follows an actual parent-child path, not the flat list of geometrically overlapping Actors.

### Event Propagation Method

- Before an owner exists, the initial hit Actor's ancestor path is checked from the root toward the target.
- After an owner is selected, the current owner's ancestor path is checked from the root up to, but excluding,
  the owner. The owner does not receive its own intercept callback.
- An overlapping sibling branch is not visited because it is not part of the owner's ancestry.

**InterceptTouchEvent follows hierarchy order. Normal TouchEvent follows coordinate-candidate order before
interception and the fixed interceptor-to-root order afterward.**


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

- If Red is the current owner, InterceptTouchEvent is called on Blue → Yellow. Red's own intercept callback is excluded.
- If there is no owner and Orange is the initial hit Actor, the Blue → Yellow → Red → Orange path may be checked.
- If every intercept callback returns `false`, normal TouchEvent delivery continues.

### Consume Behavior

Returning `true` from InterceptTouchEvent does not automatically make the interceptor the owner. It fixes the
normal TouchEvent delivery range to the `root → interceptor` hierarchy path.

When Yellow intercepts a later event from an existing owner, processing continues in this order:

1. The existing owner and previous ACTIVE recipients other than Yellow receive exactly one `INTERRUPTED`.
2. The current normal TouchEvent is delivered from Yellow to Yellow's parents and then toward the root.
3. The first Actor on this restricted path to return `true` becomes the new owner.
4. If no Actor consumes, the stream remains without an owner and later events reuse the same restricted path.
5. After a stream has been successfully intercepted, InterceptTouchEvent callbacks are not traversed again.

Therefore, Yellow becomes the owner in the common case where it has a Touch callback and that callback returns
`true`, but the InterceptTouchEvent result alone does not establish ownership.

```text
actual hierarchy: Root → Blue → Yellow → Red(owner)

intercept traversal: Root → Blue → Yellow(true)   [Red owner is excluded]
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

HoverEvent follows the geometry candidates at the coordinate from front to back.

### Event States

- When entering a view, it enters the **Started** state, and when exiting the view, it enters the **Leave** state.

### Event Propagation Example

In the figure below, if the hover moves to the right from the orange view:
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

```
Orange → Red → Yellow → Blue receive "Started" events.
```

- When exiting the Orange view, the Orange view receives a "Leave" event. Other Red, Yellow, and Blue views continue to receive "Motion" events.
- When exiting the Red view, the Red view receives a "Leave" event. Other Yellow and Blue views continue to receive "Motion" events.

### Reverse Movement

If the hover moves back from right to left:
- When entering a new view, that view receives "Started".
- Views that were already receiving Motion events continue to receive Motion.

### Sample: HoverEvent Behavior

Register HoverEvent on Yellow, Red, and Orange.

**Moving mouse from left to right:**
1. When entering Yellow, Yellow receives "Started".
2. When entering Red, Red receives "Started". Existing Yellow continues to receive Motion.
3. When entering Orange, Orange receives "Started". Existing Yellow and Red continue to receive Motion.

**Moving mouse from right to left:**
1. When exiting Orange, Orange receives "Leave". Existing Yellow and Red continue to receive Motion.
2. When exiting Red, Red receives "Leave". Existing Yellow continues to receive Motion.
3. When exiting Yellow, Yellow receives "Leave".

### Consume Behavior

- When an Actor consumes, geometry candidates behind it do not receive that event.
- Unlike touch, consuming does not mean the consuming view receives all subsequent Hover events.
- It only stops the current event from advancing to candidates behind it.

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

3. **Ownership selection**: When the detector recognizes a gesture and `HandleEvent()` returns `true`, the
   View becomes the owner. Previous active recipients receive `INTERRUPTED` and clean up their recognizers.

4. **Owner retention**: A later `false` result from `HandleEvent()` does not release ownership. The View keeps
   receiving TouchEvents until stream termination or interception by an ancestor.

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
- An actual ancestor of the owner may intercept a later Motion, restrict the delivery path, and let a new
  Touch consumer on that path become the owner.

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
      mYellowView.SetProperty(Actor::Property::COLOR, Color::YELLOW);
    }
    else
    {
      mIsDetected = true;
      mYellowView.SetProperty(Actor::Property::COLOR, Color::YELLOW * 0.7f);
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

`InterceptTouchEvent` lets an actual ancestor on the path to the current owner restrict normal Touch delivery
to itself and the path toward the root:

- **return false**: Deliver touch event to child view (default behavior)
- **return true**: Terminate previous active recipients other than the interceptor with `INTERRUPTED`, then
  begin normal Touch delivery from the interceptor toward the root

The first Actor to return `true` during that normal Touch delivery becomes the new owner. The interceptor
usually becomes the owner by consuming its own Touch callback, but that is a separate consume decision.

An overlapping sibling is not part of this path. To let a sibling behind the owner recognize a gesture
later, use normal TouchEvent coordinate candidates and late consumption.

This allows flexible control of touch events in complex gesture scenarios.

---

## Summary

| Situation | Behavior |
|------|------|
| TouchEvent `true` while there is no owner | The first consumer becomes the stable owner |
| TouchEvent `false` while there is no owner | Delivery continues to the next coordinate candidate; late consumption remains possible |
| Another Actor becomes owner | Every previous active recipient except the new owner receives exactly one `INTERRUPTED` |
| Owner returns `false` on a later TouchEvent | Ownership remains and the owner continues receiving the stream |
| InterceptTouchEvent `true` | An actual ancestor of the current owner restricts normal Touch delivery to the interceptor→root path |
| TouchEvent `true` after interception | The first consumer on the restricted path becomes the new owner |
| InterceptTouchEvent on an overlapping sibling | Not called because the sibling is not in the ancestry path |

### Stream Termination, Multi-touch, and TouchEvent Data

- `UP` detaches its device from the stream; the last active device's `UP` ends the stream.
- A raw `INTERRUPTED` is a global cancellation delivered to every active geometry stream without first
  splitting the event by point route. In a stream without a fixed interceptor, intercept callbacks run from
  the root before terminal TouchEvents are delivered to ACTIVE recipients. An intercept result cannot suppress
  termination or create a new owner.
- If a new DOWN arrives from a device ID already attached to an active stream, the entire old stream is
  cancelled first. Every ACTIVE recipient in that stream receives exactly one `INTERRUPTED`, and only then
  does hit testing and routing begin for the new DOWN.
- If the tracked initial hit or current owner becomes insensitive or unhittable, that ACTIVE recipient is
  terminated with `INTERRUPTED` during the next processing pass. Disconnecting a tracked Actor from the Scene
  terminates all ACTIVE recipients and clears the stream.
- A different non-owner ACTIVE recipient becoming temporarily ineligible does not transfer ownership. It is
  excluded from normal TouchEvent delivery while ineligible and remains subject to terminal cleanup when a
  new owner is selected or the stream ends.
- The first `Actor` argument of a Touch callback is the actual recipient.
- `TouchEvent::GetHitActor(pointIndex)` remains the stream's initial geometry hit Actor, not the callback
  recipient. Use `GetHitActor(0)` for the first point.
- Every point's local position is transformed into the actual callback recipient's coordinate system.
- An Actor with `DISPATCH_TOUCH_MOTION=false` still receives an event that mixes Motion with `UP` or
  `INTERRUPTED`, so a terminal point is not lost.
- Multi-touch points starting with the same initial hit Actor ID, or initial routing group, share one stream.
  Different initial routing groups remain independent even if they later select the same owner, and one
  pointer's UP does not terminate the remaining pointers.

```text
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

## References

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
