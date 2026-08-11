[→ 한국어 문서](https://github.sec.samsung.net/NUI/dali-ui/wiki/CanvasView-(kr))

# DALi UI Foundation - CanvasView

`CanvasView` is a `View` for rendering ThorVG-backed vector drawables. Use it when a UI needs scalable shapes, paths, gradients, SVG-like pictures, or canvas-level effects while still participating in the dali-ui layout system.

---

## Basic Setup

Create a `CanvasView`, add `CanvasRenderer::Drawable` objects, then place the view in the scene or in a layout.

```cpp
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/canvas/canvas-view.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>

using namespace Dali;
using namespace Dali::Ui;

CanvasView canvasView = CanvasView::New(Vector2(200.0f, 200.0f));
canvasView.SetRequestedWidth(200.0f);
canvasView.SetRequestedHeight(200.0f);

CanvasRenderer::Shape shape = CanvasRenderer::Shape::New();
shape.AddRect(Bounds(0.0f, 0.0f, 160.0f, 80.0f), Vector2(12.0f, 12.0f));
shape.SetFillColor(Vector4(0.1f, 0.45f, 0.95f, 1.0f));

canvasView.AddDrawable(shape);
window.Add(canvasView);
```

---

## View Box

The view box defines the logical coordinate space used by drawables. The actual pixel size is controlled by layout.

```cpp
CanvasView icon = CanvasView::New(Vector2(24.0f, 24.0f));
icon.SetRequestedWidth(48.0f);
icon.SetRequestedHeight(48.0f);
```

In this example, drawables use a 24 x 24 coordinate system and are rendered into a 48 x 48 layout size.

---

## Rasterization Control

`CanvasView` rasterizes asynchronously by default. For deterministic updates or animation-driven changes, request rasterization manually.

```cpp
canvasView.SetRasterizationRequestManually(true);

// After modifying drawables:
canvasView.RequestRasterization();
```

Use `SetSynchronousLoading(true)` when content must be rasterized immediately on the main thread, for example in tests or small deterministic assets.

---

## Canvas Effects

Canvas-level effects apply to the whole rendered content.

```cpp
canvasView.SetDropShadow(Vector4(0.0f, 0.0f, 0.0f, 0.35f), 0.0f, 4.0f, 12.0f);

canvasView.SetGaussianBlur(8.0f);
```

Drop shadow and Gaussian blur are mutually exclusive scene effects. Setting one replaces the other.

---

## Key APIs

| API | Purpose |
|---|---|
| `CanvasView::New()` | Creates a canvas with a zero view box |
| `CanvasView::New(Vector2)` | Creates a canvas with the given logical view box |
| `AddDrawable()` / `RemoveDrawable()` | Manages ThorVG drawable objects |
| `RemoveAllDrawables()` | Clears the canvas |
| `SetViewBox()` / `GetViewBox()` | Controls the logical coordinate space |
| `SetSynchronousLoading()` | Chooses synchronous or background rasterization |
| `SetRasterizationRequestManually()` | Enables explicit rasterization requests |
| `RequestRasterization()` | Requests a new rasterization pass |
| `SetDropShadow()` / `SetGaussianBlur()` | Applies a canvas-level scene effect |

---

## See Also

- [API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1CanvasView.html)
- [samples/canvas-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/canvas-view)
- [ChartView](ChartView.md), which uses `CanvasView` layers for chart rendering

---

[← Back to Components](Components.md)
