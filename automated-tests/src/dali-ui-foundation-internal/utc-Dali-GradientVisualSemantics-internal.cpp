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
#include <dali-ui-foundation/internal/visuals/gradient/linear-gradient.h>
#include <dali-ui-foundation/internal/visuals/gradient/radial-gradient.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#include <initializer_list>

using namespace Dali;

namespace
{

constexpr float EPSILON = 0.001f;

using InternalGradient = Dali::Ui::Internal::Gradient;

Vector<float> MakeOffsets(std::initializer_list<float> values)
{
  Vector<float> offsets;
  offsets.Reserve(static_cast<uint32_t>(values.size()));

  for(float value : values)
  {
    offsets.PushBack(value);
  }

  return offsets;
}

Vector<Vector4> MakeColors(std::initializer_list<Vector4> values)
{
  Vector<Vector4> colors;
  colors.Reserve(static_cast<uint32_t>(values.size()));

  for(const Vector4& value : values)
  {
    colors.PushBack(value);
  }

  return colors;
}

IntrusivePtr<InternalGradient> NewLinear()
{
  return new Dali::Ui::Internal::LinearGradient(Vector2::ZERO, Vector2::ONE, nullptr);
}

void ExpectStop(const Vector<InternalGradient::GradientStop>& stops, uint32_t index, float offset, const Vector4& color)
{
  DALI_TEST_EQUALS(stops.Count() > index, true, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[index].mOffset, offset, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stops[index].mStopColor, color, TEST_LOCATION);
}

void ExpectMatrixNear(const Matrix3& actual, const Matrix3& expected)
{
  DALI_TEST_EQUALS(actual, expected, EPSILON, TEST_LOCATION);
}

void ExpectTextureMetadata(const Texture& texture, Pixel::Format pixelFormat)
{
  DALI_TEST_CHECK(texture);
  DALI_TEST_EQUALS(texture.GetWidth(), 512u, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetHeight(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(texture.GetPixelFormat(), pixelFormat, TEST_LOCATION);
}

} // namespace

void utc_dali_gradient_visual_semantics_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gradient_visual_semantics_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliGradientInternalApplyStopsClampAndCountP(void)
{
  IntrusivePtr<InternalGradient> gradient = NewLinear();

  gradient->ApplyStops(
    MakeOffsets({-0.25f, 0.25f, 1.25f}),
    MakeColors({Color::RED, Color::GREEN, Color::BLUE}));

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 3u, TEST_LOCATION);
  ExpectStop(stops, 0u, 0.0f, Color::RED);
  ExpectStop(stops, 1u, 0.25f, Color::GREEN);
  ExpectStop(stops, 2u, 1.0f, Color::BLUE);
  END_TEST;
}

int UtcDaliGradientInternalGenerateLookupTextureCanonicalizesStopsP(void)
{
  UiTestApplication application;
  IntrusivePtr<InternalGradient> gradient = NewLinear();

  gradient->ApplyStops(
    MakeOffsets({1.0f, 0.0f}),
    MakeColors({Color::BLUE, Color::RED}));

  Texture texture = gradient->GenerateLookupTexture();
  ExpectTextureMetadata(texture, Pixel::RGB888);

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  ExpectStop(stops, 0u, 0.0f, Color::RED);
  ExpectStop(stops, 1u, 1.0f, Color::BLUE);
  END_TEST;
}

int UtcDaliGradientInternalGenerateLookupTextureFillsMissingOffsetsP(void)
{
  UiTestApplication application;
  IntrusivePtr<InternalGradient> gradient = NewLinear();

  gradient->ApplyStops(
    MakeOffsets({0.2f, 0.8f}),
    MakeColors({Color::RED, Color::GREEN, Color::BLUE, Color::YELLOW}));

  Texture texture = gradient->GenerateLookupTexture();
  ExpectTextureMetadata(texture, Pixel::RGB888);

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 4u, TEST_LOCATION);
  ExpectStop(stops, 0u, 0.2f, Color::RED);
  ExpectStop(stops, 1u, 0.8f, Color::GREEN);
  ExpectStop(stops, 2u, 0.9f, Color::BLUE);
  ExpectStop(stops, 3u, 1.0f, Color::YELLOW);
  END_TEST;
}

int UtcDaliGradientInternalGenerateLookupTextureAlphaFormatP(void)
{
  UiTestApplication application;
  IntrusivePtr<InternalGradient> gradient = NewLinear();
  const Vector4 translucentRed(1.0f, 0.0f, 0.0f, 0.5f);

  gradient->ApplyStops(
    MakeOffsets({0.0f, 1.0f}),
    MakeColors({translucentRed, Color::BLUE}));

  Texture texture = gradient->GenerateLookupTexture();
  ExpectTextureMetadata(texture, Pixel::RGBA8888);

  const Vector<InternalGradient::GradientStop>& stops = gradient->GetStops();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  ExpectStop(stops, 0u, 0.0f, translucentRed);
  ExpectStop(stops, 1u, 1.0f, Color::BLUE);
  END_TEST;
}

