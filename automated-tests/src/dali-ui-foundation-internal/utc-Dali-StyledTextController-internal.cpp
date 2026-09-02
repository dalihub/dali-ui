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
#include <dali/integration-api/pixel-data-integ.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/text/anchor/anchor-interaction-data.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/atlas/text-atlas-renderer.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/styled-text/gradient-span-data.h>
#include <dali-ui-foundation/internal/text/text-model.h>
#include <dali-ui-foundation/internal/text/visual-model-impl.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/gradient-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/gradient/conic-gradient.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/gradient/radial-gradient.h>
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

Dali::Ui::Gradient::Linear CreateGradient(const Vector4& first = Color::RED,
                                          const Vector4& second = Color::BLUE)
{
  Dali::Ui::Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(first)),
                         Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(second))});
  return gradient;
}

Dali::Ui::Gradient::Radial CreateRadialGradient(const Vector4& first, const Vector4& second)
{
  Dali::Ui::Gradient::Radial gradient(Vector2(0.0f, 0.0f), 0.5f);
  gradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(first)),
                         Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(second))});
  return gradient;
}

Dali::Ui::Gradient::Conic CreateConicGradient(const Vector4& first, const Vector4& second)
{
  Dali::Ui::Gradient::Conic gradient(Vector2(0.0f, 0.0f), Radian(0.0f));
  gradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(first)),
                         Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(second))});
  return gradient;
}

bool ContainsColoredTextPixel(const PixelData& pixelData)
{
  if(pixelData.GetPixelFormat() != Pixel::RGBA8888)
  {
    return false;
  }
  const Dali::Integration::PixelDataBuffer pixels = Dali::Integration::GetPixelDataBuffer(pixelData);
  if(!pixels.buffer)
  {
    return false;
  }
  for(uint32_t y = 0u; y < pixelData.GetHeight(); ++y)
  {
    const uint8_t* row = pixels.buffer + y * pixelData.GetStrideBytes();
    for(uint32_t x = 0u; x < pixelData.GetWidth(); ++x)
    {
      const uint8_t* pixel = row + x * 4u;
      if(pixel[3u] > 0u && (pixel[0u] > 0u || pixel[1u] > 0u || pixel[2u] > 0u))
      {
        return true;
      }
    }
  }
  return false;
}

bool ContainsVisibleTextPixel(const PixelData& pixelData)
{
  if(pixelData.GetPixelFormat() != Pixel::RGBA8888)
  {
    return false;
  }
  const Dali::Integration::PixelDataBuffer pixels = Dali::Integration::GetPixelDataBuffer(pixelData);
  if(!pixels.buffer)
  {
    return false;
  }
  for(uint32_t y = 0u; y < pixelData.GetHeight(); ++y)
  {
    const uint8_t* row = pixels.buffer + y * pixelData.GetStrideBytes();
    for(uint32_t x = 0u; x < pixelData.GetWidth(); ++x)
    {
      if(row[x * 4u + 3u] > 0u)
      {
        return true;
      }
    }
  }
  return false;
}

struct GradientUnitPixelEvidence
{
  bool     found{false};
  uint32_t x{0u};
  uint32_t y{0u};
  float    objectExpected{0.0f};
  float    userExpected{0.0f};
  float    objectObserved{0.0f};
  float    userObserved{0.0f};
};

