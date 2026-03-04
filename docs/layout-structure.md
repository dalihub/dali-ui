# DALi UI Foundation - Layout Class and Behavior Structure

## Overview

The layout system in DALi UI Foundation computes **size (Measure)** and **position (Arrange)** of child views over a **View** hierarchy. In the public API, only **Layout** and its derived classes (StackLayout, FlexLayout, GridLayout, AbsoluteLayout) expose child management APIs (AddView, GetChildAt, etc.); **View** exposes layout-related properties (size specs, alignment, visibility) but not add/remove child.

Layout processing is driven by **LayoutController** per window. Each frame, it runs Measure then Arrange on layout roots that have been invalidated.

---

## Class Structure

### 1. Public API (Handles)

- **View**  
  - Layout properties: `SetLayoutWidth` / `SetLayoutHeight`, `SetViewMargin` / `SetViewPadding`, alignment, visibility, etc.  
  - Measure/Arrange are invoked internally by the layout system; applications may request recomputation via `InvalidateMeasure()` / `InvalidateArrange()`.  
  - No child add/remove API.

- **Layout** (inherits View)  
  - Child management: `AddView(View)`, `AddView(View, index)`, `RemoveView(View)`, `RemoveViewAt(index)`, `RemoveAllViews()`, `GetChildCount()`, `GetChildAt(index)`, `IndexOfChild(View)`, `Contents(initializer_list<View>)`.  
  - Always has a LayoutManager; derived classes attach Stack/Flex/Grid/Absolute algorithms.

- **StackLayout, FlexLayout, GridLayout, AbsoluteLayout**
  - Each adds type-specific options: orientation/spacing/weight, direction/wrap/justify/align, row/column definitions, absolute bounds, etc.
  - Per-child layout parameters are set via `View::SetLayoutParams()`:
    - `view.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))`
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
  - Holds the actual Measure/Arrange logic, size specifications, margin/padding/alignment/visibility, and **optional** LayoutManager and child container.  
  - When a LayoutManager is set: provides `SetLayoutManager`, `AddView`, `RemoveView`, `RemoveAllViews`, `GetChildCount`, `GetChildAt`, `IndexOfChild`, `Contents`, etc.
  - `GetParentLayout()`, `IsLayout()`, and invalidation propagate to the parent until a layout root is reached, which registers with the LayoutController.

- **LayoutImpl** (inherits ViewImpl)  
  - On construction, creates a derived LayoutManager via `CreateLayoutManager()` and sets it with `SetLayoutManager`.  
  - Child APIs are inherited from ViewImpl.

- **StackLayoutImpl, FlexLayoutImpl, GridLayoutImpl, AbsoluteLayoutImpl**
  - Each overrides `CreateLayoutManager()` to return the corresponding Stack, Flex, Grid, or Absolute LayoutManager.

- **AbsoluteLayoutParamsImpl, FlexLayoutParamsImpl, GridLayoutParamsImpl, StackLayoutParamsImpl**
  - TraitImpl-derived classes that store per-child layout parameters (e.g., bounds/flags, grow/shrink/basis/alignSelf, row/column/span, weight).
  - Attached to child views via `ReservedTraitId` and accessed by layout managers during Measure/Arrange.
  - Each provides `Get(ViewImpl&)` (returns nullptr if not attached) and `GetOrCreate(ViewImpl&)` (creates if missing).

- **LayoutControllerImpl**  
  - Keeps layout roots in `mPendingViews`; `ProcessLayouts()` resolves constraints, then runs Measure and Arrange for each root.

### 3. Layout Managers (Algorithms)

- **LayoutManager** (abstract)  
  - `Measure(ViewImpl*, widthConstraint, heightConstraint)`: compute desired size for the container and its children.  
  - `ArrangeChildren(ViewImpl*, bounds)`: place children within the given bounds.  
  - Uses internal helpers such as `MeasureChild`, `ArrangeChild`, and `GetChildren(ViewImpl*)`.

- **StackLayoutManager, FlexLayoutManager, GridLayoutManager, AbsoluteLayoutManager**  
  - Concrete implementations that measure and arrange children according to stack, flex, grid, or absolute rules.
  - Each is defined in **integration-api** as a separate header and source pair: `stack-layout-manager.h`/`.cpp`, `grid-layout-manager.h`/`.cpp`, `flex-layout-manager.h`/`.cpp`, `absolute-layout-manager.h`/`.cpp`.
  - Each reads per-child parameters from the corresponding `*ParamsImpl` trait attached to child views (e.g., `AbsoluteLayoutManager` reads `AbsoluteLayoutParamsImpl`).
  - Custom layouts can override `CreateLayoutManager()` to return a subclass of one of these managers.

### 4. Layout Types (layout-types)

- **MeasuredSize**: measured width and height.
- **LayoutRect**: x, y, width, height (placement region).
- **LayoutDimension**: constants such as `WrapContent` (-1.0f) and `MatchParent` (-2.0f).
- **LayoutAlignment**: Fill, Start, Center, End.
- **ViewVisibility**: Visible, Hidden, Collapsed.

---

## Behavior

### Layout root

A **layout root** is a top-level View in the layout hierarchy (its parent is not a layout). When `InvalidateMeasure()` or `InvalidateArrange()` is called, it propagates up to the layout root, which then registers with the LayoutController via `RequestLayout(ViewImpl*)` so that the root is processed on the next frame.

### Two-phase layout: Measure and Arrange

1. **Measure**  
   - Given width/height constraints from the parent, the View (and its LayoutManager, if any) computes desired size for itself and its children.  
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
| Public child API | Only Layout (and Stack/Flex/Grid/Absolute) expose AddView, RemoveView, GetChildCount, GetChildAt, IndexOfChild, Contents, etc. View does not. |
| Layout processing | LayoutController collects layout roots per window and runs Measure then Arrange once per frame. |
| Implementation | ViewImpl optionally holds a LayoutManager and children; LayoutImpl creates a type-specific LayoutManager so layouts always have layout capability. |
