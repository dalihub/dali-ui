[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-Transition-(kr))

# Layout Transition

`LayoutTransition` animates a `View`'s direct children between successive
layout-pass results. When a child is added, removed, reordered, or the
parent's layout produces different bounds for it, the framework dispatches
an animation in the matching slot.

<img src="assets/Layout/dali-ui-layout-transition-grid-item.gif" height="400"/>

---

## Concepts

### Slots

| Slot     | Fired when                                                                |
|----------|---------------------------------------------------------------------------|
| `ENTER`  | A new child is added after the parent has completed its initial arrange.  |
| `EXIT`   | `View::Remove(child, RemovePolicy::ANIMATE_EXIT)` is called and the EXIT slot is configured. |
| `CHANGE` | An existing child's arranged bounds differ from the previous pass.        |

Each slot holds its own configuration. All slots dispatch independently
at frame 0. If you need strict EXIT → CHANGE → ENTER sequencing across
the same child, chain the slots through `OnFinished` lifecycle callbacks.

### Modes

A slot can be configured in **spec mode** or **animator mode**.

- **Spec mode** (declarative): the application supplies a
  `ViewAnimationSpec` (ENTER / EXIT) or a `LayoutTransitionTiming` (CHANGE)
  with timing and the properties to animate. The framework drives the
  interpolation through `dali-core` `Animation`.

- **Animator mode** (imperative): the application registers a
  `LayoutAnimatorCallback`. The framework sweeps progress 0→1 across the
  configured timing and invokes the callback once per frame; the callback
  writes `Actor` properties directly. This allows custom curves, multi-
  property choreography, or non-linear effects.

When both are configured for a slot, **animator mode wins**.

### Causes

`LayoutAnimatorContext::changeCause` distinguishes CHANGE scenarios. It is
meaningful only when `slot == CHANGE`; for ENTER / EXIT it defaults to
`OTHER` and must not be branched on.

| Cause             | Meaning                                              |
|-------------------|------------------------------------------------------|
| `SIBLING_ADDED`   | CHANGE on existing child because a sibling was added.|
| `SIBLING_REMOVED` | CHANGE on existing child because a sibling was removed.|
| `REORDERED`       | CHANGE triggered by sibling order change.            |
| `WINDOW_RESIZED`  | CHANGE triggered by top-level window resize.         |
| `OTHER`           | CHANGE triggered by any other layout-driven change.  |

---

## Quick start (spec mode)

```cpp
StackLayout layout = StackLayout::New();

ViewAnimationSpec enter = ViewAnimationSpec::New();
enter.Opacity(1.0f, Duration(0.25f), AlphaFunction(AlphaFunction::EASE_OUT));

ViewAnimationSpec exit = ViewAnimationSpec::New();
exit.Opacity(0.0f, Duration(0.2f), AlphaFunction(AlphaFunction::EASE_IN));

LayoutTransitionTiming change{Duration(0.3f),
                              AlphaFunction(AlphaFunction::EASE_IN_OUT),
                              Duration()};

LayoutTransition transition = LayoutTransition::New();
transition.SetEnterVisualSpec(enter)
          .SetExitVisualSpec(exit)
          .SetEnterBoundsEffect(LayoutBoundsEffects::SlideFrom(
            LayoutBoundsEdge::BOTTOM,
            {Duration(0.3f), AlphaFunction(AlphaFunction::EASE_OUT), Duration()}))
          .SetExitBoundsEffect(LayoutBoundsEffects::SlideTo(
            LayoutBoundsEdge::BOTTOM,
            {Duration(0.3f), AlphaFunction(AlphaFunction::EASE_IN), Duration()}))
          .SetChangeTiming(change);

layout.SetLayoutTransition(transition);

// Children added / removed afterward animate per the configured slots.
View child = View::New();
child.SetProperty(Actor::Property::OPACITY, 0.0f);  // start invisible
layout.Add(child);                                  // ENTER fades + slides up
```

The bounds-effect channel uses mirror semantics: a single `LayoutBoundsEffect`
descriptor produces an enter (endpoint → base) or an exit (base → endpoint)
depending on which slot consumes it. `LayoutBoundsEffects::SlideFrom` and
`SlideTo` therefore return identical descriptors. The same applies to
`ExpandFrom` / `ShrinkTo`.

---

## Quick start (animator mode)

