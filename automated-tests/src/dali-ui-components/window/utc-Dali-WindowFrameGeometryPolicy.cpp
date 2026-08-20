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
 */

#include <dali-ui-components/internal/window/window-frame-layout-engine.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali::Ui;
using namespace Dali::Ui::Internal;

namespace
{
constexpr float EPSILON = 0.001f;

void CheckSize(const Dali::Vector2& actual, float width, float height, const char* location)
{
  DALI_TEST_EQUALS(actual.width, width, EPSILON, location);
  DALI_TEST_EQUALS(actual.height, height, EPSILON, location);
}

void CheckRect(const Dali::Ui::LayoutRect& actual, float x, float y, float width, float height, const char* location)
{
  DALI_TEST_EQUALS(actual.x, x, EPSILON, location);
  DALI_TEST_EQUALS(actual.y, y, EPSILON, location);
  DALI_TEST_EQUALS(actual.width, width, EPSILON, location);
  DALI_TEST_EQUALS(actual.height, height, EPSILON, location);
}

WindowFrameLayout MakeLayout(const Dali::Ui::Insets& decoration,
                             const Dali::Vector2&    leftHandle,
                             const Dali::Vector2&    rightHandle)
{
  WindowFrameLayout layout;
  layout.SetDecorationInsets(decoration);
  layout.SetBottomLeftResizeHandleSize(leftHandle);
  layout.SetBottomRightResizeHandleSize(rightHandle);
  return layout;
}
} // unnamed namespace

