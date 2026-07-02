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
#include <dali-ui-foundation/public-api/gradient/gradient-base.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
#include <dali-ui-foundation/public-api/animation/duration.h>
#include <dali-ui-foundation/public-api/animation/label-animation-bridge.autogen.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

constexpr float EPSILON = 0.001f;
constexpr char  MOVED_FROM_GRADIENT_ASSERTION[] = "Cannot use a moved-from Gradient::Base object";
constexpr const char* TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME   = "uTextGradientStartOffset";

Dali::Vector<Gradient::StopNode> MakeStopNodes(const Vector4& startColor = Color::RED, const Vector4& endColor = Color::BLUE)
{
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.0f, UiColor(startColor)));
  stopNodes.PushBack(Gradient::StopNode(1.0f, UiColor(endColor)));
  return stopNodes;
}

Gradient::Linear MakeRenderableLinear(const Vector2& startPosition = Vector2::ZERO,
                                      const Vector2& endPosition   = Vector2::ONE,
                                      float          startOffset   = 0.25f)
{
  Gradient::Linear linear(startPosition, endPosition);
  linear.SetUnits(Gradient::Units::USER_SPACE);
  linear.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  linear.SetStartOffset(startOffset);
  linear.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));
  return linear;
}

Gradient::Radial MakeRenderableRadial(float startOffset = 0.25f)
{
  Gradient::Radial radial(Vector2(0.5f, 0.5f), 0.5f);
  radial.SetStartOffset(startOffset);
  radial.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));
  return radial;
}

Gradient::Conic MakeRenderableConic(float startOffset = 0.25f)
{
  Gradient::Conic conic(Vector2(0.5f, 0.5f), Radian(0.0f));
  conic.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  conic.SetStartOffset(startOffset);
  conic.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));
  return conic;
}

void ApplyTextGradientAnimationTo(Label label, Animation animation)
{
  label.Animate(animation)
    .TextGradientStartOffset(0.75f, Duration(0.1f));
}

void ApplyTextGradientAnimationBy(Label label, Animation animation)
{
  label.Animate(animation)
    .TextGradientStartOffsetBy(0.1f, Duration(0.1f));
}

void ExpectStopNode(const Dali::Vector<Gradient::StopNode>& stopNodes, uint32_t index, float offset, const Vector4& color)
{
  DALI_TEST_EQUALS(stopNodes.Count() > index, true, TEST_LOCATION);
  DALI_TEST_EQUALS(stopNodes[index].GetOffset(), offset, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(stopNodes[index].GetColor().GetRgba(), color, TEST_LOCATION);
}

void ExpectRenderableLinearGradient(const Gradient::Base& gradient, const Vector2& startPosition, const Vector2& endPosition)
{
  DALI_TEST_CHECK(gradient.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetUnits(), Gradient::Units::USER_SPACE, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetSpreadMethod(), Gradient::SpreadMethod::REFLECT, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetStartOffset(), 0.25f, EPSILON, TEST_LOCATION);

  auto linear = Gradient::Linear::DownCast(gradient);
  DALI_TEST_CHECK(linear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(linear.GetStartPosition(), startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), endPosition, TEST_LOCATION);

  const auto stopNodes = linear.GetStopNodes();
  DALI_TEST_EQUALS(stopNodes.Count(), 2u, TEST_LOCATION);
  ExpectStopNode(stopNodes, 0u, 0.0f, Color::GREEN);
  ExpectStopNode(stopNodes, 1u, 1.0f, Color::YELLOW);
}

void ExpectObjectBoundingBoxLinearGradient(const Gradient::Base& gradient, const Vector2& startPosition, const Vector2& endPosition)
{
  DALI_TEST_CHECK(gradient.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::LINEAR, TEST_LOCATION);
  DALI_TEST_EQUALS(gradient.GetUnits(), Gradient::Units::OBJECT_BOUNDING_BOX, TEST_LOCATION);

  auto linear = Gradient::Linear::DownCast(gradient);
  DALI_TEST_CHECK(linear.GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(linear.GetStartPosition(), startPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(linear.GetEndPosition(), endPosition, TEST_LOCATION);
}

} // unnamed namespace

void utc_dali_label_text_gradient_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_label_text_gradient_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLabelTextGradientDefaultP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  Gradient::Base gradient = label.GetTextGradient();
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(gradient.GetType() == Gradient::Type::NONE);

  END_TEST;
}

int UtcDaliLabelSetGetTextGradientLinearP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  const Vector2 startPosition(0.1f, 0.2f);
  const Vector2 endPosition(0.8f, 0.9f);
  label.SetTextGradient(MakeRenderableLinear(startPosition, endPosition));

  ExpectRenderableLinearGradient(label.GetTextGradient(), startPosition, endPosition);

  END_TEST;
}

