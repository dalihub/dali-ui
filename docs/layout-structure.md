# DALi UI Foundation - Layout Class and Behavior Structure

## Overview

The layout system in DALi UI Foundation computes **size (Measure)** and **position (Arrange)** of child views over a **View** hierarchy. Child management uses the inherited Actor `Add`/`Remove`/`InsertAbove`/`InsertBelow`/`RemoveAll` API.

Layout processing is driven by **LayoutController** per window. Each frame, it runs Measure then Arrange on layout roots that have been invalidated.

---

## Class Structure

### 1. Public API (Handles)

- **View**  
  - Layout properties: `SetRequestedWidth` / `SetRequestedHeight`, `SetMargin` / `SetPadding`, alignment, visibility, etc.
  - `GetSize()` returns the actual rendered size (read-only).
  - Measure/Arrange are invoked internally by the layout system; applications may request recomputation via `InvalidateMeasure()` / `InvalidateArrange()`.
  - `AttachLayoutManager(Dali::UniquePtr<LayoutManager>)` attaches a layout algorithm to any View; the View then dispatches Measure/Arrange to the manager (a Measure/Arrange callback, if set, takes priority over the manager).
  - Child add/remove uses inherited Actor `Add`/`Remove`. `InsertAbove`/`InsertBelow` and `RemoveAll()` are available for positional insertion and bulk removal. `View` adds the EXIT-aware `Remove(View, RemovePolicy)` and `RemoveAll(RemovePolicy)`; `RemoveAll(RemovePolicy::IMMEDIATE)` differs from the inherited `RemoveAll()` only in leaving in-flight EXIT ghosts to finish their EXIT.
  - `Add()` appends. `InsertAbove`/`InsertBelow` position a child relative to an existing sibling and accept both a child that is **already** a child of the parent (a reorder) and a **freshly created** view (a fresh insert); in both cases the child's logical (layout) index matches its resulting actor position, skipping non-View actor children and in-flight EXIT ghosts.

- **Layout** (inherits View)
  - Child management: `Add(View)`, `Remove(View)`, `InsertAbove(Actor, Actor)`, `InsertBelow(Actor, Actor)`, and `RemoveAll()` (all inherited from Actor), plus `Remove(View, RemovePolicy)` and `RemoveAll(RemovePolicy)` from View. `GetChildCount()` / `GetChildAt(index)` are inherited from Actor (actor tree, including in-flight EXIT ghosts and non-View actors); the logical (layout) child list is enumerated by `GetChildViewCount()`, `GetChildViewAt(index)`, and `IndexOfChildView(View)` (which skip EXIT ghosts and non-View actors and return `Ui::View` directly).
  - Always has a LayoutManager stored as a Trait (`ReservedTraitId::LAYOUT_MANAGER`); derived classes attach Stack/Flex/Grid/Absolute algorithms in `OnInitialize()`.

- **Custom Layout Callbacks**
  - `View::SetMeasureCallback(MeasureCallback)` and `View::SetArrangeCallback(ArrangeCallback)` allow applications to customize measure/arrange behavior on any View or Layout subclass.
  - `MeasureCallback` and `ArrangeCallback` are move-only typed callbacks (aliases for `Callback<Sig>`), using `CallbackBase` internally.
  - Callbacks are stored as a `LayoutCallbacks` Trait (`ReservedTraitId::LAYOUT_SIGNALS`), created lazily on first use. When set, callbacks take priority over the default LayoutManager.
    ```cpp
    Layout layout = Layout::New();
    layout.SetMeasureCallback(MeasureCallback::New(&MyMeasure));                    // static function
    layout.SetArrangeCallback(ArrangeCallback::New(&MyArrange));                    // static function
    layout.SetMeasureCallback(MeasureCallback::New(this, &MyClass::OnMeasure));     // member function
    layout.SetMeasureCallback({});                                                  // remove callback
    ```

- **StackLayout, FlexLayout, GridLayout, AbsoluteLayout**
  - Each adds type-specific options: orientation/spacing/weight, direction/wrap/justify/align, row/column definitions, absolute bounds, etc.
  - Per-child layout parameters are set via `View::SetLayoutParams()`:
    - `view.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))`
    - `view.SetLayoutParams(GridLayoutParams::New().SetRow(2).SetColumn(3).SetRowSpan(1).SetColumnSpan(2))`
    - `view.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f).SetFlexShrink(0.0f))`
    - `view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(rect).SetFlags(flags))`
  - `SetLayoutParams()` stores an independent copy and invalidates the View's measure cache.
  - Internally these store data via type-safe Trait objects attached to each child view.

