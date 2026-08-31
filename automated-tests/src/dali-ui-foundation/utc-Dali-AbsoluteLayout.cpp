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

template<typename T>
T GetRequiredLayoutParams(View view)
{
  T params;
  DALI_TEST_CHECK(view.TryGetLayoutParams(params));
  return params;
}

namespace
{
// --- Reentrant layout mutation helpers. A child removes a sibling from the
// parent during the parent's own Measure/Arrange pass, mutating the live child
// list mid-iteration. Callbacks cannot be capturing lambdas (Callback::New only
// supports free/member fns), so the parent + sibling handles are file-static. ---
Ui::View gReentrantParent;
Ui::View gSiblingToRemove;

MeasuredSize PlainMeasure(View, float, float)
{
  return MeasuredSize(40.0f, 30.0f);
}

LayoutRect PlainArrange(View, const LayoutRect& bounds)
{
  return bounds;
}

// During the child's own Measure, remove a sibling from the parent. This
// reaches ViewImpl::OnChildRemove -> mChildren.Erase mid-iteration.
MeasuredSize ReentrantRemoveMeasure(View, float, float)
{
  if(gSiblingToRemove && gSiblingToRemove.GetParent() == static_cast<Actor>(gReentrantParent))
  {
    gReentrantParent.Remove(gSiblingToRemove, RemovePolicy::IMMEDIATE);
  }
  return MeasuredSize(40.0f, 30.0f);
}

// During the child's own Arrange, remove a sibling from the parent.
LayoutRect ReentrantRemoveArrange(View, const LayoutRect& bounds)
{
  if(gSiblingToRemove && gSiblingToRemove.GetParent() == static_cast<Actor>(gReentrantParent))
  {
    gReentrantParent.Remove(gSiblingToRemove, RemovePolicy::IMMEDIATE);
  }
  return bounds;
}

} // namespace

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
  LayoutRect got = GetRequiredLayoutParams<AbsoluteLayoutParams>(child).GetBounds();
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
  LayoutRect got = GetRequiredLayoutParams<AbsoluteLayoutParams>(child).GetBounds();
  DALI_TEST_EQUALS(got.GetX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetY(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetWidth(), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(got.GetHeight(), -1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutParamsValueSemanticsP(void)
{
  UiTestApplication    application;
  View                 a     = View::New();
  View                 b     = View::New();
  View                 empty = View::New();
  LayoutRect           boundsA(1.0f, 2.0f, 30.0f, 40.0f);
  LayoutRect           boundsB(5.0f, 6.0f, 70.0f, 80.0f);
  AbsoluteLayoutParams source = AbsoluteLayoutParams::New()
                                  .SetBounds(boundsA)
                                  .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL);

  AbsoluteLayoutParams copied(source);
  AbsoluteLayoutParams assigned;
  assigned = source;

  a.SetLayoutParams(source);
  b.SetLayoutParams(source);
  source.SetBounds(boundsB).SetFlags(AbsoluteLayoutFlags::ALL);
  DALI_TEST_EQUALS(copied.GetBounds().GetX(), boundsA.GetX(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(assigned.GetFlags()), static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), TEST_LOCATION);

  auto storedA = GetRequiredLayoutParams<AbsoluteLayoutParams>(a);
  auto storedB = GetRequiredLayoutParams<AbsoluteLayoutParams>(b);
  DALI_TEST_EQUALS(storedA.GetBounds().GetX(), boundsA.GetX(), TEST_LOCATION);
  DALI_TEST_EQUALS(storedA.GetBounds().GetY(), boundsA.GetY(), TEST_LOCATION);
  DALI_TEST_EQUALS(storedA.GetBounds().GetWidth(), boundsA.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(storedA.GetBounds().GetHeight(), boundsA.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(storedA.GetFlags()), static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), TEST_LOCATION);
  DALI_TEST_EQUALS(storedB.GetBounds().GetX(), boundsA.GetX(), TEST_LOCATION);

  storedA.SetBounds(boundsB).SetFlags(AbsoluteLayoutFlags::SIZE_PROPORTIONAL);
  auto unchangedA = GetRequiredLayoutParams<AbsoluteLayoutParams>(a);
  DALI_TEST_EQUALS(unchangedA.GetBounds().GetX(), boundsA.GetX(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(unchangedA.GetFlags()), static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), TEST_LOCATION);

  a.SetLayoutParams(storedA);
  auto committedA = GetRequiredLayoutParams<AbsoluteLayoutParams>(a);
  auto unchangedB = GetRequiredLayoutParams<AbsoluteLayoutParams>(b);
  DALI_TEST_EQUALS(committedA.GetBounds().GetX(), boundsB.GetX(), TEST_LOCATION);
  DALI_TEST_EQUALS(committedA.GetBounds().GetY(), boundsB.GetY(), TEST_LOCATION);
  DALI_TEST_EQUALS(committedA.GetBounds().GetWidth(), boundsB.GetWidth(), TEST_LOCATION);
  DALI_TEST_EQUALS(committedA.GetBounds().GetHeight(), boundsB.GetHeight(), TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<uint8_t>(committedA.GetFlags()), static_cast<uint8_t>(AbsoluteLayoutFlags::SIZE_PROPORTIONAL), TEST_LOCATION);
  DALI_TEST_EQUALS(unchangedB.GetBounds().GetX(), boundsA.GetX(), TEST_LOCATION);
  AbsoluteLayoutParams missingParams = AbsoluteLayoutParams::New().SetX(7.0f);
  DALI_TEST_CHECK(!empty.TryGetLayoutParams(missingParams));
  DALI_TEST_EQUALS(missingParams.GetX(), 7.0f, TEST_LOCATION);
  StackLayoutParams wrongTypeParams;
  DALI_TEST_CHECK(!a.TryGetLayoutParams(wrongTypeParams));
  END_TEST;
}

int UtcDaliAbsoluteLayoutSetLayoutFlagsP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
  DALI_TEST_EQUALS(static_cast<uint8_t>(GetRequiredLayoutParams<AbsoluteLayoutParams>(child).GetFlags()),
                   static_cast<uint8_t>(AbsoluteLayoutFlags::POSITION_PROPORTIONAL), TEST_LOCATION);
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetFlags(AbsoluteLayoutFlags::ALL));
  DALI_TEST_EQUALS(static_cast<uint8_t>(GetRequiredLayoutParams<AbsoluteLayoutParams>(child).GetFlags()),
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
  DALI_TEST_EQUALS(static_cast<uint8_t>(GetRequiredLayoutParams<AbsoluteLayoutParams>(child).GetFlags()),
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
  LayoutRect got = GetRequiredLayoutParams<AbsoluteLayoutParams>(child).GetBounds();
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
  DALI_TEST_EQUALS(static_cast<uint8_t>(GetRequiredLayoutParams<AbsoluteLayoutParams>(child).GetFlags()),
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
  LayoutRect a = layout.Arrange(LayoutRect(0, 0, 200, 150));
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

int UtcDaliAbsoluteLayoutXProportionalOnlyP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  View              child  = View::New();
  layout.Add(child);
  // X is proportional (0.25), Y and size are absolute.
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(0.25f, 20.0f, 100.0f, 50.0f))
                          .SetFlags(AbsoluteLayoutFlags::X_PROPORTIONAL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // X = (200 - 100) * 0.25 = 25; Y stays at 20; size unchanged.
  DALI_TEST_EQUALS(child.GetPositionX(), 25.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 50.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutYProportionalOnlyP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  View              child  = View::New();
  layout.Add(child);
  // Y is proportional (0.5), X and size are absolute.
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(30.0f, 0.5f, 80.0f, 40.0f))
                          .SetFlags(AbsoluteLayoutFlags::Y_PROPORTIONAL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // Y = (150 - 40) * 0.5 = 55; X stays at 30; size unchanged.
  DALI_TEST_EQUALS(child.GetPositionX(), 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 55.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutWidthProportionalOnlyP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  View              child  = View::New();
  layout.Add(child);
  // Width is proportional (0.4), others absolute.
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(10.0f, 20.0f, 0.4f, 60.0f))
                          .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // Width = 0.4 * 200 = 80; height stays at 60; position absolute.
  DALI_TEST_EQUALS(child.GetPositionX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 60.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutHeightProportionalOnlyP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  View              child  = View::New();
  layout.Add(child);
  // Height is proportional (0.6), others absolute.
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(10.0f, 20.0f, 50.0f, 0.6f))
                          .SetFlags(AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // Height = 0.6 * 150 = 90; width stays at 50; position absolute.
  DALI_TEST_EQUALS(child.GetPositionX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 90.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutMixedProportionalP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  View              child  = View::New();
  layout.Add(child);
  // X proportional + Height proportional; Y and Width absolute.
  child.SetLayoutParams(
    AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.5f, 10.0f, 40.0f, 0.5f))
      .SetFlags(AbsoluteLayoutFlags::X_PROPORTIONAL | AbsoluteLayoutFlags::HEIGHT_PROPORTIONAL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // Height = 0.5 * 150 = 75; Width = 40; X = (200 - 40) * 0.5 = 80; Y = 10.
  DALI_TEST_EQUALS(child.GetPositionX(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 40.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 75.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutAllFlagBitsP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  View              child  = View::New();
  layout.Add(child);
  // ALL should enable proportional behavior on every axis.
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(0.5f, 0.5f, 0.25f, 0.2f))
                          .SetFlags(AbsoluteLayoutFlags::ALL));
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0, 0, 200, 150));

  // Width = 0.25 * 200 = 50; Height = 0.2 * 150 = 30.
  // X = (200 - 50) * 0.5 = 75; Y = (150 - 30) * 0.5 = 60.
  DALI_TEST_EQUALS(child.GetPositionX(), 75.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutStandaloneIgnoresParentPaddingP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  layout.SetPadding(Insets(10.0f, 10.0f, 10.0f, 10.0f));

  View standalone = View::New();
  standalone.SetLayoutMode(LayoutMode::STANDALONE);
  standalone.SetMargin(Insets(5.0f, 5.0f, 7.0f, 7.0f));
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
  standalone.SetRequestedX(60.0f);
  standalone.SetRequestedY(70.0f);
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

int UtcDaliAbsoluteLayoutDirectionLtrP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  application.GetScene().Add(layout);

  View red = View::New();
  red.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10.0f, 20.0f, 50.0f, 40.0f)));
  layout.Add(red);

  View blue = View::New();
  blue.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(100.0f, 60.0f, 30.0f, 30.0f)));
  layout.Add(blue);

  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  DALI_TEST_EQUALS(red.GetPositionX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(red.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(red.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(red.GetSize().height, 40.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(blue.GetPositionX(), 100.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blue.GetPositionY(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blue.GetSize().width, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blue.GetSize().height, 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutDirectionRtlP(void)
{
  UiTestApplication application;
  AbsoluteLayout layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);
  layout.SetLayoutDirection(LayoutDirection::RIGHT_TO_LEFT);
  application.GetScene().Add(layout);

  View red = View::New();
  red.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10.0f, 20.0f, 50.0f, 40.0f)));
  layout.Add(red);

  View blue = View::New();
  blue.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(100.0f, 60.0f, 30.0f, 30.0f)));
  layout.Add(blue);

  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  // newX = parentWidth(200) - oldX - childWidth. Sizes unchanged.
  DALI_TEST_EQUALS(red.GetPositionX(), 200.0f - 10.0f - 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(red.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(red.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(red.GetSize().height, 40.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(blue.GetPositionX(), 200.0f - 100.0f - 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blue.GetPositionY(), 60.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blue.GetSize().width, 30.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(blue.GetSize().height, 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutBoundsUpdateAfterArrangeP(void)
{
  // After an initial layout pass, mutating bounds on the existing
  // AbsoluteLayoutParams handle and calling InvalidateMeasure must propagate
  // through the layout root and produce a re-arrange that picks up the new
  // bounds. Regression: the early-exit guard in InvalidateMeasure used to
  // drop the second invalidation because the layout manager skipped
  // child.Measure for explicit-bounds children, leaving the child's measure
  // cache stuck in the DIRTY state.
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(150.0f);

  View child = View::New();
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(10.0f, 20.0f, 50.0f, 40.0f)));
  layout.Add(child);

  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  DALI_TEST_EQUALS(child.GetPositionX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 20.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 50.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 40.0f, TEST_LOCATION);

  // Modify a snapshot and commit it to request a remeasure.
  auto params = GetRequiredLayoutParams<AbsoluteLayoutParams>(child);
  params.SetBounds(LayoutRect(70.0f, 80.0f, 90.0f, 30.0f));
  child.SetLayoutParams(params);

  layout.Measure(200.0f, 150.0f);
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 150.0f));

  DALI_TEST_EQUALS(child.GetPositionX(), 70.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetPositionY(), 80.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().width, 90.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(child.GetSize().height, 30.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAbsoluteLayoutBoundsRepeatedUpdateP(void)
{
  // Multiple successive bounds mutations followed by InvalidateMeasure must
  // each take effect — verifies that the measure cache transitions back to a
  // clean state on every layout pass, not just the first one.
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(300.0f);
  layout.SetRequestedHeight(200.0f);

  View child = View::New();
  child.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0.0f, 0.0f, 40.0f, 40.0f)));
  layout.Add(child);

  const LayoutRect updates[] = {
    LayoutRect(15.0f, 25.0f, 60.0f, 35.0f),
    LayoutRect(120.0f, 10.0f, 80.0f, 20.0f),
    LayoutRect(0.0f, 0.0f, 100.0f, 100.0f),
  };

  for(const auto& rect : updates)
  {
    auto params = GetRequiredLayoutParams<AbsoluteLayoutParams>(child);
    params.SetBounds(rect);
    child.SetLayoutParams(params);

    layout.Measure(300.0f, 200.0f);
    layout.Arrange(LayoutRect(0.0f, 0.0f, 300.0f, 200.0f));

    DALI_TEST_EQUALS(child.GetPositionX(), rect.GetX(), TEST_LOCATION);
    DALI_TEST_EQUALS(child.GetPositionY(), rect.GetY(), TEST_LOCATION);
    DALI_TEST_EQUALS(child.GetSize().width, rect.GetWidth(), TEST_LOCATION);
    DALI_TEST_EQUALS(child.GetSize().height, rect.GetHeight(), TEST_LOCATION);
  }
  END_TEST;
}

// A position-only-proportional child on a WRAP container axis has a
// determinate extent, so it must contribute that extent to the WRAP intrinsic
// size (only its circular position offset is dropped). Buggy code excluded it
// entirely, collapsing the WRAP width to 0.
int UtcDaliAbsoluteLayoutWrapWidthPositionOnlyProportionalP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  // Width stays WRAP_CONTENT (default); height fixed so only width is exercised.
  layout.SetRequestedHeight(150.0f);
  View child = View::New();
  layout.Add(child);
  // X proportional (0.5) but width is a determinate fixed 100; height fixed 50.
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(0.5f, 20.0f, 100.0f, 50.0f))
                          .SetFlags(AbsoluteLayoutFlags::X_PROPORTIONAL));
  MeasuredSize m = layout.Measure(300.0f, 150.0f);
  // Buggy: child excluded -> width 0. Fixed: contributes w + marginW = 100.
  DALI_TEST_CHECK(m.GetWidth() >= 100.0f);
  END_TEST;
}

// Guard: a width-proportional child stays excluded on a WRAP width axis
// (genuinely circular), so the WRAP width remains 0. Passes before and after.
int UtcDaliAbsoluteLayoutWrapWidthSizeProportionalExcludedP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  layout.SetRequestedHeight(150.0f); // width WRAP_CONTENT
  View child = View::New();
  layout.Add(child);
  // Width is proportional (0.4): circular on a WRAP axis -> excluded.
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(10.0f, 20.0f, 0.4f, 50.0f))
                          .SetFlags(AbsoluteLayoutFlags::WIDTH_PROPORTIONAL));
  MeasuredSize m = layout.Measure(300.0f, 150.0f);
  DALI_TEST_EQUALS(m.GetWidth(), 0.0f, TEST_LOCATION);
  END_TEST;
}

