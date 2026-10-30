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

#include <stdlib.h>
#include <iostream>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/public-api/views/scroll/scroll-bar.h>
#include <dali-ui-foundation/public-api/views/scroll/scrollable-enum.h>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::Extension::LayoutScaleManager;

namespace
{
// Sets the system UI scale for the scope of a test and restores 1.0 when the scope ends, also when
// a DALI_TEST_* check throws. Declare it after the UiTestApplication so that it is destroyed first.
class ScopedUiScale
{
public:
  explicit ScopedUiScale(float scale)
  {
    LayoutScaleManager::Get().SetUiScale(scale);
  }

  ~ScopedUiScale()
  {
    LayoutScaleManager::Get().SetUiScale(1.0f);
  }

  ScopedUiScale(const ScopedUiScale&)            = delete;
  ScopedUiScale& operator=(const ScopedUiScale&) = delete;
};
} // namespace

void utc_dali_scrollbar_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_scrollbar_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliScrollBarNew(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarNew");

  ScrollBar scrollBar = ScrollBar::New();
  DALI_TEST_CHECK(scrollBar);

  END_TEST;
}

int UtcDaliScrollBarDownCast(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarDownCast");

  ScrollBar scrollBar = ScrollBar::New();
  BaseHandle handle(scrollBar);

  ScrollBar scrollBar2 = ScrollBar::DownCast(handle);
  DALI_TEST_CHECK(scrollBar2);
  DALI_TEST_CHECK(scrollBar == scrollBar2);

  BaseHandle handle2;
  ScrollBar scrollBar3 = ScrollBar::DownCast(handle2);
  DALI_TEST_CHECK(!scrollBar3);

  END_TEST;
}

int UtcDaliScrollBarCopyConstructor(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarCopyConstructor");

  ScrollBar scrollBar = ScrollBar::New();
  ScrollBar copy(scrollBar);
  DALI_TEST_CHECK(copy);

  END_TEST;
}

int UtcDaliScrollBarAssignmentOperator(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarAssignmentOperator");

  ScrollBar scrollBar = ScrollBar::New();
  ScrollBar copy;
  copy = scrollBar;
  DALI_TEST_CHECK(copy);

  END_TEST;
}

int UtcDaliScrollBarMoveConstructor(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarMoveConstructor");

  ScrollBar scrollBar = ScrollBar::New();
  ScrollBar moved = std::move(scrollBar);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!scrollBar);

  END_TEST;
}

int UtcDaliScrollBarMoveAssignmentOperator(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarMoveAssignmentOperator");

  ScrollBar scrollBar = ScrollBar::New();
  ScrollBar moved;
  moved = std::move(scrollBar);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!scrollBar);

  END_TEST;
}

