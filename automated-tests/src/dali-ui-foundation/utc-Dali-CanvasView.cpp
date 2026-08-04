/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/canvas/canvas-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-linear-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-radial-gradient.h>
#include <dali/public-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_canvasview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_canvasview_cleanup(void)
{
  test_return_value = TET_PASS;
}

// =============================================================================
// Construction
// =============================================================================

int UtcDaliCanvasViewConstructorP(void)
{
  UiTestApplication application;
  CanvasView        view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliCanvasViewNewP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliCanvasViewNewWithoutViewboxP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliCanvasViewCopyConstructorP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  CanvasView        copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliCanvasViewMoveConstructorP(void)
{
  UiTestApplication application;
  CanvasView        view  = CanvasView::New(Vector2(100, 100));
  CanvasView        moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliCanvasViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  CanvasView        copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliCanvasViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  CanvasView        moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// =============================================================================
// DownCast
// =============================================================================

int UtcDaliCanvasViewDownCastP(void)
{
  UiTestApplication application;
  CanvasView        view   = CanvasView::New(Vector2(100, 100));
  View              base   = view;
  CanvasView        result = CanvasView::DownCast(base);
  DALI_TEST_CHECK(result);
  END_TEST;
}

int UtcDaliCanvasViewDownCastN(void)
{
  UiTestApplication application;
  View              base   = View::New();
  CanvasView        result = CanvasView::DownCast(base);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

// =============================================================================
// AddDrawable
// =============================================================================

int UtcDaliCanvasViewAddDrawableP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  shape.AddRect(Bounds(10, 10, 50, 50), Vector2::ZERO);

  bool result = view.AddDrawable(shape);
  DALI_TEST_CHECK(result);
  END_TEST;
}

int UtcDaliCanvasViewAddDrawableN(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  // Adding an empty (uninitialized) drawable handle is a programming error and asserts.
  Dali::CanvasRenderer::Shape emptyShape;
  DALI_TEST_ASSERTION(view.AddDrawable(emptyShape), "Drawable handle is empty.");
  END_TEST;
}

// =============================================================================
// RemoveDrawable
// =============================================================================

int UtcDaliCanvasViewRemoveDrawableP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  shape.AddRect(Bounds(10, 10, 50, 50), Vector2::ZERO);

  view.AddDrawable(shape);
  DALI_TEST_CHECK(view.RemoveDrawable(shape));
  END_TEST;
}

int UtcDaliCanvasViewRemoveDrawableN(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  // shape was never added — expect false
  DALI_TEST_CHECK(!view.RemoveDrawable(shape));
  END_TEST;
}

// =============================================================================
// RemoveAllDrawables
// =============================================================================

int UtcDaliCanvasViewRemoveAllDrawablesP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  shape.AddRect(Bounds(10, 10, 50, 50), Vector2::ZERO);
  view.AddDrawable(shape);

  bool result = view.RemoveAllDrawables();
  DALI_TEST_CHECK(result);
  END_TEST;
}

