# DALi UI Foundation - Layout Class and Behavior Structure

## Overview

The layout system in DALi UI Foundation computes **size (Measure)** and **position (Arrange)** of child views over a **View** hierarchy. Child management uses the inherited Actor `Add`/`Remove` API; `Insert(index, View)` and `RemoveAllChildren()` are provided by View for index-based insertion and bulk removal.

Layout processing is driven by **LayoutController** per window. Each frame, it runs Measure then Arrange on layout roots that have been invalidated.

---

## Class Structure

### 1. Public API (Handles)

- **View**  
  - Layout properties: `SetRequestedWidth` / `SetRequestedHeight`, `SetMargin` / `SetPadding`, alignment, visibility, etc.
  - `GetSize()` returns the actual rendered size (read-only).
  - Measure/Arrange are invoked internally by the layout system; applications may request recomputation via `InvalidateMeasure()` / `InvalidateArrange()`.
  - Child add/remove uses inherited Actor `Add`/`Remove`. `Insert(index, View)` and `RemoveAllChildren()` are available for index-based insertion and bulk removal.

- **Layout** (inherits View)
  - Child management: `Add(View)` (inherited from Actor), `Insert(index, View)`, `Remove(View)` (inherited from Actor), `RemoveAllChildren()`, `GetChildCount()`, `GetChildAt(index)`, `IndexOfChild(View)`, `Children(initializer_list<View>)`.
  - Always has a LayoutManager stored as a Trait (`ReservedTraitId::LAYOUT_MANAGER`); derived classes attach Stack/Flex/Grid/Absolute algorithms in `OnInitialize()`.

- **Custom Layout Callbacks**
  - `View::SetMeasureCallback(UniquePtr<LayoutMeasureCallback>)` and `View::SetArrangeCallback(UniquePtr<LayoutArrangeCallback>)` allow applications to customize measure/arrange behavior on any View or Layout subclass.
  - `LayoutMeasureCallback` and `LayoutArrangeCallback` follow the DALi `RenderCallback`/`VertexBufferUpdateCallback` pattern, using `CallbackBase` internally.
  - Callbacks are stored as a `LayoutCallbacks` Trait (`ReservedTraitId::LAYOUT_SIGNALS`), created lazily on first use. When set, callbacks take priority over the default LayoutManager.
    ```cpp
    Layout layout = Layout::New();
    layout.SetMeasureCallback(LayoutMeasureCallback::New(&MyMeasure));   // free function
    layout.SetArrangeCallback(LayoutArrangeCallback::New(&MyArrange));   // free function
    // For member functions, pass the object instance:
    // layout.SetMeasureCallback(LayoutMeasureCallback::New(this, &MyClass::OnMeasure));
    ```

- **StackLayout, FlexLayout, GridLayout, AbsoluteLayout**
  - Each adds type-specific options: orientation/spacing/weight, direction/wrap/justify/align, row/column definitions, absolute bounds, etc.
  - Per-child layout parameters are set via `View::SetLayoutParams()`:
    - `view.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))`
    - `view.SetLayoutParams(GridLayoutParams::New().SetRow(2).SetColumn(3).SetRowSpan(1).SetColumnSpan(2))`
    - `view.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f).SetFlexShrink(0.0f))`
    - `view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(rect).SetFlags(flags))`
  - `SetLayoutParams()` stores the params handle and invalidates the View's measure cache.
  - Internally these store data via type-safe Trait objects attached to each child view.

- **AbsoluteLayoutParams, FlexLayoutParams, GridLayoutParams, StackLayoutParams**
  - Public Trait handle classes that provide direct access to per-child layout parameters.
  - Attached via `View::SetLayoutParams()` which stores the params handle onto the View.
  - Setters return `*this` for method chaining.
  - `New()` creates a new instance with default values; `New(other)` creates an independent copy:
    ```cpp
    auto base = GridLayoutParams::New().SetRowSpan(2).SetColumnSpan(2);
    viewA.SetLayoutParams(GridLayoutParams::New(base).SetColumn(0));
    viewB.SetLayoutParams(GridLayoutParams::New(base).SetColumn(1));
    ```
  - **Note:** `SetLayoutParams()` stores the handle as-is (no deep copy). Passing the same handle to multiple Views causes them to share state. Use `New(other)` to create an independent copy when reusing params across Views.

- **LayoutController**  
  - Singleton per window via `LayoutController::Get(Window)`.  
  - When layout is invalidated, layout roots are registered; the Adaptor calls `Process()` once per frame, and the controller runs `ProcessLayouts()` to perform Measure and Arrange.

### 2. Integration API (Implementation)