int UtcDaliWindowFrameGeometryNormalAsymmetric(void)
{
  const Dali::Ui::Insets        shadow(4.0f, 8.0f, 6.0f, 10.0f);
  const WindowFrameLayout       frameLayout = MakeLayout(Dali::Ui::Insets(12.0f, 16.0f, 24.0f, 32.0f),
                                                         Dali::Vector2(30.0f, 28.0f),
                                                         Dali::Vector2(42.0f, 34.0f));
  const WindowFrameLayoutResult layout      = WindowFrameLayoutEngine::Calculate(440.0f, 316.0f, frameLayout, WindowFrameLayoutOptions{false, false, shadow});

  CheckRect(layout.GetSurface(), 0.0f, 0.0f, 440.0f, 316.0f, TEST_LOCATION);
  CheckRect(layout.GetEffect(), 0.0f, 0.0f, 440.0f, 316.0f, TEST_LOCATION);
  CheckRect(layout.GetFrame(), 4.0f, 6.0f, 428.0f, 300.0f, TEST_LOCATION);
  CheckRect(layout.GetContent(), 16.0f, 30.0f, 400.0f, 244.0f, TEST_LOCATION);
  CheckRect(layout.GetTop(), 4.0f, 6.0f, 428.0f, 24.0f, TEST_LOCATION);
  CheckRect(layout.GetBottom(), 4.0f, 274.0f, 428.0f, 32.0f, TEST_LOCATION);
  CheckRect(layout.GetLeft(), 4.0f, 30.0f, 12.0f, 244.0f, TEST_LOCATION);
  CheckRect(layout.GetRight(), 416.0f, 30.0f, 16.0f, 244.0f, TEST_LOCATION);
  CheckRect(layout.GetBottomLeftResizeHandle(), 4.0f, 278.0f, 30.0f, 28.0f, TEST_LOCATION);
  CheckRect(layout.GetBottomRightResizeHandle(), 390.0f, 272.0f, 42.0f, 34.0f, TEST_LOCATION);

  CheckRect(WindowFrameLayoutEngine::SurfaceToFrameLocal(layout.GetContent(), layout.GetFrame()), 12.0f, 24.0f, 400.0f, 244.0f, TEST_LOCATION);
  CheckRect(WindowFrameLayoutEngine::SurfaceToFrameLocal(layout.GetTop(), layout.GetFrame()), 0.0f, 0.0f, 428.0f, 24.0f, TEST_LOCATION);
  CheckRect(WindowFrameLayoutEngine::SurfaceToFrameLocal(layout.GetBottom(), layout.GetFrame()), 0.0f, 268.0f, 428.0f, 32.0f, TEST_LOCATION);
  CheckRect(WindowFrameLayoutEngine::SurfaceToFrameLocal(layout.GetLeft(), layout.GetFrame()), 0.0f, 24.0f, 12.0f, 244.0f, TEST_LOCATION);
  CheckRect(WindowFrameLayoutEngine::SurfaceToFrameLocal(layout.GetRight(), layout.GetFrame()), 412.0f, 24.0f, 16.0f, 244.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameGeometrySizeRoundTrips(void)
{
  const Dali::Ui::Insets  shadow(4.0f, 8.0f, 6.0f, 10.0f);
  const WindowFrameLayout frameLayout = MakeLayout(Dali::Ui::Insets(12.0f, 16.0f, 24.0f, 32.0f),
                                                   Dali::Vector2(30.0f, 28.0f),
                                                   Dali::Vector2(42.0f, 34.0f));
  const Dali::Vector2     content(400.0f, 244.0f);
  const Dali::Vector2     frame   = WindowFrameLayoutEngine::ContentToFrameSize(content, frameLayout);
  const Dali::Vector2     surface = WindowFrameLayoutEngine::FrameToSurfaceSize(frame, shadow);

  CheckSize(frame, 428.0f, 300.0f, TEST_LOCATION);
  CheckSize(surface, 440.0f, 316.0f, TEST_LOCATION);
  CheckSize(WindowFrameLayoutEngine::SurfaceToFrameSize(surface, shadow), 428.0f, 300.0f, TEST_LOCATION);
  CheckSize(WindowFrameLayoutEngine::SurfaceToContentSize(surface, frameLayout, WindowFrameLayoutOptions{false, false, shadow}), 400.0f, 244.0f, TEST_LOCATION);
  CheckSize(WindowFrameLayoutEngine::ContentToSurfaceSize(content, frameLayout, shadow), 440.0f, 316.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameGeometryMaximized(void)
{
  const Dali::Ui::Insets         shadow(12.0f, 12.0f, 16.0f, 20.0f);
  const WindowFrameLayout        frameLayout = MakeLayout(Dali::Ui::Insets(8.0f, 8.0f, 64.0f, 56.0f),
                                                          Dali::Vector2(36.0f, 36.0f),
                                                          Dali::Vector2(36.0f, 36.0f));
  const WindowFrameLayoutOptions options{true, true, shadow};
  const WindowFrameLayoutResult  layout = WindowFrameLayoutEngine::Calculate(1920.0f, 1080.0f, frameLayout, options);

  CheckRect(layout.GetFrame(), 0.0f, 0.0f, 1920.0f, 1080.0f, TEST_LOCATION);
  CheckRect(layout.GetContent(), 0.0f, 0.0f, 1920.0f, 1080.0f, TEST_LOCATION);
  CheckSize(WindowFrameLayoutEngine::FrameToSurfaceSize(Dali::Vector2(400.0f, 260.0f), shadow, true), 400.0f, 260.0f, TEST_LOCATION);
  CheckSize(WindowFrameLayoutEngine::FrameToSurfaceSize(Dali::Vector2(400.0f, 260.0f), shadow, false), 424.0f, 296.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameGeometryClamping(void)
{
  const Dali::Ui::Insets        shadow(-3.0f, 8.0f, 30.0f, 30.0f);
  const WindowFrameLayout       frameLayout = MakeLayout(Dali::Ui::Insets(20.0f, 20.0f, -5.0f, 40.0f),
                                                         Dali::Vector2(40.0f, -2.0f),
                                                         Dali::Vector2(80.0f, 50.0f));
  const WindowFrameLayoutResult layout      = WindowFrameLayoutEngine::Calculate(60.0f, 40.0f, frameLayout, WindowFrameLayoutOptions{false, false, shadow});

  CheckRect(layout.GetFrame(), 0.0f, 30.0f, 52.0f, 0.0f, TEST_LOCATION);
  CheckRect(layout.GetContent(), 20.0f, 30.0f, 12.0f, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetBottomLeftResizeHandle().width + layout.GetBottomRightResizeHandle().width, layout.GetFrame().width, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetBottomLeftResizeHandle().height, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(layout.GetBottomRightResizeHandle().height, 0.0f, EPSILON, TEST_LOCATION);
  CheckSize(WindowFrameLayoutEngine::SurfaceToContentSize(Dali::Vector2(-1.0f, -1.0f), frameLayout), 0.0f, 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameGeometryDecorationInsetsClampToFrame(void)
{
  WindowFrameLayout layout;
  layout.SetDecorationInsets(Insets(0.0f, 0.0f, 64.0f, 56.0f));

  // A frame smaller than the configured decoration clamps it, so the laid-out
  // slots are what an application must trust, not the requested layout.
  const WindowFrameLayoutResult tight = WindowFrameLayoutEngine::Calculate(200.0f, 50.0f, layout, WindowFrameLayoutOptions{true, true, Insets()});
  DALI_TEST_EQUALS(tight.GetTop().height, 50.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(tight.GetBottom().height, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(tight.GetTop().height + tight.GetBottom().height, 50.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliWindowFrameGeometryComparison(void)
{
  const WindowFrameLayoutResult layout  = WindowFrameLayoutEngine::Calculate(440.0f, 316.0f, WindowFrameLayout{});
  const LayoutRect              surface = Dali::Ui::LayoutRect(80.0f, 60.0f, 440.0f, 316.0f);
  const Dali::Ui::Insets        noOverlay;

  WindowFrameGeometry first{surface, layout.GetFrame(), layout.GetContent(), noOverlay};

  Dali::Ui::LayoutRect changedContent = layout.GetContent();
  changedContent.width -= 1.0f;
  WindowFrameGeometry second{surface, layout.GetFrame(), changedContent, noOverlay};

  Dali::Ui::LayoutRect movedSurface = first.GetSurfaceBounds();
  movedSurface.x += 1.0f;
  WindowFrameGeometry moved{movedSurface, layout.GetFrame(), layout.GetContent(), noOverlay};

  // Entering overlay layout changes what decoration covers even when every
  // rectangle stays the same, so it has to count as a geometry change.
  WindowFrameGeometry overlaid{surface, layout.GetFrame(), layout.GetContent(), Dali::Ui::Insets(0.0f, 0.0f, 64.0f, 56.0f)};

  DALI_TEST_CHECK(IsSameWindowFrameGeometry(first, first));
  DALI_TEST_CHECK(!IsSameWindowFrameGeometry(first, second));
  DALI_TEST_CHECK(!IsSameWindowFrameGeometry(first, moved));
  DALI_TEST_CHECK(!IsSameWindowFrameGeometry(first, overlaid));
  DALI_TEST_EQUALS(first.GetDecorationOverlayInsets().top, 0.0f, EPSILON, TEST_LOCATION);
  END_TEST;
}