// Symmetric height: position-only Y-proportional child on a WRAP height
// axis must contribute its determinate height. Buggy: collapses to 0.
int UtcDaliAbsoluteLayoutWrapHeightPositionOnlyProportionalP(void)
{
  UiTestApplication application;
  AbsoluteLayout    layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(200.0f); // height WRAP_CONTENT
  View child = View::New();
  layout.Add(child);
  child.SetLayoutParams(AbsoluteLayoutParams::New()
                          .SetBounds(LayoutRect(10.0f, 0.5f, 80.0f, 60.0f))
                          .SetFlags(AbsoluteLayoutFlags::Y_PROPORTIONAL));
  MeasuredSize m = layout.Measure(200.0f, 300.0f);
  DALI_TEST_CHECK(m.GetHeight() >= 60.0f);
  END_TEST;
}

// A child whose Measure() removes a sibling must not corrupt the
// AbsoluteLayoutManager::Measure pass. The manager snapshots its children up
// front, so the mid-loop Erase inside OnChildRemove cannot make GetChildAt
// return an empty handle (which would DALI_ASSERT_ALWAYS in GetImpl). Without
// the snapshot this aborts.
int UtcDaliAbsoluteLayoutReentrantChildRemoveDuringMeasureP(void)
{
  UiTestApplication application;

  AbsoluteLayout layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  application.GetScene().Add(layout);
  gReentrantParent = layout;

  View first = View::New();
  first.SetRequestedWidth(10.0f);
  first.SetRequestedHeight(10.0f);
  first.SetMeasureCallback(MeasureCallback::New(&ReentrantRemoveMeasure));
  layout.Add(first);

  gSiblingToRemove = View::New();
  gSiblingToRemove.SetRequestedWidth(10.0f);
  gSiblingToRemove.SetRequestedHeight(10.0f);
  gSiblingToRemove.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  layout.Add(gSiblingToRemove);

  View third = View::New();
  third.SetRequestedWidth(10.0f);
  third.SetRequestedHeight(10.0f);
  third.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  layout.Add(third);

  DALI_TEST_EQUALS(layout.GetChildCount(), 3u, TEST_LOCATION);

  // First child's Measure removes the sibling mid-loop. Must complete cleanly.
  layout.Measure(200.0f, 100.0f);

  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);

  gReentrantParent.Reset();
  gSiblingToRemove.Reset();
  END_TEST;
}