- **ViewImpl** (DALi ControlImpl-derived)
  - Holds the actual Measure/Arrange logic, size specifications, margin/padding/alignment/visibility, and child container.
  - Provides `Insert(index, View)`, `RemoveAllChildren`, `GetChildCount`, `GetChildAt`, `IndexOfChild`, `Contents`, etc. Child add/remove uses Actor `Add`/`Remove` with `OnChildAdd`/`OnChildRemove` callbacks to sync the internal child container. Child order changes (via `Raise`/`Lower`/etc.) are detected via `ChildOrderChangedSignal` to keep `mChildren` in sync.
  - `GetParentLayout()`, `IsLayout()`, and invalidation propagate to the parent until a layout root is reached, which registers with the LayoutController.

- **LayoutImpl** (inherits ViewImpl)
  - Provides `SetLayoutManager()` (protected) for derived classes to attach a LayoutManager as a Trait in `OnInitialize()`.
  - `OnMeasure()`/`OnArrange()` check: callback (priority) → LayoutManager → ViewImpl fallback.
  - `IsLayout()` returns `true`.
  - Child APIs are inherited from ViewImpl.

- **StackLayoutImpl, FlexLayoutImpl, GridLayoutImpl, AbsoluteLayoutImpl**
  - Each creates and attaches its LayoutManager via `SetLayoutManager()` in `OnInitialize()`.

- **AbsoluteLayoutParamsImpl, FlexLayoutParamsImpl, GridLayoutParamsImpl, StackLayoutParamsImpl**
  - TraitImpl-derived classes that store per-child layout parameters (e.g., bounds/flags, grow/shrink/basis/alignSelf, row/column/span, weight).
  - Attached to child views via `ReservedTraitId` and accessed by layout managers during Measure/Arrange.
  - Each provides `Get(ViewImpl&)` (returns nullptr if not attached) and `GetOrCreate(ViewImpl&)` (creates if missing).

- **LayoutControllerImpl**  
  - Keeps layout roots in `mPendingViews`; `ProcessLayouts()` resolves constraints, then runs Measure and Arrange for each root.

### 3. Layout Managers (Algorithms)

- **LayoutManager** (abstract)  
  - `Measure(ViewImpl*, widthConstraint, heightConstraint)`: compute measured size for the container and its children.  
  - `ArrangeChildren(ViewImpl*, bounds)`: place children within the given bounds.  
  - Uses internal helpers such as `MeasureChild`, `ArrangeChild`, and `GetChildren(ViewImpl*)`.

- **StackLayoutManager, FlexLayoutManager, GridLayoutManager, AbsoluteLayoutManager**  
  - Concrete implementations that measure and arrange children according to stack, flex, grid, or absolute rules.
  - Each is defined in **integration-api** as a separate header and source pair: `stack-layout-manager.h`/`.cpp`, `grid-layout-manager.h`/`.cpp`, `flex-layout-manager.h`/`.cpp`, `absolute-layout-manager.h`/`.cpp`.
  - Each reads per-child parameters from the corresponding `*ParamsImpl` trait attached to child views (e.g., `AbsoluteLayoutManager` reads `AbsoluteLayoutParamsImpl`).
  - Custom layouts can subclass one of these managers and attach it via `LayoutImpl::SetLayoutManager()` in `OnInitialize()`, or use `View::SetMeasureCallback()`/`SetArrangeCallback()` from the public API.

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
  (use weight for proportional sharing).
- **FlexLayout**: cross-axis `MATCH_PARENT` fills the flex line's cross
  size. Main-axis `MATCH_PARENT` fills the available main axis; use
  flex-grow for proportional distribution.
- **GridLayout**: `MATCH_PARENT` children do not drive AUTO row/column
  sizing. In Arrange, cells are filled by default (FILL alignment).
- **AbsoluteLayout**: `MATCH_PARENT` children fill the available content
  area (when no explicit bounds are set via AbsoluteLayoutParams).
- **ScrollView**: `MATCH_PARENT` children fill the viewport in Arrange.

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
`ViewImpl::InvalidateMeasure()` or `InvalidateArrange()` → propagate to parent layout → at layout root, `RegisterWithLayoutController()` → `LayoutControllerImpl::RequestLayout(ViewImpl*)` adds the root to `mPendingViews` → next frame the Adaptor calls `Process()` → `ProcessLayouts()` runs Measure then Arrange for those roots.

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
| Public child API | Child add/remove uses Actor::Add/Remove. View provides Insert(index, View) for index-based insertion and RemoveAllChildren() for bulk removal. GetChildCount, GetChildAt, IndexOfChild, Children are available on View. |
| Layout processing | LayoutController collects layout roots per window and runs Measure then Arrange once per frame. |
| Implementation | ViewImpl holds children; LayoutImpl attaches a LayoutManager as a Trait in `OnInitialize()`. Applications can customize measure/arrange via `SetMeasureCallback()`/`SetArrangeCallback()`. |