GradientUnitPixelEvidence FindGradientUnitPixelEvidence(const PixelData& objectPixels,
                                                        const PixelData& userPixels,
                                                        float            spanLeft,
                                                        float            spanWidth,
                                                        float            userStart,
                                                        float            userEnd)
{
  GradientUnitPixelEvidence evidence;
  if(objectPixels.GetPixelFormat() != Pixel::RGBA8888 ||
     userPixels.GetPixelFormat() != Pixel::RGBA8888 ||
     objectPixels.GetWidth() != userPixels.GetWidth() ||
     objectPixels.GetHeight() != userPixels.GetHeight() ||
     spanWidth < Math::MACHINE_EPSILON_1000 ||
     userEnd - userStart < Math::MACHINE_EPSILON_1000)
  {
    return evidence;
  }

  const Dali::Integration::PixelDataBuffer objectBuffer = Dali::Integration::GetPixelDataBuffer(objectPixels);
  const Dali::Integration::PixelDataBuffer userBuffer   = Dali::Integration::GetPixelDataBuffer(userPixels);
  if(!objectBuffer.buffer || !userBuffer.buffer)
  {
    return evidence;
  }

  for(uint32_t y = 0u; y < objectPixels.GetHeight(); ++y)
  {
    const uint8_t* objectRow = objectBuffer.buffer + y * objectPixels.GetStrideBytes();
    const uint8_t* userRow   = userBuffer.buffer + y * userPixels.GetStrideBytes();
    for(uint32_t x = 0u; x < objectPixels.GetWidth(); ++x)
    {
      const uint8_t* objectPixel = objectRow + x * 4u;
      const uint8_t* userPixel   = userRow + x * 4u;
      if(objectPixel[3u] < 192u || userPixel[3u] < 192u)
      {
        continue;
      }

      const float localX         = static_cast<float>(x) + 0.5f - spanLeft;
      const float objectExpected = std::max(0.0f, std::min(1.0f, localX / spanWidth));
      const float userExpected   = std::max(0.0f, std::min(1.0f, (localX - userStart) / (userEnd - userStart)));
      if(std::fabs(objectExpected - userExpected) < 0.35f)
      {
        continue;
      }

      const float objectColorTotal = static_cast<float>(objectPixel[0u]) + objectPixel[2u];
      const float userColorTotal   = static_cast<float>(userPixel[0u]) + userPixel[2u];
      if(objectColorTotal < 1.0f || userColorTotal < 1.0f)
      {
        continue;
      }

      const float objectObserved = static_cast<float>(objectPixel[2u]) / objectColorTotal;
      const float userObserved   = static_cast<float>(userPixel[2u]) / userColorTotal;
      if(std::fabs(objectObserved - objectExpected) <= 0.06f &&
         std::fabs(userObserved - userExpected) <= 0.06f)
      {
        evidence.found          = true;
        evidence.x              = x;
        evidence.y              = y;
        evidence.objectExpected = objectExpected;
        evidence.userExpected   = userExpected;
        evidence.objectObserved = objectObserved;
        evidence.userObserved   = userObserved;
        return evidence;
      }
    }
  }
  return evidence;
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

int UtcDaliVisualModelCutoutDataLifecycleP(void)
{
  UiTestApplication application;

  PublicText::VisualModelPtr visualModel = PublicText::VisualModel::New();
  DALI_TEST_CHECK(!visualModel->mCutoutData);
  DALI_TEST_CHECK(!visualModel->IsCutoutEnabled());
  DALI_TEST_CHECK(!visualModel->IsBackgroundWithCutoutEnabled());
  DALI_TEST_EQUALS(visualModel->GetBackgroundColorWithCutout(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visualModel->GetOffsetWithCutout(), Vector2::ZERO, TEST_LOCATION);

  const Vector4* const defaultColorReference  = &visualModel->GetBackgroundColorWithCutout();
  const Vector2* const defaultOffsetReference = &visualModel->GetOffsetWithCutout();
  visualModel->SetCutoutEnabled(false);
  visualModel->SetBackgroundWithCutoutEnabled(false);
  visualModel->SetBackgroundColorWithCutout(Vector4::ZERO);
  visualModel->SetOffsetWithCutout(Vector2::ZERO);
  DALI_TEST_CHECK(!visualModel->mCutoutData);

  visualModel->SetCutoutEnabled(true);
  visualModel->SetBackgroundWithCutoutEnabled(true);
  DALI_TEST_CHECK(!visualModel->mCutoutData);
  DALI_TEST_EQUALS(visualModel->GetBackgroundColorWithCutout(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visualModel->GetOffsetWithCutout(), Vector2::ZERO, TEST_LOCATION);

  const Vector4 backgroundColor(0.1f, 0.2f, 0.3f, 0.4f);
  const Vector2 offset(7.0f, 11.0f);
  visualModel->SetBackgroundColorWithCutout(backgroundColor);
  DALI_TEST_CHECK(visualModel->mCutoutData);
  PublicText::CutoutData* const cutoutData = visualModel->mCutoutData;
  visualModel->SetOffsetWithCutout(offset);

  DALI_TEST_EQUALS(*defaultColorReference, Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(*defaultOffsetReference, Vector2::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(visualModel->GetBackgroundColorWithCutout(), backgroundColor, TEST_LOCATION);
  DALI_TEST_EQUALS(visualModel->GetOffsetWithCutout(), offset, TEST_LOCATION);

  const Vector4* const colorReference  = &visualModel->GetBackgroundColorWithCutout();
  const Vector2* const offsetReference = &visualModel->GetOffsetWithCutout();
  visualModel->SetCutoutEnabled(false);
  visualModel->SetBackgroundWithCutoutEnabled(false);
  DALI_TEST_CHECK(visualModel->mCutoutData == cutoutData);
  DALI_TEST_EQUALS(*colorReference, backgroundColor, TEST_LOCATION);
  DALI_TEST_EQUALS(*offsetReference, offset, TEST_LOCATION);

  visualModel->SetBackgroundColorWithCutout(Vector4::ZERO);
  visualModel->SetOffsetWithCutout(Vector2::ZERO);
  DALI_TEST_CHECK(visualModel->mCutoutData == cutoutData);
  DALI_TEST_CHECK(&visualModel->GetBackgroundColorWithCutout() == colorReference);
  DALI_TEST_CHECK(&visualModel->GetOffsetWithCutout() == offsetReference);
  DALI_TEST_EQUALS(*colorReference, Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(*offsetReference, Vector2::ZERO, TEST_LOCATION);

  PublicText::VisualModelPtr offsetOnlyModel = PublicText::VisualModel::New();
  offsetOnlyModel->SetOffsetWithCutout(offset);
  DALI_TEST_CHECK(offsetOnlyModel->mCutoutData);
  DALI_TEST_EQUALS(offsetOnlyModel->GetBackgroundColorWithCutout(), Vector4::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(offsetOnlyModel->GetOffsetWithCutout(), offset, TEST_LOCATION);

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

int UtcDaliStyledTextControllerMaximumLinesTextFitP(void)
{
  UiTestApplication application;
  constexpr float   WIDTH          = 200.0f;
  constexpr float   HEIGHT         = 1000.0f;
  constexpr float   MIN_POINT_SIZE = 6.0f;
  constexpr float   MAX_POINT_SIZE = 30.0f;
  const char* const LONG_TEXT      = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";

  auto makeRangeController = [&](const char* text, int maximumNumberOfLines, bool setMaximumNumberOfLines = true)
  {
    PublicText::ControllerPtr controller = PublicText::Controller::New();
    controller->SetText(text);
    controller->SetDefaultFontSize(18.0f, PublicText::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(true);
    controller->SetLineWrapMode(PublicText::LineWrapMode::CHARACTER);
    if(setMaximumNumberOfLines)
    {
      controller->SetMaximumNumberOfLines(maximumNumberOfLines);
    }
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(PublicText::EllipsisPosition::END);
    controller->SetTextFitEnabled(true);
    controller->SetTextFitMinSize(MIN_POINT_SIZE, PublicText::Controller::POINT_SIZE);
    controller->SetTextFitMaxSize(MAX_POINT_SIZE, PublicText::Controller::POINT_SIZE);
    controller->SetTextFitStepSize(3.0f, PublicText::Controller::POINT_SIZE);
    controller->FitPointSizeforLayout(Size(WIDTH, HEIGHT));
    return controller;
  };

  PublicText::ControllerPtr rangeDefault =
    makeRangeController(LONG_TEXT, PublicText::MAXIMUM_LINES_UNLIMITED, false);
  PublicText::ControllerPtr rangeUnlimited =
    makeRangeController(LONG_TEXT, PublicText::MAXIMUM_LINES_UNLIMITED);
  DALI_TEST_EQUALS(rangeUnlimited->GetTextFitFontSize(PublicText::Controller::POINT_SIZE),
                   rangeDefault->GetTextFitFontSize(PublicText::Controller::POINT_SIZE),
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(rangeUnlimited->GetLineCount(WIDTH), rangeDefault->GetLineCount(WIDTH), TEST_LOCATION);
  const float unlimitedRangeSize =
    rangeUnlimited->GetTextFitFontSize(PublicText::Controller::POINT_SIZE);
  DALI_TEST_CHECK(rangeUnlimited->GetLineCount(WIDTH) > 2);

  PublicText::ControllerPtr rangeCapped     = makeRangeController(LONG_TEXT, 2);
  const float               cappedRangeSize = rangeCapped->GetTextFitFontSize(PublicText::Controller::POINT_SIZE);
  DALI_TEST_CHECK(cappedRangeSize < unlimitedRangeSize);
  DALI_TEST_CHECK(rangeCapped->GetLineCount(WIDTH) <= 2);

  rangeUnlimited->SetMaximumNumberOfLines(2);
  rangeUnlimited->FitPointSizeforLayout(Size(WIDTH, HEIGHT));
  DALI_TEST_EQUALS(rangeUnlimited->GetTextFitFontSize(PublicText::Controller::POINT_SIZE),
                   cappedRangeSize,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  rangeUnlimited->SetMaximumNumberOfLines(PublicText::MAXIMUM_LINES_UNLIMITED);
  rangeUnlimited->FitPointSizeforLayout(Size(WIDTH, HEIGHT));
  DALI_TEST_EQUALS(rangeUnlimited->GetTextFitFontSize(PublicText::Controller::POINT_SIZE),
                   unlimitedRangeSize,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);

  auto makeCandidateController = [&](int maximumNumberOfLines, bool setMaximumNumberOfLines = true)
  {
    PublicText::ControllerPtr controller = PublicText::Controller::New();
    controller->SetText(LONG_TEXT);
    controller->SetDefaultFontSize(18.0f, PublicText::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(true);
    controller->SetLineWrapMode(PublicText::LineWrapMode::CHARACTER);
    if(setMaximumNumberOfLines)
    {
      controller->SetMaximumNumberOfLines(maximumNumberOfLines);
    }
    controller->SetTextFitCandidatesEnabled(true);

    Dali::Vector<PublicText::Fit::Candidate> candidates;
    candidates.PushBack(PublicText::Fit::Candidate(8.0f, 10.0f));
    candidates.PushBack(PublicText::Fit::Candidate(16.0f, 18.0f));
    candidates.PushBack(PublicText::Fit::Candidate(24.0f, 26.0f));
    candidates.PushBack(PublicText::Fit::Candidate(32.0f, 34.0f));
    candidates.PushBack(PublicText::Fit::Candidate(40.0f, 42.0f));
    controller->SetTextFitCandidates(candidates);
    controller->FitCandidatesPointSizeForLayout(Size(WIDTH, HEIGHT));
    return controller;
  };

  PublicText::ControllerPtr candidatesDefault =
    makeCandidateController(PublicText::MAXIMUM_LINES_UNLIMITED, false);
  PublicText::ControllerPtr candidatesUnlimited =
    makeCandidateController(PublicText::MAXIMUM_LINES_UNLIMITED);
  DALI_TEST_EQUALS(candidatesUnlimited->GetTextFitFontSize(PublicText::Controller::PIXEL_SIZE),
                   candidatesDefault->GetTextFitFontSize(PublicText::Controller::PIXEL_SIZE),
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(candidatesUnlimited->GetLineCount(WIDTH),
                   candidatesDefault->GetLineCount(WIDTH),
                   TEST_LOCATION);
  const float unlimitedCandidateSize =
    candidatesUnlimited->GetTextFitFontSize(PublicText::Controller::PIXEL_SIZE);
  DALI_TEST_CHECK(candidatesUnlimited->GetLineCount(WIDTH) > 2);

  PublicText::ControllerPtr candidatesCapped = makeCandidateController(2);
  const float               cappedCandidateSize =
    candidatesCapped->GetTextFitFontSize(PublicText::Controller::PIXEL_SIZE);
  DALI_TEST_CHECK(cappedCandidateSize < unlimitedCandidateSize);
  DALI_TEST_CHECK(candidatesCapped->GetLineCount(WIDTH) <= 2);

  candidatesUnlimited->SetMaximumNumberOfLines(2);
  candidatesUnlimited->FitCandidatesPointSizeForLayout(Size(WIDTH, HEIGHT));
  DALI_TEST_EQUALS(candidatesUnlimited->GetTextFitFontSize(PublicText::Controller::PIXEL_SIZE),
                   cappedCandidateSize,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  candidatesUnlimited->SetMaximumNumberOfLines(PublicText::MAXIMUM_LINES_UNLIMITED);
  candidatesUnlimited->FitCandidatesPointSizeForLayout(Size(WIDTH, HEIGHT));
  DALI_TEST_EQUALS(candidatesUnlimited->GetTextFitFontSize(PublicText::Controller::PIXEL_SIZE),
                   unlimitedCandidateSize,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);

  // Exactly N authored lines still fit at the largest size. Both an N+1
  // authored line and the empty line created by a trailing newline must not.
  PublicText::ControllerPtr exact = makeRangeController("A\nB", 2);
  DALI_TEST_EQUALS(exact->GetTextFitFontSize(PublicText::Controller::POINT_SIZE),
                   MAX_POINT_SIZE,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);

  PublicText::ControllerPtr explicitOverflow = makeRangeController("A\nB\nC", 2);
  DALI_TEST_EQUALS(explicitOverflow->GetTextFitFontSize(PublicText::Controller::POINT_SIZE),
                   MIN_POINT_SIZE,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  explicitOverflow->Relayout(Size(WIDTH, HEIGHT));
  DALI_TEST_EQUALS(explicitOverflow->GetRenderTextModel()->GetNumberOfLines(), 2, TEST_LOCATION);
  DALI_TEST_CHECK(std::any_of(GetVisualModel(explicitOverflow).mLines.Begin(),
                              GetVisualModel(explicitOverflow).mLines.End(),
                              [](const PublicText::LineRun& line)
  { return line.ellipsis; }));
  DALI_TEST_EQUALS(explicitOverflow->GetLineCount(WIDTH), 2, TEST_LOCATION);

  PublicText::ControllerPtr trailingNewline = makeRangeController("A\nB\n", 2);
  DALI_TEST_EQUALS(trailingNewline->GetTextFitFontSize(PublicText::Controller::POINT_SIZE),
                   MIN_POINT_SIZE,
                   Math::MACHINE_EPSILON_1000,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(trailingNewline->GetLineCount(WIDTH), 2, TEST_LOCATION);

  // Exercise the real StyledText path rather than relying on this test file's
  // plain SetText coverage. Spans cross soft-wrap and MaximumLines boundaries while
  // both fitting APIs must select the same result as an independently built
  // capped controller and restore the unlimited result exactly.
  const char* const STYLED_MARKUP =
    "<font weight='bold'>AAAAAAAAAAAAAAAAAAAA</font>"
    "<u>BBBBBBBBBBBBBBBBBBBB</u>"
    "<font slant='italic'>CCCCCCCCCCCCCCCCCCCC</font>";
  auto makeStyledFitController = [&](int maximumNumberOfLines, bool useCandidates)
  {
    PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::FromMarkup(STYLED_MARKUP);
    PublicText::ControllerPtr     controller = PublicText::Controller::New();
    controller->SetStyledText(builder.Build());
    controller->SetDefaultFontSize(18.0f, PublicText::Controller::PIXEL_SIZE);
    controller->SetMultiLineEnabled(true);
    controller->SetLineWrapMode(PublicText::LineWrapMode::CHARACTER);
    controller->SetTextElideEnabled(true);
    controller->SetEllipsisPosition(PublicText::EllipsisPosition::END);
    controller->SetMaximumNumberOfLines(maximumNumberOfLines);
    if(useCandidates)
    {
      controller->SetTextFitCandidatesEnabled(true);
      controller->SetTextFitCandidates(candidatesDefault->GetTextFitCandidates());
      controller->FitCandidatesPointSizeForLayout(Size(WIDTH, HEIGHT));
    }
    else
    {
      controller->SetTextFitEnabled(true);
      controller->SetTextFitMinSize(MIN_POINT_SIZE, PublicText::Controller::POINT_SIZE);
      controller->SetTextFitMaxSize(MAX_POINT_SIZE, PublicText::Controller::POINT_SIZE);
      controller->SetTextFitStepSize(3.0f, PublicText::Controller::POINT_SIZE);
      controller->FitPointSizeforLayout(Size(WIDTH, HEIGHT));
    }
    return controller;
  };

  for(bool useCandidates : {false, true})
  {
    PublicText::ControllerPtr styledUnlimited =
      makeStyledFitController(PublicText::MAXIMUM_LINES_UNLIMITED, useCandidates);
    PublicText::ControllerPtr styledCapped = makeStyledFitController(2, useCandidates);
    const PublicText::Controller::FontSizeType unit =
      useCandidates ? PublicText::Controller::PIXEL_SIZE : PublicText::Controller::POINT_SIZE;
    const float unlimitedFitSize = styledUnlimited->GetTextFitFontSize(unit);
    const float cappedFitSize    = styledCapped->GetTextFitFontSize(unit);
    DALI_TEST_CHECK(styledUnlimited->GetLineCount(WIDTH) > 2);
    DALI_TEST_CHECK(styledCapped->GetLineCount(WIDTH) <= 2);
    DALI_TEST_CHECK(cappedFitSize < unlimitedFitSize);

    styledUnlimited->SetMaximumNumberOfLines(2);
    if(useCandidates)
    {
      styledUnlimited->FitCandidatesPointSizeForLayout(Size(WIDTH, HEIGHT));
    }
    else
    {
      styledUnlimited->FitPointSizeforLayout(Size(WIDTH, HEIGHT));
    }
    DALI_TEST_EQUALS(styledUnlimited->GetTextFitFontSize(unit),
                     cappedFitSize,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);

    styledUnlimited->SetMaximumNumberOfLines(PublicText::MAXIMUM_LINES_UNLIMITED);
    if(useCandidates)
    {
      styledUnlimited->FitCandidatesPointSizeForLayout(Size(WIDTH, HEIGHT));
    }
    else
    {
      styledUnlimited->FitPointSizeforLayout(Size(WIDTH, HEIGHT));
    }
    DALI_TEST_EQUALS(styledUnlimited->GetTextFitFontSize(unit),
                     unlimitedFitSize,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
  }

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

int UtcDaliStyledTextControllerGradientSpanReachesRenderersP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("GradientSpan");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::GradientSpan::New(CreateGradient()), 0u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(
    PublicText::GradientSpan::New(CreateRadialGradient(Color::GREEN, Color::YELLOW),
                                  PublicText::GradientSpan::BoundsMode::CONTENT_BOUND),
    4u,
    8u));
  DALI_TEST_CHECK(builder.SetSpan(
    PublicText::GradientSpan::New(CreateConicGradient(Color::MAGENTA, Color::CYAN),
                                  PublicText::GradientSpan::BoundsMode::VIEW_BOUND),
    8u,
    12u));

  PublicText::ControllerPtr controller = PublicText::Controller::New();
  controller->SetDefaultFontSize(24.0f, PublicText::Controller::PIXEL_SIZE);
  controller->SetStyledText(builder.Build());
  RelayoutController(controller);

  PublicText::LogicalModel& logicalModel = GetLogicalModel(controller);
  PublicText::VisualModel&  visualModel  = GetVisualModel(controller);
  DALI_TEST_CHECK(logicalModel.mGradientSpanData);
  DALI_TEST_EQUALS(logicalModel.mGradientSpanData->paints.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mGradientSpanData->glyphPaintIndices.Count(),
                   visualModel.mGlyphs.Count(),
                   TEST_LOCATION);
  DALI_TEST_CHECK(std::find(logicalModel.mGradientSpanData->glyphPaintIndices.Begin(),
                            logicalModel.mGradientSpanData->glyphPaintIndices.End(),
                            1u) != logicalModel.mGradientSpanData->glyphPaintIndices.End());
  DALI_TEST_CHECK(std::find(logicalModel.mGradientSpanData->glyphPaintIndices.Begin(),
                            logicalModel.mGradientSpanData->glyphPaintIndices.End(),
                            2u) != logicalModel.mGradientSpanData->glyphPaintIndices.End());
  DALI_TEST_CHECK(std::find(logicalModel.mGradientSpanData->glyphPaintIndices.Begin(),
                            logicalModel.mGradientSpanData->glyphPaintIndices.End(),
                            3u) != logicalModel.mGradientSpanData->glyphPaintIndices.End());

  PublicText::TypesetterPtr typesetter = PublicText::Typesetter::New(&GetLogicalModelObject(controller));
  PixelData texture = typesetter->Render(Size(320.0f, 120.0f),
                                         PublicText::Direction::LEFT_TO_RIGHT,
                                         PublicText::Typesetter::RENDER_TEXT_AND_STYLES,
                                         false,
                                         Pixel::RGBA8888);
  DALI_TEST_CHECK(texture);
  DALI_TEST_CHECK(ContainsColoredTextPixel(texture));

  controller->SetDefaultColor(Color::TRANSPARENT);
  RelayoutController(controller);
  PixelData transparentTexture = typesetter->Render(Size(320.0f, 120.0f),
                                                     PublicText::Direction::LEFT_TO_RIGHT,
                                                     PublicText::Typesetter::RENDER_TEXT_AND_STYLES,
                                                     false,
                                                     Pixel::RGBA8888);
  DALI_TEST_CHECK(transparentTexture);
  DALI_TEST_CHECK(!ContainsVisibleTextPixel(transparentTexture));
  controller->SetDefaultColor(Color::BLACK);
  RelayoutController(controller);

  PublicText::RendererPtr atlasRenderer = PublicText::AtlasRenderer::New();
  Actor                   textControl   = Actor::New();
  float                   alignmentOffset = 0.0f;
  Actor atlasActor = atlasRenderer->Render(controller->GetView(),
                                           textControl,
                                           Property::INVALID_INDEX,
                                           alignmentOffset,
                                           0);
  DALI_TEST_CHECK(atlasActor);
  DALI_TEST_CHECK(atlasActor.GetChildCount() >= 3u);

  uint32_t gradientRendererCount = 0u;
  for(uint32_t childIndex = 0u; childIndex < atlasActor.GetChildCount(); ++childIndex)
  {
    Actor child = atlasActor.GetChildAt(childIndex);
    if(child.GetRendererCount() == 0u)
    {
      continue;
    }
    Renderer renderer = child.GetRendererAt(0u);
    if(renderer.GetPropertyIndex("uTextGradientType") != Property::INVALID_INDEX)
    {
      ++gradientRendererCount;
      DALI_TEST_CHECK(renderer.GetTextures().GetTexture(1u));
    }
  }
  DALI_TEST_CHECK(gradientRendererCount >= 3u);

  const Vector2 updatedCoordinateSize(320.0f, 120.0f);
  const Vector4 updatedViewBounds(0.125f, 0.25f, 0.5f, 0.375f);
  atlasRenderer->UpdateAtlasGradientSpanViewBounds(updatedCoordinateSize, updatedViewBounds);

  uint32_t updatedViewRendererCount = 0u;
  for(uint32_t childIndex = 0u; childIndex < atlasActor.GetChildCount(); ++childIndex)
  {
    Actor child = atlasActor.GetChildAt(childIndex);
    if(child.GetRendererCount() == 0u)
    {
      continue;
    }
    Renderer renderer = child.GetRendererAt(0u);
    const Property::Index boundsIndex = renderer.GetPropertyIndex("uTextGradientBounds");
    if(boundsIndex != Property::INVALID_INDEX && renderer.GetProperty<Vector4>(boundsIndex) == updatedViewBounds)
    {
      ++updatedViewRendererCount;
    }
  }
  DALI_TEST_CHECK(updatedViewRendererCount >= 1u);

  controller->SetText("Plain SetText source");
  DALI_TEST_CHECK(!GetLogicalModel(controller).mGradientSpanData);

  controller->SetStyledText(builder.Build());
  DALI_TEST_CHECK(GetLogicalModel(controller).mGradientSpanData);
  controller->SetStyledText(PublicText::StyledText::New("Plain StyledText source"));
  DALI_TEST_CHECK(!GetLogicalModel(controller).mGradientSpanData);

  END_TEST;
}

int UtcDaliStyledTextControllerGradientSpanUserSpaceRenderingP(void)
{
  UiTestApplication application;

  constexpr float      USER_START = 8.0f;
  constexpr float      USER_END   = 48.0f;
  const std::string    text       = "MMMMMMMM";
  const Vector2        renderSize(320.0f, 80.0f);
  Dali::Ui::Gradient::Linear objectGradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  objectGradient.SetUnits(Dali::Ui::Gradient::Units::OBJECT_BOUNDING_BOX);
  objectGradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(Color::RED)),
                               Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE))});
  Dali::Ui::Gradient::Linear userGradient(Vector2(USER_START, 0.0f), Vector2(USER_END, 0.0f));
  userGradient.SetUnits(Dali::Ui::Gradient::Units::USER_SPACE);
  userGradient.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(Color::RED)),
                             Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE))});

  auto makeController = [&](const Dali::Ui::Gradient::Base& gradient)
  {
    PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(text.c_str());
    DALI_ASSERT_ALWAYS(builder.SetSpan(PublicText::GradientSpan::New(gradient),
                                       0u,
                                       static_cast<uint32_t>(text.size())));
    PublicText::ControllerPtr controller = PublicText::Controller::New();
    controller->SetDefaultFontSize(32.0f, PublicText::Controller::PIXEL_SIZE);
    controller->SetHorizontalAlignment(PublicText::Alignment::START);
    controller->SetStyledText(builder.Build());
    controller->Relayout(renderSize);
    return controller;
  };

  PublicText::ControllerPtr objectController = makeController(objectGradient);
  PublicText::ControllerPtr userController   = makeController(userGradient);
  const auto*               userPaintData   = GetLogicalModel(userController).mGradientSpanData.get();
  DALI_TEST_CHECK(userPaintData && userPaintData->paints.Count() == 1u);
  DALI_TEST_EQUALS(userPaintData->paints[0u].style.units,
                   Dali::Ui::Gradient::Units::USER_SPACE,
                   TEST_LOCATION);
  DALI_TEST_EQUALS(userPaintData->paints[0u].style.linearStart, Vector2(USER_START, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(userPaintData->paints[0u].style.linearEnd, Vector2(USER_END, 0.0f), TEST_LOCATION);

  PublicText::TypesetterPtr objectTypesetter = PublicText::Typesetter::New(&GetLogicalModelObject(objectController));
  PublicText::TypesetterPtr userTypesetter   = PublicText::Typesetter::New(&GetLogicalModelObject(userController));
  const PixelData objectPixels = objectTypesetter->Render(renderSize,
                                                          PublicText::Direction::LEFT_TO_RIGHT,
                                                          PublicText::Typesetter::RENDER_TEXT_AND_STYLES,
                                                          false,
                                                          Pixel::RGBA8888);
  const PixelData userPixels = userTypesetter->Render(renderSize,
                                                      PublicText::Direction::LEFT_TO_RIGHT,
                                                      PublicText::Typesetter::RENDER_TEXT_AND_STYLES,
                                                      false,
                                                      Pixel::RGBA8888);
  DALI_TEST_CHECK(objectPixels && userPixels);

  const PublicText::VisualModel& userVisualModel = GetVisualModel(userController);
  DALI_TEST_EQUALS(userVisualModel.mLines.Count(), 1u, TEST_LOCATION);
  float spanLeft  = std::numeric_limits<float>::max();
  float spanRight = -std::numeric_limits<float>::max();
  for(uint32_t glyphIndex = 0u; glyphIndex < userVisualModel.mGlyphs.Count(); ++glyphIndex)
  {
    if(glyphIndex >= userPaintData->glyphPaintIndices.Count() || userPaintData->glyphPaintIndices[glyphIndex] == 0u)
    {
      continue;
    }
    const float left  = userVisualModel.mLines[0u].alignmentOffset + userVisualModel.mGlyphPositions[glyphIndex].x;
    const float right = left + userVisualModel.mGlyphs[glyphIndex].width;
    spanLeft          = std::min(spanLeft, std::max(0.0f, left));
    spanRight         = std::max(spanRight, std::min(renderSize.width, right));
  }
  DALI_TEST_CHECK(spanRight > spanLeft);

  const GradientUnitPixelEvidence evidence = FindGradientUnitPixelEvidence(objectPixels,
                                                                            userPixels,
                                                                            spanLeft,
                                                                            spanRight - spanLeft,
                                                                            USER_START,
                                                                            USER_END);
  DALI_TEST_CHECK(evidence.found);
  DALI_TEST_EQUALS(evidence.objectObserved, evidence.objectExpected, 0.06f, TEST_LOCATION);
  DALI_TEST_EQUALS(evidence.userObserved, evidence.userExpected, 0.06f, TEST_LOCATION);
  DALI_TEST_CHECK(std::fabs(evidence.objectObserved - evidence.userObserved) > 0.3f);

  PublicText::RendererPtr atlasRenderer   = PublicText::AtlasRenderer::New();
  float                   alignmentOffset = 0.0f;
  Actor                   atlasActor      = atlasRenderer->Render(userController->GetView(),
                                             Actor::New(),
                                             Property::INVALID_INDEX,
                                             alignmentOffset,
                                             0);
  DALI_TEST_CHECK(atlasActor);
  bool foundLinearAtlasGeometry = false;
  for(uint32_t childIndex = 0u; childIndex < atlasActor.GetChildCount(); ++childIndex)
  {
    Actor child = atlasActor.GetChildAt(childIndex);
    if(child.GetRendererCount() == 0u)
    {
      continue;
    }
    Renderer renderer = child.GetRendererAt(0u);
    const Property::Index typeIndex = renderer.GetPropertyIndex("uTextGradientType");
    if(typeIndex == Property::INVALID_INDEX ||
       std::fabs(renderer.GetProperty<float>(typeIndex) - static_cast<float>(Dali::Ui::Gradient::Type::LINEAR)) > 0.1f)
    {
      continue;
    }

    const Vector2 coordinateSize = renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientLayoutSize"));
    const Vector4 bounds         = renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("uTextGradientBounds"));
    const Vector2 boundsPixelSize(bounds.z * coordinateSize.x, bounds.w * coordinateSize.y);
    const Vector2 expectedStart(USER_START / boundsPixelSize.x, 0.0f);
    const Vector2 expectedEnd(USER_END / boundsPixelSize.x, 0.0f);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientStartPosition")),
                     expectedStart,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientEndPosition")),
                     expectedEnd,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    foundLinearAtlasGeometry = true;
  }
  DALI_TEST_CHECK(foundLinearAtlasGeometry);

  Dali::Ui::Gradient::Radial radial(Vector2(24.0f, 16.0f), 32.0f);
  radial.SetUnits(Dali::Ui::Gradient::Units::USER_SPACE);
  radial.SetSpreadMethod(Dali::Ui::Gradient::SpreadMethod::REPEAT);
  radial.SetStartOffset(0.125f);
  radial.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(Color::GREEN)),
                       Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(Color::BLUE))});
  Dali::Ui::Gradient::Conic conic(Vector2(30.0f, 20.0f), Radian(0.75f));
  conic.SetUnits(Dali::Ui::Gradient::Units::USER_SPACE);
  conic.SetSpreadMethod(Dali::Ui::Gradient::SpreadMethod::REFLECT);
  conic.SetStartOffset(-0.25f);
  conic.SetStopNodes({Dali::Ui::Gradient::StopNode(0.0f, Dali::Ui::UiColor(Color::MAGENTA)),
                      Dali::Ui::Gradient::StopNode(1.0f, Dali::Ui::UiColor(Color::CYAN))});

  PublicText::StyledTextBuilder typedBuilder = PublicText::StyledTextBuilder::New("RADIAL CONIC");
  DALI_TEST_CHECK(typedBuilder.SetSpan(
    PublicText::GradientSpan::New(radial, PublicText::GradientSpan::BoundsMode::CONTENT_BOUND), 0u, 6u));
  DALI_TEST_CHECK(typedBuilder.SetSpan(
    PublicText::GradientSpan::New(conic, PublicText::GradientSpan::BoundsMode::VIEW_BOUND), 7u, 12u));
  PublicText::ControllerPtr typedController = PublicText::Controller::New();
  typedController->SetDefaultFontSize(32.0f, PublicText::Controller::PIXEL_SIZE);
  typedController->SetStyledText(typedBuilder.Build());
  typedController->Relayout(renderSize);

  PublicText::TypesetterPtr typedTypesetter = PublicText::Typesetter::New(&GetLogicalModelObject(typedController));
  const PixelData typedPixels = typedTypesetter->Render(renderSize,
                                                        PublicText::Direction::LEFT_TO_RIGHT,
                                                        PublicText::Typesetter::RENDER_TEXT_AND_STYLES,
                                                        false,
                                                        Pixel::RGBA8888);
  DALI_TEST_CHECK(typedPixels && ContainsColoredTextPixel(typedPixels));

  PublicText::RendererPtr typedAtlasRenderer = PublicText::AtlasRenderer::New();
  Actor typedAtlasActor = typedAtlasRenderer->Render(typedController->GetView(),
                                                     Actor::New(),
                                                     Property::INVALID_INDEX,
                                                     alignmentOffset,
                                                     0);
  bool foundRadialGeometry = false;
  bool foundConicGeometry  = false;
  for(uint32_t childIndex = 0u; childIndex < typedAtlasActor.GetChildCount(); ++childIndex)
  {
    Actor child = typedAtlasActor.GetChildAt(childIndex);
    if(child.GetRendererCount() == 0u)
    {
      continue;
    }
    Renderer renderer = child.GetRendererAt(0u);
    const Property::Index typeIndex = renderer.GetPropertyIndex("uTextGradientType");
    if(typeIndex == Property::INVALID_INDEX)
    {
      continue;
    }
    const float   type           = renderer.GetProperty<float>(typeIndex);
    const Vector2 coordinateSize = renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientLayoutSize"));
    const Vector4 bounds         = renderer.GetProperty<Vector4>(renderer.GetPropertyIndex("uTextGradientBounds"));
    const Vector2 boundsPixelSize(bounds.z * coordinateSize.x, bounds.w * coordinateSize.y);
    if(std::fabs(type - static_cast<float>(Dali::Ui::Gradient::Type::RADIAL)) < 0.1f)
    {
      DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientRadialCenter")),
                       Vector2(24.0f / boundsPixelSize.x, 16.0f / boundsPixelSize.y),
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientRadialScale")),
                       boundsPixelSize / 32.0f,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetProperty<float>(renderer.GetPropertyIndex("uTextGradientStartOffset")),
                       0.125f,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      foundRadialGeometry = true;
    }
    else if(std::fabs(type - static_cast<float>(Dali::Ui::Gradient::Type::CONIC)) < 0.1f)
    {
      DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientConicCenter")),
                       Vector2(30.0f / boundsPixelSize.x, 20.0f / boundsPixelSize.y),
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(renderer.GetPropertyIndex("uTextGradientConicScale")),
                       boundsPixelSize,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetProperty<float>(renderer.GetPropertyIndex("uTextGradientConicStartAngle")),
                       0.75f,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      DALI_TEST_EQUALS(renderer.GetProperty<float>(renderer.GetPropertyIndex("uTextGradientStartOffset")),
                       -0.25f,
                       Math::MACHINE_EPSILON_1000,
                       TEST_LOCATION);
      foundConicGeometry = true;
    }
  }
  DALI_TEST_CHECK(foundRadialGeometry);
  DALI_TEST_CHECK(foundConicGeometry);

  END_TEST;
}

int UtcDaliStyledTextControllerGradientSpanAtlasScalingP(void)
{
  UiTestApplication application;

  constexpr std::array<uint32_t, 6u> PAINT_COUNTS{{1u, 2u, 5u, 10u, 20u, 50u}};
  const std::string                  text(50u, 'M');
  for(uint32_t paintCount : PAINT_COUNTS)
  {
    PublicText::StyledTextBuilder builder        = PublicText::StyledTextBuilder::New(text.c_str());
    const auto                    sharedGradient = CreateGradient();
    for(uint32_t index = 0u; index < paintCount; ++index)
    {
      DALI_TEST_CHECK(builder.SetSpan(PublicText::GradientSpan::New(sharedGradient), index, index + 1u));
    }

    PublicText::ControllerPtr controller = PublicText::Controller::New();
    controller->SetDefaultFontSize(18.0f, PublicText::Controller::PIXEL_SIZE);
    controller->SetStyledText(builder.Build());
    controller->Relayout(Size(1200.0f, 80.0f));
    DALI_TEST_CHECK(GetLogicalModel(controller).mGradientSpanData);
    DALI_TEST_EQUALS(GetLogicalModel(controller).mGradientSpanData->paints.Count(), paintCount, TEST_LOCATION);

    PublicText::RendererPtr atlasRenderer   = PublicText::AtlasRenderer::New();
    float                   alignmentOffset = 0.0f;
    Actor                   atlasActor      = atlasRenderer->Render(controller->GetView(),
                                                                    Actor::New(),
                                                                    Property::INVALID_INDEX,
                                                                    alignmentOffset,
                                                                    0);
    DALI_TEST_CHECK(atlasActor);

    uint32_t gradientRendererCount = 0u;
    for(uint32_t childIndex = 0u; childIndex < atlasActor.GetChildCount(); ++childIndex)
    {
      Actor child = atlasActor.GetChildAt(childIndex);
      if(child.GetRendererCount() > 0u &&
         child.GetRendererAt(0u).GetPropertyIndex("uTextGradientType") != Property::INVALID_INDEX)
      {
        ++gradientRendererCount;
        DALI_TEST_EQUALS(child.GetRendererAt(0u).GetTextures().GetTextureCount(), 2u, TEST_LOCATION);
      }
    }
    DALI_TEST_CHECK(gradientRendererCount >= paintCount);
  }

  for(uint32_t rangeCount : {5u, 10u, 50u})
  {
    PublicText::StyledTextBuilder builder        = PublicText::StyledTextBuilder::New(text.c_str());
    const auto                    sharedGradient = CreateGradient();
    for(uint32_t index = 0u; index < rangeCount; ++index)
    {
      DALI_TEST_CHECK(builder.SetSpan(
        PublicText::GradientSpan::New(sharedGradient, PublicText::GradientSpan::BoundsMode::CONTENT_BOUND),
        index,
        index + 1u));
    }

    PublicText::ControllerPtr controller = PublicText::Controller::New();
    controller->SetDefaultFontSize(18.0f, PublicText::Controller::PIXEL_SIZE);
    controller->SetStyledText(builder.Build());
    controller->Relayout(Size(1200.0f, 80.0f));
    DALI_TEST_CHECK(GetLogicalModel(controller).mGradientSpanData);
    DALI_TEST_EQUALS(GetLogicalModel(controller).mGradientSpanData->paints.Count(), 1u, TEST_LOCATION);
  }

  for(uint32_t paintCount : {5u, 10u, 50u})
  {
    PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(text.c_str());
    for(uint32_t index = 0u; index < paintCount; ++index)
    {
      auto gradient = CreateGradient();
      gradient.SetStartOffset(static_cast<float>(index) / static_cast<float>(paintCount));
      DALI_TEST_CHECK(builder.SetSpan(PublicText::GradientSpan::New(gradient), index, index + 1u));
    }

    PublicText::ControllerPtr controller = PublicText::Controller::New();
    controller->SetStyledText(builder.Build());
    DALI_TEST_CHECK(GetLogicalModel(controller).mGradientSpanData);
    DALI_TEST_EQUALS(GetLogicalModel(controller).mGradientSpanData->paints.Count(), paintCount, TEST_LOCATION);
  }

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
