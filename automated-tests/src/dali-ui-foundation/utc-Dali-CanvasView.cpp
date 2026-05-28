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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/canvas-view.h>
#include <dali/devel-api/adaptor-framework/canvas-renderer/canvas-renderer-shape.h>

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
  CanvasView         view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliCanvasViewNewP(void)
{
  UiTestApplication application;
  CanvasView         view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliCanvasViewNewWithoutViewboxP(void)
{
  UiTestApplication application;
  CanvasView         view = CanvasView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliCanvasViewCopyConstructorP(void)
{
  UiTestApplication application;
  CanvasView         view = CanvasView::New(Vector2(100, 100));
  CanvasView         copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliCanvasViewMoveConstructorP(void)
{
  UiTestApplication application;
  CanvasView         view  = CanvasView::New(Vector2(100, 100));
  CanvasView         moved = std::move(view);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliCanvasViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  CanvasView         view = CanvasView::New(Vector2(100, 100));
  CanvasView         copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliCanvasViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  CanvasView         view  = CanvasView::New(Vector2(100, 100));
  CanvasView         moved;
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
  CanvasView         view   = CanvasView::New(Vector2(100, 100));
  View               base   = view;
  CanvasView         result = CanvasView::DownCast(base);
  DALI_TEST_CHECK(result);
  END_TEST;
}

int UtcDaliCanvasViewDownCastN(void)
{
  UiTestApplication application;
  View               base   = View::New();
  CanvasView         result = CanvasView::DownCast(base);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

// =============================================================================
// AddDrawable
// =============================================================================

int UtcDaliCanvasViewAddDrawableP(void)
{
  UiTestApplication application;
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  Dali::CanvasRenderer::Shape emptyShape;
  bool                        result = view.AddDrawable(emptyShape);
  DALI_TEST_CHECK(!result);
  END_TEST;
}

// =============================================================================
// RemoveDrawable
// =============================================================================

int UtcDaliCanvasViewRemoveDrawableP(void)
{
  UiTestApplication application;
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
  DALI_TEST_CHECK(view);

  view.SetViewBox(Vector2(200, 200));
  Vector2 viewBox = view.GetViewBox();
  DALI_TEST_EQUALS(viewBox, Vector2(200, 200), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCanvasViewPropertyViewBoxP(void)
{
  UiTestApplication application;
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(100, 100));
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
  CanvasView         view = CanvasView::New(Vector2(200, 200));
  DALI_TEST_CHECK(view);

  view.SetRequestedWidth(200.0f).SetRequestedHeight(200.0f);
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
  CanvasView         view = CanvasView::New(Vector2(200, 200));
  DALI_TEST_CHECK(view);

  view.SetSynchronousLoading(false);
  view.SetRequestedWidth(200.0f).SetRequestedHeight(200.0f);
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
// Fluent API chaining
// =============================================================================

int UtcDaliCanvasViewFluentApiP(void)
{
  UiTestApplication application;

  CanvasView view = CanvasView::New(Vector2(100, 100))
                      .SetRequestedWidth(100.0f)
                      .SetRequestedHeight(100.0f)
                      .SetSynchronousLoading(true)
                      .SetRasterizationRequestManually(false)
                      .SetViewBox(Vector2(200, 200));

  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.IsSynchronousLoading(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(view.IsRasterizationRequestManually(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetViewBox(), Vector2(200, 200), TEST_LOCATION);
  END_TEST;
}
