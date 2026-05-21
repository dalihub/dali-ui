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

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_border_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_border_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliBorderVisualCreateAndOwner(void)
{
  UiTestApplication application;

  BorderVisual visual = BorderVisual::New();

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

int UtcDaliBorderVisualGetVisualType(void)
{
  UiTestApplication application;

  BorderVisual visual = BorderVisual::New();

  // Upcast the handle
  VisualBase visualBase = visual;

  DALI_TEST_EQUALS(visualBase.GetVisualType(), Ui::VisualType::BORDER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliBorderVisualDownCast(void)
{
  UiTestApplication application;

  BorderVisual visual = BorderVisual::New();

  // Upcast the handle
  BaseHandle baseHandle = visual;

  // Downcast check
  DALI_TEST_CHECK(VisualBase::DownCast(baseHandle));
  DALI_TEST_CHECK(BorderVisual::DownCast(baseHandle));

  // Do not convert to other type of visual
  DALI_TEST_CHECK(!AnimatedImageVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!ColorVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!GradientVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!ImageVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!LottieAnimationVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!TextVisual::DownCast(baseHandle));

  END_TEST;
}

int UtcDaliBorderVisualBorderSize(void)
{
  UiTestApplication application;

  BorderVisual visual = BorderVisual::New();

  visual.SetBorderSize(5.5f);
  DALI_TEST_EQUALS(visual.GetBorderSize(), 5.5f, TEST_LOCATION);

  // Don't be confused with borderline width.
  DALI_TEST_EQUALS(visual.GetBorderlineWidth(), 0.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliBorderVisualEnableAntiAliasing(void)
{
  UiTestApplication application;

  BorderVisual visual = BorderVisual::New();
  DALI_TEST_EQUALS(visual.IsAntiAliasingEnabled(), false, TEST_LOCATION);

  visual.SetEnableAntiAliasing(true);
  DALI_TEST_EQUALS(visual.IsAntiAliasingEnabled(), true, TEST_LOCATION);

  END_TEST;
}
int UtcDaliBorderVisualSetGetPropertyValue(void)
{
  UiTestApplication application;

  BorderVisual visual = BorderVisual::New();

  visual.SetBorderSize(5.5f);
  DALI_TEST_EQUALS(visual.GetBorderSize(), 5.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(BorderVisual::Property::BORDER_SIZE), 5.5f, TEST_LOCATION);

  visual.SetProperty(BorderVisual::Property::BORDER_SIZE, 3.3f);
  DALI_TEST_EQUALS(visual.GetBorderSize(), 3.3f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(BorderVisual::Property::BORDER_SIZE), 3.3f, TEST_LOCATION);


  visual.SetEnableAntiAliasing(true);
  DALI_TEST_EQUALS(visual.IsAntiAliasingEnabled(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(BorderVisual::Property::ANTI_ALIASING), true, TEST_LOCATION);

  visual.SetProperty(BorderVisual::Property::ANTI_ALIASING, false);
  DALI_TEST_EQUALS(visual.IsAntiAliasingEnabled(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<bool>(BorderVisual::Property::ANTI_ALIASING), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliBorderVisualInheritedSetters(void)
{
  UiTestApplication application;

  // Test that inherited setters from VisualBase work.
  BorderVisual visual = BorderVisual::New();

  visual.SetName("BorderVisual");
  DALI_TEST_EQUALS(visual.GetName(), "BorderVisual", TEST_LOCATION);

  visual.SetColor(UiColor(0.1f, 0.2f, 0.3f, 0.4f));
  DALI_TEST_EQUALS(visual.GetColor().GetRgba(), UiColor(0.1f, 0.2f, 0.3f, 0.4f).GetRgba(), TEST_LOCATION);

  visual.SetOffsetX(12.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 12.0f, TEST_LOCATION);

  visual.SetOffsetY(34.0f);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 34.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliBorderVisualInvalidHandle(void)
{
  UiTestApplication application;

  // Empty BorderVisual handle.
  BorderVisual empty;

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

  // BorderVisual specific
  TestAssertFunction([&](){empty.SetBorderSize(0.0f);});
  TestAssertFunction([&](){empty.SetEnableAntiAliasing(false);});

  TestAssertFunction([&](){empty.GetBorderSize();});
  TestAssertFunction([&](){empty.IsAntiAliasingEnabled();});

  END_TEST;
}