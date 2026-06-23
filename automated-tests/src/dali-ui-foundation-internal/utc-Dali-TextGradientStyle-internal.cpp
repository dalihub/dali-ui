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

#include <dali-ui-foundation/internal/text/text-gradient-style.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#include <utility>

using namespace Dali;

namespace
{

namespace PublicGradient = Dali::Ui::Gradient;
namespace TextInternal   = Dali::Ui::Text::Internal;

constexpr float EPSILON = 0.001f;
constexpr char  MOVED_FROM_GRADIENT_ASSERTION[] = "Cannot use a moved-from Gradient::Base object";

Dali::Vector<PublicGradient::StopNode> MakeStopNodes(const Vector4& startColor = Color::RED, const Vector4& endColor = Color::BLUE)
{
  Dali::Vector<PublicGradient::StopNode> stopNodes;
  stopNodes.PushBack(PublicGradient::StopNode(0.0f, Dali::Ui::UiColor(startColor)));
  stopNodes.PushBack(PublicGradient::StopNode(1.0f, Dali::Ui::UiColor(endColor)));
  return stopNodes;
}

void ExpectDisabled(const TextInternal::TextGradientStyle& style)
{
  DALI_TEST_EQUALS(style.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(style.type, PublicGradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops.Count(), 0u, TEST_LOCATION);
}

void ExpectCommonProperties(const TextInternal::TextGradientStyle& style,
                            PublicGradient::Type                   type,
                            PublicGradient::Units                  units,
                            PublicGradient::SpreadMethod           spreadMethod,
                            float                                  startOffset,
                            const Vector4&                         startColor,
                            const Vector4&                         endColor)
{
  DALI_TEST_EQUALS(style.enabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(style.type, type, TEST_LOCATION);
  DALI_TEST_EQUALS(style.units, units, TEST_LOCATION);
  DALI_TEST_EQUALS(style.spreadMethod, spreadMethod, TEST_LOCATION);
  DALI_TEST_EQUALS(style.startOffset, startOffset, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops[0u].offset, 0.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops[0u].color, startColor, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops[1u].offset, 1.0f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops[1u].color, endColor, TEST_LOCATION);
}

void SetCommonProperties(PublicGradient::Base& gradient, const Vector4& startColor = Color::GREEN, const Vector4& endColor = Color::YELLOW)
{
  gradient.SetUnits(PublicGradient::Units::USER_SPACE);
  gradient.SetSpreadMethod(PublicGradient::SpreadMethod::REFLECT);
  gradient.SetStartOffset(0.25f);
  gradient.SetStopNodes(MakeStopNodes(startColor, endColor));
}

} // namespace

void utc_dali_text_gradient_style_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_gradient_style_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliTextGradientStyleDefaultDisabledP(void)
{
  TextInternal::TextGradientStyle style;
  ExpectDisabled(style);
  END_TEST;
}

int UtcDaliTextGradientStyleNoneConvertsDisabledP(void)
{
  PublicGradient::Base base;
  base.SetStopNodes(MakeStopNodes());

  const auto style = TextInternal::CreateTextGradientStyle(base);

  ExpectDisabled(style);
  END_TEST;
}

int UtcDaliTextGradientStyleInsufficientStopsConvertsDisabledP(void)
{
  PublicGradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  ExpectDisabled(TextInternal::CreateTextGradientStyle(linear));

  Dali::Vector<PublicGradient::StopNode> singleStop;
  singleStop.PushBack(PublicGradient::StopNode(0.5f, Dali::Ui::UiColor(Color::GREEN)));
  linear.SetStopNodes(singleStop);

  ExpectDisabled(TextInternal::CreateTextGradientStyle(linear));
  END_TEST;
}

int UtcDaliTextGradientStyleLinearSnapshotP(void)
{
  const Vector2 startPosition(-0.25f, 0.5f);
  const Vector2 endPosition(1.25f, 2.0f);

  PublicGradient::Linear linear(startPosition, endPosition);
  SetCommonProperties(linear);

  const auto style = TextInternal::CreateTextGradientStyle(linear);

  ExpectCommonProperties(style, PublicGradient::Type::LINEAR, PublicGradient::Units::USER_SPACE, PublicGradient::SpreadMethod::REFLECT, 0.25f, Color::GREEN, Color::YELLOW);
  DALI_TEST_EQUALS(style.linearStart, startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(style.linearEnd, endPosition, TEST_LOCATION);

  linear.SetStartAndEndPosition(Vector2(9.0f, 8.0f), Vector2(7.0f, 6.0f));
  linear.SetStopNodes(MakeStopNodes(Color::RED, Color::BLUE));

  DALI_TEST_EQUALS(style.linearStart, startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(style.linearEnd, endPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops[0u].color, Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops[1u].color, Color::YELLOW, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientStyleRadialSnapshotP(void)
{
  const Vector2 center(0.25f, 0.75f);
  const float   radius = 0.5f;
  const Vector4 startColor(0.0f, 1.0f, 1.0f, 1.0f);
  const Vector4 endColor(1.0f, 0.0f, 1.0f, 1.0f);

  PublicGradient::Radial radial(center, radius);
  SetCommonProperties(radial, startColor, endColor);

  const auto style = TextInternal::CreateTextGradientStyle(radial);

  ExpectCommonProperties(style, PublicGradient::Type::RADIAL, PublicGradient::Units::USER_SPACE, PublicGradient::SpreadMethod::REFLECT, 0.25f, startColor, endColor);
  DALI_TEST_EQUALS(style.radialCenter, center, TEST_LOCATION);
  DALI_TEST_EQUALS(style.radialRadius, radius, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientStyleConicSnapshotP(void)
{
  const Vector2 center(0.75f, 0.25f);
  const Radian  startAngle(Math::PI * 3.0f);

  PublicGradient::Conic conic(center, startAngle);
  SetCommonProperties(conic, Color::WHITE, Color::BLACK);

  const auto style = TextInternal::CreateTextGradientStyle(conic);

  ExpectCommonProperties(style, PublicGradient::Type::CONIC, PublicGradient::Units::USER_SPACE, PublicGradient::SpreadMethod::REFLECT, 0.25f, Color::WHITE, Color::BLACK);
  DALI_TEST_EQUALS(style.conicCenter, center, TEST_LOCATION);
  DALI_TEST_EQUALS(style.conicStartAngle, startAngle, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextGradientStyleMovedFromInputN(void)
{
  PublicGradient::Linear source(Vector2::ZERO, Vector2::ONE);
  SetCommonProperties(source);

  PublicGradient::Base moved(std::move(source));
  DALI_TEST_CHECK(moved.GetType() != PublicGradient::Type::NONE);

  DALI_TEST_ASSERTION(TextInternal::CreateTextGradientStyle(source), MOVED_FROM_GRADIENT_ASSERTION);
  END_TEST;
}