int UtcDaliLabelTextGradientBoundsModeP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::VIEW_BOUND, TEST_LOCATION);

  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientObjectBoundingBoxCoordinatesP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  const Vector2 startPosition(-0.5f, 0.0f);
  const Vector2 endPosition(0.5f, 0.0f);

  Gradient::Linear gradient(startPosition, endPosition);
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetStopNodes(MakeStopNodes(Color::GREEN, Color::YELLOW));

  label.SetTextGradient(gradient);

  ExpectObjectBoundingBoxLinearGradient(label.GetTextGradient(), startPosition, endPosition);

  END_TEST;
}

int UtcDaliLabelTextGradientSnapshotCopyP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  const Vector2 originalStart(0.1f, 0.2f);
  const Vector2 originalEnd(0.8f, 0.9f);
  Gradient::Linear source = MakeRenderableLinear(originalStart, originalEnd);

  label.SetTextGradient(source);

  source.SetStartAndEndPosition(Vector2(5.0f, 6.0f), Vector2(7.0f, 8.0f));
  source.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  source.SetSpreadMethod(Gradient::SpreadMethod::REPEAT);
  source.SetStartOffset(1.5f);
  source.SetStopNodes(MakeStopNodes(Color::RED, Color::BLUE));

  ExpectRenderableLinearGradient(label.GetTextGradient(), originalStart, originalEnd);

  END_TEST;
}

int UtcDaliLabelTextGradientNoneP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradient(MakeRenderableLinear());

  DALI_TEST_CHECK(label.GetTextGradient().GetType() != Gradient::Type::NONE);

  label.SetTextGradient(Gradient::Base::None());

  Gradient::Base gradient = label.GetTextGradient();
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(gradient.GetType() == Gradient::Type::NONE);

  END_TEST;
}

int UtcDaliLabelTextGradientRejectNoneP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradient(MakeRenderableLinear());

  label.SetTextGradient(Gradient::Base::None());

  Gradient::Base gradient = label.GetTextGradient();
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(gradient.GetType() == Gradient::Type::NONE);

  END_TEST;
}

int UtcDaliLabelTextGradientRejectInsufficientStopsP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  Gradient::Linear noStops(Vector2::ZERO, Vector2::ONE);
  label.SetTextGradient(noStops);

  Gradient::Base gradient = label.GetTextGradient();
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(gradient.GetType() == Gradient::Type::NONE);

  Gradient::Linear oneStop(Vector2::ZERO, Vector2::ONE);
  Dali::Vector<Gradient::StopNode> stopNodes;
  stopNodes.PushBack(Gradient::StopNode(0.5f, UiColor(Color::GREEN)));
  oneStop.SetStopNodes(stopNodes);

  label.SetTextGradient(MakeRenderableLinear());
  DALI_TEST_CHECK(label.GetTextGradient().GetType() != Gradient::Type::NONE);

  label.SetTextGradient(oneStop);
  gradient = label.GetTextGradient();
  DALI_TEST_EQUALS(gradient.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(gradient.GetType() == Gradient::Type::NONE);

  END_TEST;
}

int UtcDaliLabelTextGradientMovedFromInputN(void)
{
  UiTestApplication application;

  Label label = Label::New();

  Gradient::Base source = MakeRenderableLinear();
  Gradient::Base moved(std::move(source));
  DALI_TEST_CHECK(moved.GetType() != Gradient::Type::NONE);

  DALI_TEST_ASSERTION(label.SetTextGradient(source), MOVED_FROM_GRADIENT_ASSERTION);

  END_TEST;
}

