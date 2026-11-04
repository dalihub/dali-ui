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

#include <dali-ui-foundation/public-api/animation/view-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/configuration/ui-scale-manager.h>
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
#include <dali-ui-foundation/integration-api/visuals/visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-base-impl.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <cmath>
#include <limits>

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

namespace
{
struct GradientUiScaleScope
{
  GradientUiScaleScope()
  : scale(UiScaleManager::Get().GetScale()),
    scalable(UiScaleManager::Get().IsScalable())
  {
    UiScaleManager::Get().SetScalable(true);
    UiScaleManager::Get().SetScale(1.0f);
  }
  ~GradientUiScaleScope()
  {
    UiScaleManager::Get().SetScale(scale);
    UiScaleManager::Get().SetScalable(scalable);
  }
  float scale;
  bool  scalable;
};

void SettleGradient(UiTestApplication& application)
{
  for(int frame = 0; frame < 3; ++frame)
  {
    application.SendNotification();
    application.Render(16);
  }
}

Gradient::Base ScaleTestGradient(int type, Gradient::Units units)
{
  Gradient::Base gradient;
  if(type == 0)
  {
    gradient = Gradient::Linear(Vector2(10.0f, 20.0f), Vector2(110.0f, 70.0f));
  }
  else if(type == 1)
  {
    gradient = Gradient::Radial(Vector2(50.0f, 30.0f), 25.0f);
  }
  else
  {
    gradient = Gradient::Conic(Vector2(50.0f, 30.0f), Radian(0.4f));
  }
  gradient.SetUnits(units);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  gradient.SetStartOffset(0.125f);
  gradient.SetStopNodes({{0.0f, UiColor(Color::RED)}, {1.0f, UiColor(Color::BLUE)}});
  return gradient;
}

Renderer GradientRenderer(View view)
{
  Renderer gradientRenderer;
  for(uint32_t index = 0u; index < view.GetRendererCount(); ++index)
  {
    auto renderer = view.GetRendererAt(index);
    if(renderer.GetPropertyIndex("uAlignmentMatrix") != Property::INVALID_INDEX)
    {
      DALI_TEST_CHECK(!gradientRenderer);
      gradientRenderer = renderer;
    }
  }
  DALI_TEST_CHECK(gradientRenderer);
  return gradientRenderer;
}

Matrix3 GradientMatrix(View view)
{
  auto renderer = GradientRenderer(view);
  return renderer.GetProperty<Matrix3>(renderer.GetPropertyIndex("uAlignmentMatrix"));
}

void ExpectGradientMapping(const Matrix3& matrix, int type, float scale)
{
  // Geometric expectations, independently of the implementation's matrix scaling.
  if(type == 0)
  {
    const Vector3 start = matrix * Vector3(10.0f * scale, 20.0f * scale, 1.0f);
    const Vector3 end   = matrix * Vector3(110.0f * scale, 70.0f * scale, 1.0f);
    DALI_TEST_EQUALS(start.y, 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(end.y, 1.0f, 0.0001f, TEST_LOCATION);
  }
  else
  {
    const Vector3 center = matrix * Vector3(50.0f * scale, 30.0f * scale, 1.0f);
    const Vector3 right  = matrix * Vector3(75.0f * scale, 30.0f * scale, 1.0f);
    DALI_TEST_EQUALS(center.x, 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(center.y, 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(right.x, type == 1 ? 1.0f : 25.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(right.y, 0.0f, 0.0001f, TEST_LOCATION);
  }
}
} // namespace

int UtcDaliGradientVisualUiScaleGeometryAndResources(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  for(int type = 0; type < 3; ++type)
  {
    for(auto units : {Gradient::Units::USER_SPACE, Gradient::Units::OBJECT_BOUNDING_BOX})
    {
      View parent = View::New();
      parent.SetUiScalePolicy(UiScalePolicy::DISABLED);
      parent.SetRequestedWidth(240.0f);
      parent.SetRequestedHeight(90.0f);
      View view = View::New();
      view.SetUiScalePolicy(UiScalePolicy::ENABLED);
      view.SetLayoutMode(LayoutMode::STANDALONE);
      view.SetRequestedWidth(MATCH_PARENT);
      view.SetRequestedHeight(MATCH_PARENT);
      const auto authored = ScaleTestGradient(type, units);
      view.SetBackgroundGradient(authored);
      parent.Add(view);
      application.GetWindow().Add(parent);
      UiScaleManager::Get().SetScale(1.0f);
      SettleGradient(application);
      const Matrix3 original = GradientMatrix(view);
      auto          renderer = GradientRenderer(view);
      auto          texture  = renderer.GetTextures().GetTexture(0u);
      auto          geometry = renderer.GetGeometry();
      auto          shader   = renderer.GetShader();

      for(float scale : {0.5f, 1.0f, 1.25f, 2.0f, 1.0f})
      {
        auto& gl = application.GetGlAbstraction();
        gl.EnableTextureCallTrace(true);
        gl.EnableShaderCallTrace(true);
        gl.GetTextureTrace().Reset();
        gl.GetShaderTrace().Reset();
        gl.GetBufferTrace().Enable(true);
        gl.GetBufferTrace().Reset();
        // Only the scale changes. MATCH_PARENT keeps actual and requested sizes unchanged.
        UiScaleManager::Get().SetScale(scale);
        SettleGradient(application);
        DALI_TEST_EQUALS(view.GetCurrentProperty<Vector3>(Actor::Property::SIZE), Vector3(240.0f, 90.0f, 0.0f), TEST_LOCATION);
        ExpectGradientMapping(GradientMatrix(view), type, units == Gradient::Units::USER_SPACE ? scale : 1.0f);
        if(units == Gradient::Units::OBJECT_BOUNDING_BOX || scale == 1.0f)
        {
          DALI_TEST_EQUALS(GradientMatrix(view), original, TEST_LOCATION);
        }
        DALI_TEST_EQUALS(GradientRenderer(view), renderer, TEST_LOCATION);
        DALI_TEST_EQUALS(renderer.GetTextures().GetTexture(0u), texture, TEST_LOCATION);
        DALI_TEST_EQUALS(renderer.GetGeometry(), geometry, TEST_LOCATION);
        DALI_TEST_EQUALS(renderer.GetShader(), shader, TEST_LOCATION);
        DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("GenTextures"), 0, TEST_LOCATION);
        DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
        DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);
        DALI_TEST_EQUALS(gl.GetShaderTrace().CountMethod("CreateShader"), 0, TEST_LOCATION);
        DALI_TEST_EQUALS(gl.GetBufferTrace().CountMethod("GenBuffers"), 0, TEST_LOCATION);
        DALI_TEST_EQUALS(authored.GetStartOffset(), 0.125f, TEST_LOCATION);
        DALI_TEST_EQUALS(authored.GetSpreadMethod(), Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
      }
      application.GetWindow().Remove(parent);
      SettleGradient(application);
    }
  }
  END_TEST;
}

int UtcDaliGradientVisualUiScaleLifecycle(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  UiScaleManager::Get().SetScale(2.0f);
  View enabled  = View::New();
  View disabled = View::New();
  enabled.SetUiScalePolicy(UiScalePolicy::ENABLED);
  disabled.SetUiScalePolicy(UiScalePolicy::DISABLED);
  application.GetWindow().Add(enabled);
  application.GetWindow().Add(disabled);
  View child = View::New();
  child.SetRequestedWidth(240.0f);
  child.SetRequestedHeight(90.0f);
  child.SetBackgroundGradient(ScaleTestGradient(1, Gradient::Units::USER_SPACE));
  disabled.Add(child);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(child), 1, 1.0f);
  disabled.Remove(child);
  enabled.Add(child);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(child), 1, 2.0f);
  child.SetUiScalePolicy(UiScalePolicy::DISABLED);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(child), 1, 1.0f);
  child.SetUiScalePolicy(UiScalePolicy::ENABLED);
  enabled.SetProperty(Actor::Property::SCALE, Vector3(1.5f, 0.75f, 1.0f));
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(child), 1, 2.0f);
  enabled.Remove(child);
  UiScaleManager::Get().SetScale(1.25f);
  enabled.Add(child);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(child), 1, 1.25f);
  UiScaleManager::Get().SetScalable(false);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(child), 1, 1.0f);
  UiScaleManager::Get().SetScalable(true);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(child), 1, 1.25f);
  END_TEST;
}