int UtcDaliGradientInternalEmptyStopsReturnEmptyTextureP(void)
{
  UiTestApplication application;
  IntrusivePtr<InternalGradient> gradient = NewLinear();

  Texture texture = gradient->GenerateLookupTexture();
  DALI_TEST_CHECK(!texture);
  DALI_TEST_EQUALS(gradient->GetStops().Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientInternalSpreadUnitsStartOffsetP(void)
{
  IntrusivePtr<InternalGradient> gradient = NewLinear();

  DALI_TEST_EQUALS(gradient->GetUnits(), Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient->GetSpreadMethod(), Dali::Ui::Gradient::SpreadMethod::PAD, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient->GetStartOffset(), 0.0f, EPSILON, TEST_LOCATION);

  gradient->SetUnits(Dali::Ui::Gradient::Units::USER_SPACE);
  gradient->SetSpreadMethod(Dali::Ui::Gradient::SpreadMethod::REFLECT);
  gradient->SetStartOffset(-0.5f);

  DALI_TEST_EQUALS(gradient->GetUnits(), Dali::Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient->GetSpreadMethod(), Dali::Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient->GetStartOffset(), -0.5f, EPSILON, TEST_LOCATION);

  gradient->SetSpreadMethod(Dali::Ui::Gradient::SpreadMethod::REPEAT);
  gradient->SetStartOffset(1.5f);

  DALI_TEST_EQUALS(gradient->GetSpreadMethod(), Dali::Ui::Gradient::SpreadMethod::REPEAT, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient->GetStartOffset(), 1.5f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientInternalGeometryStateAndTransformsP(void)
{
  Dali::Ui::Internal::LinearGradient linear(Vector2(0.0f, 0.0f), Vector2(2.0f, 0.0f), nullptr);
  DALI_TEST_EQUALS(linear.GetStartPosition(), Vector2(0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), Vector2(2.0f, 0.0f), TEST_LOCATION);

  Matrix3 expectedLinear(0.0f, -2.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
  expectedLinear.Invert();
  ExpectMatrixNear(linear.GetAlignmentTransform(), expectedLinear);

  Dali::Ui::Internal::RadialGradient radial(Vector2(2.0f, 3.0f), 4.0f, nullptr);
  DALI_TEST_EQUALS(radial.GetCenter(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetRadius(), 4.0f, EPSILON, TEST_LOCATION);

  Matrix3 expectedRadial(4.0f, 0.0f, 0.0f, 0.0f, 4.0f, 0.0f, 2.0f, 3.0f, 1.0f);
  expectedRadial.Invert();
  ExpectMatrixNear(radial.GetAlignmentTransform(), expectedRadial);

  Dali::Ui::Internal::ConicGradient conic(Vector2(2.0f, 3.0f), Radian(Math::PI * 2.5f), nullptr);
  DALI_TEST_EQUALS(conic.GetCenter(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(conic.GetStartAngle(), Dali::ANGLE_90, TEST_LOCATION);

  Matrix3 expectedConic(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -2.0f, -3.0f, 1.0f);
  ExpectMatrixNear(conic.GetAlignmentTransform(), expectedConic);
  END_TEST;
}

int UtcDaliGradientInternalInvalidGeometryIsStoredP(void)
{
  Dali::Ui::Internal::LinearGradient linear(Vector2(1.0f, 1.0f), Vector2(1.0f, 1.0f), nullptr);
  DALI_TEST_EQUALS(linear.GetStartPosition(), Vector2(1.0f, 1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), Vector2(1.0f, 1.0f), TEST_LOCATION);

  Dali::Ui::Internal::RadialGradient zeroRadius(Vector2(2.0f, 3.0f), 0.0f, nullptr);
  DALI_TEST_EQUALS(zeroRadius.GetCenter(), Vector2(2.0f, 3.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(zeroRadius.GetRadius(), 0.0f, EPSILON, TEST_LOCATION);

  Dali::Ui::Internal::RadialGradient negativeRadius(Vector2(-1.0f, -2.0f), -3.0f, nullptr);
  DALI_TEST_EQUALS(negativeRadius.GetCenter(), Vector2(-1.0f, -2.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(negativeRadius.GetRadius(), -3.0f, EPSILON, TEST_LOCATION);

  Dali::Ui::Internal::ConicGradient negativeAngle(Vector2::ZERO, Radian(-Math::PI_2), nullptr);
  DALI_TEST_EQUALS(negativeAngle.GetStartAngle(), Dali::ANGLE_270, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGradientInternalOldGradientCopiesCommonStateP(void)
{
  IntrusivePtr<InternalGradient> linear = NewLinear();
  linear->ApplyStops(
    MakeOffsets({0.0f, 1.0f}),
    MakeColors({Color::RED, Color::BLUE}));
  linear->SetUnits(Dali::Ui::Gradient::Units::USER_SPACE);
  linear->SetSpreadMethod(Dali::Ui::Gradient::SpreadMethod::REFLECT);
  linear->SetStartOffset(1.25f);

  Dali::Ui::Internal::RadialGradient radial(Vector2(3.0f, 4.0f), 5.0f, linear);

  DALI_TEST_EQUALS(radial.GetUnits(), Dali::Ui::Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetSpreadMethod(), Dali::Ui::Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetStartOffset(), 1.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetCenter(), Vector2(3.0f, 4.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(radial.GetRadius(), 5.0f, EPSILON, TEST_LOCATION);

  const Vector<InternalGradient::GradientStop>& stops = radial.GetStops();
  DALI_TEST_EQUALS(stops.Count(), 2u, TEST_LOCATION);
  ExpectStop(stops, 0u, 0.0f, Color::RED);
  ExpectStop(stops, 1u, 1.0f, Color::BLUE);
  END_TEST;
}
