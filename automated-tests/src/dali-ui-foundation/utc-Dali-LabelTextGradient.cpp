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
#include <dali-ui/ui-event-thread-callback.h>
#include <dali.h>
#include <dali/devel-api/text-abstraction/font-client.h>

#include <utility>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

constexpr float EPSILON = 0.001f;
constexpr char  MOVED_FROM_GRADIENT_ASSERTION[] = "Cannot use a moved-from Gradient::Base object";
constexpr const char* TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME   = "uTextGradientStartOffset";
constexpr const char* TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME = "uTextGradientOverlayStartOffset";

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

void ApplyTextGradientOverlayAnimationTo(Label label, Animation animation)
{
  label.Animate(animation)
    .TextGradientOverlayStartOffset(0.75f, Duration(0.1f));
}

void ApplyTextGradientOverlayAnimationBy(Label label, Animation animation)
{
  label.Animate(animation)
    .TextGradientOverlayStartOffsetBy(0.1f, Duration(0.1f));
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

int UtcDaliLabelTextGradientOverlayDefaultsP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  Gradient::Base overlay = label.GetTextGradientOverlay();
  DALI_TEST_EQUALS(overlay.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(overlay.GetType() == Gradient::Type::NONE);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlaySetGetP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  const Vector2 startPosition(0.2f, 0.3f);
  const Vector2 endPosition(0.7f, 0.8f);
  label.SetTextGradientOverlay(MakeRenderableLinear(startPosition, endPosition));

  ExpectRenderableLinearGradient(label.GetTextGradientOverlay(), startPosition, endPosition);

  label.SetTextGradientOverlay(Gradient::Base::None());
  Gradient::Base overlay = label.GetTextGradientOverlay();
  DALI_TEST_EQUALS(overlay.GetType(), Gradient::Type::NONE, TEST_LOCATION);
  DALI_TEST_CHECK(overlay.GetType() == Gradient::Type::NONE);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayBoundsModeSetGetP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::VIEW_BOUND, TEST_LOCATION);

  label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayModeSetGetP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);

  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SCREEN, TEST_LOCATION);

  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SRC_OVER);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SRC_OVER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayIndependentFromBaseGradientP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  const Vector2 baseStart(0.1f, 0.2f);
  const Vector2 baseEnd(0.8f, 0.9f);
  const Vector2 overlayStart(0.3f, 0.4f);
  const Vector2 overlayEnd(0.6f, 0.7f);

  label.SetTextGradient(MakeRenderableLinear(baseStart, baseEnd));
  label.SetTextGradientOverlay(MakeRenderableLinear(overlayStart, overlayEnd));

  ExpectRenderableLinearGradient(label.GetTextGradient(), baseStart, baseEnd);
  ExpectRenderableLinearGradient(label.GetTextGradientOverlay(), overlayStart, overlayEnd);

  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);
  label.SetTextGradientOverlayBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::VIEW_BOUND, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayBoundsMode(), Text::GradientBoundsMode::CONTENT_BOUND, TEST_LOCATION);

  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
  DALI_TEST_EQUALS(label.GetTextGradientOverlayMode(), Text::GradientOverlayMode::SCREEN, TEST_LOCATION);
  ExpectRenderableLinearGradient(label.GetTextGradient(), baseStart, baseEnd);

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

