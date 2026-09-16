# Device gesture options sample

This sample shows `GestureDeviceSelector`: the same `PanGestureDetector` and
`TapGestureDetector` recognize gestures differently depending on which input
device started them, without the application inspecting devices on every
event. Two layers are registered once, at start-up:

- **Detector-local options** (`PanGestureDetector::Options`,
  `TapGestureDetector::Options`, via `SetDeviceOptions()`): the mouse profile
  restricts panning to roughly horizontal drags (`AddDirection()`) and allows
  a double-click (`SetMaximumTapsRequired(2)`).
- **Application-wide recognition thresholds** (`Dali::GestureThresholds`, via
  `SetPanThresholds()` / `SetTapThresholds()`): the mouse profile needs a
  longer drag before a pan starts (40px vs the touch default of 15px), and a
  faster second click before it counts as a double-click (250ms vs 450ms).

**Pan zone** — drag the blue block. With a mouse, the drag only starts if it
begins within about 45 degrees of horizontal; once started it can move
freely (this is how `AddAngle`/`AddDirection` work — the angle is only
checked at the start of the gesture). With a touchscreen, the block follows
a drag started in any direction.

**Tap zone** — click or tap the green button once or twice. Both devices
recognize up to a double tap, but the window for the second click/tap to
still count as part of the same tap differs per device; the log on the right
shows the device and tap count for each recognized gesture.

The legend at the bottom restates the configured values regardless of which
device you use to test with.

A touchscreen is not required to see the effect: dragging vertically vs.
horizontally with the mouse alone demonstrates the per-device options, and a
regular click vs. a double-click demonstrates the per-device tap taps.

Build only this sample from the `samples` directory with:

```sh
cmake -S . -B _build -DDALI_UI_SAMPLE_LIST=device-gesture-options
cmake --build _build --target device-gesture-options.example
```

Press Escape or Back to quit.
