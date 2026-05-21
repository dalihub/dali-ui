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
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_gradient_visual_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gradient_visual_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGradientVisualCreateAndOwner(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

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

int UtcDaliGradientVisualGetVisualType(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  // Upcast the handle
  VisualBase visualBase = visual;

  DALI_TEST_EQUALS(visualBase.GetVisualType(), Visual::GRADIENT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualDownCast(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  // Upcast the handle
  BaseHandle baseHandle = visual;

  // Downcast check
  DALI_TEST_CHECK(VisualBase::DownCast(baseHandle));
  DALI_TEST_CHECK(GradientVisual::DownCast(baseHandle));

  // Do not convert to other type of visual
  DALI_TEST_CHECK(!AnimatedImageVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!ColorVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!ImageVisual::DownCast(baseHandle));
  DALI_TEST_CHECK(!LottieAnimationVisual::DownCast(baseHandle));

  END_TEST;
}

int UtcDaliGradientVisualSetLinearGradient(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Vector2 startPosition(-0.5f, -0.5f);
  Vector2 endPosition(0.5f, 0.5f);
  visual.SetLinearGradient(startPosition, endPosition);
  DALI_TEST_EQUALS(visual.GetStartPosition(), startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetEndPosition(), endPosition, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetRadialGradient(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Vector2 center(0.0f, 0.0f);
  float radius(0.5f);
  visual.SetRadialGradient(center, radius);
  DALI_TEST_EQUALS(visual.GetCenter(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetRadius(), radius, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetConicGradient(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Vector2 center(0.0f, 0.0f);
  Dali::Radian startAngle(Math::PI_2);
  visual.SetConicGradient(center, startAngle);
  DALI_TEST_EQUALS(visual.GetCenter(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetStartAngle(), startAngle, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualStartOffset(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  visual.SetStartOffset(0.25f);
  DALI_TEST_EQUALS(visual.GetStartOffset(), 0.25f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualStopNodes(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Dali::Vector<Ui::Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Ui::Gradient::StopNode(0.0f, UiColor(1.0f, 0.0f, 0.0f, 1.0f)));
  stopNodes.PushBack(Ui::Gradient::StopNode(0.5f, UiColor(0.0f, 1.0f, 0.0f, 1.0f)));
  stopNodes.PushBack(Ui::Gradient::StopNode(1.0f, UiColor(0.0f, 0.0f, 1.0f, 1.0f)));

  visual.SetStopNodes(stopNodes);

  Dali::Vector<Ui::Gradient::StopNode> retrievedNodes = visual.GetStopNodes();
  DALI_TEST_EQUALS(retrievedNodes.Size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[0].GetOffset(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[1].GetOffset(), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[2].GetOffset(), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualStopNodesInitializerList(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  visual.SetStopNodes({
    {0.0f, UiColor(1.0f, 0.0f, 0.0f, 1.0f)},
    {1.0f, UiColor(0.0f, 0.0f, 1.0f, 1.0f)},
  });

  Dali::Vector<Ui::Gradient::StopNode> retrievedNodes = visual.GetStopNodes();
  DALI_TEST_EQUALS(retrievedNodes.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[0].GetOffset(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[1].GetOffset(), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualUnits(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  visual.SetUnits(Ui::Gradient::Units::USER_SPACE);
  DALI_TEST_EQUALS(visual.GetUnits(), Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);

  visual.SetUnits(Ui::Gradient::Units::OBJECT_BOUNDING_BOX);
  DALI_TEST_EQUALS(visual.GetUnits(), Ui::Gradient::Units::OBJECT_BOUNDING_BOX, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSpreadMethod(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  visual.SetSpreadMethod(Ui::Gradient::SpreadMethod::REFLECT);
  DALI_TEST_EQUALS(visual.GetSpreadMethod(), Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);

  visual.SetSpreadMethod(Ui::Gradient::SpreadMethod::REPEAT);
  DALI_TEST_EQUALS(visual.GetSpreadMethod(), Ui::Gradient::SpreadMethod::REPEAT, TEST_LOCATION);

  visual.SetSpreadMethod(Ui::Gradient::SpreadMethod::PAD);
  DALI_TEST_EQUALS(visual.GetSpreadMethod(), Ui::Gradient::SpreadMethod::PAD, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetGetPropertyValue(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  // Test StartOffset
  visual.SetStartOffset(0.3f);
  DALI_TEST_EQUALS(visual.GetStartOffset(), 0.3f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(GradientVisual::Property::START_OFFSET), 0.3f, TEST_LOCATION);

  visual.SetProperty(GradientVisual::Property::START_OFFSET, 0.7f);
  DALI_TEST_EQUALS(visual.GetStartOffset(), 0.7f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(GradientVisual::Property::START_OFFSET), 0.7f, TEST_LOCATION);

  // Test RadialGradient
  visual.SetRadialGradient(Vector2(0.5f, 0.3f), 0.4f);
  DALI_TEST_EQUALS(visual.GetCenter(), Vector2(0.5f, 0.3f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetRadius(), 0.4f, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Vector2>(GradientVisual::Property::CENTER), Vector2(0.5f, 0.3f), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<float>(GradientVisual::Property::RADIUS), 0.4f, TEST_LOCATION);

  // Test SpreadMethod
  visual.SetSpreadMethod(Ui::Gradient::SpreadMethod::REFLECT);
  DALI_TEST_EQUALS(visual.GetSpreadMethod(), Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Ui::Gradient::SpreadMethod>(GradientVisual::Property::SPREAD_METHOD), Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);

  // Test Units
  visual.SetUnits(Ui::Gradient::Units::USER_SPACE);
  DALI_TEST_EQUALS(visual.GetUnits(), Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetProperty<Ui::Gradient::Units>(GradientVisual::Property::UNITS), Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualInheritedSetters(void)
{
  UiTestApplication application;

  // Test that inherited setters from VisualBase work.
  GradientVisual visual = GradientVisual::New();

  visual.SetName("GradientVisual");
  DALI_TEST_EQUALS(visual.GetName(), "GradientVisual", TEST_LOCATION);

  visual.SetColor(UiColor(0.1f, 0.2f, 0.3f, 0.4f));
  DALI_TEST_EQUALS(visual.GetColor().GetRgba(), UiColor(0.1f, 0.2f, 0.3f, 0.4f).GetRgba(), TEST_LOCATION);

  visual.SetOffsetX(12.0f);
  DALI_TEST_EQUALS(visual.GetOffsetX(), 12.0f, TEST_LOCATION);

  visual.SetOffsetY(34.0f);
  DALI_TEST_EQUALS(visual.GetOffsetY(), 34.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualInvalidHandle(void)
{
  UiTestApplication application;

  // Empty GradientVisual handle.
  GradientVisual empty;

  auto TestAssertFunction = [&](std::function<void(void)> func) {
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
  TestAssertFunction([&]() { empty.SetName("ShouldBeCrash"); });
  TestAssertFunction([&]() { empty.SetOffsetX(1.0f); });
  TestAssertFunction([&]() { empty.SetOffsetY(1.0f); });
  TestAssertFunction([&]() { empty.SetWidth(100.0f); });
  TestAssertFunction([&]() { empty.SetHeight(100.0f); });
  TestAssertFunction([&]() { empty.SetProportionFlags(Visual::Transform::ProportionFlags::ALL); });
  TestAssertFunction([&]() { empty.SetExtraWidth(10.0f); });
  TestAssertFunction([&]() { empty.SetExtraHeight(10.0f); });
  TestAssertFunction([&]() { empty.SetOrigin(Align::CENTER_BEGIN); });
  TestAssertFunction([&]() { empty.SetPivot(Align::CENTER_BEGIN); });
  TestAssertFunction([&]() { empty.SetSiblingOrder(0u); });
  TestAssertFunction([&]() { empty.SetProperty(Property::INVALID_INDEX, Property::Value()); });

  TestAssertFunction([&]() { empty.GetOwner(); });
  TestAssertFunction([&]() { empty.GetContainerRangeType(); });
  TestAssertFunction([&]() { empty.GetName(); });
  TestAssertFunction([&]() { empty.GetOffsetX(); });
  TestAssertFunction([&]() { empty.GetOffsetY(); });
  TestAssertFunction([&]() { empty.GetWidth(); });
  TestAssertFunction([&]() { empty.GetHeight(); });
  TestAssertFunction([&]() { empty.GetProportionFlags(); });
  TestAssertFunction([&]() { empty.GetExtraWidth(); });
  TestAssertFunction([&]() { empty.GetExtraHeight(); });
  TestAssertFunction([&]() { empty.GetOrigin(); });
  TestAssertFunction([&]() { empty.GetPivot(); });
  TestAssertFunction([&]() { empty.GetSiblingOrder(); });
  TestAssertFunction([&]() { empty.GetProperty(Property::INVALID_INDEX); });

  // GradientVisual specific
  TestAssertFunction([&]() { empty.SetStartOffset(0.0f); });
  TestAssertFunction([&]() { empty.SetLinearGradient(Vector2::ZERO, Vector2::ZERO); });
  TestAssertFunction([&]() { empty.SetRadialGradient(Vector2::ZERO, 1.0f); });
  TestAssertFunction([&]() { empty.SetConicGradient(Vector2::ZERO, Dali::Radian(1.0f)); });
  TestAssertFunction([&]() { empty.SetStopNodes({{0.0f, UiColor()}, {1.0f, UiColor()}}); });
  TestAssertFunction([&]() { empty.SetUnits(Ui::Gradient::Units::OBJECT_BOUNDING_BOX); });
  TestAssertFunction([&]() { empty.SetSpreadMethod(Ui::Gradient::SpreadMethod::PAD); });

  TestAssertFunction([&]() { empty.GetStartOffset(); });
  TestAssertFunction([&]() { empty.GetStartPosition(); });
  TestAssertFunction([&]() { empty.GetEndPosition(); });
  TestAssertFunction([&]() { empty.GetCenter(); });
  TestAssertFunction([&]() { empty.GetRadius(); });
  TestAssertFunction([&]() { empty.GetStartAngle(); });
  TestAssertFunction([&]() { empty.GetStopNodes(); });
  TestAssertFunction([&]() { empty.GetUnits(); });
  TestAssertFunction([&]() { empty.GetSpreadMethod(); });

  END_TEST;
}
