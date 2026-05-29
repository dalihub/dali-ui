[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-Structures-(kr))

# DALi UI Foundation - Layout

`Layout` is a View that arranges child views using a layout algorithm. The layout system computes **size (Measure)** and **position (Arrange)** of child views within a View hierarchy. dali-ui provides four built-in layout types: `StackLayout`, `FlexLayout`, `GridLayout`, and `AbsoluteLayout`. Layout processing is driven by a `LayoutController` per window, which runs Measure then Arrange on invalidated layout roots each frame.

---

## Table of Contents

1. [Basic Setup](#1-basic-setup)
2. [StackLayout](#2-stacklayout)
   - [Orientation and Spacing](#21-orientation-and-spacing)
   - [Weight](#22-weight)
   - [Cross-Axis Alignment](#23-cross-axis-alignment)
3. [FlexLayout](#3-flexlayout)
   - [Direction and Wrap](#31-direction-and-wrap)
   - [Justify Content](#32-justify-content)
   - [Align Items and Align Content](#33-align-items-and-align-content)
   - [Per-Child Flex Parameters](#34-per-child-flex-parameters)
4. [GridLayout](#4-gridlayout)
   - [Row and Column Definitions](#41-row-and-column-definitions)
   - [GridLength Sizing](#42-gridlength-sizing)
   - [Grid Spacing](#43-grid-spacing)
   - [Per-Child Grid Parameters](#44-per-child-grid-parameters)
5. [AbsoluteLayout](#5-absolutelayout)
   - [Pixel Positioning](#51-pixel-positioning)
   - [Proportional Positioning](#52-proportional-positioning)
6. [Sizing Constants and Common Properties](#6-sizing-constants-and-common-properties)
   - [WRAP_CONTENT and MATCH_PARENT](#61-wrap_content-and-match_parent)
   - [Margin and Padding](#62-margin-and-padding)
   - [LayoutParams Sharing](#63-layoutparams-sharing)
7. [Custom Layout Callbacks](#7-custom-layout-callbacks)
8. [Layout Processing](#8-layout-processing)
   - [Two-Phase Layout](#81-two-phase-layout)
   - [Invalidation Flow](#82-invalidation-flow)
9. [Method Chaining](#9-method-chaining)
10. [Default Values](#10-default-values)
11. [Important Notes](#11-important-notes)

---

## 1. Basic Setup

Create a layout using the static factory method `New()` and add child views. Set the layout's requested size and add it to the window.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

// Create a vertical StackLayout filling the window
StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);

// Add children
View child1 = View::New();
child1.SetRequestedHeight(100.0f);
root.Add(child1);

View child2 = View::New();
child2.SetRequestedHeight(200.0f);
root.Add(child2);

// Add to the scene
window.Add(root);
```

> **Note:** All layout types inherit from `Layout`, which inherits from `View`. Child management uses `Add(View)`, `Insert(index, View)`, `Remove(View)`, and `RemoveAllChildren()`.

---

## 2. StackLayout

`StackLayout` arranges children in a single row or column with optional spacing. Children can have fixed sizes or share remaining space using weight.

### 2.1 Orientation and Spacing

```cpp
// Vertical stack (default)
StackLayout vStack = StackLayout::New(StackOrientation::VERTICAL);

// Horizontal stack
StackLayout hStack = StackLayout::New(StackOrientation::HORIZONTAL);

// Change orientation after creation
vStack.SetOrientation(StackOrientation::HORIZONTAL);
StackOrientation ori = vStack.GetOrientation();

// Set spacing between children (pixels)
vStack.SetSpacing(10.0f);
float spacing = vStack.GetSpacing();
```

| Value | Behavior |
|---|---|
| `StackOrientation::VERTICAL` | Children stacked top to bottom. Default. |
| `StackOrientation::HORIZONTAL` | Children stacked left to right. |

---

### 2.2 Weight

Children with `weight > 0` share the remaining main-axis space proportionally. Their main-axis `RequestedWidth`/`RequestedHeight` is ignored. Children with `weight == 0` are measured normally.

```cpp
// Top bar: fixed height (weight 0, measured normally)
View topBar = View::New();
topBar.SetBackgroundColor(Color::RED);
topBar.SetRequestedHeight(80.0f);
topBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
root.Add(topBar);

// Middle: weighted, takes remaining space
View middle = View::New();
middle.SetBackgroundColor(Color::GREEN);
middle.SetLayoutParams(
    StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
root.Add(middle);

// Bottom bar: fixed height
View bottomBar = View::New();
bottomBar.SetBackgroundColor(Color::BLUE);
bottomBar.SetRequestedHeight(60.0f);
bottomBar.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
root.Add(bottomBar);
```

> For a vertical stack, `RequestedHeight` is the main-axis and `RequestedWidth` is the cross-axis. For a horizontal stack, the axes are swapped.

---

### 2.3 Cross-Axis Alignment

`StackLayoutParams` controls how each child is aligned on the cross-axis.

```cpp
view.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));

StackLayoutParams params = view.GetLayoutParams<StackLayoutParams>();
LayoutAlignment align = params.GetAlignment();
```

| Value | Behavior |
|---|---|
| `LayoutAlignment::FILL` | Stretch to fill cross-axis. |
| `LayoutAlignment::START` | Align to start (left/top). Default. |
| `LayoutAlignment::CENTER` | Center on cross-axis. |
| `LayoutAlignment::END` | Align to end (right/bottom). |

---

## 3. FlexLayout

`FlexLayout` arranges children using the CSS Flexbox algorithm, supporting direction, wrapping, and flexible sizing.

```cpp
FlexLayout flex = FlexLayout::New();
flex.SetRequestedWidth(MATCH_PARENT);
flex.SetRequestedHeight(MATCH_PARENT);
```

### 3.1 Direction and Wrap

```cpp
// Set flex direction
flex.SetDirection(FlexDirection::ROW);
FlexDirection dir = flex.GetDirection();

// Set wrap behavior
flex.SetWrap(FlexWrap::WRAP);
FlexWrap wrap = flex.GetWrap();
```

| `FlexDirection` | Behavior |
|---|---|
| `ROW` | Left to right. Default. |
| `ROW_REVERSE` | Right to left. |
| `COLUMN` | Top to bottom. |
| `COLUMN_REVERSE` | Bottom to top. |

| `FlexWrap` | Behavior |
|---|---|
| `NO_WRAP` | Single line, no wrapping. Default. |
| `WRAP` | Wrap to next line when space is exhausted. |
| `WRAP_REVERSE` | Wrap in reverse direction. |

---

### 3.2 Justify Content

Controls alignment of children along the main axis.

```cpp
flex.SetJustifyContent(FlexJustify::CENTER);
FlexJustify justify = flex.GetJustifyContent();
```

| Value | Behavior |
|---|---|
| `FlexJustify::FLEX_START` | Pack children to the start. Default. |
| `FlexJustify::FLEX_END` | Pack children to the end. |
| `FlexJustify::CENTER` | Center children. |
| `FlexJustify::SPACE_BETWEEN` | Equal spacing between children, no edge gaps. |
| `FlexJustify::SPACE_AROUND` | Equal spacing around each child. |
| `FlexJustify::SPACE_EVENLY` | Equal spacing between children and edges. |

---

### 3.3 Align Items and Align Content

```cpp
// Cross-axis alignment for a single line
flex.SetAlignItems(FlexAlign::STRETCH);
FlexAlign items = flex.GetAlignItems();

// Cross-axis alignment for multiple wrapped lines
flex.SetAlignContent(FlexAlign::CENTER);
FlexAlign content = flex.GetAlignContent();
```

| `FlexAlign` | Behavior |
|---|---|
| `AUTO` | Defer to parent setting. |
| `FLEX_START` | Align to start. |
| `FLEX_END` | Align to end. |
| `CENTER` | Center. |
| `STRETCH` | Stretch to fill. Default for AlignItems. |
| `BASELINE` | Align to baseline. |

---

### 3.4 Per-Child Flex Parameters

```cpp
view.SetLayoutParams(FlexLayoutParams::New()
    .SetFlexGrow(1.0f)
    .SetFlexShrink(0.0f)
    .SetFlexBasis(100.0f)
    .SetAlignSelf(FlexAlign::CENTER));

FlexLayoutParams params = view.GetLayoutParams<FlexLayoutParams>();
```

| Method | Description | Default |
|---|---|---|
| `SetFlexGrow(float)` | Factor for distributing remaining space | `0` |
| `SetFlexShrink(float)` | Factor for shrinking when space is insufficient | `1` |
| `SetFlexBasis(float)` | Initial main size before grow/shrink (`WRAP_CONTENT` for auto) | `WRAP_CONTENT` |
| `SetAlignSelf(FlexAlign)` | Cross-axis alignment override (`AUTO` defers to parent) | `AUTO` |

### Example

```cpp
FlexLayout root = FlexLayout::New();
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);
root.SetDirection(FlexDirection::ROW);
root.SetAlignItems(FlexAlign::STRETCH);
root.SetViewPadding(Extents(20, 20, 20, 20));

// Fixed-width child
View redBox = View::New();
redBox.SetBackgroundColor(Color::RED);
redBox.SetRequestedWidth(100.0f);
root.Add(redBox);

// Flexible child (takes remaining space)
View greenBox = View::New();
greenBox.SetBackgroundColor(Color::GREEN);
greenBox.SetRequestedWidth(WRAP_CONTENT);
greenBox.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f));
root.Add(greenBox);

// Fixed-width child
View blueBox = View::New();
blueBox.SetBackgroundColor(Color::BLUE);
blueBox.SetRequestedWidth(80.0f);
root.Add(blueBox);

window.Add(root);
```

---

## 4. GridLayout

`GridLayout` arranges children in a grid with configurable rows and columns. Each child is placed in a specific cell using `GridLayoutParams`.

```cpp
GridLayout grid = GridLayout::New();
grid.SetRequestedWidth(MATCH_PARENT);
grid.SetRequestedHeight(MATCH_PARENT);
```

### 4.1 Row and Column Definitions

```cpp
// Add rows and columns one at a time
grid.AddRowDefinition(GridLength::Absolute(100.0f));
grid.AddRowDefinition(GridLength::Star(1.0f));
grid.AddColumnDefinition(GridLength::Absolute(200.0f));
grid.AddColumnDefinition(GridLength::Star(2.0f));

// Or set all at once
grid.SetRowDefinitions({GridLength::Absolute(100.0f), GridLength::Star(1.0f)});
grid.SetColumnDefinitions({GridLength::Absolute(200.0f), GridLength::Star(2.0f)});

// Query
uint32_t rows = grid.GetRowCount();
uint32_t cols = grid.GetColumnCount();
std::vector<GridLength> rowDefs = grid.GetRowDefinitions();
std::vector<GridLength> colDefs = grid.GetColumnDefinitions();

// Clear definitions
grid.ClearRowDefinitions();
grid.ClearColumnDefinitions();
```

---

### 4.2 GridLength Sizing

`GridLength` specifies how row heights and column widths are calculated.

| Factory | Behavior |
|---|---|
| `GridLength::Absolute(float pixels)` | Fixed pixel size. |
| `GridLength::Star(float factor)` | Proportional share of remaining space. Default factor is `1.0`. |
| `GridLength::Auto()` | Size to fit the largest child in that row/column. |

```cpp
// Example: header (fixed), content (flexible), footer (auto)
grid.AddRowDefinition(GridLength::Absolute(60.0f));
grid.AddRowDefinition(GridLength::Star(1.0f));
grid.AddRowDefinition(GridLength::Auto());

// Two columns: 1/3 and 2/3 of available width
grid.AddColumnDefinition(GridLength::Star(1.0f));
grid.AddColumnDefinition(GridLength::Star(2.0f));
```

---

### 4.3 Grid Spacing

```cpp
grid.SetRowSpacing(10.0f);
grid.SetColumnSpacing(10.0f);

float rowSpacing = grid.GetRowSpacing();
float colSpacing = grid.GetColumnSpacing();
```

---

### 4.4 Per-Child Grid Parameters

```cpp
view.SetLayoutParams(GridLayoutParams::New()
    .SetRow(1)
    .SetColumn(2)
    .SetRowSpan(1)
    .SetColumnSpan(3)
    .SetHorizontalAlignment(LayoutAlignment::CENTER)
    .SetVerticalAlignment(LayoutAlignment::FILL));

GridLayoutParams params = view.GetLayoutParams<GridLayoutParams>();
```

| Method | Description | Default |
|---|---|---|
| `SetRow(uint32_t)` | Zero-based row index | `0` |
| `SetColumn(uint32_t)` | Zero-based column index | `0` |
| `SetRowSpan(uint32_t)` | Number of rows to span | `1` |
| `SetColumnSpan(uint32_t)` | Number of columns to span | `1` |
| `SetHorizontalAlignment(LayoutAlignment)` | Horizontal alignment within cell | `FILL` |
| `SetVerticalAlignment(LayoutAlignment)` | Vertical alignment within cell | `FILL` |

### Example

```cpp
GridLayout root = GridLayout::New();
root.SetRequestedWidth(MATCH_PARENT);
root.SetRequestedHeight(MATCH_PARENT);
root.SetViewPadding(Extents(20, 20, 20, 20));

root.AddRowDefinition(GridLength::Absolute(100.0f));
root.AddRowDefinition(GridLength::Absolute(120.0f));
root.AddRowDefinition(GridLength::Absolute(80.0f));
root.AddColumnDefinition(GridLength::Absolute(200.0f));
root.AddColumnDefinition(GridLength::Absolute(160.0f));

root.SetRowSpacing(10.0f);
root.SetColumnSpacing(10.0f);

// Cell (0,0)
View cell00 = View::New();
cell00.SetBackgroundColor(Color::RED);
cell00.SetLayoutParams(GridLayoutParams::New());
root.Add(cell00);

// Cell (0,1)
View cell01 = View::New();
cell01.SetBackgroundColor(Color::GREEN);
cell01.SetLayoutParams(GridLayoutParams::New().SetColumn(1));
root.Add(cell01);

// Cell (1,0)
View cell10 = View::New();
cell10.SetBackgroundColor(Color::BLUE);
cell10.SetLayoutParams(GridLayoutParams::New().SetRow(1));
root.Add(cell10);

window.Add(root);
```

---

## 5. AbsoluteLayout

`AbsoluteLayout` positions children at explicit coordinates with explicit sizes. Children do not interact with each other.

```cpp
AbsoluteLayout absolute = AbsoluteLayout::New();
absolute.SetRequestedWidth(MATCH_PARENT);
absolute.SetRequestedHeight(MATCH_PARENT);
```

### 5.1 Pixel Positioning

```cpp
View redBox = View::New();
redBox.SetBackgroundColor(Color::RED);
redBox.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(20.0f, 20.0f, 120.0f, 80.0f)));
absolute.Add(redBox);

// Individual setters are also available
View blueBox = View::New();
blueBox.SetBackgroundColor(Color::BLUE);
blueBox.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetX(280.0f).SetY(400.0f).SetWidth(100.0f).SetHeight(60.0f));
absolute.Add(blueBox);
```

| Method | Description | Default |
|---|---|---|
| `SetBounds(LayoutRect)` | Set position and size (x, y, width, height) | `(0, 0, 0, 0)` |
| `SetX(float)` / `SetY(float)` | Set position individually | `0` |
| `SetWidth(float)` / `SetHeight(float)` | Set size individually (`-1` uses View's own size) | `0` |
| `SetFlags(AbsoluteLayoutFlags)` | Proportional positioning/sizing flags | `NONE` |

---

### 5.2 Proportional Positioning

Use `AbsoluteLayoutFlags` to interpret bounds values as proportional (0.0 to 1.0) relative to the parent.

```cpp
// Centered box: 50% width, 30% height, at 25% from left, 35% from top
View greenBox = View::New();
greenBox.SetBackgroundColor(Color::GREEN);
greenBox.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0.25f, 0.35f, 0.5f, 0.3f))
    .SetFlags(AbsoluteLayoutFlags::ALL));
absolute.Add(greenBox);
```

| Value | Behavior |
|---|---|
| `AbsoluteLayoutFlags::NONE` | Pixel values for both position and size. Default. |
| `AbsoluteLayoutFlags::POSITION_PROPORTIONAL` | Position is proportional to parent size. |
| `AbsoluteLayoutFlags::SIZE_PROPORTIONAL` | Size is proportional to parent size. |
| `AbsoluteLayoutFlags::ALL` | Both position and size are proportional. |

---

## 6. Sizing Constants and Common Properties

### 6.1 WRAP_CONTENT and MATCH_PARENT

All Views use `SetRequestedWidth()` / `SetRequestedHeight()` to specify their desired size. Two special constants are available:

```cpp
// Size to fit content (default)
view.SetRequestedWidth(WRAP_CONTENT);   // -1.0f

// Fill parent container
view.SetRequestedHeight(MATCH_PARENT);  // -2.0f

// Fixed pixel size
view.SetRequestedWidth(200.0f);
```

| Constant | Value | Behavior |
|---|---|---|
| `WRAP_CONTENT` | `-1.0f` | View sizes to fit its content (natural size or children bounding box). Default. |
| `MATCH_PARENT` | `-2.0f` | View fills the parent container's available space. |

---

### 6.2 Margin and Padding

```cpp
// Margin: external spacing around the view
view.SetViewMargin(Extents(10, 10, 5, 5));  // start, end, top, bottom
Extents margin = view.GetViewMargin();

// Padding: internal spacing inside the view
view.SetViewPadding(Extents(20, 20, 20, 20));
Extents padding = view.GetViewPadding();
```

---

### 6.3 LayoutParams Sharing

`SetLayoutParams()` stores the handle **as-is** without deep copy. Passing the same handle to multiple Views causes them to share state. Use `New(other)` to create independent copies.

```cpp
auto base = GridLayoutParams::New().SetRowSpan(2).SetColumnSpan(2);

// Independent copies — changing one does not affect the other
viewA.SetLayoutParams(GridLayoutParams::New(base).SetColumn(0));
viewB.SetLayoutParams(GridLayoutParams::New(base).SetColumn(1));
```

> **Important:** Without `New(other)`, two views sharing the same params handle will reflect each other's changes.

---

## 7. Custom Layout Callbacks

For custom measure/arrange logic without subclassing, use `SetMeasureCallback()` and `SetArrangeCallback()` on any View or Layout. When set, callbacks take **priority** over the default LayoutManager.

```cpp
// Measure callback signature: MeasuredSize(View, float widthConstraint, float heightConstraint)
MeasuredSize MyMeasure(View view, float widthConstraint, float heightConstraint)
{
    return MeasuredSize(200.0f, 100.0f);
}

// Arrange callback signature: MeasuredSize(View, const LayoutRect& bounds)
MeasuredSize MyArrange(View view, const LayoutRect& bounds)
{
    return MeasuredSize(bounds.GetWidth(), bounds.GetHeight());
}

Layout layout = Layout::New();
layout.SetMeasureCallback(LayoutMeasureCallback::New(&MyMeasure));
layout.SetArrangeCallback(LayoutArrangeCallback::New(&MyArrange));
```

For member functions, pass the object instance:

```cpp
layout.SetMeasureCallback(LayoutMeasureCallback::New(this, &MyClass::OnMeasure));
layout.SetArrangeCallback(LayoutArrangeCallback::New(this, &MyClass::OnArrange));
```

---

## 8. Layout Processing

### 8.1 Two-Phase Layout

Each frame, the `LayoutController` runs two passes on invalidated layout roots:

1. **Measure** — Given width/height constraints from the parent, each View computes its desired `MeasuredSize`. Results are cached; if constraints are unchanged, measurement is skipped.
2. **Arrange** — Each View is given a `LayoutRect` (position + size). It applies alignment and margin, then its LayoutManager places children within the bounds.

---

### 8.2 Invalidation Flow

When a layout property changes (size, child, params), invalidation propagates up to the layout root and registers with `LayoutController` for processing on the next frame.

```
View property change (size, child, params)
        │
        ▼
  InvalidateMeasure() / InvalidateArrange()
        │
        ▼  (propagate upward)
  Layout root reached
        │
        ▼
  Register with LayoutController
        │
        ▼  (next frame)
  ProcessLayouts()
        │
        ├── Measure (top-down)
        │
        └── Arrange (top-down)
```

A **layout root** is a top-level View whose parent is not a layout. The `LayoutController` is obtained per window:

```cpp
LayoutController controller = LayoutController::Get(window);
```

> No explicit calls to `LayoutController` are needed in normal use. Invalidation triggers automatic re-layout.

---

## 9. Method Chaining

All layout setters support fluent configuration via chaining methods:

```cpp
StackLayout stack = StackLayout::New()
    .Orientation(StackOrientation::VERTICAL)
    .Spacing(10.0f)
    .RequestedWidth(MATCH_PARENT)
    .RequestedHeight(MATCH_PARENT)
    .ViewPadding(Extents(20, 20, 20, 20));

FlexLayout flex = FlexLayout::New()
    .Direction(FlexDirection::ROW)
    .Wrap(FlexWrap::WRAP)
    .JustifyContent(FlexJustify::SPACE_BETWEEN)
    .AlignItems(FlexAlign::CENTER);

GridLayout grid = GridLayout::New()
    .Rows({GridLength::Absolute(60.0f), GridLength::Star(1.0f)})
    .Columns({GridLength::Star(1.0f), GridLength::Star(2.0f)})
    .RowSpacing(10.0f)
    .ColumnSpacing(10.0f);
```

LayoutParams setters also return `*this` for chaining:

```cpp
view.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
view.SetLayoutParams(GridLayoutParams::New().SetRow(1).SetColumn(2).SetColumnSpan(3));
view.SetLayoutParams(FlexLayoutParams::New().SetFlexGrow(1.0f).SetFlexShrink(0.0f));
```

---

## 10. Default Values

### StackLayout

| Property | Default |
|---|---|
| `Orientation` | `VERTICAL` |
| `Spacing` | `0` |

### StackLayoutParams

| Property | Default |
|---|---|
| `Weight` | `0` |
| `Alignment` | `START` |

### FlexLayout

| Property | Default |
|---|---|
| `Direction` | `ROW` |
| `Wrap` | `NO_WRAP` |
| `JustifyContent` | `FLEX_START` |
| `AlignItems` | `STRETCH` |
| `AlignContent` | `STRETCH` |

### FlexLayoutParams

| Property | Default |
|---|---|
| `FlexGrow` | `0` |
| `FlexShrink` | `1` |
| `FlexBasis` | `WRAP_CONTENT` |
| `AlignSelf` | `AUTO` |

### GridLayoutParams

| Property | Default |
|---|---|
| `Row` | `0` |
| `Column` | `0` |
| `RowSpan` | `1` |
| `ColumnSpan` | `1` |
| `HorizontalAlignment` | `FILL` |
| `VerticalAlignment` | `FILL` |

### AbsoluteLayoutParams

| Property | Default |
|---|---|
| `Bounds` | `(0, 0, 0, 0)` |
| `Flags` | `NONE` |

---

## 11. Important Notes

- **Layout inherits View.** All View methods (`SetRequestedWidth`, `SetViewMargin`, `SetViewPadding`, `Add`, `Remove`, etc.) are available on every layout type.

- **One LayoutManager per View.** Each layout subclass (StackLayout, FlexLayout, etc.) attaches its LayoutManager during initialization. Custom layouts can subclass `LayoutManager` and attach it to any View via `View::AttachLayoutManager()`.

- **LayoutParams are stored as-is.** `SetLayoutParams()` does not deep-copy the handle. Use `New(other)` (e.g., `GridLayoutParams::New(base)`) to create independent copies when reusing params across multiple Views.

- **Callbacks override the LayoutManager.** When `SetMeasureCallback()` or `SetArrangeCallback()` is set on a View, the callbacks take priority over the default LayoutManager for that View.

- **Layout roots drive processing.** A layout root is a View whose parent is not a layout. Invalidation propagates upward until it reaches a layout root, which then registers with the `LayoutController`. The controller processes all pending roots once per frame.

- **Measure results are cached.** If the width and height constraints passed to `OnMeasure()` are unchanged since the last call, measurement is skipped. Call `InvalidateMeasure()` to force recomputation.

- **`GetSize()` reflects the arranged size.** `View::GetSize()` returns the actual rendered size after the Arrange phase, not the requested size.