- **AbsoluteLayoutParams, FlexLayoutParams, GridLayoutParams, StackLayoutParams**
  - Public value classes that represent snapshots of per-child layout parameters.
  - `View::SetLayoutParams()` copies the supplied values into the View. Later changes to the
    supplied value do not affect the View.
  - `View::TryGetLayoutParams(out)` copies out an independent snapshot. Modify the snapshot and
    pass it back to `SetLayoutParams()` to commit changes:
    ```cpp
    StackLayoutParams params;
    if(view.TryGetLayoutParams(params))
    {
      params.SetWeight(2.0f);
      view.SetLayoutParams(params);
    }
    ```
  - Setters return `*this` for method chaining.
  - `New()` creates a new instance with default values; copy construction creates an independent copy:
    ```cpp
    auto base = GridLayoutParams::New().SetRowSpan(2).SetColumnSpan(2);
    viewA.SetLayoutParams(GridLayoutParams(base).SetColumn(0));
    viewB.SetLayoutParams(GridLayoutParams(base).SetColumn(1));
    ```
  - Passing the same value to multiple Views is safe because each View stores an independent copy.

- **LayoutController**  
  - Singleton per window via `LayoutController::Get(Window)`.  
  - When layout is invalidated, layout roots are registered; the Adaptor calls `Process()` once per frame. The controller registers for both the pre- and post-process phases: `ProcessLayouts()` runs in the pre-process phase to perform Measure and Arrange (before core size negotiation), and the `LayoutFinished` signals (LayoutController and View) are emitted in the post-process phase (after size negotiation).

### 2. Integration API (Implementation)

- **ViewImpl** (DALi ControlImpl-derived)
  - Holds the actual Measure/Arrange logic, size specifications, margin/padding/alignment/visibility, and child container.
  - Provides `GetChildViewCount`, `GetChildViewAt`, `IndexOfChildView`, `Contents`, etc. Child add/remove uses Actor `Add`/`Remove` with `OnChildAdd`/`OnChildRemove` callbacks to sync the internal child container. Child order changes (via `Raise`/`Lower`/`InsertAbove`/`InsertBelow`/etc.) are detected via `ChildOrderChangedSignal` to keep `mChildren` in sync; a **fresh** child inserted with `InsertAbove`/`InsertBelow` emits no such signal, so `OnChildAdd` derives its logical index from the child's final actor position (skipping non-View actor children and EXIT ghosts).
  - `GetParentLayout()`, `IsLayout()`, and invalidation propagate to the parent until a layout root is reached, which registers with the LayoutController.

- **LayoutImpl** (inherits ViewImpl)
  - Base implementation for layout containers. It holds no layout algorithm itself; derived classes attach a LayoutManager via `View::AttachLayoutManager()` in `OnInitialize()`.
  - Layout dispatch — callback (priority) → LayoutManager → default `OnMeasure()`/`OnArrange()` — is handled uniformly in `ViewImpl::Measure()`/`Arrange()`.
  - `IsLayout()` returns `true`.
  - Child APIs are inherited from ViewImpl.

- **StackLayoutImpl, FlexLayoutImpl, GridLayoutImpl, AbsoluteLayoutImpl**
  - Each creates and attaches its LayoutManager via `View::AttachLayoutManager()` in `OnInitialize()`.

- **AbsoluteLayoutParamsImpl, FlexLayoutParamsImpl, GridLayoutParamsImpl, StackLayoutParamsImpl**
  - TraitObject-derived classes that store per-child layout parameters (e.g., bounds/flags, grow/shrink/basis/alignSelf, row/column/span, weight).
  - Attached to child views via `ReservedTraitId` and accessed by layout managers during Measure/Arrange.
  - Each provides `Get(ViewImpl&)`, which returns nullptr if the parameters are not attached.

- **LayoutControllerImpl**  
  - Keeps layout roots in `mPendingViews`; `ProcessLayouts()` resolves constraints, then runs Measure and Arrange for each root during the pre-process phase. Once the pending work drains, it schedules the `LayoutFinished` emit for the post-process phase (after core size negotiation) rather than emitting inline.

### 3. Layout Managers (Algorithms)

- **LayoutManager** (abstract)  
  - `Measure(ViewImpl*, widthConstraint, heightConstraint)`: compute measured size for the container and its children.  
  - `Arrange(ViewImpl*, bounds)`: place children within the given bounds.  
  - Uses protected helpers `GetChildViewCount(ViewImpl*)`, `GetChildViewAt(ViewImpl*, index)`, and `IsStandalone(ViewImpl*)` to traverse children.