```cpp
auto onChange = [](const LayoutAnimatorContext& ctx)
{
    Actor actor = ctx.view;
    if (!actor) return;
    const float p = ctx.progress;
    const float x = ctx.fromBounds.x + (ctx.toBounds.x - ctx.fromBounds.x) * p;
    const float y = ctx.fromBounds.y + (ctx.toBounds.y - ctx.fromBounds.y) * p;
    actor.SetProperty(Actor::Property::POSITION_X, x);
    actor.SetProperty(Actor::Property::POSITION_Y, y);
};

LayoutAnimatorTiming timing;
timing.duration = Duration(0.3f);
timing.alpha    = AlphaFunction(AlphaFunction::EASE_IN_OUT);

LayoutTransition transition = LayoutTransition::New();
transition.SetChangeAnimator(LayoutAnimatorCallback::New(onChange), timing);
layout.SetLayoutTransition(transition);
```

---

## Composition options

| Option                       | Default     | Effect                                                                                                    |
|------------------------------|-------------|-----------------------------------------------------------------------------------------------------------|
| `SetChangeOnWindowResize`    | `false`     | If `true`, CHANGE fires on window-resize-driven layout changes (cause `WINDOW_RESIZED`).                  |
| `SetEnterOnInitialMount`     | `false`     | If `true`, ENTER fires for children present at the parent's very first arrange pass. The default suppresses initial-mount ENTER (declarative ENTER specs are still settled to their final values; animator ENTER is skipped). Adds at runtime always fire ENTER regardless. |
| `SetReflowScope`             | `DIRECT_CHILDREN` | Selects how far the CHANGE slot reaches. `SUBTREE` reflows every descendant that has no transition of its own. See below. |
| `SetOnStart`                 | unset       | Lifecycle callback fired when a per-(view, slot) transition starts.                                       |
| `SetOnFinished`              | unset       | Lifecycle callback fired when a per-(view, slot) transition reaches normal completion (cancellation is silent — see Caveats). |

### Reflow scope

By default a transition animates only the **direct children** of the view it
is attached to. A grand-child therefore animates only when its own immediate
parent also carries a transition — the effect cascades level by level, and
each level is configured independently.

`SetReflowScope(LayoutReflowScope::SUBTREE)` removes that requirement for the
**CHANGE slot**: one transition on a container reflows the whole subtree under
it with a single timing, without a transition on every intermediate container.

```cpp
LayoutTransition t = LayoutTransition::New();
t.SetChangeTiming(LayoutTransitionTiming{Duration(0.25f), AlphaFunction(AlphaFunction::EASE_IN_OUT), Duration()})
 .SetReflowScope(LayoutReflowScope::SUBTREE);
container.SetLayoutTransition(t); // grand-children reflow too
```

Scope resolution: a node is animated by the **closest ancestor that has a
transition**. That ancestor reaches the node when it is the node's direct
parent, or when its scope is `SUBTREE`. A descendant that has its own
transition becomes the closest ancestor for its own children, so a `SUBTREE`
scope stops at that boundary (no double animation).

Notes and limits:

- Applies to **CHANGE**, and to **ENTER / EXIT** when the owner carries the
  corresponding slot effect: a child added under a no-transition descendant
  fires the owner's ENTER, and a child removed via
  `View::Remove(child, RemovePolicy::ANIMATE_EXIT)` or
  `RemoveAllChildren(RemovePolicy::ANIMATE_EXIT)` fires the owner's EXIT, while
  the no-argument `RemoveAllChildren()` is immediate and fires no EXIT. Raw `Actor::Remove` (bypassing the
  View remove API) is **not** deferred. The effect is sourced from the owner
  while geometry and the EXIT ghost use the child's real direct parent. The
  closest transition-bearing ancestor wins, so a descendant with its own
  transition governs its own subtree.
- Inherited descendants resolve their CHANGE timing with cause `OTHER` (or
  `WINDOW_RESIZED` during a window resize), so configure a **default** CHANGE
  timing or animator for `SUBTREE` CHANGE to take effect. Cause-specific timing
  (`REORDERED`, `SIBLING_*`) is honoured only for direct children.
- `SUBTREE` does not cross a standalone layout-mode boundary.
- Each governed descendant is interpolated between its old and new arranged
  bounds; the layout is not re-run at intermediate sizes.

---

## Lifecycle and removal

