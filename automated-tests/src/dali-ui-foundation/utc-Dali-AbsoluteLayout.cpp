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
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_absolutelayout_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_absolutelayout_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAbsoluteLayoutConstructorP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutNewP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  DALI_TEST_CHECK(layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutCopyConstructorP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliAbsoluteLayoutMoveConstructor(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutAssignmentOperatorP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliAbsoluteLayoutDownCastP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout layout2 = AbsoluteLayout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliAbsoluteLayoutDownCastN(void)
{
  UiTestApplication application;
  BaseHandle unInitialized;
  AbsoluteLayout layout = AbsoluteLayout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSetLayoutBoundsP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  LayoutRect bounds(10.0f, 20.0f, 100.0f, 50.0f);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(bounds));
  LayoutRect got = child.GetLayoutParams<AbsoluteLayoutParams>().GetBounds();
  DALI_TEST_EQUALS(got.GetX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutGetLayoutBoundsP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New());
  LayoutRect got = child.GetLayoutParams<AbsoluteLayoutParams>().GetBounds();
  DALI_TEST_EQUALS(got.GetX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetWidth(), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetHeight(), -1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSetLayoutFlagsP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
  DALI_TEST_EQUALS(static_cast<uint8_t>(child.GetLayoutParams<AbsoluteLayoutParams>().GetFlags()),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), TEST_LOCATION);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetFlags(AbsoluteLayoutFlags::ALL));
  DALI_TEST_EQUALS(static_cast<uint8_t>(child.GetLayoutParams<AbsoluteLayoutParams>().GetFlags()),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::ALL), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutGetLayoutFlagsP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New());
  DALI_TEST_EQUALS(static_cast<uint8_t>(child.GetLayoutParams<AbsoluteLayoutParams>().GetFlags()),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::NONE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutLayoutBoundsZeroP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, 0, 0)));
  LayoutRect got = child.GetLayoutParams<AbsoluteLayoutParams>().GetBounds();
  DALI_TEST_EQUALS(got.GetX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSizeProportionalFlagP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));
  DALI_TEST_EQUALS(static_cast<uint8_t>(child.GetLayoutParams<AbsoluteLayoutParams>().GetFlags()),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutMeasureArrangeP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10, 20, 100, 50)));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  MeasuredSize m = layout.Measure(200.0f, 150.0f);
  MeasuredSize a = layout.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 150.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(a.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutPositionProportionalP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0.1f, 0.2f, 0.3f, 0.4f))
    .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  MeasuredSize m = layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_CHECK(m.GetWidth() >= 0.0f);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSizeProportionalP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New()
    .SetBounds(LayoutRect(0, 0, 0.5f, 0.5f))
    .SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  MeasuredSize m = layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutNegativeSizeMeasureP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  child.SetRequestedWidth(70.0f);
  child.SetRequestedHeight(35.0f);
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10, 20, -1.0f, -1.0f)));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  MeasuredSize m = layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutStandaloneIgnoresParentPaddingP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  layout.SetViewPadding(Extents(10, 10, 10, 10));

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetViewMargin(Extents(5, 5, 7, 7));
  standalone.SetRequestedWidth(MATCH_PARENT);
  standalone.SetRequestedHeight(MATCH_PARENT);
  layout.Add(standalone);

  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  DALI_TEST_EQUALS(standalone.GetSize().width, 200.0f - 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().height, 150.0f - 14.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionX(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionY(), 7.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutStandaloneBypassesBoundsP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetRequestedWidth(30.0f);
  standalone.SetRequestedHeight(20.0f);
  standalone.SetPositionX(60.0f);
  standalone.SetPositionY(70.0f);
  // AbsoluteLayoutParams bounds should be ignored for Standalone children.
  standalone.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, 999, 999)));
  layout.Add(standalone);

  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  DALI_TEST_EQUALS(standalone.GetPositionX(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetPositionY(), 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().width, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(standalone.GetSize().height, 20.0f, TEST_LOCATION);
  END_TEST;
}