- **StackLayoutManager, FlexLayoutManager, GridLayoutManager, AbsoluteLayoutManager**  
  - Concrete implementations that measure and arrange children according to stack, flex, grid, or absolute rules.
  - Each is defined in **public-api** as a separate header and source pair: `stack-layout-manager.h`/`.cpp`, `grid-layout-manager.h`/`.cpp`, `flex-layout-manager.h`/`.cpp`, `absolute-layout-manager.h`/`.cpp` (and `scroll-view-layout-manager.h`/`.cpp` for ScrollView).
  - Each reads per-child parameters from the corresponding `*ParamsImpl` trait attached to child views (e.g., `AbsoluteLayoutManager` reads `AbsoluteLayoutParamsImpl`).
  - Custom layouts can subclass `LayoutManager` (or one of these managers) and attach it to any View via `View::AttachLayoutManager()`, or use `View::SetMeasureCallback()`/`SetArrangeCallback()` from the public API.

### 4. Layout Types (layout-types)

- **MeasuredSize**: measured width and height.
- **LayoutRect**: x, y, width, height (placement region).
- **WRAP_CONTENT**: constant (-1.0f) indicating the view sizes to fit its content (natural size or children bounding box).
- **MATCH_PARENT**: constant (-2.0f) indicating the view fills the parent container's available space. See **MATCH_PARENT semantics** below.
- **LayoutAlignment**: FILL, START, CENTER, END (used by GridLayoutParams and StackLayoutParams for cross-axis alignment).
---

## Behavior

### Layout root

A **layout root** is a top-level View in the layout hierarchy (its parent is not a layout). When `InvalidateMeasure()` or `InvalidateArrange()` is called, the full invalidation propagates up to the layout root, which registers with the LayoutController via `RequestLayoutInternal(ViewImpl*)`. Outside the layout processing window defined below, registration arms one coalesced outstanding ProcessEvents wake so the root is processed on the next frame. Inside that window, the root is still registered but its own invalidation does not arm an idle wake. A not-yet-started turn for that root in the current batch may consume it immediately; otherwise it remains pending until a later independently triggered ProcessEvents or an explicit `ProcessLayouts()` drains it. The *Internal* entry point is an API-layering detail, not an exemption from this wake policy.

### Two-phase layout: Measure and Arrange

1. **Measure**  
   - Given width/height constraints from the parent, the View (and its LayoutManager, if any) computes measured size for itself and its children.  
   - The result is cached as `MeasuredSize`; if constraints are unchanged, measurement is skipped.

2. **Arrange**  
   - The View is given a `LayoutRect` (typically from 0,0 with the measured size). It applies its own alignment and margin, and if it has a LayoutManager, the manager calls `Arrange(child, childBounds)` for each child to set position and size.

### Layout caching and the measure/arrange contract

Both phases are cached, and both caches skip work rather than change results.
Understanding what they key on is the whole of what a custom View, LayoutManager
or callback has to know.

The **measure cache** is unconditional. `View::Measure()` serves a stored
`MeasuredSize` whenever the normalised constraint is unchanged and nothing has
invalidated the view's layout, and the measure implementation — `OnMeasure()`, an
attached `LayoutManager::Measure()`, or a `MeasureCallback` — is simply not
called. There is no opt-out. A measure implementation is therefore **required** to be
a pure function of:

- its two constraints,
- the view's effective scale,
- the view's effective layout direction,
- the view's own layout-tracked state (requested size, padding, margin, min/max
  bounds, layout params, child list),
- its children's measured sizes.

Anything else it reads, it owns: it must call `InvalidateMeasure()` itself when
that state changes. Nothing else will: an unrelated pass re-runs the layout root,
whose implementation re-measures this view at the same constraint, and this view
serves its cache again. An invalidation on a sibling propagates upward and never
reaches this view, and an ancestor's own miss does not clear this view's cache.
The stale measured size is recovered only by an invalidation that reaches THIS
view -- its own `InvalidateMeasure()`, a recursive drop (scale change, reparent,
direction change), or a `LayoutManager`'s `InvalidateOwnerMeasure()` -- or by a
pass that hands it a different normalised constraint or effective scale. The same
reasoning applies to the arrange cache, with the input bounds and the effective
layout direction as the keyed inputs.

The effective scale is part of the measure cache key, so a scale change alone
forces a re-measure. The effective scale is resolved in one place,
`ViewDataImpl::ComputeEffectiveScale()`; when the process-wide UI-scale master
switch (`UiScaleManager::SetScalable(false)`) is off it returns `1.0` for every
view regardless of the view's `UiScalePolicy`, so the whole tree behaves as
unscaled.

The **arrange cache** uses `ArrangePolicy::IF_CHANGED` by default.
`View::Arrange()` may serve a stored result when the input bounds, effective layout
direction and effective scale are unchanged and nothing has invalidated the view.
The default applies to `OnArrange()`, a callback installed through the one-argument
`SetArrangeCallback()`, and `LayoutManager::Arrange()`. It reaches the same
guarantee about the scale by a different route: the scale is not part of the
arrange key, it is an invalidation — a scale change drops the entry.