`View::Remove(child, RemovePolicy::ANIMATE_EXIT)` performs a **deferred
remove** when an EXIT slot is configured: the child is dropped from the
layout-tracking list immediately (so siblings reflow into the freed slot)
but the actor stays attached during the EXIT animation. The actor is
unparented automatically when the EXIT animation finishes. Using the
inherited one-argument `Actor::Remove`, or `RemovePolicy::IMMEDIATE`,
unparents immediately and skips EXIT.

If you call `Actor::Remove` (or the inherited `Self().Remove`) directly,
the actor is unparented synchronously and **EXIT is skipped entirely**.

`ENTER` is not fired for children that are already present at the parent's
very first arrange pass. Those children are treated as the parent's initial
visual state because that first arrange typically completes before the window
surface is on screen, so any ENTER fade-in dispatched there would elapse
while the user can see nothing. Declarative ENTER specs are still settled
to their target values (e.g. `Opacity(1.0)` lands the child at opacity 1
even when `AppendChild` pre-set OPACITY to 0). Animator-mode ENTER is
skipped without settling property writes. To run an ENTER animation at
launch, set `SetEnterOnInitialMount(true)` on the transition, or add the
children at runtime after the parent has completed its first arrange.

`OnFinished` fires when a transition reaches **normal completion** —
the configured timing has fully elapsed (animator mode: `rawProgress`
reaches 1.0; spec mode: dali-core `Animation::FinishedSignal` fires).
The shaped `ctx.progress` is not necessarily 1.0 at that moment:
a `CUSTOM_FUNCTION` pointer may return any value at the final tick.
(`AlphaFunction::REVERSE` is rejected by LayoutTransition validation —
see AlphaFunction restrictions below. `BOUNCE` and `SIN` are additionally
rejected for layout-bounds timing because they do not end at the target
value.)
If a slot is cancelled (by a new transition starting for
the same child, by reparenting the child to a different parent, or by
view destruction / scene disconnection), the in-flight callback is
dropped silently — `OnFinished` is **not** fired. Replacing the
`LayoutTransition` handle via `View::SetLayoutTransition` does **not**
cancel in-flight transitions — see the caveat below.

---

## Caveats

- **No reference cycles in callbacks.** The dispatcher holds the
  `LayoutTransition` (which owns the callback), and the `View` holds the
  `LayoutTransition`. Capturing the target `View` by value inside an
  animator or lifecycle closure forms a cycle. Use `WeakHandle<View>` if
  a captured reference is required, or read `ctx.view` inside the
  callback body each frame.

- **No view-tree mutations from animator callbacks.** Do not call
  `Add` / `Remove` / `Unparent` / `InvalidateMeasure` from inside
  a `LayoutAnimatorCallback`. Tree mutations are supported only from
  lifecycle callbacks (`OnStart` / `OnFinished`).

- **Replacing the transition mid-flight.** Calling `SetLayoutTransition`
  on a view does not interrupt its in-flight transitions; each finishes
  on its own timing under the handle that started it. Newly added /
  removed / moved children dispatch under the new handle from the next
  layout pass onward. Pass an uninitialized handle to detach (in-flight
  ones still finish; no new transitions start).

- **No default ENTER / EXIT.** If neither a spec nor an animator is set
  for a slot, that slot is skipped. There is no implicit fade-in / out.

- **ENTER / EXIT visual specs reject bounds entries.** Bounds-related
  entries (`POSITION_X/Y`, `SIZE_WIDTH/HEIGHT`) in a `ViewAnimationSpec`
  passed to `SetEnterVisualSpec` / `SetExitVisualSpec` are rejected with
  `DALI_ABORT` (both at registration and again at apply time inside the
  dispatcher). Bounds animation belongs to the bounds-effect channel so
  the dispatcher can compose layout-driven positions with declared
  slide / expand / shrink offsets coherently. Use the bounds-effect API
  (or animator mode) to choreograph bounds during enter / exit.

- **Empty / zero-duration spec is treated as "no transition".** An
  ENTER/EXIT `ViewAnimationSpec` with no entries (or whose entries have
  zero `duration + delay` collectively) skips lifecycle dispatch
  entirely: EXIT unparents immediately, ENTER no-ops, and neither
  `OnStart` nor `OnFinished` fires. Add a non-zero entry, or use animator
  mode with a positive duration, to get lifecycle hooks.