int UtcDaliGradientVisualUiScaleUnitsAndTransform(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  UiScaleManager::Get().SetScale(2.0f);
  View view = View::New();
  view.SetRequestedWidth(240.0f);
  view.SetRequestedHeight(90.0f);
  application.GetWindow().Add(view);
  SettleGradient(application);
  // Late registration at a non-unit scale, initially BBOX.
  GradientVisual visual = GradientVisual::New();
  visual.SetGradient(ScaleTestGradient(0, Gradient::Units::OBJECT_BOUNDING_BOX));
  visual.SetWidth(0.75f);
  visual.SetHeight(0.5f);
  visual.SetOffsetX(0.1f);
  view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
  SettleGradient(application);
  auto          internal = GetImplementation(visual).GetVisual();
  Property::Map properties;
  properties.Insert(Ui::Integration::GradientVisual::Property::UNITS, Gradient::Units::USER_SPACE);
  internal.DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, properties);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(view), 0, 2.0f);
  // Transform updates must not compound the gradient's scale.
  visual.SetOffsetY(0.2f);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(view), 0, 2.0f);
  properties.Clear();
  properties.Insert(Ui::Integration::GradientVisual::Property::UNITS, Gradient::Units::OBJECT_BOUNDING_BOX);
  internal.DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, properties);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(view), 0, 1.0f);
  visual.SetGradient(ScaleTestGradient(2, Gradient::Units::USER_SPACE));
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(view), 2, 2.0f);
  visual.Detach();
  UiScaleManager::Get().SetScale(0.5f);
  view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(view), 2, 0.5f);
  END_TEST;
}