int UtcDaliCanvasViewRemoveAllDrawablesN(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  // Nothing added — still should not crash, result may be false
  view.RemoveAllDrawables();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// =============================================================================
// ViewBox property
// =============================================================================

int UtcDaliCanvasViewSetGetViewBoxP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  view.SetViewBox(Vector2(200, 200));
  Vector2 viewBox = view.GetViewBox();
  DALI_TEST_EQUALS(viewBox, Vector2(200, 200), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewPropertyViewBoxP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  view.SetProperty(CanvasView::Property::VIEW_BOX, Vector2(300, 300));
  Vector2 viewBox = view.GetProperty(CanvasView::Property::VIEW_BOX).Get<Vector2>();
  DALI_TEST_EQUALS(viewBox, Vector2(300, 300), TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// SynchronousLoading property
// =============================================================================

int UtcDaliCanvasViewSetGetSynchronousLoadingP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  // Default should be true
  DALI_TEST_EQUALS(view.IsSynchronousLoading(), true, TEST_LOCATION);

  view.SetSynchronousLoading(false);
  DALI_TEST_EQUALS(view.IsSynchronousLoading(), false, TEST_LOCATION);

  view.SetSynchronousLoading(true);
  DALI_TEST_EQUALS(view.IsSynchronousLoading(), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewPropertySynchronousLoadingP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  view.SetProperty(CanvasView::Property::SYNCHRONOUS_LOADING, false);
  bool sync = view.GetProperty(CanvasView::Property::SYNCHRONOUS_LOADING).Get<bool>();
  DALI_TEST_EQUALS(sync, false, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// RasterizationRequestManually property
// =============================================================================

int UtcDaliCanvasViewSetGetRasterizationRequestManuallyP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  // Default should be false
  DALI_TEST_EQUALS(view.IsRasterizationRequestManually(), false, TEST_LOCATION);

  view.SetRasterizationRequestManually(true);
  DALI_TEST_EQUALS(view.IsRasterizationRequestManually(), true, TEST_LOCATION);

  view.SetRasterizationRequestManually(false);
  DALI_TEST_EQUALS(view.IsRasterizationRequestManually(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewPropertyRasterizationRequestManuallyP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  view.SetProperty(CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY, true);
  bool manual = view.GetProperty(CanvasView::Property::RASTERIZATION_REQUEST_MANUALLY).Get<bool>();
  DALI_TEST_EQUALS(manual, true, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// RequestRasterization
// =============================================================================

int UtcDaliCanvasViewRequestRasterizationP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  // Should not crash
  view.RequestRasterization();

  application.SendNotification();
  application.Render();
  END_TEST;
}

// =============================================================================
// Synchronous rendering — add drawable and render
// =============================================================================

int UtcDaliCanvasViewAddDrawableAndRenderSyncP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(200, 200));
  DALI_TEST_CHECK(view);

  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  application.GetScene().Add(view);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  shape.AddRect(Bounds(10, 10, 80, 80), Vector2::ZERO);
  shape.SetFillColor(Color::RED);
  view.AddDrawable(shape);

  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();
  END_TEST;
}

int UtcDaliCanvasViewAddDrawableAndRenderAsyncP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(200, 200));
  DALI_TEST_CHECK(view);

  view.SetSynchronousLoading(false);
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  application.GetScene().Add(view);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  shape.AddRect(Bounds(10, 10, 80, 80), Vector2::ZERO);
  shape.SetFillColor(Color::BLUE);
  view.AddDrawable(shape);

  application.SendNotification();
  application.Render();

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();
  END_TEST;
}

// =============================================================================
// Scene effects (drop shadow / gaussian blur / auto-padding)
//
// These verify the dali-ui delegation and the effect state. Numeric conversion
// (sigma/distance) and the auto-padding buffer sizing are covered at the adaptor
// level in utc-Dali-CanvasRenderer.cpp.
// =============================================================================

int UtcDaliCanvasViewDropShadowP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  DALI_TEST_EQUALS(view.HasDropShadow(), false, TEST_LOCATION);

  view.SetDropShadow(Vector4(0.0f, 0.0f, 0.0f, 0.5f), 0.0f, 5.0f, 8.0f);
  DALI_TEST_EQUALS(view.HasDropShadow(), true, TEST_LOCATION);

  view.ClearDropShadow();
  DALI_TEST_EQUALS(view.HasDropShadow(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewGaussianBlurP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  DALI_TEST_EQUALS(view.HasGaussianBlur(), false, TEST_LOCATION);

  view.SetGaussianBlur(10.0f);
  DALI_TEST_EQUALS(view.HasGaussianBlur(), true, TEST_LOCATION);

  // Effects are mutually exclusive: a drop shadow replaces the blur.
  view.SetDropShadow(Vector4(0.0f, 0.0f, 0.0f, 0.5f), 0.0f, 5.0f, 8.0f);
  DALI_TEST_EQUALS(view.HasGaussianBlur(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.HasDropShadow(), true, TEST_LOCATION);

  view.ClearDropShadow();
  DALI_TEST_EQUALS(view.HasDropShadow(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewEffectAutoPaddingP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  // Enabled by default; togglable.
  DALI_TEST_EQUALS(view.IsEffectAutoPaddingEnabled(), true, TEST_LOCATION);
  view.SetEffectAutoPaddingEnabled(false);
  DALI_TEST_EQUALS(view.IsEffectAutoPaddingEnabled(), false, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewEffectRenderSyncP(void)
{
  UiTestApplication application;
  CanvasView        view = CanvasView::New(Vector2(200, 200));
  DALI_TEST_CHECK(view);

  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  application.GetScene().Add(view);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  shape.AddRect(Bounds(40, 40, 120, 120), Vector2(16, 16));
  shape.SetFillColor(Color::BLUE);
  view.AddDrawable(shape);

  // Drives the full commit/rasterize pipeline with an effect, then clears it.
  view.SetDropShadow(Vector4(0.0f, 0.0f, 0.0f, 0.5f), 0.0f, 5.0f, 8.0f);
  application.SendNotification();
  application.Render();

  view.ClearDropShadow();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.HasDropShadow(), false, TEST_LOCATION);

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();
  END_TEST;
}

// =============================================================================
// Shape::AddPath
// =============================================================================

int UtcDaliCanvasViewShapeAddPathP(void)
{
  UiTestApplication application;
  using PathCommandType = Dali::CanvasRenderer::Shape::PathCommandType;

  CanvasView view = CanvasView::New(Vector2(200, 200));
  DALI_TEST_CHECK(view);
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  application.GetScene().Add(view);

  // MOVE_TO and LINE_TO take one point each, CLOSE takes none: 3 points expected.
  Dali::Vector<PathCommandType> commands;
  commands.PushBack(PathCommandType::MOVE_TO);
  commands.PushBack(PathCommandType::LINE_TO);
  commands.PushBack(PathCommandType::LINE_TO);
  commands.PushBack(PathCommandType::CLOSE);

  Dali::Vector<Vector2> points;
  points.PushBack(Vector2(20.0f, 20.0f));
  points.PushBack(Vector2(160.0f, 20.0f));
  points.PushBack(Vector2(90.0f, 150.0f));

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  DALI_TEST_EQUALS(shape.AddPath(commands, points), true, TEST_LOCATION);
  shape.SetFillColor(Color::RED);
  view.AddDrawable(shape);

  application.SendNotification();
  application.Render();

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();
  END_TEST;
}

int UtcDaliCanvasViewShapeAddPathN(void)
{
  UiTestApplication application;
  using PathCommandType = Dali::CanvasRenderer::Shape::PathCommandType;

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  DALI_TEST_CHECK(shape);

  // An inconsistent command/point pair is rejected instead of reading past the points.
  // CUBIC_TO takes three points, so MOVE_TO + CUBIC_TO needs four in total.
  Dali::Vector<PathCommandType> commands;
  commands.PushBack(PathCommandType::MOVE_TO);
  commands.PushBack(PathCommandType::CUBIC_TO);

  Dali::Vector<Vector2> tooFewPoints;
  tooFewPoints.PushBack(Vector2(0.0f, 0.0f));
  tooFewPoints.PushBack(Vector2(10.0f, 10.0f));

  DALI_TEST_EQUALS(shape.AddPath(commands, tooFewPoints), false, TEST_LOCATION);

  Dali::Vector<Vector2> tooManyPoints;
  tooManyPoints.PushBack(Vector2(0.0f, 0.0f));
  tooManyPoints.PushBack(Vector2(10.0f, 0.0f));
  tooManyPoints.PushBack(Vector2(20.0f, 0.0f));
  tooManyPoints.PushBack(Vector2(30.0f, 0.0f));
  tooManyPoints.PushBack(Vector2(40.0f, 0.0f));

  DALI_TEST_EQUALS(shape.AddPath(commands, tooManyPoints), false, TEST_LOCATION);

  // An empty path is rejected too.
  DALI_TEST_EQUALS(shape.AddPath(Dali::Vector<PathCommandType>(), Dali::Vector<Vector2>()), false, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Gradient color stops
// =============================================================================

int UtcDaliCanvasViewGradientColorStopsP(void)
{
  UiTestApplication application;

  Dali::CanvasRenderer::LinearGradient gradient = Dali::CanvasRenderer::LinearGradient::New();
  DALI_TEST_CHECK(gradient);
  DALI_TEST_EQUALS(gradient.GetColorStopCount(), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(gradient.AddColorStop(0.0f, Color::RED), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.AddColorStop(1.0f, Color::BLUE), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopCount(), 2u, TEST_LOCATION);

  // Stops keep the order they were added in.
  DALI_TEST_EQUALS(gradient.GetColorStopOffset(0u), 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopOffset(1u), 1.0f, 0.01f, TEST_LOCATION);

  // The colour round-trips through the 8-bit channels ThorVG stores.
  DALI_TEST_EQUALS(gradient.GetColorStopColor(0u), Color::RED, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopColor(1u), Color::BLUE, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewGradientClearColorStopsP(void)
{
  UiTestApplication application;

  Dali::CanvasRenderer::RadialGradient gradient = Dali::CanvasRenderer::RadialGradient::New();
  DALI_TEST_CHECK(gradient);

  gradient.AddColorStop(0.0f, Color::GREEN);
  gradient.AddColorStop(0.5f, Color::YELLOW);
  gradient.AddColorStop(1.0f, Color::BLACK);
  DALI_TEST_EQUALS(gradient.GetColorStopCount(), 3u, TEST_LOCATION);

  DALI_TEST_EQUALS(gradient.ClearColorStops(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopCount(), 0u, TEST_LOCATION);

  // The gradient stays usable after being cleared.
  DALI_TEST_EQUALS(gradient.AddColorStop(0.25f, Color::WHITE), true, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopOffset(0u), 0.25f, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewGradientColorStopsN(void)
{
  UiTestApplication application;

  Dali::CanvasRenderer::LinearGradient gradient = Dali::CanvasRenderer::LinearGradient::New();
  DALI_TEST_CHECK(gradient);
  gradient.AddColorStop(0.0f, Color::RED);

  // Out-of-range lookups fall back to defaults rather than reading past the stops.
  DALI_TEST_EQUALS(gradient.GetColorStopOffset(1u), 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopColor(1u), Vector4::ZERO, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetColorStopOffset(99u), 0.0f, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewShapeGradientFillP(void)
{
  UiTestApplication application;

  CanvasView view = CanvasView::New(Vector2(200, 200));
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  application.GetScene().Add(view);

  Dali::CanvasRenderer::LinearGradient gradient = Dali::CanvasRenderer::LinearGradient::New();
  gradient.SetBounds(Vector2(0.0f, 0.0f), Vector2(200.0f, 200.0f));
  gradient.AddColorStop(0.0f, Color::MAGENTA);
  gradient.AddColorStop(1.0f, Color::CYAN);

  Dali::CanvasRenderer::Shape shape = Dali::CanvasRenderer::Shape::New();
  shape.AddRect(Bounds(20, 20, 160, 160), Vector2(8, 8));
  DALI_TEST_EQUALS(shape.SetFillGradient(gradient), true, TEST_LOCATION);
  view.AddDrawable(shape);

  application.SendNotification();
  application.Render();

  Dali::CanvasRenderer::Gradient result = shape.GetFillGradient();
  DALI_TEST_CHECK(result);
  DALI_TEST_EQUALS(result.GetColorStopCount(), 2u, TEST_LOCATION);

  application.GetScene().Remove(view);
  application.SendNotification();
  application.Render();
  END_TEST;
}
