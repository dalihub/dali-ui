# LayoutTransition sample

This sample demonstrates the use of **LayoutTransition** to animate views
between layout-pass results: ENTER on add, EXIT on remove, and CHANGE on
bounds change.

## Examples

- **layout-transition-spec.example**: declarative spec mode. The
  framework drives interpolation; the application supplies a
  `ViewAnimationSpec` for ENTER / EXIT and a `LayoutTransitionTiming`
  for CHANGE.
- **layout-transition-animator.example**: animator-callback mode. The
  application owns the per-frame interpolation and writes properties
  every frame, mirroring the spec-mode height-expand + opacity-fade
  contract from a callback.
- **layout-transition-reorder.example**: edit-mode reorder sample. The
  dragged item is represented by an overlay proxy while the original item
  stays in the list at opacity 0, leaving a moving empty slot. Sibling
  reflow is animated by the CHANGE slot with the same timing as the other
  examples. The list is wrapped in a vertical ScrollView; in edit mode,
  dragging an item near the top / bottom edge of the viewport
  auto-scrolls the list in that direction so items beyond the visible
  area become reachable without releasing the drag.
  The card dropped into place is lifted above its siblings until its settle
  transition completes.
- **layout-transition-subtree.example**: reflow-scope sample. A single
  transition on the root container reflows the whole subtree under
  `LayoutReflowScope::SUBTREE` — a nested card with no transition of its
  own still has its inner items reflow. Toggle the scope back to
  `DIRECT_CHILDREN` to see the inner items snap while only the card
  animates.
- **layout-transition-grid-reorder.example**: grid reorder sample. A
  white root holds a translucent rounded panel at (80, 80) with a
  right-aligned notification / edit row, a Wi-Fi / Bluetooth button row,
  and a scrollable 3-column `GridLayout` of SVG icons (each with its
  file-name label). Long-pressing a cell floats it under the window while
  an invisible proxy reserves its slot; dragging moves the proxy to the
  cell under the finger and reassigns every cell's `Row` / `Column`, so
  the CHANGE slot animates the reflow. Dragging near the top / bottom of
  the grid auto-scrolls. Because the cell captures the touch stream once a
  drag can start, free scrolling uses the empty margins between cells; the
  auto-scroll keeps off-screen cells reachable during a reorder.

## Controls

- **Tap "Click to ENTER"**: append a new colored child (ENTER expands
  height 0 → child height and fades opacity 0 → 1).
- **Tap "Click to EXIT"**: remove the last child (EXIT shrinks height
  to 0 and fades opacity to 0, then unparents).
- **Tap "Click to CHANGE"**: in the spec and animator examples, toggle
  every child's requested height between 80 and 160 (CHANGE slot
  animates width / height / position).
- **Tap "Click to Edit"**: in the reorder example, toggle edit mode. In
  edit mode, press and drag a child to move the empty slot through the
  sibling order; release to drop. Drag near the top or bottom of the
  ScrollView viewport to auto-scroll the list and continue reordering
  items beyond the visible area.
- **Tap "Toggle layout" / "Scope: ..."**: in the subtree example, toggle
  the nested sizes, or switch the root transition between `SUBTREE` and
  `DIRECT_CHILDREN`.
- **Long-press + drag a grid cell**: in the grid reorder example, hold a
  cell to pick it up, then drag to move it through the grid order; release
  to drop. Drag near the top or bottom of the grid to auto-scroll.
- **Up arrow**: same as the ENTER button (Toggle layout in the subtree
  example).
- **Down arrow**: same as the EXIT button (Toggle layout in the subtree
  example).
- **Esc / Back**: quit.

The remaining children also reflow on every add / remove; their
position and size animate via the CHANGE slot. Both samples share a
single 0.4s EASE_IN_OUT_SINE timing across all three slots.

In the animator sample the application owns the properties written from
callbacks. ENTER and EXIT both write opacity and height, while CHANGE
writes layout bounds only. If an animator transition is cancelled by a
successor slot, `OnFinished` is not emitted for the cancelled slot; any
non-layout properties written by the cancelled callback remain
application-owned and should be reset by the successor callback or by
lifecycle code when needed.

The Tizen package manifest launches the spec-mode sample entry.

## Build

### Ubuntu

Requires DALi environment to be set up first.

```bash
# From dali-ui root
cd samples/layout-transition
cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX
make -j
```

Run:

```bash
./bin/layout-transition-spec.example
./bin/layout-transition-animator.example
./bin/layout-transition-reorder.example
./bin/layout-transition-subtree.example
./bin/layout-transition-grid-reorder.example
```