int UtcDaliScrollBarGetSetBarThickness(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetBarThickness");

  ScrollBar scrollBar = ScrollBar::New();

  // Check default value
  DALI_TEST_EQUALS(scrollBar.GetBarThickness(), 4.0f, TEST_LOCATION);

  // Set new value
scrollBar.SetBarThickness(10.0f);
  DALI_TEST_EQUALS(scrollBar.GetBarThickness(), 10.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollBarGetSetBarColor(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetBarColor");

  ScrollBar scrollBar = ScrollBar::New();

  // Check default value
  Vector4 defaultColor(0.8f, 0.8f, 0.8f, 1.0f);
  DALI_TEST_EQUALS(scrollBar.GetBarColor(), defaultColor, TEST_LOCATION);

  // Set new value
  Vector4 newColor(1.0f, 0.0f, 0.0f, 1.0f);
  scrollBar.SetBarColor(newColor);
  DALI_TEST_EQUALS(scrollBar.GetBarColor(), newColor, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollBarGetSetBarMinSize(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetBarMinSize");

  ScrollBar scrollBar = ScrollBar::New();

  // Set new value
  scrollBar.SetBarMinSize(20.0f);
  DALI_TEST_EQUALS(scrollBar.GetBarMinSize(), 20.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollBarGetSetBarOffset(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetBarOffset");

  ScrollBar scrollBar = ScrollBar::New();

  // Check default value
  DALI_TEST_EQUALS(scrollBar.GetBarOffset(), 2.0f, TEST_LOCATION);

  // Set new value
  scrollBar.SetBarOffset(5.0f);
  DALI_TEST_EQUALS(scrollBar.GetBarOffset(), 5.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollBarGetSetBarCornerRadius(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetBarCornerRadius");

  ScrollBar scrollBar = ScrollBar::New();

  // Check default value
  Vector4 defaultRadius(1.0f, 1.0f, 1.0f, 1.0f);
  DALI_TEST_EQUALS(scrollBar.GetBarCornerRadius(), defaultRadius, TEST_LOCATION);

  // Set new value (same for all corners)
  scrollBar.SetBarCornerRadius(3.0f);
  Vector4 expectedRadius(3.0f, 3.0f, 3.0f, 3.0f);
  DALI_TEST_EQUALS(scrollBar.GetBarCornerRadius(), expectedRadius, TEST_LOCATION);

  // Set new value (different for each corner)
  Vector4 newRadius(1.0f, 2.0f, 3.0f, 4.0f);
  scrollBar.SetBarCornerRadius(newRadius);
  DALI_TEST_EQUALS(scrollBar.GetBarCornerRadius(), newRadius, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollBarGetSetVerticalScrollBarVisibility(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetVerticalScrollBarVisibility");

  ScrollBar scrollBar = ScrollBar::New();

  // Check default value
  DALI_TEST_EQUALS(scrollBar.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Auto, TEST_LOCATION);

  // Set new value
  scrollBar.SetVerticalScrollBarVisibility(ScrollBarVisibility::Always);
  DALI_TEST_EQUALS(scrollBar.GetVerticalScrollBarVisibility(), ScrollBarVisibility::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollBarGetSetHorizontalScrollBarVisibility(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetHorizontalScrollBarVisibility");

  ScrollBar scrollBar = ScrollBar::New();

  // Check default value
  DALI_TEST_EQUALS(scrollBar.GetHorizontalScrollBarVisibility(), ScrollBarVisibility::Auto, TEST_LOCATION);

  // Set new value
  scrollBar.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Always);
  DALI_TEST_EQUALS(scrollBar.GetHorizontalScrollBarVisibility(), ScrollBarVisibility::Always, TEST_LOCATION);

  END_TEST;
}

int UtcDaliScrollBarUpdateBarSize(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarUpdateBarSize");

  ScrollBar scrollBar = ScrollBar::New();

  // This method doesn't return a value, so we just check that it doesn't crash
  scrollBar.UpdateBarSize(100.0f, 200.0f, 50.0f, 100.0f);
  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcDaliScrollBarUpdateScrollPosition(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarUpdateScrollPosition");

  ScrollBar scrollBar = ScrollBar::New();

  // This method doesn't return a value, so we just check that it doesn't crash
  scrollBar.UpdateScrollPosition(Vector2(10.0f, 20.0f));
  DALI_TEST_CHECK(true);

  END_TEST;
}

int UtcDaliScrollBarGetSetScrollPosition(void)
{
  UiTestApplication application;
  tet_infoline(" UtcDaliScrollBarGetSetScrollPosition");

  ScrollBar scrollBar = ScrollBar::New();

  // Check default value
  Vector2 defaultPosition(0.0f, 0.0f);
  DALI_TEST_EQUALS(scrollBar.GetScrollPosition(), defaultPosition, TEST_LOCATION);

  // Set new value
  Vector2 newPosition(15.0f, 25.0f);
  scrollBar.SetScrollPosition(newPosition);
  DALI_TEST_EQUALS(scrollBar.GetScrollPosition(), newPosition, TEST_LOCATION);

  END_TEST;
}

// The fallback minimum length (SetBarMinSize(<= 0)) is a twentieth of the bar's longer side. That side
// is read from the actor in rendered pixels, while the bar bounds are natural units the layout
// multiplies by the scale, so the fallback is converted to natural units first. At a UI scale of 2 a
// 200 x 100 parent is 400 x 200: the fallback is 400 / 20 = 20 rendered pixels, 10 natural. The
// thickness 4 and offset 2 render as 8 and 4 (the bar at 400 - 4 - 8 = 388).
// Fails before this change: GetBarMinSize() returned 20 and the bar rendered 40 long.
int UtcDaliScrollBarMinSizeFallbackFollowsScaleP(void)
{
  UiTestApplication application;
  ScopedUiScale     uiScale(2.0f);

  View parent = View::New();
  parent.SetRequestedWidth(200.0f);
  parent.SetRequestedHeight(100.0f);
  ScrollBar scrollBar = ScrollBar::New();
  parent.Add(scrollBar);
  application.GetWindow().Add(parent);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(scrollBar.GetProperty<float>(Actor::Property::SIZE_WIDTH), 400.0f, 0.001f, TEST_LOCATION);

  scrollBar.SetBarMinSize(0.0f);
  DALI_TEST_EQUALS(scrollBar.GetBarMinSize(), 10.0f, 0.001f, TEST_LOCATION);

  // A content 500 times the viewport makes the proportional thumb round to 0: the minimum wins.
  scrollBar.UpdateBarSize(400.0f, 100000.0f, 400.0f, 200.0f);
  application.SendNotification();
  application.Render();

  View bar = View::DownCast(scrollBar.GetChildAt(0u));
  DALI_TEST_CHECK(bar);
  DALI_TEST_EQUALS(bar.GetProperty<float>(Actor::Property::SIZE_HEIGHT), 20.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(bar.GetProperty<float>(Actor::Property::SIZE_WIDTH), 8.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(bar.GetProperty<float>(Actor::Property::POSITION_X), 388.0f, 0.001f, TEST_LOCATION);
  END_TEST;
}