Use `ArrangePolicy::ALWAYS` when an arrange implementation reads state outside
layout invalidation or performs externally visible work on every pass:

| Arrange implementation | How it selects `ArrangePolicy::ALWAYS` |
|---|---|
| `OnArrange()` override | `ViewImpl::SetArrangePolicy(ArrangePolicy::ALWAYS)` |
| `ArrangeCallback` | `View::SetArrangeCallback(callback, ArrangePolicy::ALWAYS)` |
| `LayoutManager::Arrange()` | protected `LayoutManager::SetArrangePolicy(ArrangePolicy::ALWAYS)` |

Policy is stored on the implementation instance and inherited by subclasses. A
subclass may select another policy after its base constructor completes. An arrange
implementation that reads ancestor or world geometry (`SCREEN_POSITION`,
`WORLD_POSITION`, window coordinates), pushes state to a surface outside the actor
tree, or depends on mutable state without invalidating arrange must use
`ArrangePolicy::ALWAYS`. `VideoView`, `WebView`, `RecyclerView` and the ScrollView
layout manager are the in-library examples.

**A cache hit is an optimisation of the work, never of the result.** Serving the
arrange cache for a view with children does not prune the subtree: it replays it,
performing per node exactly the observable work a re-run performs — reconciling
the actor against the node's arranged bounds, mirroring direct children under
right-to-left, and notifying `LayoutFinishedSignal()` subscribers. Geometry
written outside layout is repaired either way. What a hit elides is the call to the
arrange implementation, and with it the recomputation of a result already known.

The policy is evaluated at every level, so an `ArrangePolicy::ALWAYS` view refuses
the cache hit of every ancestor **above** it: each of those ancestors misses and
re-runs its own arrange implementation. It does not make everything beneath the
ancestor re-run — a sibling subtree that a re-running ancestor re-enters still
evaluates its own predicate and can serve its own hit, in which case it is replayed
rather than re-run. The cost is the ancestor **path**, not every view below it.

`LayoutFinishedSignal()` is therefore **pass-based**: a subscriber is told its
view was arranged in this pass, whether that pass ran the arrange implementation or
served the cache. It is not a "bounds changed" notification.

Delivery is nevertheless gated on the window reaching quiescence. A root retained in
the pending set by an in-processing invalidation is still pending even though it did
not arm an idle wake, so completion notification is delayed until another processing
cycle drains that parked work.

### Invalidation

`InvalidateMeasure()` / `InvalidateArrange()` do two things: they mark the view,
and they walk its ancestor chain to a layout root and register that root with the
LayoutController.

The mark and required ancestor state always happen. The walk is coalesced: a view records the
*invalidation generation* in which it last walked, and while that record is current
— meaning the registration it made is still pending and the chain it marked is still
marked — a further invalidation on the same axis skips the walk. The generation ends
whenever the controller drains its pending set and whenever an outermost
Measure/Arrange pass completes (a pass is the only consumer of dirty bits, and a manual
`Measure()`/`Arrange()` call is a pass too), so the next invalidation walks again.
The controller also ends the generation after a processing frame records a no-self-wake
request. This ensures a later out-of-processing invalidation cannot be coalesced away: it
walks to the already-pending root and can arm the coalesced wake. While any pass is on
the stack the skip is disabled outright for every invalidation, because a mid-pass walk
also poisons in-progress ancestors. Coalescing changes only how often the ancestor chain
is traversed; it never distinguishes public from framework-internal origins.

The measure and arrange records are independent, because an arrange walk leaves
the ancestors' measure caches valid and an ancestor measure hit does not
re-measure its children.

#### The layout processing window

The **layout processing window** is open while either a Measure/Arrange pass is on the
stack or a `LayoutFinished` emit is in progress. A direct public invalidation from that
window is a contract violation and is logged once per View (`DALI_LOG_ERROR`, latched so
a repeating call site cannot flood the log), but the invalidation is **retained rather
than ignored**:

- relevant cache-valid state is revoked, dirty state and in-progress-pass poison are
  recorded where required, and the ancestor chain is walked;
- the layout root is registered and remains in the controller's pending set;
- registration is **PARKED** and does not request an idle ProcessEvents wake.

If the current layout batch already contains a turn for that root and the turn has not
started, that turn may consume the pending state in the same batch. Work not consumed
by the current batch remains PARKED without arming a wake.