- **Cancellation is silent.** `OnFinished` fires only when a transition
  reaches normal timing completion (animator mode: `rawProgress` reaches
  1.0; spec mode: dali-core `Animation::FinishedSignal` fires). Note that
  shaped `ctx.progress` is not necessarily 1.0 at completion — see the
  Lifecycle section. Cancellation paths drop the in-flight callback
  silently:
    - same-slot supersession (a new spec/animator started for the same
      slot of the same child),
    - cross-slot supersession on the same child:
        - `Remove` starts EXIT and cancels any in-flight CHANGE or
          ENTER on the child,
        - a new layout pass starts CHANGE and cancels any in-flight
          ENTER on the child,
    - reparent of the child to a different parent,
    - View destruction / scene disconnection.
  Spec-mode interruption preserves the current visual state when EXIT
  supersedes ENTER/CHANGE, so a child fades or shrinks out from where it
  is currently visible. When CHANGE supersedes ENTER, ENTER visual
  properties are settled to their target values and CHANGE takes over the
  layout bounds from the sampled current rectangle.

  Animator callbacks own every property they write. The framework can
  carry bounds handoff through `fromBounds` / `toBounds` / internal
  `lastLerped` state, but it cannot know which arbitrary actor
  properties an application callback touched. If an ENTER animator writes
  opacity, scale, color, or other non-layout properties, the successor
  animator or lifecycle code must clean up those properties if needed.
  Replacing the `LayoutTransition` handle on a view does NOT cancel
  in-flight transitions — each finishes on its own and fires its own
  `OnFinished`.

- **EXIT `OnFinished` fires after unparent.** Inside an EXIT
  `OnFinished`, `view.GetParent()` returns an uninitialized handle (the
  dispatcher unparents before emitting). The view handle itself is still
  valid.

- **EXIT ghost is non-resurrectable.** While EXIT is in flight the
  child is logically removed from the parent — the logical accessors
  (`GetChildViewCount` / `GetChildViewAt`) skip it — but it stays in
  the actor tree, so the inherited `Dali::Actor::GetChildCount` /
  `Dali::Actor::GetChildAt` still count and return it. Adding the same
  child back to the same parent via `View::Insert` or inherited
  `Actor::Add` is silently ignored — the EXIT continues. To cancel,
  reparent to a different parent: the dispatcher auto-cancels the
  EXIT, restores interaction state, and triggers ENTER under the new
  parent.

- **Spec-mode bounds alpha must end at the target.** CHANGE timing and
  ENTER/EXIT bounds effects animate layout-owned bounds
  (`POSITION_X/Y`, `SIZE_WIDTH/HEIGHT`). When those animations finish,
  the actor bounds must match the layout result. For that reason,
  LayoutTransition rejects built-in alpha functions that do not end at
  the target value:
    - `REVERSE`
    - `BOUNCE`
    - `SIN`

  `EASE_OUT_BACK` is allowed for spec-mode bounds timing because it
  overshoots but returns to the target at the end. `CUSTOM_FUNCTION`
  output is application-owned; if used with `LayoutTransitionTiming` or a
  `LayoutBoundsEffect`, it must return a target-ending value at progress
  1.0.

- **Spec-mode visual spec alpha.** `ViewAnimationSpec` entries passed to
  `SetEnterVisualSpec` / `SetExitVisualSpec` are routed through
  dali-core `Animation` after LayoutTransition validation. `REVERSE` is
  rejected because reverse direction is not represented by
  LayoutTransition alpha. Non-bounds visual properties such as opacity
  and scale may use dali-core target-ending curves as usual.

- **Animator-mode alpha limitations.** Animator mode evaluates the
  alpha curve on the event side before invoking the callback, so only a
  subset of `AlphaFunction` modes
  is honoured:
    - **Honoured**: `DEFAULT`, `LINEAR`, `EASE_IN_SQUARE`,
      `EASE_OUT_SQUARE`, `EASE_IN`, `EASE_OUT`, `EASE_IN_OUT`,
      `EASE_IN_SINE`, `EASE_OUT_SINE`, `EASE_IN_OUT_SINE`,
      `CUSTOM_FUNCTION`.
    - **Linear fallback** (silent): `BOUNCE`, `SIN`, `EASE_OUT_BACK`,
      `BEZIER`, `SPRING`, `CUSTOM_SPRING`. Use a built-in function from
      the honoured list or a `CUSTOM_FUNCTION` pointer if you need a
      custom curve in animator mode.
    - **Rejected by LayoutTransition validation** (`DALI_ABORT`):
      `REVERSE`. LayoutTransition direction is determined by slot
      semantics and bounds, not by alpha. See the AlphaFunction
      restrictions section below.

