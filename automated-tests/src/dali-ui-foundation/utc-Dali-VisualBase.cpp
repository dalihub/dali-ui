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

#include <dali-ui-foundation/public-api/view.h>
#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/border-visual.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

// Implement of VisualBase
#include <dali-ui-foundation/devel-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/devel-api/visuals/visual-base-impl.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_visual_base_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_visual_base_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVisualBaseCreateAndOwner(void)
{
  UiTestApplication application;

  // Create a VisualBase instance.
  VisualBase visual = ColorVisual::New();

  // Initially, the visual is not attached to any view.
  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::INVALID, TEST_LOCATION);

  View view = View::New();
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), true, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT, 0u), visual, TEST_LOCATION);

  visual.Detach();

  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::INVALID, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT), true, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT, 0u), visual, TEST_LOCATION);

  // Already added visual. Return false.
  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT), false, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT, 0u), visual, TEST_LOCATION);

  // Change anther view. Owner of visual be changed.
  View anotherView = View::New();
  DALI_TEST_EQUALS(anotherView.AddVisual(visual, Visual::ContainerRangeType::UNDER_BACKGROUND_EFFECT), true, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), anotherView, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::UNDER_BACKGROUND_EFFECT, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::ContainerRangeType::OVER_FOREGROUND_EFFECT), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(anotherView.GetVisualCount(Visual::ContainerRangeType::UNDER_BACKGROUND_EFFECT), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(anotherView.GetVisualAt(Visual::ContainerRangeType::UNDER_BACKGROUND_EFFECT, 0u), visual, TEST_LOCATION);

  for(int type = static_cast<int>(Visual::ContainerRangeType::UNDER_BACKGROUND_EFFECT) + 1; type < static_cast<int>(Visual::ContainerRangeType::MAX_COUNT); ++type)
  {
    auto rangeType = static_cast<Visual::ContainerRangeType>(type);
    DALI_TEST_EQUALS(anotherView.AddVisual(visual, rangeType), true, TEST_LOCATION);
    DALI_TEST_EQUALS(visual.GetContainerRangeType(), rangeType, TEST_LOCATION);
    DALI_TEST_EQUALS(anotherView.GetVisualCount(rangeType), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(anotherView.GetVisualAt(rangeType, 0u), visual, TEST_LOCATION);
  }

  // No effort if we try to remove from another view.
  view.RemoveVisual(visual);

  DALI_TEST_EQUALS(visual.GetOwner(), anotherView, TEST_LOCATION);

  anotherView.RemoveVisual(visual);

  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetContainerRangeType(), Visual::ContainerRangeType::INVALID, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetName(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is empty string
  DALI_TEST_EQUALS(visual.GetName(), "", TEST_LOCATION);

  // Set and get name.
  visual.SetName("TestVisual");
  DALI_TEST_EQUALS(visual.GetName(), "TestVisual", TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetColor(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is Color::WHITE
  DALI_TEST_EQUALS(visual.GetColor().GetRgba(), Color::WHITE, TEST_LOCATION);

  // Set and get color.
  UiColor color(0.1f, 0.2f, 0.3f, 0.4f);
  visual.SetColor(color);
  DALI_TEST_EQUALS(visual.GetColor().GetRgba(), color.GetRgba(), TEST_LOCATION);

  // Set and get color by id.
  UiColor color2("TestId");
  visual.SetColor(color2);
  DALI_TEST_EQUALS(visual.GetColor().GetColorId(), color2.GetColorId(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetOffset(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is 0.0f
  DALI_TEST_EQUALS(visual.GetOffsetX(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 0.0f, TEST_LOCATION);

  visual.SetOffsetX(10.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 0.0f, TEST_LOCATION);

  visual.SetOffsetY(20.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 20.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetSize(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is 1.0f
  DALI_TEST_EQUALS(visual.GetWidth(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetHeight(), 1.0f, TEST_LOCATION);

  visual.SetWidth(10.0f);
  DALI_TEST_EQUALS(visual.GetWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetHeight(), 1.0f, TEST_LOCATION);

  visual.SetHeight(20.0f);
  DALI_TEST_EQUALS(visual.GetWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetHeight(), 20.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetProportionFlags(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is ALL
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::ALL, TEST_LOCATION);

  visual.SetProportionFlags(Visual::Transform::ProportionFlags::X_PROPORTIONAL | Visual::Transform::ProportionFlags::WIDTH_PROPORTIONAL);
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::X_PROPORTIONAL | Visual::Transform::ProportionFlags::WIDTH_PROPORTIONAL, TEST_LOCATION);

  visual.SetProportionFlags(Visual::Transform::ProportionFlags::Y_PROPORTIONAL | Visual::Transform::ProportionFlags::HEIGHT_PROPORTIONAL);
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::Y_PROPORTIONAL | Visual::Transform::ProportionFlags::HEIGHT_PROPORTIONAL, TEST_LOCATION);

  visual.SetProportionFlags(Visual::Transform::ProportionFlags::OFFSET_PROPORTIONAL);
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::OFFSET_PROPORTIONAL, TEST_LOCATION);

  visual.SetProportionFlags(Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL);
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::SIZE_PROPORTIONAL, TEST_LOCATION);

  visual.SetProportionFlags(Visual::Transform::ProportionFlags::X_PROPORTIONAL);
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::X_PROPORTIONAL, TEST_LOCATION);

  visual.SetProportionFlags(Visual::Transform::ProportionFlags::NONE);
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::NONE, TEST_LOCATION);

  visual.SetProportionFlags(Visual::Transform::ProportionFlags::ALL);
  DALI_TEST_EQUALS(visual.GetProportionFlags(), Visual::Transform::ProportionFlags::ALL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetExtraSize(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is 0.0f
  DALI_TEST_EQUALS(visual.GetExtraWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetExtraHeight(), 0.0f, TEST_LOCATION);

  visual.SetExtraWidth(10.0f);
  DALI_TEST_EQUALS(visual.GetExtraWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetExtraHeight(), 0.0f, TEST_LOCATION);

  visual.SetExtraHeight(20.0f);
  DALI_TEST_EQUALS(visual.GetExtraWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetExtraHeight(), 20.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetOriginPivot(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is Align::TOP_BEGIN
  DALI_TEST_EQUALS(visual.GetOrigin(), Align::TOP_BEGIN, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetPivot(), Align::TOP_BEGIN, TEST_LOCATION);

  visual.SetOrigin(Align::BOTTOM_END);
  DALI_TEST_EQUALS(visual.GetOrigin(), Align::BOTTOM_END, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetPivot(), Align::TOP_BEGIN, TEST_LOCATION);

  visual.SetPivot(Align::CENTER);
  DALI_TEST_EQUALS(visual.GetOrigin(), Align::BOTTOM_END, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetPivot(), Align::CENTER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSetGetCornerRadius(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is Vector4::ZERO and CornerRadiusPolicy::ABSOLUTE
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::ABSOLUTE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), false, TEST_LOCATION);

  visual.SetCornerRadius(Vector4(1.0f, 2.0f, 3.0f, 4.0f));
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4(1.0f, 2.0f, 3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::ABSOLUTE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), false, TEST_LOCATION);

  visual.SetCornerRadius(10.0f, 20.0f, 30.0f, 40.0f);
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4(10.0f, 20.0f, 30.0f, 40.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::ABSOLUTE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), false, TEST_LOCATION);

  visual.SetCornerRadiusPolicyRelative();
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4(10.0f, 20.0f, 30.0f, 40.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), true, TEST_LOCATION);

  visual.SetCornerRadius(0.6f);
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4(0.6f, 0.6f, 0.6f, 0.6f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), true, TEST_LOCATION);

  visual.SetCornerSquareness(Vector4(0.4f, 0.3f, 0.2f, 0.1f));
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4(0.6f, 0.6f, 0.6f, 0.6f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4(0.4f, 0.3f, 0.2f, 0.1f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), true, TEST_LOCATION);;

  visual.SetCornerSquareness(0.1f, 0.2f, 0.3f, 0.4f);
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4(0.6f, 0.6f, 0.6f, 0.6f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4(0.1f, 0.2f, 0.3f, 0.4f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), true, TEST_LOCATION);;

  visual.SetCornerSquareness(0.2f);
  DALI_TEST_EQUALS(visual.GetCornerRadius(), Vector4(0.6f, 0.6f, 0.6f, 0.6f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerSquareness(), Vector4(0.2f, 0.2f, 0.2f, 0.2f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetCornerRadiusPolicy(), CornerRadiusPolicy::RELATIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.IsCornerRadiusPolicyRelative(), true, TEST_LOCATION);;

  END_TEST;
}

int UtcDaliVisualBaseSetGetBorderline(void)
{
  UiTestApplication application;

  VisualBase visual = ColorVisual::New();

  // Default value is 0.0f and Color::BLACK
  DALI_TEST_EQUALS(visual.GetBorderlineWidth(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineColor().GetRgba(), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineOffset(), 0.0f, TEST_LOCATION);

  visual.SetBorderlineWidth(10.0f);
  DALI_TEST_EQUALS(visual.GetBorderlineWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineColor().GetRgba(), Color::BLACK, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineOffset(), 0.0f, TEST_LOCATION);

  visual.SetBorderlineColor(UiColor(Color::RED));
  DALI_TEST_EQUALS(visual.GetBorderlineWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineOffset(), 0.0f, TEST_LOCATION);

  visual.SetBorderlineOffset(-1.0f);
  DALI_TEST_EQUALS(visual.GetBorderlineWidth(), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetBorderlineOffset(), -1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVisualBaseSiblingOrder(void)
{
  UiTestApplication application;

  // Create a view and four visuals.
  View       view    = View::New();
  VisualBase visual1 = ColorVisual::New();
  VisualBase visual2 = ColorVisual::New();
  VisualBase visual3 = ColorVisual::New();
  VisualBase visual4 = ColorVisual::New();

  auto type = Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT;

  // Add visuals to the view.
  view.AddVisual(visual1, type);
  view.AddVisual(visual2, type);
  view.AddVisual(visual3, type);
  view.AddVisual(visual4, type);

  auto TestSiblingOder = [&](int v1, int v2, int v3, int v4)
  {
    DALI_TEST_EQUALS(visual1.GetSiblingOrder(), v1, TEST_LOCATION);
    DALI_TEST_EQUALS(visual2.GetSiblingOrder(), v2, TEST_LOCATION);
    DALI_TEST_EQUALS(visual3.GetSiblingOrder(), v3, TEST_LOCATION);
    DALI_TEST_EQUALS(visual4.GetSiblingOrder(), v4, TEST_LOCATION);

    DALI_TEST_EQUALS(view.GetVisualAt(type, v1), visual1, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetVisualAt(type, v2), visual2, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetVisualAt(type, v3), visual3, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetVisualAt(type, v4), visual4, TEST_LOCATION);
  };

  // Initial sibling order: visual1 -> visual2 -> visual3 -> visual4
  TestSiblingOder(0, 1, 2, 3);

  // Change sibling order using RaiseToTop.
  visual1.RaiseToTop();
  TestSiblingOder(3, 0, 1, 2);

  // Directly set sibling order.
  visual2.SetSiblingOrder(2);
  TestSiblingOder(3, 2, 0, 1);

  // Throw exception if sibling order set over the container size.
  try
  {
    visual1.SetSiblingOrder(4);
    tet_result(TET_FAIL);
  }
  catch(DaliException& e)
  {
    tet_result(TET_PASS);
  }

  // LowerToBottom
  visual4.LowerToBottom();
  TestSiblingOder(3, 2, 1, 0);

  // Raise one step.
  visual3.Raise();
  TestSiblingOder(3, 1, 2, 0);

  // Lower one step.
  visual2.Lower();
  TestSiblingOder(3, 0, 2, 1);

  // RaiseAbove
  visual2.RaiseAbove(visual3);
  TestSiblingOder(3, 2, 1, 0);

  // LowerBelow
  visual1.LowerBelow(visual3);
  TestSiblingOder(1, 3, 2, 0);

  // No efforts cases
  visual2.RaiseToTop();
  TestSiblingOder(1, 3, 2, 0);
  visual2.Raise();
  TestSiblingOder(1, 3, 2, 0);
  visual4.LowerToBottom();
  TestSiblingOder(1, 3, 2, 0);
  visual4.Lower();
  TestSiblingOder(1, 3, 2, 0);

  visual2.RaiseAbove(visual1);
  TestSiblingOder(1, 3, 2, 0);
  visual1.LowerBelow(visual2);
  TestSiblingOder(1, 3, 2, 0);

  END_TEST;
}
int UtcDaliVisualBaseRecreateBorderVisual01(void)
{
  UiTestApplication application;

  tet_infoline("Test that visual update without newly create Visual::Base for BorderVisual\n");

  View       view    = View::New();
  VisualBase visual = BorderVisual::New();

  view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  auto TestVisualBaseChanged = [&](std::function<void(VisualBase)> func, bool expectChanged = false){
    // Hold original visual base
    Visual::Base originalVisualBase = GetImplementation(visual).GetVisual();

    static int testCount = 0;
    tet_printf("TestCase #%d\n", ++testCount);

    func(visual);

    // Do not change visual if processor is not executed.
    DALI_TEST_EQUALS(originalVisualBase, GetImplementation(visual).GetVisual(), TEST_LOCATION);

    application.SendNotification();
    application.Render();

    bool changed = (originalVisualBase != GetImplementation(visual).GetVisual());
    DALI_TEST_EQUALS(changed, expectChanged, TEST_LOCATION);
  };

  // Change the basic info didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetName("Hello");}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetColor(UiColor("Primary"));}, false);

  // Change the transform didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetX(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetY(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetHeight(0.4f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraWidth(0.5f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraHeight(0.6f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOrigin(Align::CENTER);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetPivot(Align::BOTTOM_END);}, false);

  // For BorderVisual.
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(BorderVisual::Property::BORDER_SIZE, 0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(BorderVisual::Property::ANTI_ALIASING, true);}, false);

  END_TEST;
}

int UtcDaliVisualBaseRecreateColorVisual01(void)
{
  UiTestApplication application;

  tet_infoline("Test that visual update without newly create Visual::Base for ColorVisual\n");

  View       view    = View::New();
  VisualBase visual = ColorVisual::New();

  view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  auto TestVisualBaseChanged = [&](std::function<void(VisualBase)> func, bool expectChanged = false){
    // Hold original visual base
    Visual::Base originalVisualBase = GetImplementation(visual).GetVisual();

    static int testCount = 0;
    tet_printf("TestCase #%d\n", ++testCount);

    func(visual);

    // Do not change visual if processor is not executed.
    DALI_TEST_EQUALS(originalVisualBase, GetImplementation(visual).GetVisual(), TEST_LOCATION);

    application.SendNotification();
    application.Render();

    bool changed = (originalVisualBase != GetImplementation(visual).GetVisual());
    DALI_TEST_EQUALS(changed, expectChanged, TEST_LOCATION);
  };

  // Change the basic info didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetName("Hello");}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetColor(UiColor("Primary"));}, false);

  // Change the transform didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetX(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetY(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetHeight(0.4f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraWidth(0.5f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraHeight(0.6f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOrigin(Align::CENTER);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetPivot(Align::BOTTOM_END);}, false);

  // Change decoration didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerRadius(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerSquareness(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerRadiusPolicyRelative();}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineColor(UiColor("Secondary"));}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineOffset(0.4f);}, false);

  // For ColorVisual.
  // Change BlurRadius didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){
    visual.SetProperty(ColorVisual::Property::BLUR_RADIUS, 0.1f);
  }, false);

  // Change CutoutPolicy change visual base!
  TestVisualBaseChanged([](VisualBase visual){
    visual.SetProperty(ColorVisual::Property::CUTOUT_POLICY, CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
  }, true);

  END_TEST;
}

int UtcDaliVisualBaseRecreateGradientVisual01(void)
{
  UiTestApplication application;

  tet_infoline("Test that visual update without newly create Visual::Base for GradientVisual\n");

  View       view    = View::New();
  VisualBase visual = GradientVisual::New()
    .SetLinearGradient(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f))
    .SetStopNodes({
      {0.0f, UiColor("#000000")},
      {1.0f, UiColor("#FFFFFF")},
    });

  view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  auto TestVisualBaseChanged = [&](std::function<void(VisualBase)> func, bool expectChanged = false){
    // Hold original visual base
    Visual::Base originalVisualBase = GetImplementation(visual).GetVisual();

    static int testCount = 0;
    tet_printf("TestCase #%d\n", ++testCount);

    func(visual);

    // Do not change visual if processor is not executed.
    DALI_TEST_EQUALS(originalVisualBase, GetImplementation(visual).GetVisual(), TEST_LOCATION);

    application.SendNotification();
    application.Render();

    bool changed = (originalVisualBase != GetImplementation(visual).GetVisual());
    DALI_TEST_EQUALS(changed, expectChanged, TEST_LOCATION);
  };

  // Change the basic info didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetName("Hello");}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetColor(UiColor("Primary"));}, false);

  // Change the transform didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetX(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetY(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetHeight(0.4f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraWidth(0.5f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraHeight(0.6f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOrigin(Align::CENTER);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetPivot(Align::BOTTOM_END);}, false);

  // Change decoration didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerRadius(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerSquareness(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerRadiusPolicyRelative();}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineColor(UiColor("Secondary"));}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineOffset(0.4f);}, false);

  // For GradientVisual.
  // Change MutableProperty didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){ visual.SetProperty(Ui::GradientVisual::Property::START_OFFSET, 0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetStopNodes({
      {0.0f, UiColor("#FF0000")},
      {0.5f, UiColor("#00FF00")},
      {1.0f, UiColor("#0000FF")},
    });
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetUnits(Ui::Gradient::Units::USER_SPACE);
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetSpreadMethod(Ui::Gradient::SpreadMethod::REFLECT);
  }, false);

  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetConicGradient(Vector2::ZERO, Dali::Radian(2.0f));
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetLinearGradient(Vector2::ZERO, Vector2::ONE);
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetRadialGradient(Vector2::ZERO, 5.0f);
  }, false);

  END_TEST;
}

int UtcDaliVisualBaseRecreateGradientVisual02(void)
{
  UiTestApplication application;

  tet_infoline("Test that visual update without newly create Visual::Base for GradientVisual, with in-completed property case\n");

  View       view    = View::New();
  VisualBase visual = GradientVisual::New();

  view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  auto TestVisualBaseChanged = [&](std::function<void(VisualBase)> func, bool expectChanged = false){
    // Hold original visual base
    Visual::Base originalVisualBase = GetImplementation(visual).GetVisual();

    static int testCount = 0;
    tet_printf("TestCase #%d\n", ++testCount);

    func(visual);

    // Do not change visual if processor is not executed.
    DALI_TEST_EQUALS(originalVisualBase, GetImplementation(visual).GetVisual(), TEST_LOCATION);

    application.SendNotification();
    application.Render();

    bool changed = (originalVisualBase != GetImplementation(visual).GetVisual());
    DALI_TEST_EQUALS(changed, expectChanged, TEST_LOCATION);
  };

  // Change the basic info didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetName("Hello");}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetColor(UiColor("Primary"));}, false);

  // Change the transform didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetX(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetY(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetHeight(0.4f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraWidth(0.5f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraHeight(0.6f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOrigin(Align::CENTER);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetPivot(Align::BOTTOM_END);}, false);

  // Change decoration didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerRadius(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerSquareness(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetCornerRadiusPolicyRelative();}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineColor(UiColor("Secondary"));}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetBorderlineOffset(0.4f);}, false);

  // For GradientVisual.
  // Change MutableProperty didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){ visual.SetProperty(Ui::GradientVisual::Property::START_OFFSET, 0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetStopNodes({
      {0.0f, UiColor("#FF0000")},
      {0.5f, UiColor("#00FF00")},
      {1.0f, UiColor("#0000FF")},
    });
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetUnits(Ui::Gradient::Units::USER_SPACE);
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetSpreadMethod(Ui::Gradient::SpreadMethod::REFLECT);
  }, false);

  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetConicGradient(Vector2::ZERO, Dali::Radian(2.0f));
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetLinearGradient(Vector2::ZERO, Vector2::ONE);
  }, false);
  TestVisualBaseChanged([](VisualBase visual){
    GradientVisual gradientVisual = GradientVisual::DownCast(visual);
    gradientVisual.SetRadialGradient(Vector2::ZERO, 5.0f);
  }, false);

  END_TEST;
}

