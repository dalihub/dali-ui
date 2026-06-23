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

#include <dali-ui-foundation/internal/visuals/gradient/conic-gradient.h>
#include <dali-ui-foundation/internal/visuals/gradient/gradient-factory.h>
#include <dali-ui-foundation/internal/visuals/gradient/linear-gradient.h>
#include <dali-ui-foundation/internal/visuals/gradient/radial-gradient.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#include <utility>

using namespace Dali;

namespace
{

namespace PublicGradient = Dali::Ui::Gradient;
namespace GradientFactory = Dali::Ui::Internal::GradientFactory;

using InternalGradient = Dali::Ui::Internal::Gradient;

constexpr float EPSILON = 0.001f;
constexpr char  MOVED_FROM_GRADIENT_ASSERTION[] = "Cannot use a moved-from Gradient::Base object";

Dali::Vector<PublicGradient::StopNode> MakeStopNodes(const Vector4& startColor = Color::RED, const Vector4& endColor = Color::BLUE)
{
  Dali::Vector<PublicGradient::StopNode> stopNodes;
  stopNodes.PushBack(PublicGradient::StopNode(0.0f, Dali::Ui::UiColor(startColor)));
  stopNodes.PushBack(PublicGradient::StopNode(1.0f, Dali::Ui::UiColor(endColor)));
  return stopNodes;
}

Dali::Vector<PublicGradient::StopNode> MakeStopNodes(float firstOffset, const Vector4& firstColor, float secondOffset, const Vector4& secondColor)
{
  Dali::Vector<PublicGradient::StopNode> stopNodes;
  stopNodes.PushBack(PublicGradient::StopNode(firstOffset, Dali::Ui::UiColor(firstColor)));
  stopNodes.PushBack(PublicGradient::StopNode(secondOffset, Dali::Ui::UiColor(secondColor)));
  return stopNodes;
}

void ExpectInternalStop(const Vector<InternalGradient::GradientStop>& stops, uint32_t index, float offset, const Vector4& color)
{
  DALI_TEST_EQUALS(stops.Count() > index, true, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[index].mOffset, offset, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[index].mStopColor, color, TEST_LOCATION);
}

void ExpectPublicStop(const Dali::Vector<PublicGradient::StopNode>& stops, uint32_t index, float offset, const Vector4& color)
{
  DALI_TEST_EQUALS(stops.Count() > index, true, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[index].GetOffset(), offset, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[index].GetColor().GetRgba(), color, TEST_LOCATION);
}

void ExpectCommonProperties(const InternalGradient& gradient, PublicGradient::Units units, PublicGradient::SpreadMethod spreadMethod, float startOffset)
{
  DALI_TEST_EQUALS(gradient.GetUnits(), units, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetSpreadMethod(), spreadMethod, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetStartOffset(), startOffset, EPSILON, TEST_LOCATION);
}

PublicGradient::Linear MakeRenderableLinear(const Vector2& startPosition = Vector2::ZERO, const Vector2& endPosition = Vector2::ONE)
{
  PublicGradient::Linear linear(startPosition, endPosition);
  linear.SetStopNodes(MakeStopNodes());
  return linear;
}

} // namespace

void utc_dali_gradient_factory_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gradient_factory_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGradientFactoryNoneReturnsNullP(void)
{
  PublicGradient::Base base;

  DALI_TEST_CHECK(!GradientFactory::IsRenderable(base));
  DALI_TEST_CHECK(!GradientFactory::Create(base));

  END_TEST;
}