- **Animator-mode `duration` should be positive.** A non-positive
  `LayoutAnimatorTiming::duration` collapses progress to `1.0` on the
  first tick regardless of `delay`. To delay an animator's start,
  combine a positive `duration` with the desired `delay`.

---

## AlphaFunction restrictions

`AlphaFunction::REVERSE` is **not supported** for LayoutTransition.
LayoutTransition does not provide reverse playback. Direction is
determined by slot semantics (ENTER for additions, EXIT for removals,
CHANGE for layout result changes) and by captured `fromBounds` /
`toBounds`. Alpha functions shape the current transition; they do not
choose the target state.

LayoutTransition validates `REVERSE` both when a slot is configured
and, for `ViewAnimationSpec`-based ENTER / EXIT specs, again immediately
before applying the spec:

- A `REVERSE` entry present at `SetEnterVisualSpec` / `SetExitVisualSpec`
  time is rejected there with `DALI_ABORT` (throwing
  `Dali::DaliException`).
- A `REVERSE` entry added to a shared `ViewAnimationSpec` after
  registration is rejected at LayoutTransition apply time, also with
  `DALI_ABORT`.

For `LayoutAnimatorTiming` and `LayoutTransitionTiming`, validation is
configuration-time only because those values are stored by value.

For layout-bounds timing (`SetChangeTiming` and
`LayoutBoundsEffect::timing`), `BOUNCE` and `SIN` are also rejected
because dali-core evaluates them back to the initial state at completion.
A layout transition must finish on the layout target bounds.

`CUSTOM_FUNCTION` output is application-owned. The framework does not
inspect or validate custom alpha return values. A custom function may
mimic reverse progress; this is treated as explicit application
behavior and receives no special LayoutTransition guarantee.

For decorative reverse-shaped effects independent of layout flow, use
dali-core `Animation` on non-layout-owned actor properties such as
opacity or scale. Do not use it to fight LayoutTransition's
layout-owned bounds.

---

## Spec mode vs animator mode alpha handling

Spec mode routes entries that pass LayoutTransition validation through
dali-core `Animation`.

For visual specs, LayoutTransition rejects `REVERSE` and otherwise lets
dali-core animate non-layout-owned properties. For CHANGE timing and
bounds effects, LayoutTransition additionally rejects `BOUNCE` and `SIN`
because those modes do not complete at the target bounds.

Animator mode evaluates a subset of `AlphaFunction` values itself.
Animator mode does not honor `BOUNCE`, `SIN`, `EASE_OUT_BACK`,
`BEZIER`, `SPRING`, or `CUSTOM_SPRING` today. It falls back to linear
for those modes, so visual timing can differ from spec mode for those
alphas.

`AlphaFunction::REVERSE` is different: it is rejected by LayoutTransition
validation in both modes, because reverse direction is not represented
by LayoutTransition alpha.

---

## Completion detection

Use `SetOnFinished` for end-of-transition cleanup, state transitions,
chaining logic, or view-tree mutation. Do not treat either
`progress == 1.0` or `rawProgress >= 1.0` inside the animator callback
as the lifecycle completion contract.

`rawProgress` is still useful as a per-frame linear elapsed signal.
For example, the primary effect can follow the alpha-shaped `progress`,
while a secondary visual effect follows `rawProgress`.

---

## See also

- `dali-ui-foundation/public-api/layouts/layout-transition.h` — handle.
- `dali-ui-foundation/public-api/layouts/layout-transition-types.h` —
  `LayoutTransitionSlot`, `LayoutChangeCause`, `LayoutAnimatorTiming`,
  `LayoutTransitionTiming`, `LayoutAnimatorContext`.
- `samples/layout-transition/` — runnable spec-mode and animator-mode
  examples.
- `automated-tests/src/dali-ui-foundation/utc-Dali-LayoutTransition.cpp` —
  unit tests covering the handle API surface.

---

[← Back to Layout](Layout.md)
