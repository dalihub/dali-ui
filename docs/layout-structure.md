# DALi UI Foundation - Layout Class and Behavior Structure

## Overview

The layout system in DALi UI Foundation computes **size (Measure)** and **position (Arrange)** of child views over a **View** hierarchy. Child management uses the inherited Actor `Add`/`Remove` API; `Insert(index, View)` and `RemoveAllChildren()` are provided by View for index-based insertion and bulk removal.

Layout processing is driven by **LayoutController** per window. Each frame, it runs Measure then Arrange on layout roots that have been invalidated.

---

## Class Structure

### 1. Public API (Handles)

- **View**  
  - Layout properties: `SetRequestedWidth` / `SetRequestedHeight`, `SetViewMargin` / `SetViewPadding`, alignment, visibility, etc.
  - `GetSize()` returns the actual rendered size (read-only).
  - Measure/Arrange are invoked internally by the layout system; applications may request recomputation via `InvalidateMeasure()` / `InvalidateArrange()`.
  - Child add/remove uses inherited Actor `Add`/`Remove`. `Insert(index, View)` and `RemoveAllChildren()` are available for index-based insertion and bulk removal.

- **Layout** (inherits View)
  - Child management: `Add(View)` (inherited from Actor), `Insert(index, View)`, `Remove(View)` (inherited from Actor), `RemoveAllChildren()`, `GetChildCount()`, `GetChildAt(index)`, `IndexOfChild(View)`, `Contents(initializer_list<View>)`.
  - Always has a LayoutManager stored as a Trait (`ReservedTraitId::LAYOUT_MANAGER`); derived classes attach Stack/Flex/Grid/Absolute algorithms in `OnInitialize()`.

- **Custom Layout Callbacks**
  - `View::SetMeasureCallback(UniquePtr<LayoutMeasureCallback>)` and `View::SetArrangeCallback(UniquePtr<LayoutArrangeCallback>)` allow applications to customize measure/arrange behavior on any View or Layout subclass.
  - `LayoutMeasureCallback` and `LayoutArrangeCallback` follow the DALi `RenderCallback`/`VertexBufferUpdateCallback` pattern, using `CallbackBase` internally.
  - Callbacks are stored as a `LayoutCallbacks` Trait (`ReservedTraitId::LAYOUT_SIGNALS`), created lazily on first use. When set, callbacks take priority over the default LayoutManager.
    ```cpp
    Layout layout = Layout::New();
    layout.SetMeasureCallback(LayoutMeasureCallback::New(&MyLayout::OnMeasure));
    layout.SetArrangeCallback(LayoutArrangeCallback::New(&MyLayout::OnArrange));
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
- **MATCH_PARENT**: constant (-2.0f) indicating the view fills the parent container's available space.
- **LayoutAlignment**: Fill, Start, Center, End (used by GridLayoutParams and StackLayoutParams for cross-axis alignment).
- **ViewVisibility**: Visible, Hidden, Collapsed.

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
| Public child API | Child add/remove uses Actor::Add/Remove. View provides Add(View, index) for index-based insertion and RemoveAllChildren() for bulk removal. GetChildCount, GetChildAt, IndexOfChild, Contents are available on View. |
| Layout processing | LayoutController collects layout roots per window and runs Measure then Arrange once per frame. |
| Implementation | ViewImpl holds children; LayoutImpl attaches a LayoutManager as a Trait in `OnInitialize()`. Applications can customize measure/arrange via `SetMeasureCallback()`/`SetArrangeCallback()`. |