int UtcDaliVisualBaseRecreateTextVisual01(void)
{
  UiTestApplication application;

  tet_infoline("Test that visual update without newly create Visual::Base for TextVisual\n");

  View       view    = View::New();
  TextVisual visual  = TextVisual::New()
    .SetText("Hello")
    .SetFontSize(20.0f);

  view.AddVisual(visual, Visual::ContainerRangeType::BETWEEN_BACKGROUND_AND_CONTENT);

  application.GetScene().Add(view);

  application.SendNotification();
  application.Render();

  auto TestVisualBaseChanged = [&](std::function<void(VisualBase)> func, bool expectChanged = false){
    // Hold original visual base
    Visual::Base originalVisualBase = GetImplementation(visual).GetVisual();

    static int testCount = 0;
    tet_printf("TestCase #%d\n", ++testCount);

    func(visual);

    // Do not change visual if processor is not executed.
    DALI_TEST_EQUALS(originalVisualBase, GetImplementation(visual).GetVisual(), TEST_LOCATION);

    application.SendNotification();
    application.Render();

    bool changed = (originalVisualBase != GetImplementation(visual).GetVisual());
    DALI_TEST_EQUALS(changed, expectChanged, TEST_LOCATION);
  };

  // Change the basic info didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetName("Hello");}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetColor(UiColor("Primary"));}, false);

  // Change the transform didn't change visual base
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetX(0.1f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOffsetY(0.2f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetWidth(0.3f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetHeight(0.4f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraWidth(0.5f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetExtraHeight(0.6f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetOrigin(Align::CENTER);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetPivot(Align::BOTTOM_END);}, false);

  // For TextVisual.
  // All properties of TextVisual are mutable, so changing them should NOT recreate visual base.
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::TEXT, "World");}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::FONT_FAMILY, "Arial");}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::FONT_SIZE, 30.0f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::FONT_WEIGHT, Text::FontWeight::BOLD);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::FONT_WIDTH, Text::FontWidth::EXPANDED);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::FONT_SLANT, Text::FontSlant::ITALIC);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::MULTI_LINE, true);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::LINE_WRAP_MODE, Text::LineWrapMode::CHARACTER);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::HORIZONTAL_ALIGNMENT, Text::Alignment::CENTER);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::VERTICAL_ALIGNMENT, Text::Alignment::END);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::OVERFLOW_MODE, Text::OverflowMode::ELLIPSIS);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::LINE_HEIGHT, 1.5f);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::LINE_HEIGHT_MODE, Text::LineHeightMode::RELATIVE);}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::TEXT_COLOR, Vector4(1.0f, 0.0f, 0.0f, 1.0f));}, false);
  TestVisualBaseChanged([](VisualBase visual){visual.SetProperty(TextVisual::Property::MARKUP_ENABLED, true);}, false);

  END_TEST;
}

