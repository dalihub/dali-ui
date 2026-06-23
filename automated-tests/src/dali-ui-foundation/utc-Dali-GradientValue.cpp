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

#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float EPSILON = 0.001f;
constexpr char  MOVED_FROM_GRADIENT_ASSERTION[] = "Cannot use a moved-from Gradient::Base object";

Dali::Vector<Gradient::StopNode> MakeStopNodes(const Vector4& startColor = Color::RED, const Vector4& endColor = Color::BLUE)
{
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, UiColor(startColor)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, UiColor(endColor)));
  return stopNodes;
}

void ExpectStopNode(const Dali::Vector<Gradient::StopNode>& stopNodes, uint32_t index, float offset, const Vector4& color)
{
  DALI_TEST_EQUALS(stopNodes.Count() > index, true, TEST_LOCATION);
  DALI_TEST_EQUALS(stopNodes[index].GetOffset(), offset, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stopNodes[index].GetColor().GetRgba(), color, TEST_LOCATION);
}

void ExpectCommonProperties(const Gradient::Base& gradient, Gradient::Units units, Gradient::SpreadMethod spreadMethod, float startOffset, const Vector4& startColor, const Vector4& endColor)
{
  DALI_TEST_EQUALS(gradient.GetUnits(), units, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetSpreadMethod(), spreadMethod, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetStartOffset(), startOffset, EPSILON, TEST_LOCATION);

  const Dali::Vector<Gradient::StopNode> stopNodes = gradient.GetStopNodes();
  DALI_TEST_EQUALS(stopNodes.Count(), 2u, TEST_LOCATION);
  ExpectStopNode(stopNodes, 0u, 0.0f, startColor);
  ExpectStopNode(stopNodes, 1u, 1.0f, endColor);
}
} // unnamed namespace

