[→ English](https://github.sec.samsung.net/NUI/dali-ui/wiki/CanvasView)

# DALi UI Foundation - CanvasView

`CanvasView`는 ThorVG 기반 vector drawable을 렌더링하는 `View`입니다. 확장 가능한 shape, path, gradient, SVG와 유사한 picture, canvas 단위 효과가 필요하면서 dali-ui 레이아웃 시스템에도 참여해야 할 때 사용합니다.

---

## 기본 설정

`CanvasView`를 생성하고 `CanvasRenderer::Drawable` 객체를 추가한 뒤 scene 또는 layout에 배치합니다.

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

View box는 drawable이 사용하는 논리 좌표계를 정의합니다. 실제 픽셀 크기는 레이아웃이 결정합니다.

```cpp
CanvasView icon = CanvasView::New(Vector2(24.0f, 24.0f));
icon.SetRequestedWidth(48.0f);
icon.SetRequestedHeight(48.0f);
```

이 예제에서는 drawable이 24 x 24 좌표계를 사용하고, 결과는 48 x 48 레이아웃 크기로 렌더링됩니다.

---

## Rasterization 제어

`CanvasView`는 기본적으로 비동기 rasterization을 사용합니다. 결정적인 갱신이나 애니메이션에 따른 변경이 필요하면 rasterization을 직접 요청합니다.

```cpp
canvasView.SetRasterizationRequestManually(true);

// Drawable 변경 후:
canvasView.RequestRasterization();
```

테스트나 작은 고정 asset처럼 즉시 rasterization이 필요한 경우에는 `SetSynchronousLoading(true)`를 사용할 수 있습니다.

---

## Canvas 효과

Canvas 단위 효과는 렌더링된 전체 콘텐츠에 적용됩니다.

```cpp
canvasView.SetDropShadow(Vector4(0.0f, 0.0f, 0.0f, 0.35f), 0.0f, 4.0f, 12.0f);

canvasView.SetGaussianBlur(8.0f);
```

Drop shadow와 Gaussian blur는 동시에 적용되지 않습니다. 하나를 설정하면 다른 효과가 대체됩니다.

---

## 주요 API

| API | 목적 |
|---|---|
| `CanvasView::New()` | zero view box를 가진 canvas 생성 |
| `CanvasView::New(Vector2)` | 지정한 논리 view box를 가진 canvas 생성 |
| `AddDrawable()` / `RemoveDrawable()` | ThorVG drawable 객체 관리 |
| `RemoveAllDrawables()` | canvas 비우기 |
| `SetViewBox()` / `GetViewBox()` | 논리 좌표계 제어 |
| `SetSynchronousLoading()` | 동기 또는 background rasterization 선택 |
| `SetRasterizationRequestManually()` | 명시적 rasterization 요청 모드 활성화 |
| `RequestRasterization()` | 새 rasterization pass 요청 |
| `SetDropShadow()` / `SetGaussianBlur()` | canvas 단위 scene effect 적용 |

---

## 함께 보기

- [API Reference](https://pages.github.sec.samsung.net/NUI/dali-ui/daliUi/classDali_1_1Ui_1_1CanvasView.html)
- [samples/canvas-view](https://github.sec.samsung.net/NUI/dali-ui/tree/devel/samples/canvas-view)
- `CanvasView` 레이어를 사용해 차트를 렌더링하는 [ChartView](ChartView-(kr).md)

---

[← Back to list](https://github.sec.samsung.net/NUI/dali-ui/wiki/Home-(kr)#development-guides)
