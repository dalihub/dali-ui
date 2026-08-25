# Geometry Touch & Gesture diagnostic sample

`geometry-touch-gesture.example` is an interactive diagnostic application for
geometry hit testing, touch ownership, interception, and manually fed gesture
detectors. `UiConfig::Apply()` enables geometry hit testing once during
application startup. The sample does not change that mode while it is running.
It does not request a window size; the shell, test canvas, and scenario roots
use the platform-provided window size.

The sample is based on the active behavior of these Tizen NUI samples:

| Page | Source sample | Main purpose |
| --- | --- | --- |
| 1 | `GeometryTouchEvent.cs` | Basic coordinate Touch order, Actor consumption, and hierarchy interception |
| 2 | `FeedGestureSample.cs` | Shared detectors versus per-Actor detectors and explicit gesture cancellation |
| 3 | `FeedTapAndLongPressGestureSample.cs` | Tap and long-press competition through a nested hierarchy |
| 4 | `FeedTapAndPanGestureSample.cs` | Tap versus a manually fed horizontal pan |
| 5 | `FeedNestedPanGestureSample.cs` | Vertical outer pan, horizontal inner pans, and item gestures |
| 6 | `FeedNestedPanGestureSampleVertical.cs` | Same-direction pan propagation at a scroll boundary |

The source samples' commented-out pinch, rotation, and hover experiments are
not included.

## Controls

- Touch the numbered buttons or press `1` through `6` to select a page.
- Touch `<` / `>` or press Left / Right to move between pages.
- Touch `RESET` or press `R` to reconstruct the current page and every gesture
  detector on it.
- Touch `CLEAR` or press `C` to clear the event log.
- Press Escape or Back to quit.

The event log keeps the latest nine entries and also writes them to standard
output. A touch entry contains the callback Actor, event time, and every
point's device ID, state, hit Actor, screen position, local position, and
callback result. `SCENE BOUNDARY` entries are the Window touch signal and
therefore show stream boundaries rather than every motion.

## Manual checks

### 1. Basic Touch & Intercept Flow

The page uses one deterministic common zone:

```text
TestRoot
|- BackSibling
`- Parent
   `- Child       <- TOUCH HERE
```

With every Touch signal connected and every callback returning `false`, the
expected common-zone phases are:

```text
INTERCEPT: TestRoot -> Parent
TOUCH:     Child -> Parent -> BackSibling -> TestRoot
```

The table physically connects or disconnects each supported signal and changes
its callback return value. Intercept is available only on `TestRoot` and
`Parent`, because the leaf Actors have no descendant to intercept. Change the
table only between streams; it is locked from the first down until the last up
or interruption.

Use the presets in order:

- `BASIC PASS`: all Touch callbacks run and return `false`.
- `BASIC CONSUME`: Child returns `false`, Parent returns `true`, Child receives
  `INTERRUPTED`, and Parent becomes owner.
- `INTERCEPT PASS`: TestRoot and Parent intercept callbacks run and return
  `false`, so ordinary Touch order is unchanged.
- `INTERCEPT ONLY`: Parent intercept returns `true`; ordinary Touch is limited
  to `Parent -> TestRoot`, but no owner is created because both Touch callbacks
  return `false`.
- `INTERCEPT + OWNER`: Parent intercept and Parent Touch both return `true`, so
  Parent becomes owner while Child and BackSibling receive no ordinary Touch.

The compact trace separates `INTERCEPT PHASE`, `TOUCH PHASE`, and `RESULT`.
The bottom diagnostic log retains all point details. `SCENE BOUNDARY` is a
separate Window observer and still reports first down, last up, or interruption
regardless of Actor consumption.

### 2. Shared vs Independent Detectors

- In the left panel, Red and Blue feed the same Tap, Pan, and LongPress handles.
- In the right panel, Red and Blue own separate Pan and LongPress handles.
- A selected Pan moves the Actor reported by the detector. The Actor remains
  partially visible at the canvas or parent boundary; `RESET` restores its
  initial position.
- LongPress changes the detected Actor to cyan at `STARTED`. A normal
  `FINISHED` restores its Red/Blue color, while `CANCELLED` leaves it magenta
  so cancellation remains visible until the next LongPress or `RESET`.
- When the right Blue pan is selected, it calls
  `CancelAllOtherGestureDetectors()`; competing recognizers should report
  cancellation.
- The brown blocker always returns `true` and should prevent the Actors behind
  it from becoming recipients.
- Pan detectors accept up to two touch points.

### 3. Tap vs LongPress

- Quick taps increment the counter for the Actor reported by the shared Tap
  detector.
- Holding on the Parent starts LongPress; releasing finishes it.
- Grandchild feeds Tap but intentionally returns `false`, allowing subsequent
  coordinate recipients to run until one consumes the stream.
- The Parent feeds both Tap and LongPress before combining their results, so a
  consumer cannot accidentally starve the other recognizer of its terminal
  event.

### 4. Tap vs Pan

- Tap the orange background or red overlay to increment its tap counter.
- Drag the horizontal card strip to start Pan and change its offset.
- When Pan consumes the stream, a competing Tap that previously received the
  stream should be cancelled through `INTERRUPTED`.

### 5. Nested Orthogonal Pan

- Drag vertically between rows to move the outer scroller.
- Drag horizontally within a row to move only that row.
- Tap or hold a dark item to exercise the shared item Tap/LongPress detectors.
- Item LongPress is cyan at `STARTED`, returns to its dark color at `FINISHED`,
  and remains magenta after `CANCELLED` so Pan arbitration is visible.
- When a pan starts, that scroller connects its InterceptTouch signal and
  disconnects it at finish/cancel. The late-consuming pan itself is what
  interrupts earlier item recipients.
- If the selected scroller is already the current owner, its own intercept
  callback is excluded from the owner ancestry check and is not expected to
  run again in that stream.

### 6. Same-direction Pan Handoff

The outer scroller starts at offset `-220`; the inner scroller starts at its
top boundary.

1. Start a downward drag inside the inner scroller.
2. Inner cannot move in that direction and calls
   `DevelActor::SetNeedGesturePropagation(inner, true)` from its `STARTED`
   callback.
3. Inner's `HandleEvent()` returns `false`; the outer detector receives the
   same touch and can move toward offset `0`.
4. Scroll inner to its bottom, release, then begin a new upward drag to verify
   the equivalent handoff at the other boundary.

The log marks the decision as `propagate-to-parent=true`. Propagation is chosen
when a gesture starts; reaching a boundary midway through an already selected
pan does not transfer that active gesture.

## Build

Set up the DALi desktop environment, then run from the `dali-ui` root:

```bash
cmake -S samples -B /tmp/dali-ui-geometry-touch-gesture-build \
  -DDALI_UI_SAMPLE_LIST=geometry-touch-gesture \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build /tmp/dali-ui-geometry-touch-gesture-build \
  --target geometry-touch-gesture.example -j
./samples/geometry-touch-gesture/bin/geometry-touch-gesture.example
```

The target uses DALi public APIs plus `DevelActor::SetNeedGesturePropagation()`.
It does not include DALi internal or integration headers.
