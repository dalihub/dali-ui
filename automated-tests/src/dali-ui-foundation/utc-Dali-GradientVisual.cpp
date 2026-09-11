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

#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali-ui-foundation/public-api/visuals/animated-image-visual.h>
#include <dali-ui-foundation/public-api/visuals/color-visual.h>
#include <dali-ui-foundation/public-api/visuals/gradient-visual.h>
#include <dali-ui-foundation/public-api/visuals/image-visual.h>
#include <dali-ui-foundation/public-api/visuals/lottie-animation-visual.h>
#include <dali-ui-foundation/public-api/visuals/text-visual.h>
#include <dali-ui-foundation/public-api/visuals/visual-base.h>

#include <dali-ui-foundation/integration-api/visuals/gradient-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
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
  DALI_TEST_EQUALS(visual.GetDepthLayer(), Visual::DepthLayer::NONE, TEST_LOCATION);

  View view = View::New();
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::DepthLayer::BACKGROUND), 0u, TEST_LOCATION);

  DALI_TEST_EQUALS(view.AddVisual(visual, Visual::DepthLayer::BACKGROUND), true, TEST_LOCATION);

  DALI_TEST_EQUALS(visual.GetOwner(), view, TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetDepthLayer(), Visual::DepthLayer::BACKGROUND, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::DepthLayer::BACKGROUND), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualAt(Visual::DepthLayer::BACKGROUND, 0u), visual, TEST_LOCATION);

  visual.Detach();

  DALI_TEST_EQUALS(visual.GetOwner(), View(), TEST_LOCATION);
  DALI_TEST_EQUALS(visual.GetDepthLayer(), Visual::DepthLayer::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetVisualCount(Visual::DepthLayer::BACKGROUND), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualGetVisualType(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  // Upcast the handle
  VisualBase visualBase = visual;

  DALI_TEST_EQUALS(visualBase.GetVisualType(), Ui::VisualType::GRADIENT, TEST_LOCATION);

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
  DALI_TEST_CHECK(!TextVisual::DownCast(baseHandle));

  END_TEST;
}

int UtcDaliGradientVisualSetLinearGradient(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Vector2 startPosition(-0.5f, -0.5f);
  Vector2 endPosition(0.5f, 0.5f);

  Ui::Gradient::Linear gradient(startPosition, endPosition);
  gradient.SetStopNodes({
    {0.0f, UiColor(1.0f, 0.0f, 0.0f, 1.0f)},
    {1.0f, UiColor(0.0f, 0.0f, 1.0f, 1.0f)},
  });
  visual.SetGradient(gradient);

  Ui::Gradient::Base retrieved = visual.GetGradient();
  DALI_TEST_EQUALS(retrieved.GetType(), Ui::Gradient::Type::LINEAR, TEST_LOCATION);

  Ui::Gradient::Linear retrievedLinear = Ui::Gradient::Linear::DownCast(retrieved);
  DALI_TEST_EQUALS(retrievedLinear.GetType(), Ui::Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedLinear.GetStartPosition(), startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedLinear.GetEndPosition(), endPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedLinear.GetStopNodes().Size(), 2u, TEST_LOCATION);

  // The other typed views must not accept a linear gradient.
  DALI_TEST_EQUALS(Ui::Gradient::Radial::DownCast(retrieved).GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Gradient::Conic::DownCast(retrieved).GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetRadialGradient(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Vector2 center(0.0f, 0.0f);
  float   radius(0.5f);

  Ui::Gradient::Radial gradient(center, radius);
  visual.SetGradient(gradient);

  Ui::Gradient::Base retrieved = visual.GetGradient();
  DALI_TEST_EQUALS(retrieved.GetType(), Ui::Gradient::Type::RADIAL, TEST_LOCATION);

  Ui::Gradient::Radial retrievedRadial = Ui::Gradient::Radial::DownCast(retrieved);
  DALI_TEST_EQUALS(retrievedRadial.GetCenter(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedRadial.GetRadius(), radius, TEST_LOCATION);

  DALI_TEST_EQUALS(Ui::Gradient::Linear::DownCast(retrieved).GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Gradient::Conic::DownCast(retrieved).GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetConicGradient(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Vector2      center(0.0f, 0.0f);
  Dali::Radian startAngle(Math::PI_2);

  Ui::Gradient::Conic gradient(center, startAngle);
  visual.SetGradient(gradient);

  Ui::Gradient::Base retrieved = visual.GetGradient();
  DALI_TEST_EQUALS(retrieved.GetType(), Ui::Gradient::Type::CONIC, TEST_LOCATION);

  Ui::Gradient::Conic retrievedConic = Ui::Gradient::Conic::DownCast(retrieved);
  DALI_TEST_EQUALS(retrievedConic.GetCenter(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedConic.GetStartAngle(), startAngle, TEST_LOCATION);

  DALI_TEST_EQUALS(Ui::Gradient::Linear::DownCast(retrieved).GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Gradient::Radial::DownCast(retrieved).GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetGradientTypeChanged(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  visual.SetGradient(Ui::Gradient::Linear(Vector2(-0.5f, -0.5f), Vector2(0.5f, 0.5f)));
  DALI_TEST_EQUALS(visual.GetGradient().GetType(), Ui::Gradient::Type::LINEAR, TEST_LOCATION);

  // Setting another type must discard the geometry of the previous type.
  visual.SetGradient(Ui::Gradient::Conic(Vector2(0.25f, 0.75f), Dali::Radian(0.75f)));

  Ui::Gradient::Base retrieved = visual.GetGradient();
  DALI_TEST_EQUALS(retrieved.GetType(), Ui::Gradient::Type::CONIC, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Gradient::Linear::DownCast(retrieved).GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);

  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::START_POSITION).GetType(), Property::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::END_POSITION).GetType(), Property::NONE, TEST_LOCATION);

  // And back to a radial one.
  visual.SetGradient(Ui::Gradient::Radial(Vector2(0.5f, 0.3f), 0.4f));
  DALI_TEST_EQUALS(visual.GetGradient().GetType(), Ui::Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::START_ANGLE).GetType(), Property::NONE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetGradientNone(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  // A newly created visual has no gradient yet.
  DALI_TEST_EQUALS(visual.GetGradient().GetType(), Ui::Gradient::Type::NONE, TEST_LOCATION);

  Ui::Gradient::Radial radial(Vector2(0.25f, 0.25f), 0.5f);
  radial.SetStopNodes({
    {0.0f, UiColor(1.0f, 0.0f, 0.0f, 1.0f)},
    {1.0f, UiColor(0.0f, 0.0f, 1.0f, 1.0f)},
  });
  visual.SetGradient(radial);
  DALI_TEST_EQUALS(visual.GetGradient().GetType(), Ui::Gradient::Type::RADIAL, TEST_LOCATION);

  // A none gradient is ignored, so the visual keeps the gradient it already has.
  visual.SetGradient(Ui::Gradient::Base::None());

  Ui::Gradient::Base retrieved = visual.GetGradient();
  DALI_TEST_EQUALS(retrieved.GetType(), Ui::Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Gradient::Radial::DownCast(retrieved).GetCenter(), Vector2(0.25f, 0.25f), TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Gradient::Radial::DownCast(retrieved).GetRadius(), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrieved.GetStopNodes().Size(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualStartOffset(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  // The start offset is common to every gradient type, and is carried by the value.
  Ui::Gradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  linear.SetStartOffset(0.25f);
  visual.SetGradient(linear);
  DALI_TEST_EQUALS(visual.GetGradient().GetStartOffset(), 0.25f, TEST_LOCATION);

  Ui::Gradient::Conic conic(Vector2::ZERO, Dali::Radian(1.0f));
  conic.SetStartOffset(0.5f);
  visual.SetGradient(conic);
  DALI_TEST_EQUALS(visual.GetGradient().GetStartOffset(), 0.5f, TEST_LOCATION);

  Ui::Gradient::Radial radial(Vector2::ZERO, 0.5f);
  radial.SetStartOffset(0.75f);
  visual.SetGradient(radial);
  DALI_TEST_EQUALS(visual.GetGradient().GetStartOffset(), 0.75f, TEST_LOCATION);

  // A value that leaves the start offset alone resets it to the default.
  visual.SetGradient(Ui::Gradient::Radial(Vector2::ZERO, 0.5f));
  DALI_TEST_EQUALS(visual.GetGradient().GetStartOffset(), 0.0f, TEST_LOCATION);

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

  Ui::Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes(stopNodes);
  visual.SetGradient(gradient);

  Dali::Vector<Ui::Gradient::StopNode> retrievedNodes = visual.GetGradient().GetStopNodes();
  DALI_TEST_EQUALS(retrievedNodes.Size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[0].GetOffset(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[1].GetOffset(), 0.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[2].GetOffset(), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[1].GetColor().GetRgba(), UiColor(0.0f, 1.0f, 0.0f, 1.0f).GetRgba(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualStopNodesInitializerList(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Ui::Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({
    {0.0f, UiColor(1.0f, 0.0f, 0.0f, 1.0f)},
    {1.0f, UiColor(0.0f, 0.0f, 1.0f, 1.0f)},
  });
  visual.SetGradient(gradient);

  Dali::Vector<Ui::Gradient::StopNode> retrievedNodes = visual.GetGradient().GetStopNodes();
  DALI_TEST_EQUALS(retrievedNodes.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[0].GetOffset(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(retrievedNodes[1].GetOffset(), 1.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualUnits(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Ui::Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));

  gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
  visual.SetGradient(gradient);
  DALI_TEST_EQUALS(visual.GetGradient().GetUnits(), Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);

  gradient.SetUnits(Ui::Gradient::Units::OBJECT_BOUNDING_BOX);
  visual.SetGradient(gradient);
  DALI_TEST_EQUALS(visual.GetGradient().GetUnits(), Ui::Gradient::Units::OBJECT_BOUNDING_BOX, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSpreadMethod(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  Ui::Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));

  gradient.SetSpreadMethod(Ui::Gradient::SpreadMethod::REFLECT);
  visual.SetGradient(gradient);
  DALI_TEST_EQUALS(visual.GetGradient().GetSpreadMethod(), Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);

  gradient.SetSpreadMethod(Ui::Gradient::SpreadMethod::REPEAT);
  visual.SetGradient(gradient);
  DALI_TEST_EQUALS(visual.GetGradient().GetSpreadMethod(), Ui::Gradient::SpreadMethod::REPEAT, TEST_LOCATION);

  gradient.SetSpreadMethod(Ui::Gradient::SpreadMethod::PAD);
  visual.SetGradient(gradient);
  DALI_TEST_EQUALS(visual.GetGradient().GetSpreadMethod(), Ui::Gradient::SpreadMethod::PAD, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientVisualSetGetPropertyValue(void)
{
  UiTestApplication application;

  GradientVisual visual = GradientVisual::New();

  // Test StartOffset. This is the property View::Animate() drives, so a value set
  // through Integration must be visible on the gradient value and vice versa.
  Ui::Gradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  linear.SetStartOffset(0.3f);
  visual.SetGradient(linear);
  DALI_TEST_EQUALS(visual.GetGradient().GetStartOffset(), 0.3f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::START_OFFSET).Get<float>(), 0.3f, TEST_LOCATION);

  Dali::Ui::GetImplementation(visual).SetProperty(Dali::Ui::Integration::GradientVisual::Property::START_OFFSET, 0.7f);
  DALI_TEST_EQUALS(visual.GetGradient().GetStartOffset(), 0.7f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::START_OFFSET).Get<float>(), 0.7f, TEST_LOCATION);

  // Test RadialGradient
  Ui::Gradient::Radial radial(Vector2(0.5f, 0.3f), 0.4f);
  radial.SetSpreadMethod(Ui::Gradient::SpreadMethod::REFLECT);
  radial.SetUnits(Ui::Gradient::Units::USER_SPACE);
  visual.SetGradient(radial);

  Ui::Gradient::Radial retrieved = Ui::Gradient::Radial::DownCast(visual.GetGradient());
  DALI_TEST_EQUALS(retrieved.GetCenter(), Vector2(0.5f, 0.3f), TEST_LOCATION);
  DALI_TEST_EQUALS(retrieved.GetRadius(), 0.4f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::CENTER).Get<Vector2>(), Vector2(0.5f, 0.3f), TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::RADIUS).Get<float>(), 0.4f, TEST_LOCATION);

  // Test SpreadMethod
  DALI_TEST_EQUALS(retrieved.GetSpreadMethod(), Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::SPREAD_METHOD).Get<Ui::Gradient::SpreadMethod>(), Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);

  // Test Units
  DALI_TEST_EQUALS(retrieved.GetUnits(), Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::GetImplementation(visual).GetProperty(Dali::Ui::Integration::GradientVisual::Property::UNITS).Get<Ui::Gradient::Units>(), Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);

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

  auto TestAssertFunction = [&](std::function<void(void)> func)
  {
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
  TestAssertFunction([&]()
  { empty.SetName("ShouldBeCrash"); });
  TestAssertFunction([&]()
  { empty.SetOffsetX(1.0f); });
  TestAssertFunction([&]()
  { empty.SetOffsetY(1.0f); });
  TestAssertFunction([&]()
  { empty.SetWidth(100.0f); });
  TestAssertFunction([&]()
  { empty.SetHeight(100.0f); });
  TestAssertFunction([&]()
  { empty.SetTransformProportionFlags(Visual::Transform::ProportionFlags::ALL); });
  TestAssertFunction([&]()
  { empty.SetExtraWidth(10.0f); });
  TestAssertFunction([&]()
  { empty.SetExtraHeight(10.0f); });
  TestAssertFunction([&]()
  { empty.SetOrigin(VisualOrigin::CENTER_LEFT); });
  TestAssertFunction([&]()
  { empty.SetPivot(VisualPivot::CENTER_LEFT); });
  TestAssertFunction([&]()
  { empty.SetSiblingOrder(0u); });

  TestAssertFunction([&]()
  { empty.GetOwner(); });
  TestAssertFunction([&]()
  { empty.GetDepthLayer(); });
  TestAssertFunction([&]()
  { empty.GetName(); });
  TestAssertFunction([&]()
  { empty.GetOffsetX(); });
  TestAssertFunction([&]()
  { empty.GetOffsetY(); });
  TestAssertFunction([&]()
  { empty.GetWidth(); });
  TestAssertFunction([&]()
  { empty.GetHeight(); });
  TestAssertFunction([&]()
  { empty.GetTransformProportionFlags(); });
  TestAssertFunction([&]()
  { empty.GetExtraWidth(); });
  TestAssertFunction([&]()
  { empty.GetExtraHeight(); });
  TestAssertFunction([&]()
  { empty.GetOrigin(); });
  TestAssertFunction([&]()
  { empty.GetPivot(); });
  TestAssertFunction([&]()
  { empty.GetSiblingOrder(); });

  // GradientVisual specific
  TestAssertFunction([&]()
  { empty.SetGradient(Ui::Gradient::Linear(Vector2::ZERO, Vector2::ONE)); });
  TestAssertFunction([&]()
  { empty.GetGradient(); });

  END_TEST;
}