int UtcDaliVisualBaseInvalidHandle(void)
{
  UiTestApplication application;

  // Default constructed VisualBase is an empty handle.
  VisualBase empty;

  auto TestAssertFunction = [&](std::function<void(void)> func){
    try
    {
      func();
      tet_result(TET_FAIL);
    }
    catch(DaliException& e)
    {
      tet_result(TET_PASS);
    }
  };

  TestAssertFunction([&](){empty.SetName("ShouldBeCrash");});
  TestAssertFunction([&](){empty.SetOffsetX(1.0f);});
  TestAssertFunction([&](){empty.SetOffsetY(1.0f);});
  TestAssertFunction([&](){empty.SetWidth(100.0f);});
  TestAssertFunction([&](){empty.SetHeight(100.0f);});
  TestAssertFunction([&](){empty.SetProportionFlags(Visual::Transform::ProportionFlags::ALL);});
  TestAssertFunction([&](){empty.SetExtraWidth(10.0f);});
  TestAssertFunction([&](){empty.SetExtraHeight(10.0f);});
  TestAssertFunction([&](){empty.SetExtraHeight(10.0f);});
  TestAssertFunction([&](){empty.SetOrigin(Align::CENTER_BEGIN);});
  TestAssertFunction([&](){empty.SetPivot(Align::CENTER_BEGIN);});
  TestAssertFunction([&](){empty.SetSiblingOrder(0u);});
  TestAssertFunction([&](){empty.SetProperty(Property::INVALID_INDEX, Property::Value());});

  TestAssertFunction([&](){empty.GetOwner();});
  TestAssertFunction([&](){empty.GetContainerRangeType();});
  TestAssertFunction([&](){empty.GetName();});
  TestAssertFunction([&](){empty.GetOffsetX();});
  TestAssertFunction([&](){empty.GetOffsetY();});
  TestAssertFunction([&](){empty.GetWidth();});
  TestAssertFunction([&](){empty.GetHeight();});
  TestAssertFunction([&](){empty.GetProportionFlags();});
  TestAssertFunction([&](){empty.GetExtraWidth();});
  TestAssertFunction([&](){empty.GetExtraHeight();});
  TestAssertFunction([&](){empty.GetOrigin();});
  TestAssertFunction([&](){empty.GetPivot();});
  TestAssertFunction([&](){empty.GetSiblingOrder();});
  TestAssertFunction([&](){empty.GetProperty(Property::INVALID_INDEX);});

  END_TEST;
}