This separates correctness state from main-loop scheduling. A self-invalidating
producer cannot create an endless pass -> emit -> idle-wake cycle, but its pending work
is still processed by a later independently triggered ProcessEvents or an explicit
`LayoutController::ProcessLayouts()`. An out-of-processing event-time request walks to
the root and arms at most one coalesced outstanding wake, draining any work that was
already parked. After a processing frame records a no-self-wake request, it ends the
invalidation generation so that this event-time walk cannot be skipped by generation
coalescing.

Layout-transition lifecycle callbacks run after the Measure/Arrange pass and outside
this window. Their documented mutation and transition-chaining paths therefore remain
wakeable and use the same coalesced outstanding wake.

The scheduling rule applies to every origin. Property setters, resource paths and tree
mutations (`Add()` / `Remove()`) use the same PARK behavior when they run inside the
window; `ViewDataImpl` and `LayoutController::RequestLayoutInternal()` are not exempt.
Defer layout-affecting state changes to event time, or arrange an independent idle/timer
wake, when prompt follow-up is required.

Revoking a cache entry prevents it from satisfying a later cache hit; it does not
immediately replace the last completed result. Until parked work is drained,
`GetMeasuredSize()` or actor geometry may therefore still expose the previous completed
pass.

**The contract, stated plainly.** Invalidating layout during layout processing is
prohibited in principle and honoured only best-effort — exactly dali-core's relayout
policy, where `RequestRelayout()` raised while `ProcessEvents` runs is retained but
requests no wake. Parked work is serviced by the NEXT externally triggered
ProcessEvents cycle; on a quiescent application (no input, animation or timer) that
next cycle may be indefinitely later, and `LayoutFinished` stays deferred with it.
Components and applications must therefore never rely on in-processing invalidation
for the correctness of the CURRENT frame. When a processing frame ends with parked
work and no outstanding wake, the controller logs one `DALI_LOG_ERROR` per parked
episode (covering framework-internal origins the per-View diagnostic cannot see);
the episode latch resets when the pending set drains.

**`LayoutManager` state.** A manager that keeps state of its own — an
orientation, a spacing, a set of row definitions — is outside every cache key,
because neither key can see it. Pair every such setter with
`LayoutManager::InvalidateOwnerMeasure()` (or `InvalidateOwnerArrange()` when only
placement is affected); the in-library managers all do, which is what makes that
state part of the layout-tracked inputs of their `Measure()` and `Arrange()`
implementations.

### Measure constraints (sign-encoded budget)

During Measure the parent passes the width and height constraints each as a single **sign-encoded `float`** — there is no separate mode field and no measure-mode enum anywhere in the API:

- **Non-negative value** — a concrete available-space budget in *visual* (scale-applied) pixels. `OnMeasure` divides it by `GetEffectiveScale()` to obtain the natural-unit budget before computing sizes. This budget acts as an **at-most ceiling**.
- **Negative value** — a sentinel carrying intent, not a size: `WRAP_CONTENT` (-1.0f) or `MATCH_PARENT` (-2.0f).

Before `OnMeasure` runs, `ViewImpl::Measure` reconciles the incoming constraint against this view's min/max bounds: `effective = std::min(std::max(constraint, Minimum), Maximum)`. With the default `Minimum` of `0`, a negative sentinel constraint is therefore **floored to 0** — so the budget that actually reaches `OnMeasure` is always `>= 0`. (An axis is left "unbounded" not by a negative value but by passing a large positive budget such as `FLT_MAX`, as ScrollView does on its scrollable axes.)

For a `WRAP_CONTENT` view, the measured result (children bounding box for a container, or `GetNaturalSize()` for a leaf) is **clamped down to the incoming budget** via `std::min(size, budget)` whenever that budget is non-negative. So a `WRAP_CONTENT` child measured against a finite budget is clamped to it and does **not** overflow the parent; measured against a very large budget it is effectively unclamped and sizes purely to its content. `MATCH_PARENT` and fixed-size requests do not go through this clamp — they resolve directly to the minimum size and the requested size respectively.

This clamp bounds a `WRAP_CONTENT` result to the available budget. Setting `SetMaximumWidth()` / `SetMaximumHeight()` is therefore not needed to stop a `WRAP_CONTENT` child from overflowing its parent's budget — those are a separate min/max bounds mechanism applied earlier in Measure.

### Requested vs Minimum/Maximum (max-wins)

Each view can carry independent minimum and maximum size bounds
(`SetMinimumWidth()` / `SetMaximumWidth()` and the height equivalents).
After the measured size is produced by `OnMeasure` / the LayoutManager /
the measure callback, `ViewImpl::ApplyConstraints` reconciles it against
those bounds, for width and height independently.

