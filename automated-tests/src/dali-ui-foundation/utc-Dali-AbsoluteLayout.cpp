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

#include <dali-test-suite-utils.h>
#include <dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/absolute-layout.h>
#include <dali-ui-foundation/public-api/layout-types.h>

using namespace Dali;
using namespace Dali::UI;

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
  TestApplication application;
  AbsoluteLayout layout;
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutNewP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  DALI_TEST_CHECK(layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutCopyConstructorP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout copy(layout);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliAbsoluteLayoutMoveConstructor(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout moved = std::move(layout);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutAssignmentOperatorP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout copy;
  copy = layout;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(layout == copy);
  END_TEST;
}

int UtcDaliAbsoluteLayoutDownCastP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  AbsoluteLayout layout2 = AbsoluteLayout::DownCast(layout);
  DALI_TEST_CHECK(layout2);
  DALI_TEST_CHECK(layout == layout2);
  END_TEST;
}

int UtcDaliAbsoluteLayoutDownCastN(void)
{
  TestApplication application;
  BaseHandle unInitialized;
  AbsoluteLayout layout = AbsoluteLayout::DownCast(unInitialized);
  DALI_TEST_CHECK(!layout);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSetLayoutBoundsP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  LayoutRect bounds(10.0f, 20.0f, 100.0f, 50.0f);
  AbsoluteLayout::SetLayoutBounds(child, bounds);
  LayoutRect got = AbsoluteLayout::GetLayoutBounds(child);
  DALI_TEST_EQUALS(got.GetX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetWidth(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetHeight(), 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutGetLayoutBoundsP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  LayoutRect got = AbsoluteLayout::GetLayoutBounds(child);
  DALI_TEST_EQUALS(got.GetX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetWidth(), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetHeight(), -1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSetLayoutFlagsP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  AbsoluteLayout::SetLayoutFlags(child, AbsoluteLayoutFlags::POSITION_PROPORTIONAL);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayout::GetLayoutFlags(child)),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), TEST_LOCATION);
  AbsoluteLayout::SetLayoutFlags(child, AbsoluteLayoutFlags::ALL);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayout::GetLayoutFlags(child)),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::ALL), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutGetLayoutFlagsP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayout::GetLayoutFlags(child)),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::NONE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutLayoutBoundsZeroP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  AbsoluteLayout::SetLayoutBounds(child, LayoutRect(0, 0, 0, 0));
  LayoutRect got = AbsoluteLayout::GetLayoutBounds(child);
  DALI_TEST_EQUALS(got.GetX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetHeight(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSizeProportionalFlagP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  AbsoluteLayout::SetLayoutFlags(child, AbsoluteLayoutFlags::SIZE_PROPORTIONAL);
  DALI_TEST_EQUALS(static_cast<uint8_t>(AbsoluteLayout::GetLayoutFlags(child)),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutMeasureArrangeP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  AbsoluteLayout::SetLayoutBounds(child, LayoutRect(10, 20, 100, 50));
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(150.0f);
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
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  AbsoluteLayout::SetLayoutBounds(child, LayoutRect(0.1f, 0.2f, 0.3f, 0.4f));
  AbsoluteLayout::SetLayoutFlags(child, AbsoluteLayoutFlags::POSITION_PROPORTIONAL);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(150.0f);
  MeasuredSize m = layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_CHECK(m.GetWidth() >= 0.0f);
  END_TEST;
}

int UtcDaliAbsoluteLayoutSizeProportionalP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.AddView(child);
  AbsoluteLayout::SetLayoutBounds(child, LayoutRect(0, 0, 0.5f, 0.5f));
  AbsoluteLayout::SetLayoutFlags(child, AbsoluteLayoutFlags::SIZE_PROPORTIONAL);
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(150.0f);
  MeasuredSize m = layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutNegativeSizeMeasureP(void)
{
  TestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  child.SetLayoutWidth(70.0f);
  child.SetLayoutHeight(35.0f);
  layout.AddView(child);
  AbsoluteLayout::SetLayoutBounds(child, LayoutRect(10, 20, -1.0f, -1.0f));
  layout.SetLayoutWidth(200.0f);
  layout.SetLayoutHeight(150.0f);
  MeasuredSize m = layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));
  DALI_TEST_EQUALS(m.GetWidth(), 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(m.GetHeight(), 150.0f, TEST_LOCATION);
  END_TEST;
}
