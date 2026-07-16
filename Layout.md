[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-(kr))

# Layout

dali-ui's layout system operates in two phases: **Measure** then **Arrange**.
When layout is invalidated, the LayoutController processes the layout root in both phases within the same frame.

For a detailed look at the layout calculation mechanism, see [Layout Structures](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-Structures).

<br/>

## Size Specification

Every View specifies its desired size via `SetRequestedWidth` / `SetRequestedHeight`.

| Value | Meaning |
|---|---|
| `WRAP_CONTENT` (`-1`) | Size to fit content or children (default) |
| `MATCH_PARENT` (`-2`) | Fill the parent container |
| Positive number (e.g. `100.0f`) | Fixed size |

<br/>

## Margin & Padding

Margin is the **outer spacing** around a View — it creates space between the View and its
neighboring Views or the edges of its parent container.
Padding is the **inner spacing** inside a View — it creates space between the View's
background boundary and its child content.

![layout-margin-padding](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/b0fa6f35-d006-4109-9261-0e3479e52808)

The `Extents` argument order is **(start, end, top, bottom)**. `start`/`end` are
layout-direction aware; under LTR they map to left/right respectively.

```cpp
// argument order: Extents(start, end, top, bottom)
view.SetViewMargin(Extents(4_spx, 8_spx, 12_spx, 16_spx));    // start 4, end 8, top 12, bottom 16
view.SetViewPadding(Extents(12_spx, 12_spx, 12_spx, 12_spx)); // 12spx on all sides
```

<br/>

## Visibility

```cpp
view.SetVisibility(true);   // visible
view.SetVisibility(false);  // hidden (layout space is not preserved)
```

To control transparency, use `SetOpacity(float)`.

<br/>

## StackLayout

Stacks children sequentially in a single direction.

![layout-stack](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/796f7d76-440e-4efe-9d49-8085b9cc4089)

**When to use**: Simple vertical or horizontal lists — button rows, menu items, form fields.

```cpp
StackLayout stack = StackLayout::New(StackOrientation::VERTICAL);
stack.SetSpacing(8_spx);
stack.SetRequestedWidth(MATCH_PARENT);
stack.SetRequestedHeight(WRAP_CONTENT);
stack.AddChildren({
  label,
  button,
  input,
});
```

**Per-child options** (`StackLayoutParams`):

| Option | Description |
|---|---|
| `SetWeight(float)` | Distributes remaining space proportionally. If weight > 0, RequestedSize on the main axis is ignored |
| `SetAlignment(LayoutAlignment)` | Cross-axis alignment: FILL / START / CENTER / END |

```cpp
// Distribute remaining space 2:1
viewA.SetLayoutParams(StackLayoutParams::New().SetWeight(2.0f));
viewB.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
```