// A child whose Arrange() removes a sibling must not corrupt the
// AbsoluteLayoutManager::Arrange pass. Same snapshot guarantee as the Measure
// variant; without the snapshot this aborts.
int UtcDaliAbsoluteLayoutReentrantChildRemoveDuringArrangeP(void)
{
  UiTestApplication application;

  AbsoluteLayout layout = AbsoluteLayout::New();
  layout.SetRequestedWidth(200.0f);
  layout.SetRequestedHeight(100.0f);
  application.GetScene().Add(layout);
  gReentrantParent = layout;

  View first = View::New();
  first.SetRequestedWidth(10.0f);
  first.SetRequestedHeight(10.0f);
  first.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  first.SetArrangeCallback(ArrangeCallback::New(&ReentrantRemoveArrange));
  layout.Add(first);

  gSiblingToRemove = View::New();
  gSiblingToRemove.SetRequestedWidth(10.0f);
  gSiblingToRemove.SetRequestedHeight(10.0f);
  gSiblingToRemove.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  gSiblingToRemove.SetArrangeCallback(ArrangeCallback::New(&PlainArrange));
  layout.Add(gSiblingToRemove);

  View third = View::New();
  third.SetRequestedWidth(10.0f);
  third.SetRequestedHeight(10.0f);
  third.SetMeasureCallback(MeasureCallback::New(&PlainMeasure));
  third.SetArrangeCallback(ArrangeCallback::New(&PlainArrange));
  layout.Add(third);

  DALI_TEST_EQUALS(layout.GetChildCount(), 3u, TEST_LOCATION);

  layout.Measure(200.0f, 100.0f);
  // First child's Arrange removes the sibling mid-loop. Must complete cleanly.
  layout.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(layout.GetChildCount(), 2u, TEST_LOCATION);

  gReentrantParent.Reset();
  gSiblingToRemove.Reset();
  END_TEST;
}
