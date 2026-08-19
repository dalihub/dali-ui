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

A **layout root** is a top-level View in the layout hierarchy (its parent is not a layout). When `InvalidateMeasure()` or `InvalidateArrange()` is called, it propagates up to the layout root, which then registers with the LayoutController via `RequestLayout(ViewImpl*)` so that the root is processed on the next frame.

### Two-phase layout: Measure and Arrange

1. **Measure**  
   - Given width/height constraints from the parent, the View (and its LayoutManager, if any) computes measured size for itself and its children.  
   - The result is cached as `MeasuredSize`; if constraints are unchanged, measurement is skipped.

2. **Arrange**  
   - The View is given a `LayoutRect` (typically from 0,0 with the measured size). It applies its own alignment and margin, and if it has a LayoutManager, the manager calls `Arrange(child, childBounds)` for each child to set position and size.

### Layout caching and the producer contract

Both phases are cached, and both caches skip work rather than change results.
Understanding what they key on is the whole of what a custom View, LayoutManager
or callback has to know.

The **measure cache** is unconditional. `View::Measure()` serves a stored
`MeasuredSize` whenever the normalised constraint is unchanged and nothing has
invalidated the view's layout, and the measure producer — `OnMeasure()`, an
attached `LayoutManager::Measure()`, or a `MeasureCallback` — is simply not
called. There is no opt-out. A measure producer is therefore **required** to be a
pure function of:

- its two constraints,
- the view's effective scale,
- the view's effective layout direction,
- the view's own layout-tracked state (requested size, padding, margin, min/max
  bounds, layout params, child list),
- its children's measured sizes.

Anything else it reads, it owns: it must call `InvalidateMeasure()` itself when
that state changes, or the view keeps its previous measured size until some
unrelated invalidation happens to arrive.

The effective scale is resolved in one place,
`ViewDataImpl::ComputeEffectiveScale()`; when the process-wide UI-scale master
switch (`UiScaleManager::SetScalable(false)`) is off it returns `1.0` for every
view regardless of the view's `UiScalePolicy`, so the whole tree behaves as
unscaled.

The **arrange cache** uses `ArrangePolicy::IF_CHANGED` by default.
`View::Arrange()` may serve a stored result when the input bounds, effective layout
direction and effective scale are unchanged and nothing has invalidated the view.
The default applies to `OnArrange()`, a callback installed through the one-argument
`SetArrangeCallback()`, and `LayoutManager::Arrange()`.

Use `ArrangePolicy::ALWAYS` when a producer reads state outside layout
invalidation or performs externally visible work on every pass:

| Producer | How it selects `ALWAYS` |
|---|---|
| `OnArrange()` override | `ViewImpl::SetArrangePolicy(ArrangePolicy::ALWAYS)` |
| `ArrangeCallback` | `View::SetArrangeCallback(callback, ArrangePolicy::ALWAYS)` |
| `LayoutManager::Arrange()` | protected `LayoutManager::SetArrangePolicy(ArrangePolicy::ALWAYS)` |

Policy is stored on the implementation instance and inherited by subclasses. A
subclass may select another policy after its base constructor completes. Producers
that read ancestor or world geometry (`SCREEN_POSITION`, `WORLD_POSITION`, window
coordinates), push state to a surface outside the actor tree, or depend on mutable
state without invalidating arrange must use `ALWAYS`. `VideoView`, `WebView`,
`RecyclerView` and the ScrollView layout manager are the in-library examples.

**A cache hit is an optimisation of the work, never of the result.** Serving the
arrange cache for a view with children does not prune the subtree: it replays it,
performing per node exactly the observable work a re-run performs — reconciling
the actor against the node's arranged bounds, mirroring direct children under
right-to-left, and notifying `LayoutFinishedSignal()` subscribers. Geometry
written outside layout is repaired either way. What a hit elides is the producer
call, and with it the recomputation of a result already known. Because the policy
is evaluated at every level, a single `ALWAYS` producer anywhere in a
subtree makes that whole subtree re-run.

`LayoutFinishedSignal()` is therefore **pass-based**: a subscriber is told its
view was arranged in this pass, whether that pass ran the producer or served the
cache. It is not a "bounds changed" notification.

### Invalidation

`InvalidateMeasure()` / `InvalidateArrange()` do two things: they mark the view,
and they walk its ancestor chain to a layout root and register that root with the
LayoutController.

The mark always happens. The walk is coalesced: a view records the *invalidation
epoch* in which it last walked, and while that record is current — meaning the
registration it made is still pending and the chain it marked is still marked — a
further invalidation on the same axis skips the walk. The epoch ends whenever the
controller drains its pending set and whenever an outermost Measure/Arrange pass
completes (a pass is the only consumer of dirty bits, and a manual
`Measure()`/`Arrange()` call is a pass too), so the next invalidation walks again.
While any pass is on the stack the skip is disabled outright, because a mid-pass
walk also poisons in-progress ancestors. Coalescing changes only how often the
ancestor chain is traversed; a batch of invalidations before one pass is all
serviced by that pass.

The measure and arrange records are independent, because an arrange walk leaves
the ancestors' measure caches valid and an ancestor measure hit does not
re-measure its children.

**`LayoutManager` state.** A manager that keeps state of its own — an
orientation, a spacing, a set of row definitions — is outside every cache key,
because neither key can see it. Pair every such setter with
`LayoutManager::InvalidateOwnerMeasure()` (or `InvalidateOwnerArrange()` when only
placement is affected); the in-library managers all do, which is what makes their
that state part of the layout-tracked inputs used by their `Arrange()` implementations.

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
involution over the actor's persistent position. A child that a producer has
not arranged has no parent-owned logical bounds and is left untouched. Reading
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

### Invalidation flow

When layout must be recomputed (e.g. size or child change):  
`ViewImpl::InvalidateMeasure()` or `InvalidateArrange()` → propagate to parent layout → at layout root, `RegisterWithLayoutController()` → `LayoutControllerImpl::RequestLayout(ViewImpl*)` adds the root to `mPendingViews` → next frame the Adaptor calls `Process()` → in the pre-process phase `ProcessLayouts()` runs Measure then Arrange for those roots → after core size negotiation, the post-process phase emits the `LayoutFinished` signals.

---

## Diagrams

- **Class structure**: `layout-class-diagram.puml`  
  - Public API (View, Layout, StackLayout, …), Integration (ViewImpl, LayoutImpl, …), LayoutManagers, and layout types with inheritance and references.
- **Calculation flow**: `layout-sequence-diagram.puml`  
  - Sequence from Adaptor → LayoutControllerImpl → layout root ViewImpl → LayoutManager → child ViewImpl for Measure/Arrange and a summary of invalidation.

---

## Summary

| Area | Description |
|------|-------------|
| Public child API | Child add/remove/insert/bulk-remove uses Actor::Add/Remove/InsertAbove/InsertBelow/RemoveAll. View provides Remove(View, RemovePolicy) and RemoveAll(RemovePolicy). GetChildCount/GetChildAt are inherited from Actor (actor tree, ghost-inclusive); the logical layout child list is exposed via GetChildViewCount, GetChildViewAt and IndexOfChildView. |
| Layout processing | LayoutController collects layout roots per window and runs Measure then Arrange once per frame. |
| Implementation | ViewImpl holds children and can attach a LayoutManager as a Trait via `View::AttachLayoutManager()`. Applications can customize measure/arrange via `SetMeasureCallback()`/`SetArrangeCallback()`. |