**Clamp order:** the minimum (floor) is applied first, then the maximum
(ceiling) — `result = std::min(std::max(value, Minimum), Maximum)`.
Because the `std::min` against `Maximum` is the *last* operation, the
maximum always wins on conflict. So when `Minimum > Maximum`, the result
collapses to `Maximum` ("max-wins"). Example: with `value = 50`,
`Minimum = 100`, `Maximum = 30`, the floor lifts it to `100`, then the
ceiling drops it to `30` — the final size is `30`.

- **Defaults:** `Minimum` is `0` and `Maximum` is `FLT_MAX`, so by
  default the clamp is a no-op; the bound storage is allocated lazily.
- **Units:** the bounds are stored in natural (pre-scale) units and
  multiplied by the effective scale at clamp time, so they compare
  against the visual-unit measured size.
- The incoming constraint is itself pre-clamped to `[Minimum, Maximum]`
  before children are measured, so children see the effective available
  space rather than the raw constraint.

DALi UI unconditionally lets the maximum win via the fixed clamp order —
there is no min-wins branch or configurable policy for a
`Minimum > Maximum` conflict.

### MATCH_PARENT semantics

A `MATCH_PARENT` view acts as a **follower**: it fills the space assigned by
its parent during Arrange, rather than influencing the parent's own size
during Measure.

**Measure phase:**
- A `MATCH_PARENT` view reports its minimum size (`SetMinimumWidth` /
  `SetMinimumHeight`, default 0) as its desired size, but its children
  are still measured normally with the incoming constraint. Only the
  view's own return value is minSize.
- Because the desired size is 0 (or minimum), a `WRAP_CONTENT` parent
  determines its own size exclusively from non-MATCH_PARENT children.
- Example: parent is `WRAP_CONTENT`, child A is fixed 100px, child B is
  `MATCH_PARENT` → parent's desired width = 100 (child B contributes 0).

**Arrange phase:**
- The parent assigns `MATCH_PARENT` children the full available content
  area (parent size − parent padding − child margin).
- Before arranging, the parent re-measures the `MATCH_PARENT` child with
  the final bounds, so its internal state (text layout, nested children)
  reflects the real available space. The Measure cache prevents redundant
  work when the constraint has not changed.
- Continuing the example: parent arranged at 100px → child B is
  re-measured and arranged at 100px.

**Per-LayoutManager behavior:**
- **StackLayout**: cross-axis `MATCH_PARENT` fills the available cross-axis
  space. Main-axis `MATCH_PARENT` fills the full available main-axis space
  unless the child also has weight > 0, in which case the weight share
  determines the main-axis size and main-axis `MATCH_PARENT` is ignored.
- **FlexLayout**: cross-axis `MATCH_PARENT` fills the flex line's cross
  size. Main-axis `MATCH_PARENT` fills the available main axis; use
  flex-grow for proportional distribution.
- **GridLayout**: `MATCH_PARENT` children do not drive AUTO row/column
  sizing. In Arrange, cells are filled by default (FILL alignment).
- **AbsoluteLayout**: `MATCH_PARENT` children fill the available content
  area (when no explicit bounds are set via AbsoluteLayoutParams).
- **ScrollView**: `MATCH_PARENT` children fill the viewport in Arrange.

### Grid placement (explicit cells)

`GridLayout` uses **explicit placement**. Every child's cell
comes solely from its `GridLayoutParams` `Row` / `Column` / `RowSpan` /
`ColumnSpan` properties; there is no automatic placement (auto-flow). A
child with no params defaults to row 0, column 0, span 1 (`mRow=0`,
`mColumn=0`, `mRowSpan=1`, `mColumnSpan=1`). A consequence is that placement is
**not auto-distributed** — every unplaced child (and any children sharing the
same row/column) simply **stacks on cell (0,0)**.

- **Track count is fixed.** Rows/columns count = `max(1, definitions count)`,
  so an axis with no definitions falls back to a single implicit track. No
  implicit tracks are ever created for out-of-range indices.
- **Out-of-range indices clamp to the last track.** `row`/`col` are clamped to
  `[0, count-1]` and spans are clamped to the remaining tracks
  (`rowSpan = min(rowSpan, rowCount - row)`), in both Measure and Arrange. An
  index past the grid is pinned to the last existing track, not given a new one.

**Spanning children and AUTO tracks.** Auto-track intrinsic sizing runs in two
passes (in both Measure and Arrange). Pass 1 records the content size of each
**non-spanning** child as the floor of its **auto-like** track — a track that is
explicitly `AUTO`, or the implicit single track of an axis with no definitions.
Pass 2 distributes extra space for spanning children: for
a child with span > 1, it sums the sizes of the spanned tracks plus the gaps
between them; if the child (size + margin) needs more, the deficit is
distributed **equally** across only the spanned auto-like tracks
(`share = deficit / spannedAutoCount`). `STAR` and `ABSOLUTE` tracks crossed by
the span are left untouched. A spanning child therefore enlarges a crossed auto
track **only** when its measured size exceeds the floors already accumulated for
the tracks it spans — if they are already large enough, it adds nothing. (As
elsewhere, `MATCH_PARENT` children do not drive auto sizing.)