void utc_dali_gradient_value_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gradient_value_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGradientTypeEnumP(void)
{
  UiTestApplication application;

  DALI_TEST_EQUALS(static_cast<int>(Gradient::Type::NONE), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(Gradient::Type::LINEAR), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(Gradient::Type::RADIAL), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(static_cast<int>(Gradient::Type::CONIC), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientBaseDefaultP(void)
{
  UiTestApplication application;

  Gradient::Base base;

  DALI_TEST_EQUALS(base.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(base.GetType() == Gradient::Type::NONE);
  DALI_TEST_EQUALS(base.GetUnits(), Gradient::Units::OBJECT_BOUNDING_BOX, TEST_LOCATION);
  DALI_TEST_EQUALS(base.GetSpreadMethod(), Gradient::SpreadMethod::PAD, TEST_LOCATION);
  DALI_TEST_EQUALS(base.GetStartOffset(), 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(base.GetStopNodes().Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientCommonSetterOnNoneP(void)
{
  UiTestApplication application;

  Gradient::Base base;
  base.SetStopNodes(MakeStopNodes());
  base.SetUnits(Gradient::Units::USER_SPACE);
  base.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  base.SetStartOffset(0.25f);

  DALI_TEST_EQUALS(base.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(base.GetType() == Gradient::Type::NONE);
  DALI_TEST_EQUALS(base.GetUnits(), Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(base.GetSpreadMethod(), Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(base.GetStartOffset(), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(base.GetStopNodes().Count(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientTypedDefaultConstructorP(void)
{
  UiTestApplication application;

  Gradient::Linear linear;
  DALI_TEST_EQUALS(linear.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(linear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(linear.GetStartPosition(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), Vector2::ONE, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetStopNodes().Count(), 0u, TEST_LOCATION);

  Gradient::Radial radial;
  DALI_TEST_EQUALS(radial.GetType(), Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_CHECK(radial.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(radial.GetCenter(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetRadius(), 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetStopNodes().Count(), 0u, TEST_LOCATION);

  Gradient::Conic conic;
  DALI_TEST_EQUALS(conic.GetType(), Gradient::Type::CONIC, TEST_LOCATION);
  DALI_TEST_CHECK(conic.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(conic.GetCenter(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartAngle(), Radian(0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStopNodes().Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientTypedValueConstructorP(void)
{
  UiTestApplication application;

  const Vector2 linearStart(1.0f, 2.0f);
  const Vector2 linearEnd(3.0f, 4.0f);
  Gradient::Linear linear(linearStart, linearEnd);
  DALI_TEST_EQUALS(linear.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(linear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(linear.GetStartPosition(), linearStart, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), linearEnd, TEST_LOCATION);

  const Vector2 radialCenter(5.0f, 6.0f);
  const float   radius = 7.0f;
  Gradient::Radial radial(radialCenter, radius);
  DALI_TEST_EQUALS(radial.GetType(), Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_CHECK(radial.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(radial.GetCenter(), radialCenter, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetRadius(), radius, EPSILON, TEST_LOCATION);

  const Vector2 conicCenter(8.0f, 9.0f);
  const Radian  startAngle(Math::PI_2);
  Gradient::Conic conic(conicCenter, startAngle);
  DALI_TEST_EQUALS(conic.GetType(), Gradient::Type::CONIC, TEST_LOCATION);
  DALI_TEST_CHECK(conic.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(conic.GetCenter(), conicCenter, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartAngle(), startAngle, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientCommonPropertiesP(void)
{
  UiTestApplication application;

  Gradient::Linear linear(Vector2::ZERO, Vector2::ONE);

  Dali::Vector<Gradient::StopNode> sourceStops = MakeStopNodes(Color::GREEN, Color::YELLOW);
  linear.SetUnits(Gradient::Units::USER_SPACE);
  linear.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  linear.SetStartOffset(0.25f);
  linear.SetStopNodes(sourceStops);

  sourceStops[0].SetColor(UiColor(Color::RED));
  ExpectCommonProperties(linear, Gradient::Units::USER_SPACE, Gradient::SpreadMethod::REFLECT, 0.25f, Color::GREEN, Color::YELLOW);

  Dali::Vector<Gradient::StopNode> readStops = linear.GetStopNodes();
  readStops[1].SetColor(UiColor(Color::BLUE));
  ExpectCommonProperties(linear, Gradient::Units::USER_SPACE, Gradient::SpreadMethod::REFLECT, 0.25f, Color::GREEN, Color::YELLOW);

  linear.SetSpreadMethod(Gradient::SpreadMethod::REPEAT);
  DALI_TEST_EQUALS(linear.GetSpreadMethod(), Gradient::SpreadMethod::REPEAT, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientBaseCopyFromLinearPreservesGeometryP(void)
{
  UiTestApplication application;

  const Vector2 startPosition(-0.5f, 0.0f);
  const Vector2 endPosition(0.5f, 1.0f);

  Gradient::Base base = Gradient::Linear(startPosition, endPosition);

  DALI_TEST_EQUALS(base.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(base.GetType() != Gradient::Type::NONE);

  auto linear = Gradient::Linear::DownCast(base);
  DALI_TEST_CHECK(linear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(linear.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetStartPosition(), startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), endPosition, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientDownCastP(void)
{
  UiTestApplication application;

  Gradient::Linear source(Vector2(1.0f, 2.0f), Vector2(3.0f, 4.0f));
  source.SetUnits(Gradient::Units::USER_SPACE);
  source.SetSpreadMethod(Gradient::SpreadMethod::REPEAT);
  source.SetStartOffset(0.5f);
  source.SetStopNodes({{0.4f, UiColor(Color::GREEN)}});

  Gradient::Base base = source;

  auto linear = Gradient::Linear::DownCast(base);
  DALI_TEST_CHECK(linear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(linear.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetStartPosition(), Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), Vector2(3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetUnits(), Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetSpreadMethod(), Gradient::SpreadMethod::REPEAT, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetStartOffset(), 0.5f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetStopNodes().Count(), 1u, TEST_LOCATION);
  ExpectStopNode(linear.GetStopNodes(), 0u, 0.4f, Color::GREEN);

  auto radial = Gradient::Radial::DownCast(base);
  DALI_TEST_CHECK(radial.GetType() == Gradient::Type::NONE);
  DALI_TEST_EQUALS(radial.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetCenter(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetRadius(), 0.0f, EPSILON, TEST_LOCATION);

  auto conic = Gradient::Conic::DownCast(base);
  DALI_TEST_CHECK(conic.GetType() == Gradient::Type::NONE);
  DALI_TEST_EQUALS(conic.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetCenter(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartAngle(), Radian(0.0f), TEST_LOCATION);

  DALI_TEST_EQUALS(base.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(base.GetType() != Gradient::Type::NONE);

  END_TEST;
}

int UtcDaliGradientCopyIndependenceP(void)
{
  UiTestApplication application;

  const Vector2 originalStart(-1.0f, -2.0f);
  const Vector2 originalEnd(1.0f, 2.0f);
  Gradient::Linear original(originalStart, originalEnd);
  original.SetUnits(Gradient::Units::USER_SPACE);
  original.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  original.SetStartOffset(0.33f);
  original.SetStopNodes(MakeStopNodes(Color::RED, Color::BLUE));

  Gradient::Base copied = original;

  original.SetStartAndEndPosition(Vector2(10.0f, 20.0f), Vector2(30.0f, 40.0f));
  original.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  original.SetSpreadMethod(Gradient::SpreadMethod::REPEAT);
  original.SetStartOffset(0.66f);
  original.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));

  auto copiedLinear = Gradient::Linear::DownCast(copied);
  DALI_TEST_CHECK(copiedLinear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(copiedLinear.GetStartPosition(), originalStart, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedLinear.GetEndPosition(), originalEnd, TEST_LOCATION);
  ExpectCommonProperties(copiedLinear, Gradient::Units::USER_SPACE, Gradient::SpreadMethod::REFLECT, 0.33f, Color::RED, Color::BLUE);

  END_TEST;
}

int UtcDaliGradientMoveP(void)
{
  UiTestApplication application;

  Gradient::Linear source(Vector2(1.0f, 2.0f), Vector2(3.0f, 4.0f));
  source.SetUnits(Gradient::Units::USER_SPACE);
  source.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  source.SetStartOffset(0.75f);
  source.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));

  Gradient::Base moved(std::move(source));
  DALI_TEST_EQUALS(moved.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(moved.GetType() != Gradient::Type::NONE);

  auto movedLinear = Gradient::Linear::DownCast(moved);
  DALI_TEST_CHECK(movedLinear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(movedLinear.GetStartPosition(), Vector2(1.0f, 2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(movedLinear.GetEndPosition(), Vector2(3.0f, 4.0f), TEST_LOCATION);
  ExpectCommonProperties(movedLinear, Gradient::Units::USER_SPACE, Gradient::SpreadMethod::REFLECT, 0.75f, Color::GREEN, Color::YELLOW);

  source = Gradient::Linear(Vector2(5.0f, 6.0f), Vector2(7.0f, 8.0f));
  DALI_TEST_EQUALS(source.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(source.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(source.GetStartPosition(), Vector2(5.0f, 6.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(source.GetEndPosition(), Vector2(7.0f, 8.0f), TEST_LOCATION);

  Gradient::Base assigned;
  assigned = std::move(moved);
  DALI_TEST_EQUALS(assigned.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(assigned.GetType() != Gradient::Type::NONE);

  Gradient::Radial radialSource(Vector2(2.0f, 3.0f), 4.0f);
  Gradient::Base   radialMoved(std::move(radialSource));
  DALI_TEST_EQUALS(radialMoved.GetType(), Gradient::Type::RADIAL, TEST_LOCATION);

  radialSource = Gradient::Radial(Vector2(9.0f, 10.0f), -3.0f);
  DALI_TEST_EQUALS(radialSource.GetType(), Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_CHECK(radialSource.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(radialSource.GetCenter(), Vector2(9.0f, 10.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(radialSource.GetRadius(), -3.0f, EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientMovedFromBaseAccessN(void)
{
  UiTestApplication application;

  Gradient::Base source = Gradient::Linear(Vector2::ZERO, Vector2::ONE);
  Gradient::Base moved(std::move(source));
  (void)moved;

  DALI_TEST_ASSERTION(source.GetType(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(source.GetUnits(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(source.GetStopNodes(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(source.SetUnits(Gradient::Units::USER_SPACE), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(source.SetStopNodes(MakeStopNodes()), MOVED_FROM_GRADIENT_ASSERTION);

  END_TEST;
}

int UtcDaliGradientMovedFromTypedAccessN(void)
{
  UiTestApplication application;

  Gradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  Gradient::Linear movedLinear(std::move(linear));
  (void)movedLinear;

  DALI_TEST_ASSERTION(linear.GetStartPosition(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(linear.GetEndPosition(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(linear.SetStartAndEndPosition(Vector2(1.0f, 2.0f), Vector2(3.0f, 4.0f)), MOVED_FROM_GRADIENT_ASSERTION);

  Gradient::Radial radial(Vector2::ZERO, 1.0f);
  Gradient::Radial movedRadial(std::move(radial));
  (void)movedRadial;

  DALI_TEST_ASSERTION(radial.GetCenter(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(radial.GetRadius(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(radial.SetCenterAndRadius(Vector2(1.0f, 2.0f), 3.0f), MOVED_FROM_GRADIENT_ASSERTION);

  Gradient::Conic conic(Vector2::ZERO, Radian(0.5f));
  Gradient::Conic movedConic(std::move(conic));
  (void)movedConic;

  DALI_TEST_ASSERTION(conic.GetCenter(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(conic.GetStartAngle(), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(conic.SetCenterAndStartAngle(Vector2(1.0f, 2.0f), Radian(1.0f)), MOVED_FROM_GRADIENT_ASSERTION);

  END_TEST;
}

int UtcDaliGradientMovedFromCopyN(void)
{
  UiTestApplication application;

  Gradient::Base source = Gradient::Linear(Vector2::ZERO, Vector2::ONE);
  Gradient::Base moved(std::move(source));
  (void)moved;

  DALI_TEST_ASSERTION(Gradient::Base copied(source), MOVED_FROM_GRADIENT_ASSERTION);

  Gradient::Base target;
  DALI_TEST_ASSERTION(target = source, MOVED_FROM_GRADIENT_ASSERTION);

  DALI_TEST_EQUALS(target.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(target.GetType() == Gradient::Type::NONE);

  END_TEST;
}

int UtcDaliGradientMovedFromDownCastN(void)
{
  UiTestApplication application;

  Gradient::Base source = Gradient::Linear(Vector2::ZERO, Vector2::ONE);
  Gradient::Base moved(std::move(source));
  (void)moved;

  DALI_TEST_ASSERTION(Gradient::Linear::DownCast(source), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(Gradient::Radial::DownCast(source), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(Gradient::Conic::DownCast(source), MOVED_FROM_GRADIENT_ASSERTION);

  END_TEST;
}

int UtcDaliGradientInitializerListStopNodesP(void)
{
  UiTestApplication application;

  Gradient::Base base;
  base.SetStopNodes({
    {0.25f, UiColor(Color::RED)},
    {0.75f, UiColor(Color::BLUE)},
  });

  DALI_TEST_EQUALS(base.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(base.GetType() == Gradient::Type::NONE);

  const Dali::Vector<Gradient::StopNode> stopNodes = base.GetStopNodes();
  DALI_TEST_EQUALS(stopNodes.Count(), 2u, TEST_LOCATION);
  ExpectStopNode(stopNodes, 0u, 0.25f, Color::RED);
  ExpectStopNode(stopNodes, 1u, 0.75f, Color::BLUE);

  END_TEST;
}

int UtcDaliGradientInvalidGeometryDoesNotRejectP(void)
{
  UiTestApplication application;

  Gradient::Linear linear(Vector2::ZERO, Vector2::ZERO);
  DALI_TEST_EQUALS(linear.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(linear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(linear.GetStartPosition(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetStopNodes().Count(), 0u, TEST_LOCATION);

  Gradient::Radial zeroRadiusRadial(Vector2::ZERO, 0.0f);
  DALI_TEST_EQUALS(zeroRadiusRadial.GetType(), Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_CHECK(zeroRadiusRadial.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(zeroRadiusRadial.GetCenter(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(zeroRadiusRadial.GetRadius(), 0.0f, EPSILON, TEST_LOCATION);

  Gradient::Radial negativeRadiusRadial(Vector2::ZERO, -1.0f);
  DALI_TEST_EQUALS(negativeRadiusRadial.GetType(), Gradient::Type::RADIAL, TEST_LOCATION);
  DALI_TEST_CHECK(negativeRadiusRadial.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(negativeRadiusRadial.GetCenter(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(negativeRadiusRadial.GetRadius(), -1.0f, EPSILON, TEST_LOCATION);

  const Radian negativeAngle(-Math::PI_2);
  Gradient::Conic conic(Vector2::ZERO, negativeAngle);
  DALI_TEST_EQUALS(conic.GetType(), Gradient::Type::CONIC, TEST_LOCATION);
  DALI_TEST_CHECK(conic.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(conic.GetCenter(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartAngle(), negativeAngle, TEST_LOCATION);

  END_TEST;
}