Sample: [stacklayout](https://github.com/dalihub/dali-ui/tree/devel/samples/stacklayout)

<br/>

## AbsoluteLayout

Positions children at explicit coordinates.

![layout-absolute](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/7be0b741-8c9c-404b-940d-ffbfeea99f06)

**When to use**: Overlays, floating UI elements, or any layout requiring pixel-precise positioning.

```cpp
icon.SetLayoutParams(
  AbsoluteLayoutParams::New()
    .SetX(20_spx).SetY(20_spx)
    .SetWidth(48_spx).SetHeight(48_spx));

badge.SetLayoutParams(
  AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0.9f, 0.0f, 0.1f, 0.1f))
    .SetFlags(AbsoluteLayoutFlags::ALL));  // proportional

AbsoluteLayout absolute = AbsoluteLayout::New();
absolute.SetRequestedWidth(MATCH_PARENT);
absolute.SetRequestedHeight(MATCH_PARENT);
absolute.AddChildren({icon, badge});
```

**AbsoluteLayoutFlags**:

| Flag | Meaning |
|---|---|
| `NONE` | Absolute pixel values |
| `POSITION_PROPORTIONAL` | x, y interpreted as a ratio (0.0–1.0) relative to parent size |
| `SIZE_PROPORTIONAL` | width, height interpreted as a ratio relative to parent size |
| `ALL` | Both position and size are proportional |

Sample: [absolutelayout](https://github.com/dalihub/dali-ui/tree/devel/samples/absolutelayout)

<br/>

## FlexLayout

Uses the CSS Flexbox algorithm.

![layout-flex](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/f66017e6-2f92-4df9-b60b-124b217a8be4)

**When to use**: Complex one-dimensional layouts requiring direction control, wrapping, or fine-grained alignment.

```cpp
FlexLayout flex = FlexLayout::New();
flex.SetDirection(FlexDirection::ROW);
flex.SetWrap(FlexWrap::WRAP);
flex.SetJustifyContent(FlexJustify::SPACE_BETWEEN);
flex.SetAlignItems(FlexAlign::CENTER);
flex.AddChildren({itemA, itemB, itemC});
```

**Per-child options** (`FlexLayoutParams`):

| Option | Description |
|---|---|
| `SetFlexGrow(float)` | Grows to fill remaining space proportionally |
| `SetFlexShrink(float)` | Shrinks proportionally when space is insufficient |
| `SetFlexBasis(float)` | Initial size before grow/shrink (`WRAP_CONTENT` for auto) |
| `SetAlignSelf(FlexAlign)` | Per-child cross-axis alignment (overrides parent's AlignItems) |

Sample: [flexlayout](https://github.com/dalihub/dali-ui/tree/devel/samples/flexlayout)

<br/>

## GridLayout

Positions children in a grid of defined rows and columns.

![layout-grid](https://github.sec.samsung.net/NUI/dali-ui/assets/1000/cd3ef92e-d819-4760-90b0-6a398f4e84ae)

**When to use**: App grids, settings screens, dashboards — any two-dimensional arrangement.

```cpp
GridLayout grid = GridLayout::New();
grid.SetRowDefinitions({GridLength::Auto(), GridLength::Star(), GridLength::Absolute(100_spx)});
grid.SetColumnDefinitions({GridLength::Star(), GridLength::Star(2.0f)});
grid.SetRowSpacing(4_spx);
grid.SetColumnSpacing(4_spx);
grid.AddChildren({header, sideBar, main, footer});
```

**GridLength types**:

| Type | Constructor | Meaning |
|---|---|---|
| Absolute | `GridLength::Absolute(100_spx)` | Fixed pixel size |
| Star | `GridLength::Star(1.0f)` | Proportional share of remaining space |
| Auto | `GridLength::Auto()` | Sizes to child's natural size |

**Per-child options** (`GridLayoutParams`):

| Option | Description |
|---|---|
| `SetRow(uint32_t)` | Zero-based row index |
| `SetColumn(uint32_t)` | Zero-based column index |
| `SetRowSpan(uint32_t)` | Number of rows to span |
| `SetColumnSpan(uint32_t)` | Number of columns to span |
| `SetHorizontalAlignment(LayoutAlignment)` | Horizontal alignment within the cell |
| `SetVerticalAlignment(LayoutAlignment)` | Vertical alignment within the cell |

```cpp
// Copy base params to reuse across multiple children
auto base = GridLayoutParams::New().SetRowSpan(2).SetColumnSpan(2);
viewA.SetLayoutParams(GridLayoutParams::New(base).SetRow(0).SetColumn(0));
viewB.SetLayoutParams(GridLayoutParams::New(base).SetRow(0).SetColumn(2));
```

> **Caution**: `SetLayoutParams()` stores the handle as-is. Passing the same params handle to
> multiple Views causes them to share state. Always use `New(other)` to create an independent copy.

Sample: [gridlayout](https://github.com/dalihub/dali-ui/tree/devel/samples/gridlayout)

<br/>

## Custom Layouting by registering Callbacks

Inject custom layout logic into any `Layout` using `SetMeasureCallback` / `SetArrangeCallback`.

```cpp
// Custom layout that places children diagonally
struct DiagonalLayout
{
  static MeasuredSize OnMeasure(View self, float wConstraint, float hConstraint)
  {
    float totalW = 0, totalH = 0;
    for(uint32_t i = 0; i < self.GetChildViewCount(); ++i)
    {
      auto sz = self.GetChildViewAt(i).Measure(wConstraint - totalW, hConstraint - totalH);
      totalW += sz.width;
      totalH += sz.height;
    }
    return {totalW, totalH};
  }

  static LayoutRect OnArrange(View self, const LayoutRect& bounds)
  {
    float x = bounds.x, y = bounds.y;
    for(uint32_t i = 0; i < self.GetChildViewCount(); ++i)
    {
      auto sz = self.GetChildViewAt(i).GetMeasuredSize();
      self.GetChildViewAt(i).Arrange({x, y, sz.width, sz.height});
      x += sz.width;
      y += sz.height;
    }
    return bounds; // final self bounds; the framework applies x/y/width/height
  }
};

Layout root = Layout::New();
root.SetMeasureCallback(LayoutMeasureCallback::New(&DiagonalLayout::OnMeasure));
root.SetArrangeCallback(LayoutArrangeCallback::New(&DiagonalLayout::OnArrange));
```

Sample: [customlayout](https://github.com/dalihub/dali-ui/tree/devel/samples/customlayout)

<br/>

## Component Layouting by using LayoutManager

The predefined layout manager modules (`StackLayoutManager`, `GridLayoutManager`, etc.) available in
the public-api can be reused or subclassed to build a component on top of an existing algorithm.
A component attaches a manager via `View::AttachLayoutManager()`; the View then dispatches
Measure / Arrange to the manager automatically (a `MeasureCallback` / `ArrangeCallback`, if set,
takes priority over the manager).

```cpp
class MyButtonImpl : public Dali::Ui::ViewImpl
{
protected:
  void OnInitialize() override
  {
    ViewImpl::OnInitialize();
    // Reuse an existing manager; the View dispatches Measure/Arrange to it.
    AttachLayoutManager(Dali::MakeUnique<Dali::Ui::StackLayoutManager>(StackOrientation::VERTICAL, 0.0f));
  }
};
```

To implement a fully custom algorithm, subclass `Dali::Ui::LayoutManager` and override
`Measure()` / `Arrange()`, then attach it the same way
(see the [custom-layout-manager](https://github.com/dalihub/dali-ui/tree/devel/samples/custom-layout-manager) sample).

<br/>

## Layout Transition

`LayoutTransition` animates layout-driven changes for a view's children. It can run ENTER animations when children are added, EXIT animations when children are removed with `RemovePolicy::ANIMATE_EXIT`, and CHANGE animations when layout results move or resize existing children.

Use it when a layout reflow should feel continuous, such as inserting items into a stack, removing cards from a grid, or animating resize-driven layout changes.

For slot behavior, spec mode, animator callbacks, reflow scope, and caveats, see [Layout Transition](https://github.sec.samsung.net/NUI/dali-ui/wiki/Layout-Transition).

Sample: [layout-transition](https://github.com/dalihub/dali-ui/tree/devel/samples/layout-transition)

<br/>

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki#development-guides)