int UtcDaliLabelTextGradientAnimationsPropagateAcrossFramesP(void)
{
  UiTestApplication application;

  Label label = Label::New("Gradient animation propagation without Reveal");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.1f));
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ONE, Vector2::ZERO, 0.2f));
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  Animation animation = Animation::New(1.0f);
  label.Animate(animation)
    .TextGradientStartOffset(0.8f, Duration(1.0f))
    .TextGradientOverlayStartOffset(0.9f, Duration(1.0f));

  const Property::Index baseSourceIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  const Property::Index overlaySourceIndex = label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(baseSourceIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(overlaySourceIndex != Property::INVALID_INDEX);

  Renderer renderer = label.GetRendererAt(0u);
  const Property::Index baseRendererIndex = renderer.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  const Property::Index overlayRendererIndex = renderer.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(baseRendererIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(overlayRendererIndex != Property::INVALID_INDEX);

  animation.Play();
  application.SendNotification();
  application.Render(160);
  application.SendNotification();
  application.Render(16);

  const float baseSourceFirst = label.GetCurrentProperty<float>(baseSourceIndex);
  const float overlaySourceFirst = label.GetCurrentProperty<float>(overlaySourceIndex);
  const float baseRendererFirst = renderer.GetCurrentProperty<float>(baseRendererIndex);
  const float overlayRendererFirst = renderer.GetCurrentProperty<float>(overlayRendererIndex);
  DALI_TEST_CHECK(baseSourceFirst > 0.1f && baseSourceFirst < 0.8f);
  DALI_TEST_CHECK(overlaySourceFirst > 0.2f && overlaySourceFirst < 0.9f);
  DALI_TEST_EQUALS(baseRendererFirst, baseSourceFirst, 0.02f, TEST_LOCATION);
  DALI_TEST_EQUALS(overlayRendererFirst, overlaySourceFirst, 0.02f, TEST_LOCATION);

  application.Render(160);
  application.SendNotification();
  application.Render(16);

  const float baseSourceSecond = label.GetCurrentProperty<float>(baseSourceIndex);
  const float overlaySourceSecond = label.GetCurrentProperty<float>(overlaySourceIndex);
  const float baseRendererSecond = renderer.GetCurrentProperty<float>(baseRendererIndex);
  const float overlayRendererSecond = renderer.GetCurrentProperty<float>(overlayRendererIndex);
  DALI_TEST_CHECK(baseSourceSecond > baseSourceFirst);
  DALI_TEST_CHECK(overlaySourceSecond > overlaySourceFirst);
  DALI_TEST_CHECK(baseRendererSecond > baseRendererFirst);
  DALI_TEST_CHECK(overlayRendererSecond > overlayRendererFirst);
  DALI_TEST_EQUALS(baseRendererSecond, baseSourceSecond, 0.02f, TEST_LOCATION);
  DALI_TEST_EQUALS(overlayRendererSecond, overlaySourceSecond, 0.02f, TEST_LOCATION);

  application.Render(800);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(baseSourceIndex), 0.8f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(overlaySourceIndex), 0.9f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(baseRendererIndex), 0.8f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(overlayRendererIndex), 0.9f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextGradientGenericAnimationSurvivesRendererRebuildP(void)
{
  UiTestApplication application;

  Label label = Label::New("Generic gradient Property animation without Reveal");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.1f));
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  // The typed bridge creates the lazy source. The animation under test uses
  // only DALi's generic Property API after that point.
  Animation sourceRegistration = Animation::New(1.0f);
  label.Animate(sourceRegistration).TextGradientStartOffset(0.1f, Duration(1.0f));
  const Property::Index sourceIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(sourceIndex != Property::INVALID_INDEX);

  Animation animation = Animation::New(1.0f);
  animation.AnimateTo(Property(label, sourceIndex), 0.9f);
  animation.Play();
  application.SendNotification();
  application.Render(160);
  application.SendNotification();
  application.Render(16);

  Renderer renderer = label.GetRendererAt(0u);
  Property::Index rendererIndex = renderer.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(rendererIndex != Property::INVALID_INDEX);
  const float sourceBeforeRebuild = label.GetCurrentProperty<float>(sourceIndex);
  const float rendererBeforeRebuild = renderer.GetCurrentProperty<float>(rendererIndex);
  DALI_TEST_CHECK(sourceBeforeRebuild > 0.1f && sourceBeforeRebuild < 0.9f);
  DALI_TEST_EQUALS(rendererBeforeRebuild, sourceBeforeRebuild, 0.02f, TEST_LOCATION);

  label.SetTextGradient(Gradient::Base::None());
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.35f));
  application.SendNotification();
  application.Render(64);
  application.SendNotification();
  application.Render(16);

  renderer = label.GetRendererAt(0u);
  rendererIndex = renderer.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(rendererIndex != Property::INVALID_INDEX);
  const float sourceAfterRebuild = label.GetCurrentProperty<float>(sourceIndex);
  const float rendererAfterRebuild = renderer.GetCurrentProperty<float>(rendererIndex);
  DALI_TEST_CHECK(sourceAfterRebuild > sourceBeforeRebuild);
  DALI_TEST_CHECK(rendererAfterRebuild > rendererBeforeRebuild);
  DALI_TEST_EQUALS(rendererAfterRebuild, sourceAfterRebuild, 0.02f, TEST_LOCATION);

  application.Render(800);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetCurrentProperty<float>(sourceIndex), 0.9f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererIndex), 0.9f, EPSILON, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLabelTextGradientOverlayAnimationLazyPropertyP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.25f));

  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation animation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(label, animation);

  const Property::Index overlayStartOffsetIndex =
    label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(overlayStartOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation byAnimation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationBy(label, byAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), overlayStartOffsetIndex, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayAnimationNoOpWhenNoneP(void)
{
  UiTestApplication application;

  Label label = Label::New();

  Animation noOverlayAnimation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(label, noOverlayAnimation);
  ApplyTextGradientOverlayAnimationBy(label, noOverlayAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  label.SetTextGradientOverlay(Gradient::Base::None());
  Animation noneAnimation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(label, noneAnimation);
  ApplyTextGradientOverlayAnimationBy(label, noneAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayAnimationAsyncMarqueeCreatesLazyPropertyP(void)
{
  UiTestApplication application;

  Label asyncMarqueeLabel = Label::New("Animated marquee text that is long enough to scroll");
  asyncMarqueeLabel.SetAsyncRendering(true);
  asyncMarqueeLabel.SetRequestedWidth(120.0f);
  asyncMarqueeLabel.SetRequestedHeight(40.0f);
  asyncMarqueeLabel.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  asyncMarqueeLabel.SetMarqueeLoopCount(1);
  asyncMarqueeLabel.SetMarqueeSpeed(80);
  asyncMarqueeLabel.SetTextGradientOverlay(MakeRenderableLinear());
  asyncMarqueeLabel.StartMarquee();

  DALI_TEST_EQUALS(asyncMarqueeLabel.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation marqueeAnimation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(asyncMarqueeLabel, marqueeAnimation);

  const Property::Index asyncOverlayStartOffsetIndex =
    asyncMarqueeLabel.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(asyncOverlayStartOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(asyncMarqueeLabel.GetProperty<float>(asyncOverlayStartOffsetIndex), 0.25f, EPSILON, TEST_LOCATION);

  ApplyTextGradientOverlayAnimationBy(asyncMarqueeLabel, marqueeAnimation);
  DALI_TEST_EQUALS(asyncMarqueeLabel.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), asyncOverlayStartOffsetIndex, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayAnimationPendingMarqueeCreatesLazyPropertyP(void)
{
  UiTestApplication application;

  Label label = Label::New("Animated marquee text that is long enough to scroll");
  label.SetAsyncRendering(false);
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(40.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(1);
  label.SetMarqueeSpeed(80);
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.4f));

  label.StartMarquee();

  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation animation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(label, animation);

  const Property::Index overlayStartOffsetIndex =
    label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(overlayStartOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.4f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayAnimationSingleMarqueeLazyPropertyP(void)
{
  UiTestApplication application;

  Label label = Label::New("Animated marquee text that is long enough to scroll");
  label.SetAsyncRendering(false);
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(40.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(1);
  label.SetMarqueeSpeed(80);
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.3f));
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  label.StartMarquee();
  application.SendNotification();
  application.Render(16u);

  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation animation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(label, animation);

  const Property::Index overlayStartOffsetIndex =
    label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(overlayStartOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.3f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation byAnimation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationBy(label, byAnimation);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), overlayStartOffsetIndex, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientMarqueeAnimationsPropagateAcrossFramesP(void)
{
  UiTestApplication application;
  Label label = Label::New("Gradient marquee text long enough to exercise the scrolling renderer");
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(40.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(2);
  label.SetMarqueeSpeed(40);
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.1f));
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ONE, Vector2::ZERO, 0.2f));
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  label.StartMarquee();
  application.SendNotification();
  application.Render(16);

  // The typed bridge creates the lazy instance properties. Animate them
  // through the generic API to cover the lifecycle path that does not notify
  // LabelImpl::OnAnimateAnimatableProperty().
  Animation propertyBridge = Animation::New(0.5f);
  label.Animate(propertyBridge)
    .TextGradientStartOffset(0.8f, Duration(0.5f))
    .TextGradientOverlayStartOffset(0.9f, Duration(0.5f));
  const Property::Index baseIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  const Property::Index overlayIndex = label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(baseIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(overlayIndex != Property::INVALID_INDEX);

  application.SendNotification();
  application.Render(16);
  Renderer renderer = label.GetRendererAt(0u);
  const Property::Index rendererBase = renderer.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  const Property::Index rendererOverlay = renderer.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(rendererBase != Property::INVALID_INDEX);
  DALI_TEST_CHECK(rendererOverlay != Property::INVALID_INDEX);

  Animation animation = Animation::New(0.5f);
  animation.AnimateTo(Property(label, baseIndex), 0.8f);
  animation.AnimateTo(Property(label, overlayIndex), 0.9f);
  animation.Play();
  float previousBase = renderer.GetCurrentProperty<float>(rendererBase);
  float previousOverlay = renderer.GetCurrentProperty<float>(rendererOverlay);
  for(uint32_t frame = 0u; frame < 5u; ++frame)
  {
    application.SendNotification();
    application.Render(80);
    application.SendNotification();
    application.Render(16);
    const float currentBase = renderer.GetCurrentProperty<float>(rendererBase);
    const float currentOverlay = renderer.GetCurrentProperty<float>(rendererOverlay);
    DALI_TEST_CHECK(currentBase > previousBase);
    DALI_TEST_CHECK(currentOverlay > previousOverlay);
    previousBase = currentBase;
    previousOverlay = currentOverlay;
  }

  application.SendNotification();
  application.Render(120);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererBase), 0.8f, 0.02f, TEST_LOCATION);
  DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererOverlay), 0.9f, 0.02f, TEST_LOCATION);
  DALI_TEST_CHECK(label.IsMarqueeRunning());
  END_TEST;
}

int UtcDaliLabelTextGradientOverlayMarqueeReapplySyncsHiddenPropertyP(void)
{
  UiTestApplication application;

  Label label = Label::New("Animated marquee text that is long enough to scroll");
  label.SetAsyncRendering(false);
  label.SetRequestedWidth(120.0f);
  label.SetRequestedHeight(40.0f);
  label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
  label.SetMarqueeLoopCount(1);
  label.SetMarqueeSpeed(80);
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.3f));
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render();

  label.StartMarquee();
  application.SendNotification();
  application.Render(16u);

  Animation animation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(label, animation);

  const Property::Index overlayStartOffsetIndex =
    label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(overlayStartOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.3f, EPSILON, TEST_LOCATION);

  label.SetTextGradientOverlay(Gradient::Base::None());
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), overlayStartOffsetIndex, TEST_LOCATION);

  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.65f));
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.65f, EPSILON, TEST_LOCATION);

  label.StopMarquee();
  application.SendNotification();
  application.Render(16u);

  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.45f));
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.45f, EPSILON, TEST_LOCATION);

  label.StartMarquee();
  application.SendNotification();
  application.Render(16u);

  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.75f));
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.75f, EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientOverlayAnimationIndependentFromBaseP(void)
{
  UiTestApplication application;

  Label label = Label::New();
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.15f));
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.35f));

  Animation baseAnimation = Animation::New(0.1f);
  ApplyTextGradientAnimationTo(label, baseAnimation);

  const Property::Index baseStartOffsetIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(baseStartOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), Property::INVALID_INDEX, TEST_LOCATION);

  Animation overlayAnimation = Animation::New(0.1f);
  ApplyTextGradientOverlayAnimationTo(label, overlayAnimation);

  const Property::Index overlayStartOffsetIndex =
    label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(overlayStartOffsetIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(overlayStartOffsetIndex != baseStartOffsetIndex);
  DALI_TEST_EQUALS(label.GetProperty<float>(baseStartOffsetIndex), 0.15f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.35f, EPSILON, TEST_LOCATION);

  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.45f));
  DALI_TEST_EQUALS(label.GetProperty<float>(baseStartOffsetIndex), 0.45f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.35f, EPSILON, TEST_LOCATION);

  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.65f));
  DALI_TEST_EQUALS(label.GetProperty<float>(baseStartOffsetIndex), 0.45f, EPSILON, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetProperty<float>(overlayStartOffsetIndex), 0.65f, EPSILON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLabelTextGradientSameSizePaddingRepublishesP(void)
{
  UiTestApplication application;
  auto&             gl = application.GetGlAbstraction();
  gl.SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  gl.EnableTextureCallTrace(true);

  Label label = Label::New(
    "Gradient text must republish its raster and mapping when padding changes content bounds at fixed outer geometry");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(180.0f);
  label.SetFontSize(20.0f);
  label.SetMultiLine(true);
  label.SetTextGradient(MakeRenderableLinear());
  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::VIEW_BOUND);
  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  const Vector3  fixedSize           = label.GetCurrentSize();
  const uint32_t initialTextureWidth = label.GetRendererAt(0u).GetTextures().GetTexture(0u).GetWidth();

  gl.ResetTextureCallStack();
  label.SetPadding(Insets(80.0f, 90.0f, 5.0f, 9.0f));
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_EQUALS(label.GetCurrentSize(), fixedSize, TEST_LOCATION);
  DALI_TEST_CHECK(gl.GetTextureTrace().CountMethod("TexImage2D") > 0);
  DALI_TEST_CHECK(label.GetRendererAt(0u).GetTextures().GetTexture(0u).GetWidth() < initialTextureWidth);
  DALI_TEST_CHECK(label.GetTextGradient().GetType() != Gradient::Type::NONE);
  DALI_TEST_EQUALS(label.GetTextGradientBoundsMode(), Text::GradientBoundsMode::VIEW_BOUND, TEST_LOCATION);

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

int UtcDaliLabelTextRevealGradientOverlayCompositionP(void)
{
  UiTestApplication application;

  Label label = Label::New("Gradient and overlay remain composable with Text::Reveal");
  label.SetProperty(Actor::Property::SIZE, Vector3(420.0f, 80.0f, 0.0f));
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.2f));
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ONE, Vector2::ZERO, 0.4f));
  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
  Text::Reveal reveal;
  reveal.SetUnit(Text::Reveal::Unit::PIXEL);
  label.SetTextReveal(reveal);
  label.SetTextRevealProgress(0.5f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.GetRendererCount() > 0u);
  Renderer renderer = label.GetRendererAt(0u);
  DALI_TEST_CHECK(renderer.GetPropertyIndex("uTextRevealProgress") != Property::INVALID_INDEX);
  DALI_TEST_CHECK(renderer.GetTextures().GetTextureCount() >= 4u);
  END_TEST;
}