int UtcDaliLabelTextGradientAnimationNoOpAndLinearP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  Animation noGradientAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, noGradientAnimation);
  ApplyTextGradientAnimationBy(label, noGradientAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  label.SetTextGradient(Gradient::Base::None());
  Animation noneAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, noneAnimation);
  ApplyTextGradientAnimationBy(label, noneAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  label.SetTextGradient(MakeRenderableConic());
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation conicAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, conicAnimation);

  const Property::Index startOffsetIndex =
    label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(startOffsetIndex != Property::INVALID_INDEX);

  Animation conicByAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationBy(label, conicByAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);

  label.SetTextGradient(MakeRenderableRadial());
  Animation radialAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, radialAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);

  Animation radialByAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationBy(label, radialByAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);

  label.SetTextGradient(MakeRenderableLinear());
  Animation linearAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, linearAnimation);

  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);

  Animation linearByAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationBy(label, linearByAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);

  linearAnimation.Play();
  application.SendNotification();
  application.Render(100);
  linearAnimation.Stop();

  label.SetTextGradient(Gradient::Base::None());
  Animation afterNoneAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, afterNoneAnimation);
  ApplyTextGradientAnimationBy(label, afterNoneAnimation);
  afterNoneAnimation.Play();
  application.SendNotification();
  application.Render(100);
  afterNoneAnimation.Stop();
  application.SendNotification();
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientRepeatedAnimationKeepsPropertyIndicesP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradient(MakeRenderableLinear());

  Animation initialAnimation = Animation::New(0.05f);
  ApplyTextGradientAnimationTo(label, initialAnimation);

  const Property::Index startOffsetIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(startOffsetIndex != Property::INVALID_INDEX);

  for(int i = 0; i < 20; ++i)
  {
    Animation animation = Animation::New(0.05f);
    if((i % 2) == 0)
    {
      ApplyTextGradientAnimationTo(label, animation);
    }
    else
    {
      ApplyTextGradientAnimationBy(label, animation);
    }

    animation.Play();
    application.SendNotification();
    application.Render(50);
    animation.Stop();
    application.SendNotification();

    DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliLabelTextGradientSetAfterAnimationUsesNewGradientP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.25f));

  Animation animation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, animation);

  const Property::Index startOffsetIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(startOffsetIndex != Property::INVALID_INDEX);

  animation.Play();
  application.SendNotification();
  application.Render(100);
  animation.Stop();
  animation.Clear();
  application.SendNotification();

  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.55f));

  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetProperty<float>(startOffsetIndex), 0.55f, EPSILON, TEST_LOCATION);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(startOffsetIndex), 0.55f, EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientSetImmediatelyAfterAnimationStopP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.25f));

  Animation animation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, animation);

  const Property::Index startOffsetIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(startOffsetIndex != Property::INVALID_INDEX);

  animation.Play();
  application.SendNotification();
  application.Render(50);
  animation.Stop();
  animation.Clear();

  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.65f));

  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetProperty<float>(startOffsetIndex), 0.65f, EPSILON, TEST_LOCATION);
  application.SendNotification();
  application.Render(0);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(startOffsetIndex), 0.65f, EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientRegisteredThenUnsupportedNoOpP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradient(MakeRenderableLinear());

  Animation initialAnimation = Animation::New(0.05f);
  ApplyTextGradientAnimationTo(label, initialAnimation);

  const Property::Index startOffsetIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(startOffsetIndex != Property::INVALID_INDEX);

  const float before = label.GetCurrentProperty<float>(startOffsetIndex);
  label.SetTextGradient(Gradient::Base::None());

  Animation noneAnimation = Animation::New(0.05f);
  ApplyTextGradientAnimationTo(label, noneAnimation);
  noneAnimation.Play();
  application.SendNotification();
  application.Render(50);

  const float after = label.GetCurrentProperty<float>(startOffsetIndex);
  DALI_TEST_EQUALS(after, before, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), startOffsetIndex, TEST_LOCATION);

  END_TEST;
}