int UtcDaliGradientVisualUiScaleAnimation(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  View                 view = View::New();
  view.SetRequestedWidth(240.0f);
  view.SetRequestedHeight(90.0f);
  view.SetBackgroundGradient(ScaleTestGradient(0, Gradient::Units::USER_SPACE));
  application.GetWindow().Add(view);
  SettleGradient(application);
  auto       renderer    = GradientRenderer(view);
  const auto offsetIndex = renderer.GetPropertyIndex("uGradientOffset");
  Animation  animation   = Animation::New(2.0f);
  animation.AnimateTo(Property(view, Actor::Property::POSITION_X), 70.0f);
  animation.AnimateTo(Property(view, Actor::Property::OPACITY), 0.5f);
  animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(1.5f, 0.75f, 1.0f));
  view.Animate(animation).BackgroundGradientStartOffset(0.75f, Duration(2.0f));
  animation.Play();
  SettleGradient(application);
  const float before = renderer.GetCurrentProperty<float>(offsetIndex);
  UiScaleManager::Get().SetScale(2.0f);
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(view), 0, 2.0f);
  DALI_TEST_CHECK(renderer.GetCurrentProperty<float>(offsetIndex) > before);
  animation.Stop();
  SettleGradient(application);
  animation.Play();
  application.SendNotification();
  application.Render(2100);
  application.SendNotification();
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(offsetIndex), 0.75f, 0.0001f, TEST_LOCATION);
  ExpectGradientMapping(GradientMatrix(view), 0, 2.0f);
  END_TEST;
}

