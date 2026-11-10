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

#include <limits>
#include <dali-ui-foundation/internal/text/text-gradient-helper.h>
#include <dali-ui-foundation/internal/text/text-gradient-bounds.h>
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

void ExpectDisabled(const TextInternal::Gradient::Style& style)
{
  DALI_TEST_EQUALS(style.enabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(style.type, PublicGradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(style.stops.Count(), 0u, TEST_LOCATION);
}

void ExpectCommonProperties(const TextInternal::Gradient::Style& style,
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
  TextInternal::Gradient::Style style;
  ExpectDisabled(style);
  END_TEST;
}

int UtcDaliTextGradientStyleNoneConvertsDisabledP(void)
{
  PublicGradient::Base base;
  base.SetStopNodes(MakeStopNodes());

  const auto style = TextInternal::Gradient::CreateStyle(base);

  ExpectDisabled(style);
  END_TEST;
}

int UtcDaliTextGradientStyleInsufficientStopsConvertsDisabledP(void)
{
  PublicGradient::Linear linear(Vector2::ZERO, Vector2::ONE);
  ExpectDisabled(TextInternal::Gradient::CreateStyle(linear));

  Dali::Vector<PublicGradient::StopNode> singleStop;
  singleStop.PushBack(PublicGradient::StopNode(0.5f, Dali::Ui::UiColor(Color::GREEN)));
  linear.SetStopNodes(singleStop);

  ExpectDisabled(TextInternal::Gradient::CreateStyle(linear));
  END_TEST;
}

int UtcDaliTextGradientStyleLinearSnapshotP(void)
{
  const Vector2 startPosition(-0.25f, 0.5f);
  const Vector2 endPosition(1.25f, 2.0f);

  PublicGradient::Linear linear(startPosition, endPosition);
  SetCommonProperties(linear);

  const auto style = TextInternal::Gradient::CreateStyle(linear);

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

  const auto style = TextInternal::Gradient::CreateStyle(radial);

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

  const auto style = TextInternal::Gradient::CreateStyle(conic);

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

  DALI_TEST_ASSERTION(TextInternal::Gradient::CreateStyle(source), MOVED_FROM_GRADIENT_ASSERTION);
  END_TEST;
}

int UtcDaliTextGradientUiScaleFiniteGeometryP(void)
{
  const Vector4 bounds(0.0f, 0.0f, 1.0f, 1.0f);
  const Vector2 display(400.0f, 120.0f);
  PublicGradient::Radial gradient(Vector2(30.0f, 20.0f), 0.125f);
  SetCommonProperties(gradient);
  auto style = TextInternal::Gradient::CreateStyle(gradient);
  for(float scale : {0.5f, 1.0f, 1.25f, 2.0f, std::numeric_limits<float>::max()})
  {
    auto size = TextInternal::ResolveGradientCoordinateSize(style.units, display, scale);
    auto data = TextInternal::Gradient::ResolveRenderData(style, bounds, size);
    DALI_TEST_CHECK(data.enabled);
    DALI_TEST_CHECK(std::isfinite(data.radialScale.x) && std::isfinite(data.radialScale.y));
  }
  auto size = TextInternal::ResolveGradientCoordinateSize(style.units, display, std::numeric_limits<float>::min());
  DALI_TEST_CHECK(!TextInternal::Gradient::ResolveRenderData(style, bounds, size).enabled);
  gradient = PublicGradient::Radial(Vector2(30.0f, 20.0f), 0.001f);
  SetCommonProperties(gradient);
  style = TextInternal::Gradient::CreateStyle(gradient);
  size = TextInternal::ResolveGradientCoordinateSize(style.units, display, 1.0e-35f);
  DALI_TEST_CHECK(!TextInternal::Gradient::ResolveRenderData(style, bounds, size).enabled);
  gradient.SetUnits(PublicGradient::Units::OBJECT_BOUNDING_BOX);
  style = TextInternal::Gradient::CreateStyle(gradient);
  DALI_TEST_EQUALS(TextInternal::ResolveGradientCoordinateSize(style.units, display, std::numeric_limits<float>::min()), display, TEST_LOCATION);
  DALI_TEST_CHECK(TextInternal::Gradient::ResolveRenderData(style, bounds, display).enabled);
  END_TEST;
}

int UtcDaliTextGradientNonFiniteGeometryN(void)
{
  const Vector4 bounds(0.f, 0.f, 1.f, 1.f);
  const Vector2 size(200.f, 80.f);
  for(auto units : {PublicGradient::Units::USER_SPACE, PublicGradient::Units::OBJECT_BOUNDING_BOX})
  {
    for(auto type : {PublicGradient::Type::LINEAR, PublicGradient::Type::RADIAL, PublicGradient::Type::CONIC})
    {
      for(float value : {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()})
      {
        TextInternal::Gradient::Style base;
        base.enabled = true;
        base.units   = units;
        base.type    = type;
        base.stops.PushBack({0.f, Color::RED});
        base.stops.PushBack({1.f, Color::BLUE});
        base.radialRadius = .5f;
        auto style        = base;
        if(type == PublicGradient::Type::LINEAR)
        {
          style.linearEnd.x = value;
        }
        if(type == PublicGradient::Type::RADIAL)
        {
          style.radialCenter.x = value;
        }
        if(type == PublicGradient::Type::CONIC)
        {
          style.conicCenter.x = value;
        }
        DALI_TEST_CHECK(!TextInternal::Gradient::ResolveRenderData(style, bounds, size).enabled);
        style             = base;
        style.startOffset = value;
        DALI_TEST_CHECK(!TextInternal::Gradient::ResolveRenderData(style, bounds, size).enabled);
        style = base;
        if(type == PublicGradient::Type::RADIAL)
        {
          style.radialRadius = value;
          DALI_TEST_CHECK(!TextInternal::Gradient::ResolveRenderData(style, bounds, size).enabled);
        }
        if(type == PublicGradient::Type::CONIC)
        {
          style.conicStartAngle = Radian(value);
          DALI_TEST_CHECK(!TextInternal::Gradient::ResolveRenderData(style, bounds, size).enabled);
        }
        for(unsigned i = 0; i < 4; ++i)
        {
          Vector4 invalid = bounds;
          invalid[i]      = value;
          DALI_TEST_CHECK(!TextInternal::Gradient::ResolveRenderData(base, invalid, size).enabled);
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextGradientAcceptedSmallGeometryP(void)
{
  for(auto units : {PublicGradient::Units::USER_SPACE, PublicGradient::Units::OBJECT_BOUNDING_BOX})
  {
    PublicGradient::Linear line(Vector2::ZERO, Vector2(.02f, 0.f));
    SetCommonProperties(line);
    line.SetUnits(units);
    auto style = TextInternal::Gradient::CreateStyle(line);
    DALI_TEST_CHECK(TextInternal::Gradient::ResolveRenderData(style, Vector4(0, 0, 1, 1), Vector2(.001f, .002f)).enabled);
    for(float radius : {.001f, -.001f})
    {
      PublicGradient::Radial radial(Vector2(.001f, .002f), radius);
      SetCommonProperties(radial);
      radial.SetUnits(units);
      style       = TextInternal::Gradient::CreateStyle(radial);
      auto result = TextInternal::Gradient::ResolveRenderData(style, Vector4(0, 0, 1, 1), Vector2(.001f, .002f));
      DALI_TEST_CHECK(result.enabled);
      DALI_TEST_CHECK(std::isfinite(result.radialScale.x) && std::isfinite(result.radialScale.y));
    }
  }
  END_TEST;
}

int UtcDaliTextGradientUserSpaceLinearMetricP(void)
{
  TextInternal::Gradient::Style style;
  style.enabled = true;
  style.type    = PublicGradient::Type::LINEAR;
  style.units   = PublicGradient::Units::USER_SPACE;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});
  const Vector2 sizes[] = {Vector2(100, 100), Vector2(200, 100), Vector2(100, 200), Vector2(300, 50), Vector2(50, 300), Vector2(400, 400)};
  for(const auto& size : sizes)
  {
    for(float uiScale : {0.5f, 1.0f, 1.25f, 2.0f})
    {
      for(float angle : {0.0f, 30.0f, 45.0f, 60.0f, 90.0f, 135.0f})
      {
        style.linearStart = Vector2(17.0f, 9.0f);
        style.linearEnd   = style.linearStart + Vector2(80.0f * std::cos(angle * Math::PI / 180.0f),
                                                        80.0f * std::sin(angle * Math::PI / 180.0f));
        for(const auto& bounds : {Vector4(0, 0, 1, 1), Vector4(-0.1f, 0.2f, 0.8f, 0.6f)})
        {
          const Vector2 displaySize = size * uiScale;
          const auto    data        = TextInternal::Gradient::ResolveRenderData(
            style, bounds, TextInternal::ResolveGradientCoordinateSize(style.units, displaySize, uiScale));
          DALI_TEST_CHECK(data.enabled);
          DALI_TEST_EQUALS(data.type, PublicGradient::Type::LINEAR, TEST_LOCATION);
          DALI_TEST_EQUALS(data.radialScale, Vector2::ZERO, TEST_LOCATION);
          const double dx = static_cast<double>(style.linearEnd.x) - style.linearStart.x;
          const double dy = static_cast<double>(style.linearEnd.y) - style.linearStart.y;
          for(const auto& sample : {Vector2(0.1f, 0.2f), Vector2(0.5f, 0.5f), Vector2(0.9f, 0.8f)})
          {
            const Vector2 local((sample.x - bounds.x) / bounds.z, (sample.y - bounds.y) / bounds.w);
            const Vector2 delta     = local - data.startPosition;
            const Vector2 direction = data.endPosition - data.startPosition;
            const float   actual    = data.evaluationMode == TextInternal::Gradient::EvaluationMode::LINEAR_USER_SPACE
                                        ? delta.x * data.linearProjection.x + delta.y * data.linearProjection.y
                                        : (delta.x * direction.x + delta.y * direction.y) / direction.LengthSquared();
            // Independent reference: project the authored-space sample directly.
            const double px       = (static_cast<double>(sample.x) - bounds.x) * size.x;
            const double py       = (static_cast<double>(sample.y) - bounds.y) * size.y;
            const double expected = ((px - style.linearStart.x) * dx + (py - style.linearStart.y) * dy) / (dx * dx + dy * dy);
            DALI_TEST_EQUALS(actual, static_cast<float>(expected), 0.00001f, TEST_LOCATION);
          }
        }
      }
    }
  }
  END_TEST;
}

int UtcDaliTextGradientLinearMetricShortAndBoundingBoxP(void)
{
  TextInternal::Gradient::Style style;
  style.enabled = true;
  style.type    = PublicGradient::Type::LINEAR;
  style.stops.PushBack({0.0f, Color::RED});
  style.stops.PushBack({1.0f, Color::BLUE});
  const Vector4 bounds(0, 0, 1, 1);
  const Vector2 size(4096, 100);
  style.units       = PublicGradient::Units::USER_SPACE;
  style.linearStart = Vector2(10, 20);
  style.linearEnd   = Vector2(11, 20);
  const auto user   = TextInternal::Gradient::ResolveRenderData(style, bounds, size);
  DALI_TEST_CHECK(user.enabled);
  DALI_TEST_EQUALS(user.type, PublicGradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(user.evaluationMode, TextInternal::Gradient::EvaluationMode::LINEAR_USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(user.radialScale, Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(user.GetEvaluationScale(), user.linearProjection, TEST_LOCATION);
  const Vector2 midpoint(10.5f / size.x, 20.0f / size.y);
  const Vector2 delta = midpoint - user.startPosition;
  DALI_TEST_EQUALS(delta.x * user.linearProjection.x + delta.y * user.linearProjection.y, 0.5f, 0.00001f, TEST_LOCATION);

  style.units       = PublicGradient::Units::OBJECT_BOUNDING_BOX;
  style.linearStart = Vector2(-0.4f, -0.3f);
  style.linearEnd   = Vector2(0.4f, 0.2f);
  for(const auto& target : {Vector2(100, 100), size, Vector2(200, 800)})
  {
    const auto box = TextInternal::Gradient::ResolveRenderData(style, bounds, target);
    DALI_TEST_CHECK(box.enabled);
    DALI_TEST_EQUALS(box.type, PublicGradient::Type::LINEAR, TEST_LOCATION);
    DALI_TEST_EQUALS(box.evaluationMode, TextInternal::Gradient::EvaluationMode::LINEAR_NORMALIZED, TEST_LOCATION);
    DALI_TEST_EQUALS(box.linearProjection, Vector2::ZERO, TEST_LOCATION);
    DALI_TEST_EQUALS(box.startPosition, style.linearStart + Vector2(0.5f, 0.5f), TEST_LOCATION);
    DALI_TEST_EQUALS(box.endPosition, style.linearEnd + Vector2(0.5f, 0.5f), TEST_LOCATION);
    DALI_TEST_EQUALS(box.radialScale, Vector2::ZERO, TEST_LOCATION);
    DALI_TEST_EQUALS(box.bounds, bounds, TEST_LOCATION);
  }

  // Changing units or shape must not retain the previous evaluation payload.
  style.linearStart  = Vector2::ZERO;
  style.linearEnd    = Vector2(100.0f, 50.0f);
  style.radialRadius = 25.0f;
  for(unsigned int step = 0u; step < 5u; ++step)
  {
    style.units         = step == 0u || step == 4u ? PublicGradient::Units::OBJECT_BOUNDING_BOX : PublicGradient::Units::USER_SPACE;
    style.type          = step == 2u ? PublicGradient::Type::RADIAL : PublicGradient::Type::LINEAR;
    const auto prepared = TextInternal::Gradient::ResolveRenderData(style, bounds, Vector2(200.0f, 100.0f));
    DALI_TEST_EQUALS(prepared.type, style.type, TEST_LOCATION);
    if(step == 2u)
    {
      DALI_TEST_EQUALS(prepared.evaluationMode, TextInternal::Gradient::EvaluationMode::RADIAL, TEST_LOCATION);
      DALI_TEST_EQUALS(prepared.linearProjection, Vector2::ZERO, TEST_LOCATION);
      DALI_TEST_EQUALS(prepared.GetEvaluationScale(), Vector2(8.0f, 4.0f), TEST_LOCATION);
    }
    else if(step == 1u || step == 3u)
    {
      DALI_TEST_EQUALS(prepared.evaluationMode, TextInternal::Gradient::EvaluationMode::LINEAR_USER_SPACE, TEST_LOCATION);
      DALI_TEST_EQUALS(prepared.radialScale, Vector2::ZERO, TEST_LOCATION);
      DALI_TEST_EQUALS(prepared.GetEvaluationScale(), Vector2(1.6f, 0.4f), 0.00001f, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_EQUALS(prepared.evaluationMode, TextInternal::Gradient::EvaluationMode::LINEAR_NORMALIZED, TEST_LOCATION);
      DALI_TEST_EQUALS(prepared.linearProjection, Vector2::ZERO, TEST_LOCATION);
      DALI_TEST_EQUALS(prepared.GetEvaluationScale(), Vector2::ZERO, TEST_LOCATION);
    }
  }
  END_TEST;
}