int UtcDaliLabelTextRevealConcurrentAnimationsAndRebuildP(void)
{
  UiTestApplication application;
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE);
  (void)Dali::TextAbstraction::FontClient::Get();

  Label label = Label::New("Reveal, gradients and text color animate independently");
  label.SetRequestedWidth(420.0f);
  label.SetRequestedHeight(96.0f);
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.1f));
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ONE, Vector2::ZERO, 0.2f));
  label.SetTextGradientOverlayMode(Text::GradientOverlayMode::SCREEN);
  label.SetTextColor(UiColor(Color::RED));
  label.SetTextReveal(Text::Reveal());
  label.SetTextRevealProgress(0.0f);

  application.GetScene().Add(label);
  application.SendNotification();
  application.Render(16);
  application.SendNotification();
  application.Render(16);

  Animation animation = Animation::New(2.0f);
  label.Animate(animation)
    .TextRevealProgress(1.0f, Duration(2.0f))
    .TextGradientStartOffset(0.8f, Duration(2.0f))
    .TextGradientOverlayStartOffset(0.9f, Duration(2.0f))
    .TextColor(UiColor(Color::BLUE), Duration(2.0f));

  const Property::Index revealIndex = label.GetPropertyIndex("uTextRevealProgress");
  const Property::Index baseIndex = label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
  const Property::Index overlayIndex = label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
  DALI_TEST_CHECK(revealIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(baseIndex != Property::INVALID_INDEX);
  DALI_TEST_CHECK(overlayIndex != Property::INVALID_INDEX);

  Renderer   initialRenderer = label.GetRendererAt(0u);
  TextureSet initialTextures = initialRenderer.GetTextures();
  const Property::Index initialRendererReveal = initialRenderer.GetPropertyIndex("uTextRevealProgress");
  const Property::Index initialRendererFade = initialRenderer.GetPropertyIndex("uTextRevealFadeDuration");
  const uint32_t initialRendererPropertyCount = initialRenderer.GetPropertyCount();
  DALI_TEST_CHECK(initialRendererReveal != Property::INVALID_INDEX);
  DALI_TEST_CHECK(initialRendererFade != Property::INVALID_INDEX);
  std::vector<Texture> textureResources;
  textureResources.reserve(initialTextures.GetTextureCount());
  for(uint32_t index = 0u; index < initialTextures.GetTextureCount(); ++index)
  {
    textureResources.push_back(initialTextures.GetTexture(index));
  }

  TestGlAbstraction& gl = application.GetGlAbstraction();
  gl.EnableTextureCallTrace(true);

  gl.ResetTextureCallStack();
  animation.Play();
  application.SendNotification();
  application.Render(300);
  application.SendNotification();
  application.Render(16);

  DALI_TEST_CHECK(label.GetRendererAt(0u) == initialRenderer);
  TextureSet currentTextures = label.GetRendererAt(0u).GetTextures();
  DALI_TEST_EQUALS(currentTextures.GetTextureCount(), textureResources.size(), TEST_LOCATION);
  for(uint32_t index = 0u; index < currentTextures.GetTextureCount(); ++index)
  {
    DALI_TEST_CHECK(currentTextures.GetTexture(index) == textureResources[index]);
  }
  const float revealProgress = label.GetCurrentProperty<float>(revealIndex);
  const float baseOffset = label.GetCurrentProperty<float>(baseIndex);
  const float overlayOffset = label.GetCurrentProperty<float>(overlayIndex);
  const Vector4 textColor = label.GetCurrentProperty<Vector4>(Label::Property::TEXT_COLOR);
  DALI_TEST_CHECK(revealProgress > 0.0f && revealProgress < 1.0f);
  DALI_TEST_CHECK(baseOffset > 0.1f && baseOffset < 0.8f);
  DALI_TEST_CHECK(overlayOffset > 0.2f && overlayOffset < 0.9f);
  DALI_TEST_CHECK(textColor.r < 1.0f && textColor.b > 0.0f);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);

  auto ExpectRendererBindings = [&]()
  {
    Renderer renderer = label.GetRendererAt(0u);
    const Property::Index rendererReveal = renderer.GetPropertyIndex("uTextRevealProgress");
    const Property::Index rendererBase = renderer.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME);
    const Property::Index rendererOverlay = renderer.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME);
    const Property::Index rendererColor = renderer.GetPropertyIndex("uTextColorAnimatable");
    DALI_TEST_CHECK(rendererReveal != Property::INVALID_INDEX);
    DALI_TEST_CHECK(rendererBase != Property::INVALID_INDEX);
    DALI_TEST_CHECK(rendererOverlay != Property::INVALID_INDEX);
    DALI_TEST_CHECK(rendererColor != Property::INVALID_INDEX);
    if(renderer == initialRenderer)
    {
      DALI_TEST_EQUALS(rendererReveal, initialRendererReveal, TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetPropertyIndex("uTextRevealFadeDuration"), initialRendererFade, TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetPropertyCount(), initialRendererPropertyCount, TEST_LOCATION);
    }
    // Renderer constraints may trail their animated Actor source by one update
    // frame. They must nevertheless track the same independently animated
    // values, including after a renderer rebuild.
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererReveal),
                     label.GetCurrentProperty<float>(revealIndex), 0.02f, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererBase),
                     label.GetCurrentProperty<float>(baseIndex), 0.02f, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetCurrentProperty<float>(rendererOverlay),
                     label.GetCurrentProperty<float>(overlayIndex), 0.02f, TEST_LOCATION);
  };
  ExpectRendererBindings();

  Renderer stableRenderer = label.GetRendererAt(0u);
  const float rendererRevealBefore = stableRenderer.GetCurrentProperty<float>(initialRendererReveal);
  const float rendererBaseBefore = stableRenderer.GetCurrentProperty<float>(
    stableRenderer.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME));
  const float rendererOverlayBefore = stableRenderer.GetCurrentProperty<float>(
    stableRenderer.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME));
  application.Render(100);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(stableRenderer.GetCurrentProperty<float>(initialRendererReveal) > rendererRevealBefore);
  DALI_TEST_CHECK(stableRenderer.GetCurrentProperty<float>(
                    stableRenderer.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME)) > rendererBaseBefore);
  DALI_TEST_CHECK(stableRenderer.GetCurrentProperty<float>(
                    stableRenderer.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME)) > rendererOverlayBefore);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexImage2D"), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(gl.GetTextureTrace().CountMethod("TexSubImage2D"), 0, TEST_LOCATION);

  // Exercise the legal renderer-rebuild path while every source property is
  // still animating. The stable Label indices must survive and the replacement
  // renderer must bind to their current values.
  const float beforeRebuild = label.GetTextRevealProgress();
  label.SetTextGradient(Gradient::Base::None());
  label.SetTextGradientOverlay(Gradient::Base::None());
  Text::Outline outline;
  outline.SetWidth(2.0f);
  label.SetTextOutline(outline);
  label.SetTextGradient(MakeRenderableLinear(Vector2::ZERO, Vector2::ONE, 0.35f));
  label.SetTextGradientOverlay(MakeRenderableLinear(Vector2::ONE, Vector2::ZERO, 0.45f));
  application.SendNotification();
  application.Render(32);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), revealIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), baseIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), overlayIndex, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetTextRevealProgress() >= beforeRebuild);
  ExpectRendererBindings();

  // Switch the text pipeline while both reveal and gradient animations remain
  // live. Publication may replace textures/renderers but not their source
  // properties or constraints.
  label.SetAsyncRendering(true);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
  application.SendNotification();
  application.Render(100);
  application.SendNotification();
  application.Render(16);
  DALI_TEST_EQUALS(label.GetPropertyIndex("uTextRevealProgress"), revealIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_START_OFFSET_PROPERTY_NAME), baseIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetPropertyIndex(TEXT_GRADIENT_OVERLAY_START_OFFSET_PROPERTY_NAME), overlayIndex, TEST_LOCATION);
  DALI_TEST_CHECK(label.GetTextRevealProgress() > beforeRebuild);
  ExpectRendererBindings();
  END_TEST;
}