int UtcDaliGradientVisualUiScaleUniformStability(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  View                 view = View::New();
  view.SetRequestedWidth(240.0f);
  view.SetRequestedHeight(90.0f);
  ColorVisual color = ColorVisual::New();
  color.SetColor(UiColor(Color::WHITE));
  view.AddVisual(color, Visual::DepthLayer::BACKGROUND_EFFECT);
  GradientVisual visual = GradientVisual::New();
  visual.SetGradient(ScaleTestGradient(1, Gradient::Units::USER_SPACE));
  view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
  application.GetWindow().Add(view);
  SettleGradient(application);
  auto internal = GetImplementation(visual).GetVisual();
  auto renderer = internal.GetRenderer();
  DALI_TEST_EQUALS(view.GetRendererCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(GradientRenderer(view), renderer, TEST_LOCATION);
  const auto index = renderer.GetPropertyIndex("uAlignmentMatrix");
  const auto count = renderer.GetPropertyCount();
  DALI_TEST_CHECK(index != Property::INVALID_INDEX);
  const auto original = renderer.GetProperty<Matrix3>(index);
  for(int cycle = 0; cycle < 10; ++cycle)
  {
    for(float scale : {1.0f, 2.0f, 2.0f, 1.0f})
    {
      UiScaleManager::Get().SetScale(scale);
      SettleGradient(application);
      ExpectGradientMapping(GradientMatrix(view), 1, scale);
      DALI_TEST_EQUALS(renderer.GetPropertyIndex("uAlignmentMatrix"), index, TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
      DALI_TEST_EQUALS(internal.GetRenderer(), renderer, TEST_LOCATION);
      if(scale == 1.0f)
      {
        const auto matrix = renderer.GetProperty<Matrix3>(index);
        for(unsigned int i = 0; i < 9u; ++i)
        {
          DALI_TEST_EQUALS(matrix.AsFloat()[i], original.AsFloat()[i], TEST_LOCATION);
        }
      }
    }
    visual.SetOffsetX(0.01f * cycle);
    visual.SetWidth(0.8f + 0.01f * cycle);
    visual.SetHeight(0.7f + 0.01f * cycle);
    view.SetRequestedWidth(240.0f + cycle);
    SettleGradient(application);
    DALI_TEST_EQUALS(GradientMatrix(view), original, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
  }
  visual.Detach();
  SettleGradient(application);
  view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
  SettleGradient(application);
  DALI_TEST_EQUALS(GetImplementation(visual).GetVisual().GetRenderer(), renderer, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyIndex("uAlignmentMatrix"), index, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientVisualUiScaleDirectTransform(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  View                 view = View::New();
  view.SetRequestedWidth(240.0f);
  view.SetRequestedHeight(90.0f);
  GradientVisual visual = GradientVisual::New();
  visual.SetGradient(ScaleTestGradient(0, Gradient::Units::USER_SPACE));
  view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
  application.GetWindow().Add(view);
  SettleGradient(application);
  auto          internal = GetImplementation(visual).GetVisual();
  auto          renderer = internal.GetRenderer();
  const auto    index    = renderer.GetPropertyIndex("uAlignmentMatrix");
  const auto    count    = renderer.GetPropertyCount();
  Property::Map transform;
  for(float scale : {2.0f, 1.25f, 0.5f, 1.0f})
  {
    // No View layout event: Base stores the new scale before OnSetTransform.
    internal.SetTransformAndSize(transform, Size(240.0f, 90.0f), scale);
    ExpectGradientMapping(renderer.GetProperty<Matrix3>(index), 0, scale);
    DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
  }
  visual.Detach();
  SettleGradient(application);
  internal.SetTransformAndSize(transform, Size(240.0f, 90.0f), 2.0f);
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(240.0f, 90.0f));
  application.GetWindow().Add(actor);
  internal.SetOnScene(actor);
  SettleGradient(application);
  DALI_TEST_EQUALS(actor.GetRendererAt(0u), renderer, TEST_LOCATION);
  ExpectGradientMapping(renderer.GetProperty<Matrix3>(index), 0, 1.0f);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientVisualUiScalePropertyTransitions(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  UiScaleManager::Get().SetScale(2.0f);
  View view = View::New();
  view.SetRequestedWidth(240.0f);
  view.SetRequestedHeight(90.0f);
  GradientVisual visual   = GradientVisual::New();
  auto           identity = Gradient::Conic(Vector2::ZERO, Radian(0.4f));
  identity.SetUnits(Gradient::Units::USER_SPACE);
  identity.SetStopNodes({{0.0f, UiColor(Color::RED)}, {1.0f, UiColor(Color::BLUE)}});
  visual.SetGradient(identity);
  view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
  application.GetWindow().Add(view);
  SettleGradient(application);
  auto       internal = GetImplementation(visual).GetVisual();
  auto       renderer = internal.GetRenderer();
  const auto index    = renderer.GetPropertyIndex("uAlignmentMatrix");
  DALI_TEST_CHECK(index != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(renderer.GetProperty<Matrix3>(index) * Vector3(20.0f, 40.0f, 1.0f), Vector3(10.0f, 20.0f, 1.0f), TEST_LOCATION);
  const auto count = renderer.GetPropertyCount();
  // Type and units change together, then scale changes after both have settled.
  for(auto units : {Gradient::Units::OBJECT_BOUNDING_BOX, Gradient::Units::USER_SPACE})
  {
    for(int type : {0, 1, 2})
    {
      visual.SetGradient(ScaleTestGradient(type, units));
      SettleGradient(application);
      for(float scale : {0.5f, 2.0f})
      {
        UiScaleManager::Get().SetScale(scale);
        SettleGradient(application);
        ExpectGradientMapping(GradientMatrix(view), type, units == Gradient::Units::USER_SPACE ? scale : 1.0f);
        DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
        DALI_TEST_EQUALS(renderer.GetPropertyIndex("uAlignmentMatrix"), index, TEST_LOCATION);
      }
    }
  }
  UiScaleManager::Get().SetScale(1.0f);
  auto precise = Gradient::Linear(Vector2::ZERO, Vector2(100000.0f, 0.0f));
  precise.SetUnits(Gradient::Units::USER_SPACE);
  precise.SetStopNodes({{0.0f, UiColor(Color::RED)}, {1.0f, UiColor(Color::BLUE)}});
  visual.SetGradient(precise);
  SettleGradient(application);
  const auto before = renderer.GetProperty<Matrix3>(index);
  UiScaleManager::Get().SetScale(1.0001f);
  SettleGradient(application);
  const auto after = renderer.GetProperty<Matrix3>(index);
  DALI_TEST_CHECK(before.AsFloat()[1] != after.AsFloat()[1]);
  DALI_TEST_EQUALS((after * Vector3(100000.0f * 1.0001f, 0.0f, 1.0f)).y, 1.0f, 0.000001f, TEST_LOCATION);
  Property::Map properties;
  properties.Insert(Ui::Integration::GradientVisual::Property::START_OFFSET, 0.625f);
  internal.DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, properties);
  SettleGradient(application);
  DALI_TEST_EQUALS(renderer.GetProperty<Matrix3>(index), after, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
  const auto      texture = renderer.GetTextures().GetTexture(0u);
  Property::Array colors;
  colors.PushBack(Color::GREEN);
  colors.PushBack(Color::BLUE);
  properties.Clear();
  properties.Insert(Ui::Integration::GradientVisual::Property::STOP_COLOR, colors);
  internal.DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, properties);
  SettleGradient(application);
  DALI_TEST_EQUALS(renderer.GetProperty<Matrix3>(index), after, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
  DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u) != texture);
  END_TEST;
}

int UtcDaliGradientVisualUiScaleNonFiniteAlignment(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  for(int type = 0; type < 3; ++type)
  {
    View view = View::New();
    view.SetRequestedWidth(240.0f);
    view.SetRequestedHeight(90.0f);
    GradientVisual visual = GradientVisual::New();
    Gradient::Base gradient;
    if(type == 0)
    {
      gradient = Gradient::Linear(Vector2::ZERO, Vector2(0.125f, 0.0f));
    }
    else if(type == 1)
    {
      gradient = Gradient::Radial(Vector2::ZERO, 0.125f);
    }
    else
    {
      gradient = Gradient::Conic(Vector2::ZERO, Radian(0.4f));
    }
    gradient.SetUnits(Gradient::Units::USER_SPACE);
    gradient.SetStopNodes({{0.0f, UiColor(Color::RED)}, {1.0f, UiColor(Color::BLUE)}});
    visual.SetGradient(gradient);
    view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
    application.GetWindow().Add(view);
    SettleGradient(application);
    auto          internal = GetImplementation(visual).GetVisual();
    auto          renderer = internal.GetRenderer();
    const auto    index    = renderer.GetPropertyIndex("uAlignmentMatrix");
    const auto    count    = renderer.GetPropertyCount();
    const auto    original = renderer.GetProperty<Matrix3>(index);
    const auto    texture  = renderer.GetTextures().GetTexture(0u);
    Property::Map transform;
    // The global policy accepts FLT_MIN; linear/radial coefficients can overflow.
    if(type < 2)
    {
      internal.SetTransformAndSize(transform, Size(240.0f, 90.0f), std::numeric_limits<float>::min());
      DALI_TEST_EQUALS(renderer.GetProperty<Matrix3>(index), original, TEST_LOCATION);
    }
    // Direct integration callers bypass the global scale validation.
    for(float scale : {0.0f, std::numeric_limits<float>::quiet_NaN()})
    {
      internal.SetTransformAndSize(transform, Size(240.0f, 90.0f), scale);
      const auto rejected = renderer.GetProperty<Matrix3>(index);
      for(unsigned int i = 0; i < 9u; ++i)
      {
        DALI_TEST_CHECK(std::isfinite(rejected.AsFloat()[i]));
        DALI_TEST_EQUALS(rejected.AsFloat()[i], original.AsFloat()[i], TEST_LOCATION);
      }
    }
    for(float scale : {0.5f, 1.0f, 1.25f, 2.0f, 1.0e-20f, 1.0e20f, std::numeric_limits<float>::max(),
                       std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()})
    {
      internal.SetTransformAndSize(transform, Size(240.0f, 90.0f), scale);
      const auto matrix = renderer.GetProperty<Matrix3>(index);
      for(unsigned int i = 0; i < 9u; ++i)
      {
        DALI_TEST_CHECK(std::isfinite(matrix.AsFloat()[i]));
      }
      // Infinite integration scale has a zero reciprocal, hence a finite matrix.
      // The guard validates the result, not the integration caller's scale contract.
      if(std::isinf(scale))
      {
        for(unsigned int i = 0; i < 6u; ++i)
        {
          DALI_TEST_EQUALS(matrix.AsFloat()[i], 0.0f, TEST_LOCATION);
        }
      }
      else if(scale == std::numeric_limits<float>::max() && type == 2)
      {
        DALI_TEST_CHECK(matrix.AsFloat()[0] > 0.0f);
        DALI_TEST_CHECK(matrix.AsFloat()[0] < std::numeric_limits<float>::min());
      }
    }
    internal.SetTransformAndSize(transform, Size(240.0f, 90.0f), 2.0f);
    const auto recovered = renderer.GetProperty<Matrix3>(index);
    for(unsigned int i = 0; i < 9u; ++i)
    {
      DALI_TEST_EQUALS(recovered.AsFloat()[i], original.AsFloat()[i] * (i < 6u ? 0.5f : 1.0f), TEST_LOCATION);
    }
    DALI_TEST_EQUALS(renderer.GetPropertyIndex("uAlignmentMatrix"), index, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetTextures().GetTexture(0u), texture, TEST_LOCATION);
    application.GetWindow().Remove(view);
    SettleGradient(application);
  }
  END_TEST;
}

int UtcDaliGradientVisualUiScaleRejectedPropertyTransitions(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  View                 view = View::New();
  view.SetRequestedWidth(240.0f);
  view.SetRequestedHeight(90.0f);
  GradientVisual visual = GradientVisual::New();
  visual.SetGradient(ScaleTestGradient(0, Gradient::Units::USER_SPACE));
  view.AddVisual(visual, Visual::DepthLayer::BACKGROUND);
  application.GetWindow().Add(view);
  SettleGradient(application);
  auto       renderer = GradientRenderer(view);
  const auto index    = renderer.GetPropertyIndex("uAlignmentMatrix");
  auto       count    = renderer.GetPropertyCount();
  for(auto units : {Gradient::Units::OBJECT_BOUNDING_BOX, Gradient::Units::USER_SPACE})
  {
    for(int type = 0; type < 3; ++type)
    {
      const auto previous = GradientMatrix(view);
      // The first conic transition registers its existing angle uniform once.
      if(type == 2 && renderer.GetPropertyIndex("uStartAngle") == Property::INVALID_INDEX)
      {
        ++count;
      }
      for(float invalid : {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()})
      {
        Gradient::Base gradient;
        if(type == 0)
        {
          gradient = Gradient::Linear(Vector2(invalid, 0.0f), Vector2(100.0f, 0.0f));
        }
        else if(type == 1)
        {
          gradient = Gradient::Radial(Vector2(invalid, 0.0f), 25.0f);
        }
        else
        {
          gradient = Gradient::Conic(Vector2(invalid, 0.0f), Radian(0.4f));
        }
        gradient.SetUnits(units);
        gradient.SetStopNodes({{0.0f, UiColor(Color::GREEN)}, {1.0f, UiColor(Color::BLUE)}});
        visual.SetGradient(gradient);
        SettleGradient(application);
        const auto rejected = GradientMatrix(view);
        for(unsigned int i = 0; i < 9u; ++i)
        {
          DALI_TEST_CHECK(std::isfinite(rejected.AsFloat()[i]));
          DALI_TEST_EQUALS(rejected.AsFloat()[i], previous.AsFloat()[i], TEST_LOCATION);
        }
        DALI_TEST_EQUALS(visual.GetGradient().GetType(), gradient.GetType(), TEST_LOCATION);
        DALI_TEST_EQUALS(visual.GetGradient().GetUnits(), units, TEST_LOCATION);
        DALI_TEST_EQUALS(GradientRenderer(view), renderer, TEST_LOCATION);
        DALI_TEST_EQUALS(renderer.GetPropertyIndex("uAlignmentMatrix"), index, TEST_LOCATION);
        DALI_TEST_EQUALS(renderer.GetPropertyCount(), count, TEST_LOCATION);
        DALI_TEST_CHECK(renderer.GetShader());
        DALI_TEST_CHECK(renderer.GetTextures().GetTexture(0u));
      }
      visual.SetGradient(ScaleTestGradient(type, units));
      SettleGradient(application);
      ExpectGradientMapping(GradientMatrix(view), type, 1.0f);
    }
  }
  // Finite authored geometry can overflow after a type change at a valid scale.
  UiScaleManager::Get().SetScale(std::numeric_limits<float>::min());
  SettleGradient(application);
  const auto beforeOverflow = GradientMatrix(view);
  auto       small          = Gradient::Radial(Vector2::ZERO, 0.125f);
  small.SetUnits(Gradient::Units::USER_SPACE);
  small.SetStopNodes({{0.0f, UiColor(Color::RED)}, {1.0f, UiColor(Color::BLUE)}});
  visual.SetGradient(small);
  SettleGradient(application);
  DALI_TEST_EQUALS(GradientMatrix(view), beforeOverflow, TEST_LOCATION);
  Property::Map properties;
  properties.Insert(Ui::Integration::GradientVisual::Property::UNITS, Gradient::Units::OBJECT_BOUNDING_BOX);
  auto internal = GetImplementation(visual).GetVisual();
  internal.DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, properties);
  SettleGradient(application);
  const auto bbox = GradientMatrix(view);
  DALI_TEST_EQUALS((bbox * Vector3(0.125f, 0.0f, 1.0f)).x, 1.0f, TEST_LOCATION);
  properties.Clear();
  properties.Insert(Ui::Integration::GradientVisual::Property::UNITS, Gradient::Units::USER_SPACE);
  internal.DoAction(Ui::Integration::Visual::Action::UPDATE_PROPERTY, properties);
  SettleGradient(application);
  DALI_TEST_EQUALS(GradientMatrix(view), bbox, TEST_LOCATION);
  UiScaleManager::Get().SetScale(2.0f);
  SettleGradient(application);
  DALI_TEST_EQUALS((GradientMatrix(view) * Vector3(0.25f, 0.0f, 1.0f)).x, 1.0f, TEST_LOCATION);
  UiScaleManager::Get().SetScale(1.0f);
  // Reject an invalid authored alignment before the first renderer registration.
  GradientVisual initial = GradientVisual::New();
  auto           invalid = Gradient::Conic(Vector2(std::numeric_limits<float>::quiet_NaN(), 0.0f), Radian(0.4f));
  initial.SetGradient(invalid);
  View other = View::New();
  other.SetRequestedWidth(100.0f);
  other.SetRequestedHeight(50.0f);
  other.AddVisual(initial, Visual::DepthLayer::BACKGROUND);
  application.GetWindow().Add(other);
  SettleGradient(application);
  const auto initialMatrix = GradientMatrix(other);
  for(unsigned int i = 0; i < 9u; ++i)
  {
    DALI_TEST_CHECK(std::isfinite(initialMatrix.AsFloat()[i]));
  }
  initial.Detach();
  SettleGradient(application);
  other.AddVisual(initial, Visual::DepthLayer::BACKGROUND);
  initial.SetGradient(ScaleTestGradient(2, Gradient::Units::USER_SPACE));
  SettleGradient(application);
  ExpectGradientMapping(GradientMatrix(other), 2, 1.0f);
  END_TEST;
}

int UtcDaliGradientVisualUiScaleSharedGradient(void)
{
  UiTestApplication    application;
  GradientUiScaleScope scaleScope;
  UiScaleManager::Get().SetScale(2.0f);
  for(int type = 0; type < 3; ++type)
  {
    const auto authored = ScaleTestGradient(type, Gradient::Units::USER_SPACE);
    View       enabled  = View::New();
    View       disabled = View::New();
    enabled.SetUiScalePolicy(UiScalePolicy::ENABLED);
    disabled.SetUiScalePolicy(UiScalePolicy::DISABLED);
    for(auto view : {enabled, disabled})
    {
      view.SetRequestedWidth(240.0f);
      view.SetRequestedHeight(90.0f);
      view.SetBackgroundGradient(authored);
      application.GetWindow().Add(view);
    }
    SettleGradient(application);
    ExpectGradientMapping(GradientMatrix(enabled), type, 2.0f);
    ExpectGradientMapping(GradientMatrix(disabled), type, 1.0f);
    DALI_TEST_EQUALS(authored.GetUnits(), Gradient::Units::USER_SPACE, TEST_LOCATION);
    DALI_TEST_EQUALS(authored.GetStartOffset(), 0.125f, TEST_LOCATION);
    application.GetWindow().Remove(enabled);
    application.GetWindow().Remove(disabled);
    SettleGradient(application);
  }
  END_TEST;
}
