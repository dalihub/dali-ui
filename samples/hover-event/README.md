# Hover event sample

This sample places three partially overlapping plain `View`s (A, B, and C) and
displays each view's current `HOVER ON/OFF` state, callback consume value, last
raw hover state, raw-event count, state-change count, pointer position, and a
recent-event trace.

Views are added in A, B, C order, so C is the topmost hit-test target in an
overlap, followed by B and A. A and C return `true`; B returns `false`. In the A/B
overlap, the trace shows `B STARTED -> B MOTION -> A MOTION`, and both views remain
`HOVER ON`. In the B/C overlap, C consumes the event; unvisited B and A then receive
`LEAVE`. This demonstrates GEOMETRY hover's Android-style visited-prefix dispatch.

Build only this sample from the `samples` directory with:

```sh
cmake -S . -B _build -DDALI_UI_SAMPLE_LIST=hover-event
cmake --build _build --target hover-event.example
```

Press Escape or Back to quit.