int UtcDaliGradientFactoryInsufficientStopsReturnsNullP(void)
{
  PublicGradient::Linear linear;

  DALI_TEST_EQUALS(linear.GetType(), PublicGradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(!GradientFactory::IsRenderable(linear));
  DALI_TEST_CHECK(!GradientFactory::Create(linear));

  Dali::Vector<PublicGradient::StopNode> singleStop;
  singleStop.PushBack(PublicGradient::StopNode(0.5f, Dali::Ui::UiColor(Color::GREEN)));
  linear.SetStopNodes(singleStop);

  DALI_TEST_EQUALS(linear.GetType(), PublicGradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_CHECK(!GradientFactory::IsRenderable(linear));
  DALI_TEST_CHECK(!GradientFactory::Create(linear));

  END_TEST;
}

int UtcDaliGradientFactoryLinearCreatesInternalLinearP(void)
{
  const Vector2 startPosition(-0.25f, 0.5f);
  const Vector2 endPosition(1.25f, 2.0f);

  PublicGradient::Linear linear(startPosition, endPosition);
  linear.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));

  IntrusivePtr<InternalGradient> gradient = GradientFactory::Create(linear);
  DALI_TEST_CHECK(gradient);

  auto* internalLinear = dynamic_cast<Dali::Ui::Internal::LinearGradient*>(gradient.Get());
  DALI_TEST_CHECK(internalLinear);
  DALI_TEST_EQUALS(internalLinear->GetStartPosition(), startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(internalLinear->GetEndPosition(), endPosition, TEST_LOCATION);

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  ExpectInternalStop(stops, 0u, 0.0f, Color::GREEN);
  ExpectInternalStop(stops, 1u, 1.0f, Color::YELLOW);

  END_TEST;
}

int UtcDaliGradientFactoryRadialCreatesInternalRadialP(void)
{
  const Vector2 center(0.25f, 0.75f);
  const float   radius = 1.5f;

  PublicGradient::Radial radial(center, radius);
  radial.SetUnits(PublicGradient::Units::USER_SPACE);
  radial.SetSpreadMethod(PublicGradient::SpreadMethod::REFLECT);
  radial.SetStartOffset(0.25f);
  radial.SetStopNodes(MakeStopNodes(Color::RED, Color::BLUE));

  IntrusivePtr<InternalGradient> gradient = GradientFactory::Create(radial);
  DALI_TEST_CHECK(gradient);

  auto* internalRadial = dynamic_cast<Dali::Ui::Internal::RadialGradient*>(gradient.Get());
  DALI_TEST_CHECK(internalRadial);
  DALI_TEST_EQUALS(internalRadial->GetCenter(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(internalRadial->GetRadius(), radius, EPSILON, TEST_LOCATION);
  ExpectCommonProperties(*gradient, PublicGradient::Units::USER_SPACE, PublicGradient::SpreadMethod::REFLECT, 0.25f);

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  ExpectInternalStop(stops, 0u, 0.0f, Color::RED);
  ExpectInternalStop(stops, 1u, 1.0f, Color::BLUE);

  END_TEST;
}

int UtcDaliGradientFactoryConicCreatesInternalConicP(void)
{
  const Vector2 center(0.5f, 0.75f);
  const Radian  authoredAngle(Math::PI * 2.5f);

  PublicGradient::Conic conic(center, authoredAngle);
  conic.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));

  IntrusivePtr<InternalGradient> gradient = GradientFactory::Create(conic);
  DALI_TEST_CHECK(gradient);

  auto* internalConic = dynamic_cast<Dali::Ui::Internal::ConicGradient*>(gradient.Get());
  DALI_TEST_CHECK(internalConic);
  DALI_TEST_EQUALS(internalConic->GetCenter(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(internalConic->GetStartAngle(), Dali::ANGLE_90, TEST_LOCATION);

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  ExpectInternalStop(stops, 0u, 0.0f, Color::GREEN);
  ExpectInternalStop(stops, 1u, 1.0f, Color::YELLOW);

  END_TEST;
}

int UtcDaliGradientFactoryCopiesCommonPropertiesP(void)
{
  PublicGradient::Linear linear = MakeRenderableLinear();
  linear.SetUnits(PublicGradient::Units::USER_SPACE);
  linear.SetSpreadMethod(PublicGradient::SpreadMethod::REPEAT);
  linear.SetStartOffset(1.25f);

  IntrusivePtr<InternalGradient> gradient = GradientFactory::Create(linear);
  DALI_TEST_CHECK(gradient);
  ExpectCommonProperties(*gradient, PublicGradient::Units::USER_SPACE, PublicGradient::SpreadMethod::REPEAT, 1.25f);

  linear.SetSpreadMethod(PublicGradient::SpreadMethod::REFLECT);
  linear.SetStartOffset(-0.5f);

  IntrusivePtr<InternalGradient> negativeOffsetGradient = GradientFactory::Create(linear);
  DALI_TEST_CHECK(negativeOffsetGradient);
  ExpectCommonProperties(*negativeOffsetGradient, PublicGradient::Units::USER_SPACE, PublicGradient::SpreadMethod::REFLECT, -0.5f);

  END_TEST;
}

int UtcDaliGradientFactoryCopiesStopsP(void)
{
  PublicGradient::Linear linear = MakeRenderableLinear();
  linear.SetStopNodes(MakeStopNodes(1.25f, Color::GREEN, -0.25f, Color::YELLOW));

  IntrusivePtr<InternalGradient> gradient = GradientFactory::Create(linear);
  DALI_TEST_CHECK(gradient);

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  ExpectInternalStop(stops, 0u, 1.0f, Color::GREEN);
  ExpectInternalStop(stops, 1u, 0.0f, Color::YELLOW);

  const Dali::Vector<PublicGradient::StopNode> publicStops = linear.GetStopNodes();
  DALI_TEST_EQUALS(publicStops.Count(), 2u, TEST_LOCATION);
  ExpectPublicStop(publicStops, 0u, 1.25f, Color::GREEN);
  ExpectPublicStop(publicStops, 1u, -0.25f, Color::YELLOW);

  END_TEST;
}

int UtcDaliGradientFactoryInvalidGeometryIsNotRejectedP(void)
{
  PublicGradient::Linear linear(Vector2(1.0f, 1.0f), Vector2(1.0f, 1.0f));
  linear.SetStopNodes(MakeStopNodes());

  IntrusivePtr<InternalGradient> linearGradient = GradientFactory::Create(linear);
  DALI_TEST_CHECK(linearGradient);

  auto* internalLinear = dynamic_cast<Dali::Ui::Internal::LinearGradient*>(linearGradient.Get());
  DALI_TEST_CHECK(internalLinear);
  DALI_TEST_EQUALS(internalLinear->GetStartPosition(), Vector2(1.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(internalLinear->GetEndPosition(), Vector2(1.0f, 1.0f), TEST_LOCATION);

  PublicGradient::Radial zeroRadius(Vector2(2.0f, 3.0f), 0.0f);
  zeroRadius.SetStopNodes(MakeStopNodes());

  IntrusivePtr<InternalGradient> zeroRadiusGradient = GradientFactory::Create(zeroRadius);
  DALI_TEST_CHECK(zeroRadiusGradient);

  auto* internalZeroRadius = dynamic_cast<Dali::Ui::Internal::RadialGradient*>(zeroRadiusGradient.Get());
  DALI_TEST_CHECK(internalZeroRadius);
  DALI_TEST_EQUALS(internalZeroRadius->GetCenter(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(internalZeroRadius->GetRadius(), 0.0f, EPSILON, TEST_LOCATION);

  PublicGradient::Radial negativeRadius(Vector2(-1.0f, -2.0f), -3.0f);
  negativeRadius.SetStopNodes(MakeStopNodes());

  IntrusivePtr<InternalGradient> negativeRadiusGradient = GradientFactory::Create(negativeRadius);
  DALI_TEST_CHECK(negativeRadiusGradient);

  auto* internalNegativeRadius = dynamic_cast<Dali::Ui::Internal::RadialGradient*>(negativeRadiusGradient.Get());
  DALI_TEST_CHECK(internalNegativeRadius);
  DALI_TEST_EQUALS(internalNegativeRadius->GetCenter(), Vector2(-1.0f, -2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(internalNegativeRadius->GetRadius(), -3.0f, EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGradientFactorySourceUnchangedP(void)
{
  const Vector2 center(0.25f, 0.5f);
  const Radian  authoredAngle(-Math::PI_2);

  PublicGradient::Conic conic(center, authoredAngle);
  conic.SetUnits(PublicGradient::Units::USER_SPACE);
  conic.SetSpreadMethod(PublicGradient::SpreadMethod::REFLECT);
  conic.SetStartOffset(-0.25f);
  conic.SetStopNodes(MakeStopNodes(0.2f, Color::RED, 0.8f, Color::BLUE));

  IntrusivePtr<InternalGradient> gradient = GradientFactory::Create(conic);
  DALI_TEST_CHECK(gradient);

  DALI_TEST_EQUALS(conic.GetType(), PublicGradient::Type::CONIC, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetUnits(), PublicGradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetSpreadMethod(), PublicGradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartOffset(), -0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetCenter(), center, TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartAngle(), authoredAngle, TEST_LOCATION);

  const Dali::Vector<PublicGradient::StopNode> stopNodes = conic.GetStopNodes();
  DALI_TEST_EQUALS(stopNodes.Count(), 2u, TEST_LOCATION);
  ExpectPublicStop(stopNodes, 0u, 0.2f, Color::RED);
  ExpectPublicStop(stopNodes, 1u, 0.8f, Color::BLUE);

  END_TEST;
}

int UtcDaliGradientFactoryMovedFromSourceAssertsN(void)
{
  PublicGradient::Base source = MakeRenderableLinear();
  PublicGradient::Base moved(std::move(source));
  DALI_TEST_CHECK(moved.GetType() != PublicGradient::Type::NONE);

  DALI_TEST_ASSERTION(GradientFactory::IsRenderable(source), MOVED_FROM_GRADIENT_ASSERTION);
  DALI_TEST_ASSERTION(GradientFactory::Create(source), MOVED_FROM_GRADIENT_ASSERTION);

  END_TEST;
}