The model is deliberately simple: no auto-placement, no implicit track
creation, and the deficit is shared equally across the spanned auto tracks
rather than via any growth-limit ordering — an explicit, fixed-track grid.

### RTL layout direction (child X mirroring)

Right-to-left (RTL) handling lives entirely at the View level, not in any
LayoutManager. After every `OnArrange` variant — `ArrangeCallback`,
LayoutManager, or the default `OnArrange` — `ViewImpl::Arrange` makes a
single call to `ViewImpl::ApplyLayoutDirection(bounds.width)`, keeping all
layout managers **direction-agnostic**. Layout managers (including
`AbsoluteLayoutManager`) always arrange in a `LEFT_TO_RIGHT` frame and never
inspect the layout direction themselves.

`ApplyLayoutDirection` early-returns unless the View's effective layout
direction resolves to `RIGHT_TO_LEFT`, so under `LEFT_TO_RIGHT` nothing is
mirrored. Under `RIGHT_TO_LEFT` it flips each direct child's X about the
parent's arranged width: `POSITION_X = parentWidth − logicalX − childW`,
where `logicalX` and `childW` are the child's **logical** arranged bounds
(`GetArrangedBounds().x` / `.width`), not its actor `POSITION_X`. Mirroring
from the logical bounds makes the flip a pure function of arranged geometry --
idempotent and immune to an external `POSITION_X` write -- rather than an
involution over the actor's persistent position. The write is also suppressed
when the actor already holds the mirrored value, so a settled `RIGHT_TO_LEFT`
subtree performs no position writes at all. A child that no arrange
implementation has arranged has no parent-owned logical bounds and is left
untouched. Reading
its current physical actor position as logical input would make repeated
identical passes alternate between mirrored and unmirrored coordinates.

- The mirror is **generic**: it applies uniformly to every direct child of
  every View. `AbsoluteLayout` is just one case — a child arranged at some X
  under `LEFT_TO_RIGHT` is flipped about the parent's content width under
  `RIGHT_TO_LEFT`. A child placed at `X = 0` therefore lands flush against
  the right edge, so the layout origin effectively moves to the **top-right**
  under RTL.
- **STANDALONE children are skipped.** `ApplyLayoutDirection` `continue`s
  past any child whose `LayoutMode` is `STANDALONE`, so the only way to opt a
  child out of mirroring is `SetLayoutMode(LayoutMode::STANDALONE)`.
- There is **no RTL-specific opt-out flag** for Absolute children:
  `AbsoluteLayoutFlags` contains only proportional sizing/positioning flags
  (`X_PROPORTIONAL`, `WIDTH_PROPORTIONAL`, `POSITION_PROPORTIONAL`, …) and
  nothing related to layout direction.
- `ArrangeCallback`s share the same contract: arrange children in a
  `LEFT_TO_RIGHT` frame and let the framework mirror — callbacks must not
  apply RTL mirroring themselves.

Note the offset value itself is mirrored: under `RIGHT_TO_LEFT` every direct
child's X is reflected about the parent width regardless of how it was
positioned, so even an absolutely positioned child is moved to the mirrored
edge.

#### How a direction change reaches layout

The direction itself lives in dali-core, so a change has to be turned into a
layout invalidation explicitly. Four things make this hold -- two mechanisms, one
structural fact and one backstop -- and none of them costs a plain child View
anything:

- **Layout roots hook the actor signal, lazily.** The first time a View
  registers with the `LayoutController` **on a live window** it connects the
  actor's layout-direction-changed signal, once and for good. Only layout roots
  that are on-scene in a window (and on-scene `STANDALONE` boundary views, which
  register themselves) ever do, so the per-View cost of a signal connection is
  gone. This is the only mechanism that can see a direction set on a **non-View
  ancestor** — an intermediate `Layer`, or the window's root layer.
- **Direction property writes on a View are intercepted.** `LAYOUT_DIRECTION`
  (and the two legacy indices) reach `ViewDataImpl::OnPropertySet`, which raises
  the same invalidation. This is what covers a **mid-tree** View that is not a
  layout root, and — being window-independent — an off-scene write too. A hooked
  view skips it (the signal has already fired), and a write that does not move
  the resolved direction is dropped by a value guard.
- **An off-scene move needs no hook.** No layout pass can run without a window,
  and reconnection (`OnViewSceneConnection`) drops the whole subtree's cached
  results via `ResetSubtreeScaleAndLayoutCaches()` before registering the
  reconnecting root, so nothing stale survives it.
