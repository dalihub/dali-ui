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

// EXTERNAL INCLUDES
#include <dali.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/anchor/anchor-interaction-data.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/text-model.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;

namespace
{

namespace PublicText = Dali::Ui::Text;

PublicText::LogicalModel& GetLogicalModel(const PublicText::ControllerPtr& controller)
{
  PublicText::Controller::Impl& impl = PublicText::Controller::Impl::GetImplementation(*controller.Get());
  return *impl.mModel->mLogicalModel;
}

PublicText::VisualModel& GetVisualModel(const PublicText::ControllerPtr& controller)
{
  PublicText::Controller::Impl& impl = PublicText::Controller::Impl::GetImplementation(*controller.Get());
  return *impl.mModel->mVisualModel;
}

PublicText::Model& GetLogicalModelObject(const PublicText::ControllerPtr& controller)
{
  PublicText::Controller::Impl& impl = PublicText::Controller::Impl::GetImplementation(*controller.Get());
  return *impl.mModel;
}

PublicText::Length CountGlyphsForCharacters(const PublicText::VisualModel& visualModel, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters)
{
  PublicText::Length numberOfGlyphs = 0u;
  for(PublicText::Length index = 0u; index < numberOfCharacters; ++index)
  {
    numberOfGlyphs += visualModel.mGlyphsPerCharacter[characterIndex + index];
  }
  return numberOfGlyphs;
}

void RelayoutController(const PublicText::ControllerPtr& controller)
{
  controller->Relayout(Size(320.0f, 120.0f));
}

void CheckTypesetterDecorationInput(PublicText::Model& model, bool expectedUnderlineRuns, bool expectedStrikethroughRuns)
{
  PublicText::TypesetterPtr typesetter = PublicText::Typesetter::New(&model);
  PublicText::ViewModel*    viewModel  = typesetter->GetViewModel();

  DALI_TEST_CHECK(viewModel != nullptr);
  DALI_TEST_EQUALS(viewModel->IsMarkupUnderlineSet(), expectedUnderlineRuns, TEST_LOCATION);
  DALI_TEST_EQUALS(viewModel->IsMarkupStrikethroughSet(), expectedStrikethroughRuns, TEST_LOCATION);
  DALI_TEST_EQUALS(viewModel->GetNumberOfUnderlineRuns() > 0u, expectedUnderlineRuns, TEST_LOCATION);
  DALI_TEST_EQUALS(viewModel->GetNumberOfStrikethroughRuns() > 0u, expectedStrikethroughRuns, TEST_LOCATION);
}

void CheckColorRun(const PublicText::ColorRun& colorRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters, const Vector4& color)
{
  DALI_TEST_EQUALS(colorRun.characterRun.characterIndex, characterIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(colorRun.characterRun.numberOfCharacters, numberOfCharacters, TEST_LOCATION);
  DALI_TEST_EQUALS(colorRun.color, color, TEST_LOCATION);
}

PublicText::Underline CreateUnderline(const Vector4& color, float thickness, PublicText::Underline::Type type = PublicText::Underline::Type::SOLID, float dashLength = 2.0f, float dashGap = 1.0f)
{
  PublicText::Underline underline;
  underline.SetColor(Dali::Ui::UiColor(color));
  underline.SetThickness(thickness);
  underline.SetType(type);
  underline.SetDashLength(dashLength);
  underline.SetDashGap(dashGap);
  return underline;
}

PublicText::LineThrough CreateLineThrough(const Vector4& color, float thickness)
{
  PublicText::LineThrough lineThrough;
  lineThrough.SetColor(Dali::Ui::UiColor(color));
  lineThrough.SetThickness(thickness);
  return lineThrough;
}

void CheckUnderlineRun(const PublicText::UnderlinedCharacterRun& underlineRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters, const PublicText::Underline& underline)
{
  DALI_TEST_EQUALS(underlineRun.characterRun.characterIndex, characterIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.characterRun.numberOfCharacters, numberOfCharacters, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.type, underline.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.color, underline.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.height, underline.GetThickness(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashWidth, underline.GetDashLength(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashGap, underline.GetDashGap(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.typeDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.colorDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.heightDefined, true, TEST_LOCATION);
}

void CheckUnderlineGlyphRun(const PublicText::VisualModel& visualModel, const PublicText::UnderlinedGlyphRun& underlineRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters, const PublicText::Underline& underline)
{
  DALI_TEST_EQUALS(underlineRun.glyphRun.glyphIndex, visualModel.mCharactersToGlyph[characterIndex], TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.glyphRun.numberOfGlyphs, CountGlyphsForCharacters(visualModel, characterIndex, numberOfCharacters), TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.type, underline.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.color, underline.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.height, underline.GetThickness(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashWidth, underline.GetDashLength(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashGap, underline.GetDashGap(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.typeDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.colorDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.heightDefined, true, TEST_LOCATION);
}

void CheckLineThroughRun(const PublicText::StrikethroughCharacterRun& lineThroughRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters, const PublicText::LineThrough& lineThrough)
{
  DALI_TEST_EQUALS(lineThroughRun.characterRun.characterIndex, characterIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.characterRun.numberOfCharacters, numberOfCharacters, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.color, lineThrough.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.height, lineThrough.GetThickness(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.colorDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.heightDefined, true, TEST_LOCATION);
}

void CheckLineThroughGlyphRun(const PublicText::VisualModel& visualModel, const PublicText::StrikethroughGlyphRun& lineThroughRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters, const PublicText::LineThrough& lineThrough)
{
  DALI_TEST_EQUALS(lineThroughRun.glyphRun.glyphIndex, visualModel.mCharactersToGlyph[characterIndex], TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.glyphRun.numberOfGlyphs, CountGlyphsForCharacters(visualModel, characterIndex, numberOfCharacters), TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.color, lineThrough.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.height, lineThrough.GetThickness(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.colorDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.heightDefined, true, TEST_LOCATION);
}

} // namespace

void utc_dali_styled_text_controller_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_styled_text_controller_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLabelAnchorTouchInterruptedP(void)
{
  UiTestApplication application;
  Dali::Ui::Label   label = Dali::Ui::Label::New();

  label.SetStyledText(PublicText::StyledText::FromMarkup("<a href='docs'>link</a>"));
  DALI_TEST_EQUALS(label.InterceptTouchEventSignal().GetConnectionCount(), 1u, TEST_LOCATION);

  TouchEvent started = TouchEvent::New(1u);
  started.AddPoint(1, PointState::STARTED, Vector2(10.0f, 10.0f));
  label.InterceptTouchEventSignal().Emit(label, started);

  Dali::Ui::Internal::Text::AnchorInteractionData* data =
    Dali::Ui::Internal::Text::GetAnchorInteractionData(label);
  DALI_TEST_CHECK(data && data->IsTouchDown());

  TouchEvent interrupted = TouchEvent::New(2u);
  interrupted.AddPoint(1, PointState::INTERRUPTED, Vector2(10.0f, 10.0f));
  label.InterceptTouchEventSignal().Emit(label, interrupted);
  DALI_TEST_CHECK(data && !data->IsTouchDown());

  END_TEST;
}

int UtcDaliStyledTextControllerAnchorColorsP(void)
{
  UiTestApplication application;

  PublicText::ControllerPtr   controller = PublicText::Controller::New();
  PublicText::Controller::Impl& impl       = PublicText::Controller::Impl::GetImplementation(*controller.Get());

  DALI_TEST_CHECK(!impl.mAnchorColorData);
  DALI_TEST_EQUALS(controller->GetAnchorColor(), Color::MEDIUM_BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetAnchorClickedColor(), Color::DARK_MAGENTA, TEST_LOCATION);

  controller->SetAnchorColor(Color::MEDIUM_BLUE);
  controller->SetAnchorClickedColor(Color::DARK_MAGENTA);
  DALI_TEST_CHECK(!impl.mAnchorColorData);

  controller->SetAnchorColor(Color::RED);
  controller->SetAnchorClickedColor(Color::GREEN);
  DALI_TEST_CHECK(impl.mAnchorColorData);
  DALI_TEST_EQUALS(controller->GetAnchorColor(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetAnchorClickedColor(), Color::GREEN, TEST_LOCATION);

  const auto*    colorData          = impl.mAnchorColorData.get();
  const Vector4& anchorColor        = controller->GetAnchorColor();
  const Vector4& anchorClickedColor = controller->GetAnchorClickedColor();

  controller->SetAnchorColor(Color::MEDIUM_BLUE);
  controller->SetAnchorClickedColor(Color::DARK_MAGENTA);
  DALI_TEST_CHECK(impl.mAnchorColorData.get() == colorData);
  DALI_TEST_EQUALS(anchorColor, Color::MEDIUM_BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(anchorClickedColor, Color::DARK_MAGENTA, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextControllerEmbossDataLifecycleP(void)
{
  UiTestApplication application;

  PublicText::ControllerPtr     controller = PublicText::Controller::New();
  PublicText::Controller::Impl& impl       = PublicText::Controller::Impl::GetImplementation(*controller.Get());

  DALI_TEST_CHECK(!impl.mEmbossData);
  DALI_TEST_CHECK(!controller->IsEmbossEnabled());
  DALI_TEST_EQUALS(controller->GetEmbossDirection(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossStrength(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossLightColor(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossShadowColor(), Vector4::ZERO, TEST_LOCATION);

  controller->SetDefaultEmbossProperties("");
  controller->SetEmbossDirection(Vector2::ZERO);
  controller->SetEmbossStrength(0.0f);
  controller->SetEmbossLightColor(Vector4::ZERO);
  controller->SetEmbossShadowColor(Vector4::ZERO);
  controller->SetEmbossEnabled(false);
  DALI_TEST_CHECK(!impl.mEmbossData);

  controller->SetEmbossEnabled(true);
  DALI_TEST_CHECK(controller->IsEmbossEnabled());
  DALI_TEST_CHECK(!impl.mEmbossData);
  DALI_TEST_EQUALS(controller->GetEmbossDirection(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossStrength(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossLightColor(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossShadowColor(), Vector4::ZERO, TEST_LOCATION);

  controller->SetDefaultEmbossProperties("emboss-properties");
  DALI_TEST_CHECK(impl.mEmbossData);
  PublicText::EmbossData* const embossData = impl.mEmbossData;
  DALI_TEST_EQUALS(controller->GetDefaultEmbossProperties(), "emboss-properties", TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossDirection(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossStrength(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossLightColor(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossShadowColor(), Vector4::ZERO, TEST_LOCATION);

  const Vector2 direction(-0.75f, 0.25f);
  const Vector4 lightColor(0.8f, 0.7f, 0.6f, 0.5f);
  const Vector4 shadowColor(0.1f, 0.2f, 0.3f, 0.4f);
  controller->SetEmbossDirection(direction);
  controller->SetEmbossStrength(2.5f);
  controller->SetEmbossLightColor(lightColor);
  controller->SetEmbossShadowColor(shadowColor);

  DALI_TEST_CHECK(impl.mEmbossData == embossData);
  const Vector2* const directionReference   = &controller->GetEmbossDirection();
  const Vector4* const lightColorReference  = &controller->GetEmbossLightColor();
  const Vector4* const shadowColorReference = &controller->GetEmbossShadowColor();

  controller->SetEmbossEnabled(false);
  DALI_TEST_CHECK(!controller->IsEmbossEnabled());
  DALI_TEST_CHECK(impl.mEmbossData == embossData);
  DALI_TEST_EQUALS(controller->GetEmbossDirection(), direction, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossStrength(), 2.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossLightColor(), lightColor, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetEmbossShadowColor(), shadowColor, TEST_LOCATION);

  controller->SetEmbossEnabled(true);
  DALI_TEST_CHECK(impl.mEmbossData == embossData);
  DALI_TEST_CHECK(&controller->GetEmbossDirection() == directionReference);
  DALI_TEST_CHECK(&controller->GetEmbossLightColor() == lightColorReference);
  DALI_TEST_CHECK(&controller->GetEmbossShadowColor() == shadowColorReference);

  const Vector2 updatedDirection(0.5f, -0.5f);
  const Vector4 updatedLightColor(0.4f, 0.5f, 0.6f, 0.7f);
  const Vector4 updatedShadowColor(0.7f, 0.6f, 0.5f, 0.4f);
  controller->SetEmbossDirection(updatedDirection);
  controller->SetEmbossLightColor(updatedLightColor);
  controller->SetEmbossShadowColor(updatedShadowColor);
  DALI_TEST_EQUALS(*directionReference, updatedDirection, TEST_LOCATION);
  DALI_TEST_EQUALS(*lightColorReference, updatedLightColor, TEST_LOCATION);
  DALI_TEST_EQUALS(*shadowColorReference, updatedShadowColor, TEST_LOCATION);

  controller->SetDefaultEmbossProperties("");
  DALI_TEST_CHECK(impl.mEmbossData == embossData);
  DALI_TEST_EQUALS(controller->GetDefaultEmbossProperties(), "", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextControllerTextFitDataP(void)
{
  UiTestApplication application;

  PublicText::ControllerPtr     controller = PublicText::Controller::New();
  PublicText::Controller::Impl& impl       = PublicText::Controller::Impl::GetImplementation(*controller.Get());

  DALI_TEST_CHECK(!impl.mTextFitData);
  DALI_TEST_CHECK(!controller->IsTextFitEnabled());
  DALI_TEST_CHECK(!controller->IsTextFitCandidatesEnabled());
  DALI_TEST_EQUALS(controller->GetTextFitMinSize(PublicText::Controller::POINT_SIZE), PublicText::DEFAULT_TEXTFIT_MIN, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetTextFitMaxSize(PublicText::Controller::POINT_SIZE), PublicText::DEFAULT_TEXTFIT_MAX, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetTextFitStepSize(PublicText::Controller::POINT_SIZE), PublicText::DEFAULT_TEXTFIT_STEP, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetTextFitContentSize(), Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_CHECK(controller->GetTextFitCandidates().Empty());

  controller->SetTextFitEnabled(false);
  controller->SetTextFitCandidatesEnabled(false);
  controller->SetTextFitChanged(false);
  controller->SetCurrentLineSize(0.0f);
  controller->SetTextFitMinSize(PublicText::DEFAULT_TEXTFIT_MIN, PublicText::Controller::POINT_SIZE);
  controller->SetTextFitMaxSize(PublicText::DEFAULT_TEXTFIT_MAX, PublicText::Controller::POINT_SIZE);
  controller->SetTextFitStepSize(PublicText::DEFAULT_TEXTFIT_STEP, PublicText::Controller::POINT_SIZE);
  controller->SetTextFitContentSize(Vector2::ZERO);
  controller->ClearTextFitCandidates();
  DALI_TEST_CHECK(!impl.mTextFitData);

  controller->SetTextFitEnabled(true);
  controller->SetTextFitMinSize(12.0f, PublicText::Controller::POINT_SIZE);
  controller->SetTextFitMaxSize(36.0f, PublicText::Controller::POINT_SIZE);
  controller->SetTextFitStepSize(2.0f, PublicText::Controller::POINT_SIZE);
  controller->SetCurrentLineSize(1.5f);
  controller->SetTextFitContentSize(Vector2(100.0f, 50.0f));
  controller->SetTextFitChanged(true);
  DALI_TEST_CHECK(impl.mTextFitData);
  DALI_TEST_CHECK(controller->IsTextFitEnabled());
  DALI_TEST_CHECK(controller->IsTextFitChanged());
  DALI_TEST_EQUALS(controller->GetTextFitMinSize(PublicText::Controller::POINT_SIZE), 12.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetTextFitMaxSize(PublicText::Controller::POINT_SIZE), 36.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetTextFitStepSize(PublicText::Controller::POINT_SIZE), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetCurrentLineSize(), 1.5f, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetTextFitContentSize(), Vector2(100.0f, 50.0f), TEST_LOCATION);

  Dali::Vector<PublicText::Fit::Candidate> candidates;
  candidates.PushBack(PublicText::Fit::Candidate(16.0f, 32.0f));
  candidates.PushBack(PublicText::Fit::Candidate(24.0f, 40.0f));
  controller->SetTextFitEnabled(false);
  controller->SetTextFitCandidatesEnabled(true);
  controller->SetTextFitCandidates(candidates);
  DALI_TEST_CHECK(!controller->IsTextFitEnabled());
  DALI_TEST_CHECK(controller->IsTextFitCandidatesEnabled());
  DALI_TEST_EQUALS(controller->GetTextFitCandidates().Count(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(controller->GetMaxFitCandidate());
  DALI_TEST_EQUALS(controller->GetMaxFitCandidate()->GetFontSize(), 24.0f, TEST_LOCATION);

  controller->SetText("TextFit should select a candidate that fits the available layout size");
  controller->SetDefaultFontSize(20.0f, PublicText::Controller::POINT_SIZE);
  controller->SetMultiLineEnabled(false);
  controller->FitCandidatesPointSizeForLayout(Size(1000.0f, 200.0f));
  DALI_TEST_EQUALS(controller->GetTextFitFontSize(PublicText::Controller::PIXEL_SIZE), 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  controller->SetTextFitCandidatesEnabled(false);
  controller->ClearTextFitCandidates();
  DALI_TEST_CHECK(controller->GetTextFitCandidates().Empty());
  DALI_TEST_EQUALS(controller->GetTextFitMinSize(PublicText::Controller::POINT_SIZE), 12.0f, TEST_LOCATION);

  controller->SetTextFitEnabled(true);
  controller->FitPointSizeforLayout(Size(320.0f, 80.0f));
  const float wideFitSize = controller->GetTextFitFontSize(PublicText::Controller::POINT_SIZE);
  DALI_TEST_CHECK(wideFitSize >= 12.0f && wideFitSize <= 36.0f);

  controller->SetTextFitContentSize(Size(320.0f, 80.0f));
  controller->FitPointSizeforLayout(Size(100.0f, 40.0f));
  const float narrowFitSize = controller->GetTextFitFontSize(PublicText::Controller::POINT_SIZE);
  DALI_TEST_CHECK(narrowFitSize >= 12.0f && narrowFitSize <= wideFitSize);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextForegroundColorSpanP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::ForegroundColorSpan         span    = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  std::string text;
  controller->GetText(text);
  DALI_TEST_EQUALS(text, std::string("Hello"), TEST_LOCATION);

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(logicalModel.mColorRuns[0u], 1u, 3u, Color::RED);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextBackgroundColorSpanP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::BackgroundColorSpan span    = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::CYAN));

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  std::string text;
  controller->GetText(text);
  DALI_TEST_EQUALS(text, std::string("Hello"), TEST_LOCATION);

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mBackgroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(logicalModel.mBackgroundColorRuns[0u], 1u, 3u, Color::CYAN);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextUnderlineSpanP(void)
{
  UiTestApplication application;

  const PublicText::Underline underline = CreateUnderline(Color::GREEN, 2.0f, PublicText::Underline::Type::DASHED, 4.0f, 2.0f);
  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::UnderlineSpan     span    = PublicText::UnderlineSpan::New(underline);

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mUnderlinedCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckUnderlineRun(logicalModel.mUnderlinedCharacterRuns[0u], 1u, 3u, underline);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextLineThroughSpanP(void)
{
  UiTestApplication application;

  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::RED, 2.5f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("Hello");
  PublicText::LineThroughSpan   span        = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mStrikethroughCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckLineThroughRun(logicalModel.mStrikethroughCharacterRuns[0u], 0u, 5u, lineThrough);

  END_TEST;
}

int UtcDaliStyledTextControllerUnderlineSpanReachesVisualModelP(void)
{
  UiTestApplication application;

  const PublicText::Underline underline = CreateUnderline(Color::GREEN, 2.0f, PublicText::Underline::Type::DASHED, 4.0f, 2.0f);
  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::UnderlineSpan     span    = PublicText::UnderlineSpan::New(underline);

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());
  RelayoutController(controller);

  PublicText::VisualModel& visualModel = GetVisualModel(controller);
  DALI_TEST_EQUALS(visualModel.GetNumberOfUnderlineRuns(), 1u, TEST_LOCATION);
  PublicText::UnderlinedGlyphRun underlineRun{};
  visualModel.GetUnderlineRuns(&underlineRun, 0u, 1u);
  CheckUnderlineGlyphRun(visualModel, underlineRun, 1u, 3u, underline);
  visualModel.GetStrikethroughRuns(nullptr, 0u, 0u);
  CheckTypesetterDecorationInput(GetLogicalModelObject(controller), true, false);

  END_TEST;
}

int UtcDaliStyledTextControllerLineThroughSpanReachesVisualModelP(void)
{
  UiTestApplication application;

  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::RED, 2.5f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("Hello");
  PublicText::LineThroughSpan   span        = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());
  RelayoutController(controller);

  PublicText::VisualModel& visualModel = GetVisualModel(controller);
  DALI_TEST_EQUALS(visualModel.GetNumberOfStrikethroughRuns(), 1u, TEST_LOCATION);
  PublicText::StrikethroughGlyphRun lineThroughRun{};
  visualModel.GetStrikethroughRuns(&lineThroughRun, 0u, 1u);
  CheckLineThroughGlyphRun(visualModel, lineThroughRun, 0u, 5u, lineThrough);
  visualModel.GetUnderlineRuns(nullptr, 0u, 0u);
  CheckTypesetterDecorationInput(GetLogicalModelObject(controller), false, true);

  END_TEST;
}

int UtcDaliStyledTextControllerDecorationMatchesFromMarkupP(void)
{
  UiTestApplication application;

  PublicText::ControllerPtr fromMarkupController = PublicText::Controller::New();
  fromMarkupController->SetStyledText(PublicText::StyledText::FromMarkup("H<u color='green' height='2.0f' type='dashed' dash-width='4.0f' dash-gap='2.0f'>ell</u><s color='red' height='2.5f'>o</s>"));
  RelayoutController(fromMarkupController);

  const PublicText::Underline   underline   = CreateUnderline(Color::GREEN, 2.0f, PublicText::Underline::Type::DASHED, 4.0f, 2.0f);
  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::RED, 2.5f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("Hello");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::UnderlineSpan::New(underline), 1u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::LineThroughSpan::New(lineThrough), 4u, 5u));

  PublicText::ControllerPtr styledController = PublicText::Controller::New();
  styledController->SetStyledText(builder.Build());
  RelayoutController(styledController);

  PublicText::VisualModel& fromMarkupVisualModel = GetVisualModel(fromMarkupController);
  PublicText::VisualModel& styledVisualModel = GetVisualModel(styledController);

  DALI_TEST_EQUALS(fromMarkupVisualModel.GetNumberOfUnderlineRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.GetNumberOfUnderlineRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(fromMarkupVisualModel.GetNumberOfStrikethroughRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.GetNumberOfStrikethroughRuns(), 1u, TEST_LOCATION);
  CheckTypesetterDecorationInput(GetLogicalModelObject(fromMarkupController), true, true);
  CheckTypesetterDecorationInput(GetLogicalModelObject(styledController), true, true);

  DALI_TEST_EQUALS(styledVisualModel.mUnderlineRuns[0u].glyphRun.glyphIndex, fromMarkupVisualModel.mUnderlineRuns[0u].glyphRun.glyphIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mUnderlineRuns[0u].glyphRun.numberOfGlyphs, fromMarkupVisualModel.mUnderlineRuns[0u].glyphRun.numberOfGlyphs, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mUnderlineRuns[0u].properties.color, fromMarkupVisualModel.mUnderlineRuns[0u].properties.color, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mUnderlineRuns[0u].properties.height, fromMarkupVisualModel.mUnderlineRuns[0u].properties.height, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mUnderlineRuns[0u].properties.type, fromMarkupVisualModel.mUnderlineRuns[0u].properties.type, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mUnderlineRuns[0u].properties.dashWidth, fromMarkupVisualModel.mUnderlineRuns[0u].properties.dashWidth, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mUnderlineRuns[0u].properties.dashGap, fromMarkupVisualModel.mUnderlineRuns[0u].properties.dashGap, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  DALI_TEST_EQUALS(styledVisualModel.mStrikethroughRuns[0u].glyphRun.glyphIndex, fromMarkupVisualModel.mStrikethroughRuns[0u].glyphRun.glyphIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mStrikethroughRuns[0u].glyphRun.numberOfGlyphs, fromMarkupVisualModel.mStrikethroughRuns[0u].glyphRun.numberOfGlyphs, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mStrikethroughRuns[0u].properties.color, fromMarkupVisualModel.mStrikethroughRuns[0u].properties.color, TEST_LOCATION);
  DALI_TEST_EQUALS(styledVisualModel.mStrikethroughRuns[0u].properties.height, fromMarkupVisualModel.mStrikethroughRuns[0u].properties.height, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextForegroundAndBackgroundIndependentP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder        = PublicText::StyledTextBuilder::New("Hello");
  PublicText::ForegroundColorSpan           foregroundSpan = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));
  PublicText::BackgroundColorSpan backgroundSpan = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW));

  DALI_TEST_CHECK(builder.SetSpan(foregroundSpan, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(backgroundSpan, 1u, 5u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mColorRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mBackgroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(logicalModel.mColorRuns[0u], 0u, 2u, Color::RED);
  CheckColorRun(logicalModel.mBackgroundColorRuns[0u], 1u, 4u, Color::YELLOW);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextForegroundBackgroundDecorationIndependentP(void)
{
  UiTestApplication application;

  const PublicText::Underline   underline   = CreateUnderline(Color::BLUE, 2.0f);
  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::MAGENTA, 3.0f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("Hello");
  PublicText::ForegroundColorSpan foregroundSpan = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));
  PublicText::BackgroundColorSpan backgroundSpan = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW));
  PublicText::UnderlineSpan       underlineSpan = PublicText::UnderlineSpan::New(underline);
  PublicText::LineThroughSpan     lineThroughSpan = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(foregroundSpan, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(backgroundSpan, 1u, 5u));
  DALI_TEST_CHECK(builder.SetSpan(underlineSpan, 2u, 5u));
  DALI_TEST_CHECK(builder.SetSpan(lineThroughSpan, 0u, 5u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mColorRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mBackgroundColorRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mUnderlinedCharacterRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mStrikethroughCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(logicalModel.mColorRuns[0u], 0u, 2u, Color::RED);
  CheckColorRun(logicalModel.mBackgroundColorRuns[0u], 1u, 4u, Color::YELLOW);
  CheckUnderlineRun(logicalModel.mUnderlinedCharacterRuns[0u], 2u, 3u, underline);
  CheckLineThroughRun(logicalModel.mStrikethroughCharacterRuns[0u], 0u, 5u, lineThrough);

  END_TEST;
}

int UtcDaliStyledTextControllerSetTextTreatsMarkupAsPlainTextP(void)
{
  UiTestApplication application;

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetText("Default <color value='red'>Red</color> Default");

  std::string text;
  controller->GetText(text);
  DALI_TEST_EQUALS(text, std::string("Default <color value='red'>Red</color> Default"), TEST_LOCATION);
  DALI_TEST_EQUALS(GetLogicalModel(controller).mColorRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextClearsOldDecorationRunsP(void)
{
  UiTestApplication application;

  const PublicText::Underline   underline   = CreateUnderline(Color::GREEN, 2.0f);
  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::RED, 3.0f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("Styled decorations");
  PublicText::UnderlineSpan     underlineSpan = PublicText::UnderlineSpan::New(underline);
  PublicText::LineThroughSpan   lineThroughSpan = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(underlineSpan, 0u, 6u));
  DALI_TEST_CHECK(builder.SetSpan(lineThroughSpan, 7u, 18u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mUnderlinedCharacterRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mStrikethroughCharacterRuns.Count(), 1u, TEST_LOCATION);

  RelayoutController(controller);
  PublicText::VisualModel& visualModel = GetVisualModel(controller);
  DALI_TEST_EQUALS(visualModel.GetNumberOfUnderlineRuns(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(visualModel.GetNumberOfStrikethroughRuns(), 1u, TEST_LOCATION);

  controller->SetStyledText(PublicText::StyledText::New("Plain"));
  RelayoutController(controller);

  std::string text;
  controller->GetText(text);
  DALI_TEST_EQUALS(text, std::string("Plain"), TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mUnderlinedCharacterRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mStrikethroughCharacterRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(visualModel.GetNumberOfUnderlineRuns(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(visualModel.GetNumberOfStrikethroughRuns(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextClearsOldBackgroundColorRunsP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder = PublicText::StyledTextBuilder::New("Styled background");
  PublicText::BackgroundColorSpan span    = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::CYAN));

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 6u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(builder.Build());

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_EQUALS(logicalModel.mBackgroundColorRuns.Count(), 1u, TEST_LOCATION);

  controller->SetStyledText(PublicText::StyledText::New("Plain"));

  std::string text;
  controller->GetText(text);
  DALI_TEST_EQUALS(text, std::string("Plain"), TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mBackgroundColorRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextControllerSetStyledTextClearsOldFromMarkupColorRunsP(void)
{
  UiTestApplication application;

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetStyledText(PublicText::StyledText::FromMarkup("Default <color value='red'>Red</color> Default"));

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  DALI_TEST_CHECK(logicalModel.mColorRuns.Count() > 0u);

  controller->SetStyledText(PublicText::StyledText::New("Plain"));

  std::string text;
  controller->GetText(text);
  DALI_TEST_EQUALS(text, std::string("Plain"), TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mBackgroundColorRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}
