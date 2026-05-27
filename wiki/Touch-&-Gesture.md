[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Touch-&-Gesture-(kr))

# Touch & Gesture Behavior and Code Implementation Guide


## Table of Contents
1. [TouchEvent](#touchevent)
2. [InterceptTouchEvent](#intercepttouchevent)
3. [HoverEvent](#hoverevent)
4. [Gesture Detection by HandleEvent](#gesture-detection-by-handleevent)
5. [Gesture Propagation](#gesture-propagation)
6. [Example 1: Basic Gesture Recognition](#example-1-basic-gesture-recognition)
7. [Example 2: Gesture Handling Using InterceptTouchEvent](#example-2-gesture-handling-using-intercepttouchevent)

---

## TouchEvent

Hit testing and event propagation occur along the geometry Z-axis.

### Handling Touch Events

A View receives touch events via `TouchedSignal`. Note that the callback receives an `Actor` (not a `View`), so you need to downcast it if you want to use View-specific APIs.

Using a member function:

```cpp
class MyController : public ConnectionTracker
{
public:
  void SetupTouchHandler(View view)
  {
    view.TouchedSignal().Connect(this, &MyController::OnTouched);
  }

private:
  bool OnTouched(Actor actor, const TouchEvent& touch)
  {
    // Downcast Actor to View
    View view = View::DownCast(actor);

    if(touch.GetState(0) == PointState::DOWN)
    {
      // Handle touch down
      return true;  // consumed — this View will receive subsequent touch events
    }
    return false;  // let the event propagate to views below
  }
};
```

The same can be written concisely with a lambda:

```cpp
view.TouchedSignal().Connect(&tracker, [](Actor actor, const TouchEvent& touch) -> bool {
  View view = View::DownCast(actor);

  if(touch.GetState(0) == PointState::DOWN)
  {
    // Handle touch down
    return true;  // consumed
  }
  return false;  // let the event propagate
});
```

> [!NOTE]
> Return `true` (consume) to continue receiving subsequent touch events (Motion, Finished) on this View. If you return `false`, the event propagates to the next View in the geometry Z-order, and you will **not** receive follow-up events for this touch sequence.

<br/>

### Event Propagation Method

When touching the screen with 4 views overlapping as shown in the figure below, hit testing is performed along the geometry direction based on the touched coordinates.

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

- If a TouchEvent is registered on the "Red" view, the "Red" view is hit, and the event propagates from the "Red" view in the geometry child direction.
- If `return false` is used in the "Red" view's TouchEvent, the event propagates in the direction: "Red" → "Yellow" → "Blue".

### Consume Behavior

Returning `true` in TouchEvent means consume, indicating that this view will receive all subsequent TouchEvents.

If `return true` is used in the "Yellow" view's TouchEvent, it means the Yellow view will consume the TouchEvent, so only the Yellow view will receive subsequent TouchEvents.

### Event Propagation Example

The sequence will be as follows:

```
Red View TouchDown return false
Yellow View TouchDown return true
Yellow View TouchMotion return true
Yellow View TouchMotion return true
Yellow View TouchFinished return true
```

### Touch Event Propagation Characteristics

- Touch event propagation occurs along the geometry Z-axis regardless of parent-child relationships.
- By default, a view can only continue to receive touch events that started from itself.
- This is equivalent to having both `AllowOnlyOwnTouch` and `GrabTouchAfterLeave` options enabled.
- These two options are not separately provided in the new touch system. They are enabled by default.
- `Leaved` has been removed.

### Parent Area and Touch Events

When a touch event is delivered, if the touch occurs outside the parent's area, the touch event is not delivered. (Same as Android)

#### Example: When Red is the parent and Orange is the child

- When touching Orange, if the touched coordinate is outside the parent's area, the touch event is not delivered.
- When touching an area where Red and Orange overlap, both Red and Orange can receive intercept and touch events.
- When touching an Orange area outside the Red area, Orange cannot receive intercept or touch events.

```
          Blue
        /      \
   Yellow      Red
                 |
              Orange
```


---

## InterceptTouchEvent

Hit testing and event propagation occur along the geometry Z-axis.

### Event Propagation Method

- InterceptTouchEvent propagates through **parent-child relationships**. (Same as Android)
- When sibling ViewA (below) and ViewB (above) overlap, if ViewA intercepts, ViewB will not receive touch events.

**InterceptTouchEvent propagates from parent to child based on the hit View, while TouchEvent propagates in Z-order.**


### Event Propagation Example

When touching with 4 views overlapping as shown in the figure below, hit testing is performed along the child geometry direction based on the touched coordinates.

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

- If an InterceptTouchEvent is registered on the "Blue" view based on the touched coordinates, the InterceptTouchEvent is called on the "Blue" view, and the event propagates in the parent geometry direction to children.
- If `return false` is used in the "Blue" view's InterceptTouchEvent, the event propagates in the direction: "Blue" → "Yellow" → "Red".

### Consume Behavior

Returning `true` in InterceptTouchEvent means consume, indicating that this view will receive all subsequent TouchEvents.

If `return true` is used in the "Yellow" view's InterceptTouchEvent, the Yellow view has intercepted the touch, so only the Yellow view will receive subsequent TouchEvents.

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

InterceptTouchEvent is registered on Blue and Yellow. Additionally, Yellow consumes the InterceptTouch.

**Behavior Process:**
1. On touch, Blue and Yellow receive InterceptTouch.
2. Since Yellow consumed the interceptTouch, Yellow will receive all subsequent touches.
3. Therefore, even if TouchEvent is registered on Red, Yellow receives the TouchEvent.

---

## HoverEvent

Event propagation occurs along the geometry Z-axis.

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
1. When entering Yellow, Yellow receives "Down" ("Started").
2. When entering Red, Red receives "Down". Existing Yellow continues to receive Motion.
3. When entering Orange, Orange receives "Down". Existing Yellow and Red continue to receive Motion.

**Moving mouse from right to left:**
1. When exiting Orange, Orange receives "Leave". Existing Yellow and Red continue to receive Motion.
2. When exiting Red, Red receives "Leave". Existing Yellow continues to receive Motion.
3. When exiting Yellow, Yellow receives "Leave".

### Consume Behavior

- When consuming, child views do not receive events.
- Unlike touch, consuming does not mean the consuming view receives all subsequent Hover events.
- It only blocks event propagation to child views. (Same as Android)

---

## Gesture Detection by HandleEvent

Gestures are performed by calling `HandleEvent` within TouchEvent.

### Basic Usage

```csharp
// Create PanGestureDetector and connect signal
PanGestureDetector panGestureDetector = PanGestureDetector::New();
panGestureDetector.DetectedSignal().Connect(this, &YourClass::OnPan);

// Call HandleEvent in TouchEvent callback
bool YourClass::OnTouched(Actor actor, const TouchEvent& touch)
{
    // Must return true for the actor to continue receiving touch events for gesture recognition
    bool consumed = tapGestureDetector.HandleEvent(actor, touch);
    return consumed;
}


void YourClass::OnPan(Actor actor, const PanGesture& pan)
{
    DALI_LOG_ERROR("OnPan\n");
}
```

### How It Works

1. **Receive TouchEvent**: When receiving a TouchEvent, call `HandleEvent` on the desired GestureDetector. Pass the view where the gesture should be recognized and the touch event as arguments.

2. **Touch Consume Required**: After `HandleEvent`, the view must **consume** the subsequent touch events to continue receiving touches for gesture recognition.

3. **If Not Consumed**: If `return false` is used to pass the TouchEvent after `GestureDetector.HandleEvent`, the view will not receive subsequent TouchEvents and cannot recognize gestures.

---

## Gesture Propagation

To recognize a gesture in a View's TouchEvent, the View must **consume** the touch. Therefore, gestures cannot be propagated to child views.

### Limitations of Gesture Recognition

- Gesture recognition requires **continuous touch events**.
- Whether a gesture is recognized or not, the consume decision for TouchEvent cannot be changed later.
- If a specific View calls `HandleEvent` for gesture recognition, child views will not receive touch events and cannot recognize gestures.

---

## Example 1: Basic Gesture Recognition

### Scenario Description

- Register TouchEvent on all Blue, Yellow, Red, and Orange views.
- Register `TapGestureDetector` on Red and `LongPressGestureDetector` on Yellow.

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

1. On touch, **Orange first** receives the touch event, and since it didn't consume it, the next **RedView** receives the touch event.

2. In RedView's TouchEvent, gesture recognition starts through `GestureDetector.HandleEvent()`.

3. For gesture recognition to work, the View must consume the event, so **TouchEvent is consumed in RedView**.

### RedView TouchEvent Callback

```csharp
bool RedTouched(Actor actor, const TouchEvent& touch)
{
    DALI_LOG_ERROR(" ->Red View %d\n", touch.GetState(0));
    return redConsumed || tapGestureDetector.HandleEvent(actor, touch);
}
```

### Result

- **YellowView below does not receive touch events.**

- If you tap/release Orange once, RedView receives TapGesture and `OnTap` log is printed.

- If you long-press on YellowView, LongPressGesture is received and `OnLong` log is printed.

- **If you long-press on RedView**: Since RedView consumed TouchEvent for TapGesture, TouchEvent is not delivered to YellowView, and YellowView cannot receive LongPressGestureEvent.

### Key Point

> If a specific View calls `HandleEvent` for gesture recognition, child views will not receive touch events and cannot recognize gestures. This is the same in Android, which does not provide gesture propagation separately.

---

## Example 2: Gesture Handling Using InterceptTouchEvent

### Scenario

When OrangeView should receive LongPressGesture, and YellowView below should receive PanGesture, how can this be achieved?

- When you long-press OrangeView and then move the touch, the LongPress is released, and you want YellowView to receive PanGesture.

### Implementation Code

```csharp
class LongPanGestureTest
{
public:
    void Setup()
    {
        // OrangeView TouchEvent setup
        orangeView.TouchedSignal().Connect(this, &LongPanGestureTest::OnOrangeTouched);

        // Create PanGestureDetector and connect signal
        mPanGestureDetector = PanGestureDetector::New();
        mPanGestureDetector.DetectedSignal().Connect(this, &LongPanGestureTest::OnPan);

        // YellowView InterceptTouchEvent setup
        yellowView.InterceptTouchedSignal().Connect(this, &LongPanGestureTest::OnYellowInterceptTouch);

        // YellowView TouchEvent setup
        yellowView.TouchedSignal().Connect(this, &LongPanGestureTest::OnYellowTouched);

        mIsDetected = false;
    }

private:
    bool OnOrangeTouched(Actor actor, const TouchEvent& touch)
    {
        DALI_LOG_ERROR(" ->orangeView touch %d\n", touch.GetState(0));
        mLongPressGestureDetector.HandleEvent(actor, touch);
        return true;
    }

    void OnPan(Actor actor, const PanGesture& pan)
    {
        DALI_LOG_ERROR(" ->yellowView OnPan %d %d\n", pan.GetState(), mIsDetected);
        if (pan.GetState() == GestureState::FINISHED || pan.GetState() == GestureState::CANCELLED)
        {
            mIsDetected = false;
            yellowView.SetProperty(Actor::Property::COLOR, Color::YELLOW);
        }
        else
        {
            mIsDetected = true;
            yellowView.SetProperty(Actor::Property::COLOR, Color::YELLOW * 0.7f);
        }
    }

    // YellowView intercepts touch and consumes it when Pan gesture is recognized.
    // Then all subsequent touches will be received by yellowView.
    bool OnYellowInterceptTouch(Actor actor, const TouchEvent& touch)
    {
        DALI_LOG_ERROR(" ->yellowView intercepted %d!!! %d\n", mIsDetected, touch.GetState(0));
        mPanGestureDetector.HandleEvent(actor, touch);
        return mIsDetected;
    }

    bool OnYellowTouched(Actor actor, const TouchEvent& touch)
    {
        DALI_LOG_ERROR(" ->yellowView TouchEvent!!! %d\n", touch.GetState(0));
        mPanGestureDetector.HandleEvent(actor, touch);
        return true;
    }

private:
    Actor               orangeView;
    Actor               yellowView;
    LongPressGestureDetector mLongPressGestureDetector;
    PanGestureDetector  mPanGestureDetector;
    bool                mIsDetected;
};
```

### Behavior Process

```
┌─────────────────────────────────────────────────────────────────┐
│  1. LongPress on OrangeView → LongPressGesture occurs           │
│                                                                 │
│  2. Touch movement starts → YellowView's InterceptTouchEvent    │
│     monitors                                                    │
│                                                                 │
│  3. Movement exceeds threshold → PanGesture recognized          │
│     - YellowView.InterceptTouchEvent return true (consume)      │
│     - OrangeView receives Interrupted                           │
│     - LongPressGesture ends                                     │
│                                                                 │
│  4. Subsequent touches → YellowView receives PanGesture         │
└─────────────────────────────────────────────────────────────────┘
```

### Detailed Description

1. **Long-pressing OrangeView**: LongPressGesture occurs.

2. **Moving touch in this state**: When movement exceeds a certain threshold, YellowView's `InterceptTouchEvent` consumes the event, OrangeView receives `Interrupted`, and LongPressGesture ends simultaneously.

3. **YellowView now receives PanGesture.**

### Role of InterceptTouchEvent

`InterceptTouchEvent` is a mechanism where a parent view intercepts touch events being delivered to child views:

- **return false**: Deliver touch event to child view (default behavior)
- **return true**: Intercept touch event and handle it in parent view

This allows flexible control of touch events in complex gesture scenarios.

---

## Summary

| Situation | Behavior |
|------|------|
| TouchEvent consume | View continues to receive touch events |
| TouchEvent not consumed | Cannot receive subsequent touch events, cannot recognize gestures |
| Gesture recognition in parent view | Touch cannot propagate to child views |
| Using InterceptTouchEvent | Parent view can intercept touch and transition gestures |

---

## References

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)