- **The arrange cache keys on the recorded direction** (`mLastArrangeDirection`),
  which is the backstop under all of it: a missed invalidation degrades to a
  cache **miss**, never to an arrangement mirrored the wrong way round.

The invalidation is **subtree-recursive**: it drops both layout caches and raises
both dirty bits on the changed view and on every descendant that inherits from
it, and it **prunes** at any child holding a direction of its own — the exact
mirror of dali-core's inherit walk, whose resolved direction did not move either.
A `STANDALONE` descendant is a scheduling boundary, so it takes a full
`InvalidateMeasure()` and re-registers itself instead.

### LayoutMode::STANDALONE

A child View can opt out of its parent's layout flow by calling
`SetLayoutMode(LayoutMode::STANDALONE)`. Standalone children remain in the
parent hierarchy (they are still measured, arranged and rendered) but are
treated as floating elements rather than as participants in the parent's
layout algorithm. This is useful for floating overlays, drag previews,
tooltips and absolute positioning inside any LayoutManager.

A Standalone child:

- Is excluded from the parent's accumulation, spacing, line/cell building,
  flex-grow / flex-shrink, weight distribution and visible-child counting,
  in both `ViewImpl::OnMeasure` (plain View parent) and the Stack / Grid /
  Flex / Absolute layout managers.
- Is still measured normally so `WRAP_CONTENT` and explicit
  `RequestedWidth` / `RequestedHeight` all resolve. `MATCH_PARENT` reports
  minimum desired in Measure and fills the parent's full size (minus own
  margin) in Arrange.
- **Ignores the parent's padding entirely.** Its measured size is the
  parent's full inner size minus the child's own margin, and its final
  position is `PositionX` / `PositionY` plus the child's own margin in the
  parent's coordinate space. Size and position therefore follow the same rule
  (parent padding ignored, own margin honored), so a Standalone child with
  `MATCH_PARENT` fills the parent edge to edge regardless of parent padding,
  and any margin set on the Standalone child shifts it inward consistently in
  both axes.
- **Applies its own min/max to its slot.** A boundary child has no parent
  layout to clamp it, so the derivation of its slot enforces its
  `Minimum`/`Maximum` width and height. The same derivation runs whether the
  slot comes from the parent's arrange pass or from the child's own
  layout-root pass, so both agree; for a `MATCH_PARENT` axis it is the only
  place the clamp reaches, because the measured value is discarded there.

### Invalidation flow

When layout must be recomputed (e.g. size or child change):  
`ViewImpl::InvalidateMeasure()` or `InvalidateArrange()` → propagate to parent layout → at layout root, `RegisterWithLayoutController()` → `LayoutControllerImpl::RequestLayout(ViewImpl*)` adds the root to `mPendingViews`. From there scheduling has two branches:

- outside the layout processing window, the request arms one coalesced outstanding wake;
  the next ProcessEvents runs Measure/Arrange in the pre-process phase and emits settled
  `LayoutFinished` signals in post-process;
- inside the window, the same full invalidation and pending registration occur but no
  self idle wake is requested. The next independently triggered ProcessEvents or an
  explicit `ProcessLayouts()` drains the parked work, and `LayoutFinished` remains
  delayed until the pending set is empty.

---

## Diagrams

Two PlantUML diagram SOURCE files live beside this document in the repository. They are not rendered here; open them with a PlantUML viewer.

- **Class structure**: `docs/layout-class-diagram.puml`  
  - Public API (View, Layout, StackLayout, …), Integration (ViewImpl, LayoutImpl, …), LayoutManagers, and layout types with inheritance and references.
- **Calculation flow**: `docs/layout-sequence-diagram.puml`  
  - Sequence from Adaptor → LayoutControllerImpl → layout root ViewImpl → LayoutManager → child ViewImpl for Measure/Arrange and a summary of invalidation.

---

## Summary

| Area | Description |
|------|-------------|
| Public child API | Child add/remove/insert/bulk-remove uses Actor::Add/Remove/InsertAbove/InsertBelow/RemoveAll. View provides Remove(View, RemovePolicy) and RemoveAll(RemovePolicy). GetChildCount/GetChildAt are inherited from Actor (actor tree, ghost-inclusive); the logical layout child list is exposed via GetChildViewCount, GetChildViewAt and IndexOfChildView. |
| Layout processing | LayoutController collects layout roots per window and runs Measure then Arrange once per frame. |
| Implementation | ViewImpl holds children and can attach a LayoutManager as a Trait via `View::AttachLayoutManager()`. Applications can customize measure/arrange via `SetMeasureCallback()`/`SetArrangeCallback()`. |
