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
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_color_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_color_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliColorVisualCreateAndOwner(void)
{
  UiTestApplication application;

  ColorVisual visual = ColorVisual::New();

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

  END_TEST;
}

int UtcDaliColorVisualGetVisualType(void)
{
  UiTestApplication application;

  ColorVisual visual = ColorVisual::New();

  // Upcast the handle
  VisualBase visualBase = visual;

  DALI_TEST_EQUALS(visualBase.GetVisualType(), Visual::COLOR, TEST_LOCATION);

  END_TEST;
}

int UtcDaliColorVisualBlurRadius(void)
{
  UiTestApplication application;

  ColorVisual visual = ColorVisual::New();

  visual.SetBlurRadius(5.5f);
  DALI_TEST_EQUALS(visual.GetBlurRadius(), 5.5f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliColorVisualCutoutPolicy(void)
{
  UiTestApplication application;

  ColorVisual visual = ColorVisual::New();

  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS);
  DALI_TEST_EQUALS(visual.GetCutoutPolicy(), CutoutPolicy::CUTOUT_OUTSIDE_WITH_CORNER_RADIUS, TEST_LOCATION);

  END_TEST;
}
int UtcDaliColorVisualSetGetPropertyValue(void)
{
  UiTestApplication application;

  ColorVisual visual = ColorVisual::New();

  visual.SetBlurRadius(5.5f);
  DALI_TEST_EQUALS(visual.GetBlurRadius(), 5.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(ColorVisual::Property::BLUR_RADIUS), 5.5f, TEST_LOCATION);

  visual.SetProperty(ColorVisual::Property::BLUR_RADIUS, 3.3f);
  DALI_TEST_EQUALS(visual.GetBlurRadius(), 3.3f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(ColorVisual::Property::BLUR_RADIUS), 3.3f, TEST_LOCATION);


  visual.SetCutoutPolicy(CutoutPolicy::CUTOUT_OUTSIDE);
  DALI_TEST_EQUALS(visual.GetCutoutPolicy(), CutoutPolicy::CUTOUT_OUTSIDE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<CutoutPolicy>(ColorVisual::Property::CUTOUT_POLICY), CutoutPolicy::CUTOUT_OUTSIDE, TEST_LOCATION);

  visual.SetProperty(ColorVisual::Property::CUTOUT_POLICY, CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS);
  DALI_TEST_EQUALS(visual.GetCutoutPolicy(), CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<CutoutPolicy>(ColorVisual::Property::CUTOUT_POLICY), CutoutPolicy::CUTOUT_VIEW_WITH_CORNER_RADIUS, TEST_LOCATION);

  END_TEST;
}

int UtcDaliColorVisualInheritedSetters(void)
{
  UiTestApplication application;

  // Test that inherited setters from VisualBase work.
  ColorVisual visual = ColorVisual::New();

  visual.SetName("ColorVisual");
  DALI_TEST_EQUALS(visual.GetName(), "ColorVisual", TEST_LOCATION);

  visual.SetColor(UiColor(0.1f, 0.2f, 0.3f, 0.4f));
  DALI_TEST_EQUALS(visual.GetColor().GetRgba(), UiColor(0.1f, 0.2f, 0.3f, 0.4f).GetRgba(), TEST_LOCATION);

  visual.SetOffsetX(12.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 12.0f, TEST_LOCATION);

  visual.SetOffsetY(34.0f);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 34.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliColorVisualInvalidHandle(void)
{
  UiTestApplication application;

  // Empty ColorVisual handle.
  ColorVisual empty;

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

  // Inherit
  TestAssertFunction([&](){empty.SetName("ShouldBeCrash");});
  TestAssertFunction([&](){empty.SetOffsetX(1.0f);});
  TestAssertFunction([&](){empty.SetOffsetY(1.0f);});
  TestAssertFunction([&](){empty.SetWidth(100.0f);});
  TestAssertFunction([&](){empty.SetHeight(100.0f);});
  TestAssertFunction([&](){empty.SetProportionFlags(Visual::Transform::ProportionFlags::ALL);});
  TestAssertFunction([&](){empty.SetExtraWidth(10.0f);});
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

  // ColorVisual specific
  TestAssertFunction([&](){empty.SetBlurRadius(0u);});
  TestAssertFunction([&](){empty.SetCutoutPolicy(CutoutPolicy::CUTOUT_VIEW);});

  TestAssertFunction([&](){empty.GetBlurRadius();});
  TestAssertFunction([&](){empty.GetCutoutPolicy();});

  END_TEST;
}