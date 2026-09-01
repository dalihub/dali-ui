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
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/multi-language-helper-functions.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;

namespace
{

namespace StyledTextInternal = Dali::Ui::Internal::Text;
namespace PublicText         = Dali::Ui::Text;

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
  DALI_TEST_EQUALS(underlineRun.properties.typeDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.colorDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.heightDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashWidthDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashGapDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.type, underline.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.color, underline.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.height, underline.GetThickness(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashWidth, underline.GetDashLength(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.dashGap, underline.GetDashGap(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
}

void CheckAnchorUnderlineRun(const PublicText::UnderlinedCharacterRun& underlineRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters, const Vector4& color)
{
  DALI_TEST_EQUALS(underlineRun.characterRun.characterIndex, characterIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.characterRun.numberOfCharacters, numberOfCharacters, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.colorDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(underlineRun.properties.color, color, TEST_LOCATION);
}

void CheckLineThroughRun(const PublicText::StrikethroughCharacterRun& lineThroughRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters, const PublicText::LineThrough& lineThrough)
{
  DALI_TEST_EQUALS(lineThroughRun.characterRun.characterIndex, characterIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.characterRun.numberOfCharacters, numberOfCharacters, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.colorDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.heightDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.color, lineThrough.GetColor().GetRgba(), TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughRun.properties.height, lineThrough.GetThickness(), Math::MACHINE_EPSILON_1000, TEST_LOCATION);
}

void CheckFontRunRange(const PublicText::FontDescriptionRun& fontRun, PublicText::CharacterIndex characterIndex, PublicText::Length numberOfCharacters)
{
  DALI_TEST_EQUALS(fontRun.characterRun.characterIndex, characterIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.characterRun.numberOfCharacters, numberOfCharacters, TEST_LOCATION);
}

TextAbstraction::FontDescription DefaultFontDescription()
{
  TextAbstraction::FontDescription description;
  description.family = "Default";
  description.weight = TextAbstraction::FontWeight::NORMAL;
  description.width  = TextAbstraction::FontWidth::NORMAL;
  description.slant  = TextAbstraction::FontSlant::NORMAL;
  return description;
}

Dali::Ui::Text::AsyncTextParameters CreateAsyncRenderParameters(const std::string& text)
{
  Dali::Ui::Text::AsyncTextParameters parameters;
  parameters.text           = text;
  parameters.fontSize       = 18.0f;
  parameters.textColor      = Color::BLACK;
  parameters.textWidth      = 240.0f;
  parameters.textHeight     = 80.0f;
  parameters.originWidth    = parameters.textWidth;
  parameters.originHeight   = parameters.textHeight;
  parameters.maxTextureSize = 4096;
  parameters.requestType    = Dali::Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
  return parameters;
}

} // namespace

void utc_dali_styled_text_applier_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_styled_text_applier_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliStyledTextApplierNoSpansP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("ABC");
  const auto                     result  = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.text.Count(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.text[0u], static_cast<PublicText::Character>('A'), TEST_LOCATION);
  DALI_TEST_EQUALS(result.text[1u], static_cast<PublicText::Character>('B'), TEST_LOCATION);
  DALI_TEST_EQUALS(result.text[2u], static_cast<PublicText::Character>('C'), TEST_LOCATION);
  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.underlinedCharacterRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.strikethroughCharacterRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierAnnotationSpanNoOpP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello TV");
  PublicText::AnnotationSpan    style   = PublicText::AnnotationSpan::New("style", "gradient");
  PublicText::AnnotationSpan    role    = PublicText::AnnotationSpan::New("role", "link");

  DALI_TEST_CHECK(builder.SetSpan(style, 6u, 8u));
  DALI_TEST_CHECK(builder.SetSpan(role, 6u, 8u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.text.Count(), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.underlinedCharacterRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.strikethroughCharacterRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierOneForegroundColorSpanP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::ForegroundColorSpan         span    = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(result.foregroundColorRuns[0u], 1u, 3u, Color::RED);
  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierOneBackgroundColorSpanP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::BackgroundColorSpan span    = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::CYAN));

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(result.backgroundColorRuns[0u], 1u, 3u, Color::CYAN);

  END_TEST;
}

int UtcDaliStyledTextApplierOneUnderlineSpanP(void)
{
  UiTestApplication application;

  const PublicText::Underline underline = CreateUnderline(Color::GREEN, 2.0f, PublicText::Underline::Type::DASHED, 4.0f, 2.0f);
  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::UnderlineSpan     span    = PublicText::UnderlineSpan::New(underline);

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.underlinedCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckUnderlineRun(result.underlinedCharacterRuns[0u], 1u, 3u, underline);
  DALI_TEST_EQUALS(result.strikethroughCharacterRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierOneLineThroughSpanP(void)
{
  UiTestApplication application;

  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::RED, 2.5f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("Hello");
  PublicText::LineThroughSpan   span        = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.underlinedCharacterRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.strikethroughCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckLineThroughRun(result.strikethroughCharacterRuns[0u], 0u, 5u, lineThrough);

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanFamilyOnlyP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes attributes;
  attributes.SetFamily("Ubuntu Mono");

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::FontSpan          span    = PublicText::FontSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  const PublicText::FontDescriptionRun& fontRun = result.fontDescriptionRuns[0u];
  CheckFontRunRange(fontRun, 1u, 3u);
  DALI_TEST_EQUALS(fontRun.familyDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(fontRun.familyName, fontRun.familyLength), std::string("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.sizeDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.weightDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.widthDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.slantDefined, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanResultOwnsFamilyRunsP(void)
{
  UiTestApplication application;

  {
    PublicText::FontAttributes attributes;
    attributes.SetFamily("Ubuntu Mono");

    PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
    DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(attributes), 0u, 5u));

    auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

    DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 1u, TEST_LOCATION);
    const PublicText::FontDescriptionRun& fontRun = result.fontDescriptionRuns[0u];
    DALI_TEST_EQUALS(fontRun.familyDefined, true, TEST_LOCATION);
    DALI_TEST_EQUALS(std::string(fontRun.familyName, fontRun.familyLength), std::string("Ubuntu Mono"), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanSizeOnlyP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes attributes;
  attributes.SetSize(32.0f);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::FontSpan          span    = PublicText::FontSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));

  auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  const PublicText::FontDescriptionRun& fontRun = result.fontDescriptionRuns[0u];
  CheckFontRunRange(fontRun, 0u, 5u);
  DALI_TEST_EQUALS(fontRun.sizeDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.size, static_cast<PublicText::PointSize26Dot6>(32.0f * 72.0f / 96.0f * 64.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.familyDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.weightDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.widthDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.slantDefined, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanMixedFamilyAndNonFamilyRunsP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes familyAttributes;
  familyAttributes.SetFamily("Ubuntu Mono");

  PublicText::FontAttributes weightAttributes;
  weightAttributes.SetWeight(PublicText::FontWeight::BOLD);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(familyAttributes), 0u, 3u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(weightAttributes), 3u, 6u));

  auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.fontDescriptionRuns[0u].familyDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(result.fontDescriptionRuns[0u].familyName, result.fontDescriptionRuns[0u].familyLength), std::string("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_EQUALS(result.fontDescriptionRuns[1u].familyDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(result.fontDescriptionRuns[1u].familyName == nullptr, true, TEST_LOCATION);
  DALI_TEST_EQUALS(result.fontDescriptionRuns[1u].weightDefined, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanWeightSlantWidthP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes attributes;
  attributes.SetWeight(PublicText::FontWeight::BOLD);
  attributes.SetSlant(PublicText::FontSlant::ITALIC);
  attributes.SetWidth(PublicText::FontWidth::CONDENSED);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::FontSpan          span    = PublicText::FontSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));

  auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  const PublicText::FontDescriptionRun& fontRun = result.fontDescriptionRuns[0u];
  CheckFontRunRange(fontRun, 0u, 5u);
  DALI_TEST_EQUALS(fontRun.weightDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.slantDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.slant, TextAbstraction::FontSlant::ITALIC, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.widthDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.width, TextAbstraction::FontWidth::CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.familyDefined, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.sizeDefined, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierEmptyFontSpanNoOpP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes attributes;
  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::FontSpan          span    = PublicText::FontSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.fontDescriptionRuns.Count(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanFieldMergeP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes familyAttributes;
  familyAttributes.SetFamily("Ubuntu Mono");

  PublicText::FontAttributes weightAttributes;
  weightAttributes.SetWeight(PublicText::FontWeight::BOLD);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(familyAttributes), 0u, 6u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(weightAttributes), 0u, 6u));

  auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  TextAbstraction::FontDescription resolvedDescription;
  TextAbstraction::PointSize26Dot6  resolvedPointSize = 0u;
  bool                              isDefaultFont     = true;
  PublicText::MergeFontDescriptions(result.fontDescriptionRuns, DefaultFontDescription(), 12u * 64u, 1.0f, 2u, resolvedDescription, resolvedPointSize, isDefaultFont);

  DALI_TEST_EQUALS(isDefaultFont, false, TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedDescription.family, std::string("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedDescription.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedPointSize, static_cast<TextAbstraction::PointSize26Dot6>(12u * 64u), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanSameFieldLaterWinsP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes boldAttributes;
  boldAttributes.SetWeight(PublicText::FontWeight::BOLD);

  PublicText::FontAttributes normalAttributes;
  normalAttributes.SetWeight(PublicText::FontWeight::NORMAL);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(boldAttributes), 0u, 6u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(normalAttributes), 2u, 4u));

  auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  TextAbstraction::FontDescription resolvedDescription;
  TextAbstraction::PointSize26Dot6  resolvedPointSize = 0u;
  bool                              isDefaultFont     = true;
  PublicText::MergeFontDescriptions(result.fontDescriptionRuns, DefaultFontDescription(), 12u * 64u, 1.0f, 3u, resolvedDescription, resolvedPointSize, isDefaultFont);
  DALI_TEST_EQUALS(resolvedDescription.weight, TextAbstraction::FontWeight::NORMAL, TEST_LOCATION);

  PublicText::MergeFontDescriptions(result.fontDescriptionRuns, DefaultFontDescription(), 12u * 64u, 1.0f, 1u, resolvedDescription, resolvedPointSize, isDefaultFont);
  DALI_TEST_EQUALS(resolvedDescription.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierFontSpanUnsetFieldDoesNotEraseEarlierP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes fontAttributes;
  fontAttributes.SetFamily("Ubuntu Mono");
  fontAttributes.SetWeight(PublicText::FontWeight::BOLD);

  PublicText::FontAttributes sizeAttributes;
  sizeAttributes.SetSize(32.0f);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(fontAttributes), 0u, 6u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(sizeAttributes), 1u, 5u));

  auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  TextAbstraction::FontDescription resolvedDescription;
  TextAbstraction::PointSize26Dot6  resolvedPointSize = 0u;
  bool                              isDefaultFont     = true;
  PublicText::MergeFontDescriptions(result.fontDescriptionRuns, DefaultFontDescription(), 12u * 64u, 1.0f, 2u, resolvedDescription, resolvedPointSize, isDefaultFont);

  DALI_TEST_EQUALS(resolvedDescription.family, std::string("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedDescription.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(resolvedPointSize, static_cast<TextAbstraction::PointSize26Dot6>(32.0f * 72.0f / 96.0f * 64.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierForegroundAndBackgroundIndependentP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder        = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::ForegroundColorSpan           foregroundSpan = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));
  PublicText::BackgroundColorSpan backgroundSpan = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW));

  DALI_TEST_CHECK(builder.SetSpan(foregroundSpan, 0u, 3u));
  DALI_TEST_CHECK(builder.SetSpan(backgroundSpan, 2u, 6u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(result.foregroundColorRuns[0u], 0u, 3u, Color::RED);
  CheckColorRun(result.backgroundColorRuns[0u], 2u, 4u, Color::YELLOW);

  END_TEST;
}

int UtcDaliStyledTextApplierForegroundBackgroundDecorationIndependentP(void)
{
  UiTestApplication application;

  const PublicText::Underline   underline   = CreateUnderline(Color::BLUE, 2.0f);
  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::MAGENTA, 3.0f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::ForegroundColorSpan         foregroundSpan = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));
  PublicText::BackgroundColorSpan backgroundSpan = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW));
  PublicText::UnderlineSpan       underlineSpan = PublicText::UnderlineSpan::New(underline);
  PublicText::LineThroughSpan     lineThroughSpan = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(foregroundSpan, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(backgroundSpan, 1u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(underlineSpan, 2u, 5u));
  DALI_TEST_CHECK(builder.SetSpan(lineThroughSpan, 3u, 6u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.underlinedCharacterRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.strikethroughCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(result.foregroundColorRuns[0u], 0u, 2u, Color::RED);
  CheckColorRun(result.backgroundColorRuns[0u], 1u, 3u, Color::YELLOW);
  CheckUnderlineRun(result.underlinedCharacterRuns[0u], 2u, 3u, underline);
  CheckLineThroughRun(result.strikethroughCharacterRuns[0u], 3u, 3u, lineThrough);

  END_TEST;
}

int UtcDaliStyledTextApplierComplexUnicodeTextAndColorRangeP(void)
{
  UiTestApplication application;

  // "A👩‍💻B"
  // U+0041 U+1F469 U+200D U+1F4BB U+0042
  // expected UTF-32 text count: 5
  const Dali::String zwjEmojiText("A"
                                  "\xF0\x9F\x91\xA9"
                                  "\xE2\x80\x8D"
                                  "\xF0\x9F\x92\xBB"
                                  "B");

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(zwjEmojiText);
  PublicText::ForegroundColorSpan         span    = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW));

  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.text.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(result.foregroundColorRuns[0u], 1u, 3u, Color::YELLOW);

  END_TEST;
}

int UtcDaliStyledTextApplierMixedUnicodeSingleCharacterRangeP(void)
{
  UiTestApplication application;

  const Dali::String texts[] =
  {
    "A가B",
    Dali::String("A"
                 "\xF0\x9F\x98\x80"
                 "B"),
  };

  for(const auto& text : texts)
  {
    PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(text);
    PublicText::ForegroundColorSpan         span    = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));

    DALI_TEST_CHECK(builder.SetSpan(span, 1u, 2u));

    const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

    DALI_TEST_EQUALS(result.text.Count(), 3u, TEST_LOCATION);
    DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 1u, TEST_LOCATION);
    CheckColorRun(result.foregroundColorRuns[0u], 1u, 1u, Color::RED);
  }

  END_TEST;
}

int UtcDaliStyledTextApplierApplyTextAndStyleRunsToLogicalModelP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder        = PublicText::StyledTextBuilder::New("Hello");
  PublicText::ForegroundColorSpan           foregroundSpan = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));
  PublicText::BackgroundColorSpan backgroundSpan = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::CYAN));
  const PublicText::Underline     underline      = CreateUnderline(Color::GREEN, 2.0f);
  const PublicText::LineThrough   lineThrough    = CreateLineThrough(Color::MAGENTA, 2.5f);
  PublicText::UnderlineSpan       underlineSpan  = PublicText::UnderlineSpan::New(underline);
  PublicText::LineThroughSpan     lineThroughSpan = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(foregroundSpan, 1u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(backgroundSpan, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(underlineSpan, 2u, 5u));
  DALI_TEST_CHECK(builder.SetSpan(lineThroughSpan, 0u, 5u));

  PublicText::LogicalModelPtr logicalModel = PublicText::LogicalModel::New();

  StyledTextInternal::StyledTextApplier::ApplyTextAndStyleRunsToLogicalModel(builder.Build(), *logicalModel, 96.0f);

  DALI_TEST_EQUALS(logicalModel->mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(logicalModel->mColorRuns[0u], 1u, 3u, Color::RED);
  DALI_TEST_EQUALS(logicalModel->mBackgroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(logicalModel->mBackgroundColorRuns[0u], 0u, 2u, Color::CYAN);
  DALI_TEST_EQUALS(logicalModel->mFontDescriptionRuns.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mUnderlinedCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckUnderlineRun(logicalModel->mUnderlinedCharacterRuns[0u], 2u, 3u, underline);
  DALI_TEST_EQUALS(logicalModel->mStrikethroughCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckLineThroughRun(logicalModel->mStrikethroughCharacterRuns[0u], 0u, 5u, lineThrough);

  END_TEST;
}

int UtcDaliStyledTextApplierApplyFontSpanToLogicalModelP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes attributes;
  attributes.SetFamily("Ubuntu Mono");
  attributes.SetWeight(PublicText::FontWeight::BOLD);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  PublicText::FontSpan          span    = PublicText::FontSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 1u, 4u));

  PublicText::LogicalModelPtr logicalModel = PublicText::LogicalModel::New();
  StyledTextInternal::StyledTextApplier::ApplyTextAndStyleRunsToLogicalModel(builder.Build(), *logicalModel, 96.0f);

  DALI_TEST_EQUALS(logicalModel->mText.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mFontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  const PublicText::FontDescriptionRun& fontRun = logicalModel->mFontDescriptionRuns[0u];
  CheckFontRunRange(fontRun, 1u, 3u);
  DALI_TEST_EQUALS(fontRun.familyDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(fontRun.familyName, fontRun.familyLength), std::string("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.weightDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);

  logicalModel->ClearFontDescriptionRuns();
  END_TEST;
}

int UtcDaliStyledTextApplierApplyAnchorSpanToLogicalModelP(void)
{
  UiTestApplication application;

  PublicText::AnchorAttributes fallbackAttributes;
  fallbackAttributes.SetHref("fallback");

  PublicText::AnchorAttributes explicitAttributes;
  explicitAttributes.SetHref("");
  explicitAttributes.SetColor(Dali::Ui::UiColor(Color::YELLOW));
  explicitAttributes.SetClickedColor(Dali::Ui::UiColor(Color::RED));

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("open docs");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::AnchorSpan::New(fallbackAttributes), 0u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::AnchorSpan::New(explicitAttributes), 5u, 9u));

  DALI_TEST_CHECK(StyledTextInternal::StyledTextApplier::HasAnchorSpans(builder.Build()));

  auto asyncSnapshot = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(),
                                                                                        96.0f,
                                                                                        Color::GREEN,
                                                                                        Color::MAGENTA);
  DALI_TEST_EQUALS(static_cast<uint32_t>(asyncSnapshot.anchorRuns.size()), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[0u].characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[0u].numberOfCharacters, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[0u].href, std::string("fallback"), TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[0u].color, Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[0u].clickedColor, Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[0u].hasColor, false, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[0u].hasClickedColor, false, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[1u].characterIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[1u].numberOfCharacters, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[1u].href, std::string(""), TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[1u].color, Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[1u].clickedColor, Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[1u].hasColor, true, TEST_LOCATION);
  DALI_TEST_EQUALS(asyncSnapshot.anchorRuns[1u].hasClickedColor, true, TEST_LOCATION);

  PublicText::LogicalModelPtr logicalModel = PublicText::LogicalModel::New();
  StyledTextInternal::StyledTextApplier::ApplyTextAndStyleRunsToLogicalModel(builder.Build(),
                                                                             *logicalModel,
                                                                             96.0f,
                                                                             Color::GREEN,
                                                                             Color::MAGENTA);

  DALI_TEST_EQUALS(logicalModel->mText.Count(), 9u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mColorRuns.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mUnderlinedCharacterRuns.Count(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mAnchors.Count(), 2u, TEST_LOCATION);

  CheckColorRun(logicalModel->mColorRuns[0u], 0u, 4u, Color::GREEN);
  CheckColorRun(logicalModel->mColorRuns[1u], 5u, 4u, Color::YELLOW);
  CheckAnchorUnderlineRun(logicalModel->mUnderlinedCharacterRuns[0u], 0u, 4u, Color::GREEN);
  CheckAnchorUnderlineRun(logicalModel->mUnderlinedCharacterRuns[1u], 5u, 4u, Color::YELLOW);

  const auto& fallbackAnchor = logicalModel->mAnchors[0u];
  DALI_TEST_EQUALS(fallbackAnchor.startIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAnchor.endIndex, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(fallbackAnchor.href), std::string("fallback"), TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAnchor.colorRunIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAnchor.underlinedCharacterRunIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAnchor.markupClickedColor, Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAnchor.isMarkupColorSet, false, TEST_LOCATION);
  DALI_TEST_EQUALS(fallbackAnchor.isMarkupClickedColorSet, false, TEST_LOCATION);

  const auto& explicitAnchor = logicalModel->mAnchors[1u];
  DALI_TEST_EQUALS(explicitAnchor.startIndex, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitAnchor.endIndex, 9u, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(explicitAnchor.href), std::string(""), TEST_LOCATION);
  DALI_TEST_EQUALS(explicitAnchor.colorRunIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitAnchor.underlinedCharacterRunIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitAnchor.markupClickedColor, Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitAnchor.isMarkupColorSet, true, TEST_LOCATION);
  DALI_TEST_EQUALS(explicitAnchor.isMarkupClickedColorSet, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierApplyFontSpanOverwriteReleasesStaleFamilyP(void)
{
  UiTestApplication application;

  PublicText::LogicalModelPtr logicalModel = PublicText::LogicalModel::New();

  PublicText::FontAttributes firstAttributes;
  firstAttributes.SetFamily("Ubuntu Mono");
  PublicText::StyledTextBuilder firstBuilder = PublicText::StyledTextBuilder::New("First");
  DALI_TEST_CHECK(firstBuilder.SetSpan(PublicText::FontSpan::New(firstAttributes), 0u, 5u));

  StyledTextInternal::StyledTextApplier::ApplyTextAndStyleRunsToLogicalModel(firstBuilder.Build(), *logicalModel, 96.0f);
  DALI_TEST_EQUALS(logicalModel->mFontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(logicalModel->mFontDescriptionRuns[0u].familyName, logicalModel->mFontDescriptionRuns[0u].familyLength), std::string("Ubuntu Mono"), TEST_LOCATION);

  PublicText::FontAttributes secondAttributes;
  secondAttributes.SetFamily("Ubuntu Mono Alt");
  PublicText::StyledTextBuilder secondBuilder = PublicText::StyledTextBuilder::New("Second");
  DALI_TEST_CHECK(secondBuilder.SetSpan(PublicText::FontSpan::New(secondAttributes), 0u, 6u));

  StyledTextInternal::StyledTextApplier::ApplyTextAndStyleRunsToLogicalModel(secondBuilder.Build(), *logicalModel, 96.0f);
  DALI_TEST_EQUALS(logicalModel->mFontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(logicalModel->mFontDescriptionRuns[0u].familyName, logicalModel->mFontDescriptionRuns[0u].familyLength), std::string("Ubuntu Mono Alt"), TEST_LOCATION);

  logicalModel->ClearFontDescriptionRuns();
  DALI_TEST_EQUALS(logicalModel->mFontDescriptionRuns.Count(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStyledTextApplierBuildTextStyleRunSnapshotP(void)
{
  UiTestApplication application;

  PublicText::FontAttributes attributes;
  attributes.SetFamily("Ubuntu Mono");
  attributes.SetSize(16.0f);
  attributes.SetWeight(PublicText::FontWeight::BOLD);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("Hello");
  DALI_TEST_CHECK(builder.SetSpan(PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED)), 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(attributes), 1u, 4u));

  auto snapshot = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(static_cast<uint32_t>(snapshot.foregroundColorRuns.size()), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.foregroundColorRuns[0u].characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.foregroundColorRuns[0u].numberOfCharacters, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.foregroundColorRuns[0u].color, Color::RED, TEST_LOCATION);

  DALI_TEST_EQUALS(static_cast<uint32_t>(snapshot.fontRuns.size()), 1u, TEST_LOCATION);
  const auto& fontRun = snapshot.fontRuns[0u];
  DALI_TEST_EQUALS(fontRun.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.numberOfCharacters, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.hasFamily, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.family, std::string("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.hasSize, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.size, static_cast<PublicText::PointSize26Dot6>(16.0f * 72.0f / 96.0f * 64.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.hasWeight, true, TEST_LOCATION);
  DALI_TEST_EQUALS(fontRun.weight, TextAbstraction::FontWeight::BOLD, TEST_LOCATION);

  auto copiedSnapshot             = snapshot;
  snapshot.fontRuns[0u].family    = "Mutated Family";
  DALI_TEST_EQUALS(copiedSnapshot.fontRuns[0u].family, std::string("Ubuntu Mono"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierApplySnapshotToLogicalModelP(void)
{
  UiTestApplication application;

  const std::string plainText = "<b>Hi</b>";

  PublicText::FontAttributes attributes;
  attributes.SetFamily("Ubuntu Mono");

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(plainText.c_str());
  DALI_TEST_CHECK(builder.SetSpan(PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::GREEN)), 0u, 3u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(attributes), 3u, 5u));

  auto snapshot = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);

  PublicText::LogicalModelPtr logicalModel = PublicText::LogicalModel::New();
  StyledTextInternal::StyledTextApplier::ApplySnapshotToLogicalModel(snapshot, plainText, *logicalModel);

  DALI_TEST_EQUALS(logicalModel->mText.Count(), static_cast<uint32_t>(plainText.size()), TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mText[0u], static_cast<PublicText::Character>('<'), TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mText[1u], static_cast<PublicText::Character>('b'), TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel->mText[2u], static_cast<PublicText::Character>('>'), TEST_LOCATION);

  DALI_TEST_EQUALS(logicalModel->mColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(logicalModel->mColorRuns[0u], 0u, 3u, Color::GREEN);

  DALI_TEST_EQUALS(logicalModel->mFontDescriptionRuns.Count(), 1u, TEST_LOCATION);
  const PublicText::FontDescriptionRun& fontRun = logicalModel->mFontDescriptionRuns[0u];
  CheckFontRunRange(fontRun, 3u, 2u);
  DALI_TEST_EQUALS(fontRun.familyDefined, true, TEST_LOCATION);
  DALI_TEST_EQUALS(std::string(fontRun.familyName, fontRun.familyLength), std::string("Ubuntu Mono"), TEST_LOCATION);
  DALI_TEST_CHECK(fontRun.familyName != snapshot.fontRuns[0u].family.c_str());

  logicalModel->ClearFontDescriptionRuns();
  END_TEST;
}

int UtcDaliStyledTextApplierAsyncSnapshotKeepsTextP(void)
{
  UiTestApplication application;

  const std::string plainText = "<b>Hi</b>";

  PublicText::FontAttributes attributes;
  attributes.SetSize(80.0f);

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(plainText.c_str());
  DALI_TEST_CHECK(builder.SetSpan(PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::GREEN)), 0u, 3u));
  DALI_TEST_CHECK(builder.SetSpan(PublicText::FontSpan::New(attributes), 0u, plainText.size()));

  Dali::Ui::Text::AsyncTextParameters baseParameters;
  baseParameters.text      = plainText;
  baseParameters.fontSize  = 16.0f;
  baseParameters.textColor = Color::BLACK;

  Dali::Ui::Text::AsyncTextParameters styledParameters = baseParameters;
  styledParameters.hasStyledTextStyleSnapshot          = true;
  styledParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);

  Dali::Ui::Text::AsyncTextLoader loader = Dali::Ui::Text::AsyncTextLoader::New();
  const Size                       baseSize   = loader.ComputeNaturalSize(baseParameters);
  const Size                       styledSize = loader.ComputeNaturalSize(styledParameters);

  DALI_TEST_CHECK(baseSize.height > 0.0f);
  DALI_TEST_CHECK(styledSize.height > baseSize.height);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncAnchorRenderInfoP(void)
{
  UiTestApplication application;

  const std::string plainText = "open docs";

  PublicText::AnchorAttributes attributes;
  attributes.SetHref("docs://open");

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(plainText.c_str());
  DALI_TEST_CHECK(builder.SetSpan(PublicText::AnchorSpan::New(attributes), 0u, 4u));

  Dali::Ui::Text::AsyncTextParameters parameters;
  parameters.text                       = plainText;
  parameters.fontSize                   = 18.0f;
  parameters.textColor                  = Color::BLACK;
  parameters.anchorColor                = Color::GREEN;
  parameters.anchorClickedColor         = Color::MAGENTA;
  parameters.textWidth                  = 240.0f;
  parameters.textHeight                 = 80.0f;
  parameters.originWidth                = parameters.textWidth;
  parameters.originHeight               = parameters.textHeight;
  parameters.maxTextureSize             = 4096;
  parameters.requestType                = Dali::Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
  parameters.hasStyledTextStyleSnapshot = true;
  parameters.styledTextStyleSnapshot    = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(),
                                                                                                         96.0f,
                                                                                                         parameters.anchorColor,
                                                                                                         parameters.anchorClickedColor);

  Dali::Ui::Text::AsyncTextLoader     loader     = Dali::Ui::Text::AsyncTextLoader::New();
  Dali::Ui::Text::AsyncTextRenderInfo renderInfo = loader.RenderText(parameters, false, Size::ZERO);

  DALI_TEST_EQUALS(static_cast<uint32_t>(renderInfo.anchorHitRegions.size()), 1u, TEST_LOCATION);
  const auto& region = renderInfo.anchorHitRegions[0u];
  DALI_TEST_EQUALS(region.characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(region.numberOfCharacters, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(region.href, std::string("docs://open"), TEST_LOCATION);
  DALI_TEST_EQUALS(region.color, Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(region.clickedColor, Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS(region.hasColor, false, TEST_LOCATION);
  DALI_TEST_EQUALS(region.hasClickedColor, false, TEST_LOCATION);
  DALI_TEST_EQUALS(region.isClicked, false, TEST_LOCATION);
  DALI_TEST_CHECK(!region.rectangles.empty());
  DALI_TEST_CHECK(region.rectangles[0u].width > 0.0f);
  DALI_TEST_CHECK(region.rectangles[0u].height > 0.0f);

  parameters.clickedAnchors.push_back(Dali::Ui::Text::AsyncAnchorClickedState{0u, 4u, "docs://open"});
  Dali::Ui::Text::AsyncTextRenderInfo clickedRenderInfo = loader.RenderText(parameters, false, Size::ZERO);

  DALI_TEST_EQUALS(static_cast<uint32_t>(clickedRenderInfo.anchorHitRegions.size()), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(clickedRenderInfo.anchorHitRegions[0u].isClicked, true, TEST_LOCATION);
  DALI_TEST_EQUALS(clickedRenderInfo.anchorHitRegions[0u].color, Color::MAGENTA, TEST_LOCATION);
  DALI_TEST_EQUALS(clickedRenderInfo.anchorHitRegions[0u].clickedColor, Color::MAGENTA, TEST_LOCATION);

  // An anchor whose semantic range is replaced by an ImageSpan must use the
  // same compacted CENTER + END ellipsis x coordinate as the renderer and
  // replacement placement. It must not retain the pre-elision line offset.
  const std::string imageAnchorText = "Long prefix words [image] trailing text";
  PublicText::StyledTextBuilder imageAnchorBuilder = PublicText::StyledTextBuilder::New(imageAnchorText.c_str());
  PublicText::ImageAttributes imageAttributes("icon.png", Vector2(70.0f, 28.0f));
  PublicText::ImageSpan imageSpan = PublicText::ImageSpan::New(imageAttributes);
  PublicText::AnchorAttributes imageAnchorAttributes;
  imageAnchorAttributes.SetHref("docs://image");
  PublicText::AnchorSpan imageAnchorSpan = PublicText::AnchorSpan::New(imageAnchorAttributes);
  DALI_TEST_CHECK(imageAnchorBuilder.SetSpan(imageSpan, 18u, 25u));
  DALI_TEST_CHECK(imageAnchorBuilder.SetSpan(imageAnchorSpan, 18u, 25u));
  const PublicText::StyledText imageAnchorStyledText = imageAnchorBuilder.Build();

  Dali::Ui::Text::AsyncTextParameters imageAnchorParameters;
  imageAnchorParameters.text                        = imageAnchorText;
  imageAnchorParameters.fontSize                    = 18.0f;
  imageAnchorParameters.textColor                   = Color::BLACK;
  imageAnchorParameters.anchorColor                 = Color::GREEN;
  imageAnchorParameters.anchorClickedColor          = Color::MAGENTA;
  imageAnchorParameters.textHeight                  = 80.0f;
  imageAnchorParameters.originHeight                = imageAnchorParameters.textHeight;
  imageAnchorParameters.maxTextureSize              = 4096;
  imageAnchorParameters.requestType                 = Dali::Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
  imageAnchorParameters.horizontalAlignment         = Dali::Ui::Text::Alignment::CENTER;
  imageAnchorParameters.lineWrapMode                = Dali::Ui::Text::LineWrapMode::CHARACTER;
  imageAnchorParameters.ellipsisPosition             = Dali::Ui::Text::EllipsisPosition::END;
  imageAnchorParameters.hasStyledTextStyleSnapshot   = true;
  imageAnchorParameters.styledTextStyleSnapshot      =
    StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(imageAnchorStyledText,
                                                                     96.0f,
                                                                     imageAnchorParameters.anchorColor,
                                                                     imageAnchorParameters.anchorClickedColor);
  imageAnchorParameters.replacementSourceSnapshot =
    StyledTextInternal::StyledTextApplier::BuildReplacementSourceSnapshot(imageAnchorStyledText, 73u);

  Dali::Ui::Text::AsyncTextLoader imageAnchorLoader = Dali::Ui::Text::AsyncTextLoader::New();
  bool foundVisibleElidedImageAnchor = false;
  bool foundHiddenElidedImageAnchor  = false;
  for(float width = 80.0f; width <= 480.0f && !foundVisibleElidedImageAnchor; width += 2.0f)
  {
    imageAnchorParameters.textWidth                   = width;
    imageAnchorParameters.originWidth                 = width;
    imageAnchorParameters.replacementLayoutGeneration = static_cast<uint64_t>(width);
    const Dali::Ui::Text::AsyncTextRenderInfo imageAnchorRenderInfo =
      imageAnchorLoader.RenderText(imageAnchorParameters, false, Size::ZERO);
    const Dali::Ui::Text::ReplacementRenderState* imageAnchorState =
      Dali::Ui::Text::GetImplementation(imageAnchorLoader).GetReplacementRenderState();
    if(!imageAnchorState)
    {
      continue;
    }
    if(imageAnchorState->finalElision.textElided && imageAnchorState->placements.Count() == 1u &&
       imageAnchorState->placements[0u].elided)
    {
      DALI_TEST_CHECK(!imageAnchorState->placements[0u].visible);
      DALI_TEST_CHECK(imageAnchorRenderInfo.anchorHitRegions.empty());
      foundHiddenElidedImageAnchor = true;
      continue;
    }
    if(!imageAnchorState->finalElision.textElided || imageAnchorState->placements.Count() != 1u ||
       !imageAnchorState->placements[0u].visible)
    {
      continue;
    }

    const Dali::Ui::Text::ReplacementPlacement& imagePlacement = imageAnchorState->placements[0u];
    Vector2 finalGlyphPosition;
    DALI_TEST_CHECK(imageAnchorState->finalElision.GetFinalGlyphPosition(
      imagePlacement.syntheticGlyphIndex,
      finalGlyphPosition));
    DALI_TEST_EQUALS(imagePlacement.position.x,
                     finalGlyphPosition.x,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(imageAnchorRenderInfo.replacementPlacements.Count(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(imageAnchorRenderInfo.replacementPlacements[0u].position.x,
                     finalGlyphPosition.x,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(static_cast<uint32_t>(imageAnchorRenderInfo.anchorHitRegions.size()), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(static_cast<uint32_t>(imageAnchorRenderInfo.anchorHitRegions[0u].rectangles.size()),
                     1u,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(imageAnchorRenderInfo.anchorHitRegions[0u].rectangles[0u].x,
                     finalGlyphPosition.x,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    DALI_TEST_EQUALS(imageAnchorRenderInfo.anchorHitRegions[0u].rectangles[0u].width,
                     imagePlacement.size.x,
                     Math::MACHINE_EPSILON_1000,
                     TEST_LOCATION);
    foundVisibleElidedImageAnchor = true;
  }
  DALI_TEST_CHECK(foundVisibleElidedImageAnchor);
  DALI_TEST_CHECK(foundHiddenElidedImageAnchor);

  PublicText::StyledText fromMarkupStyledText = PublicText::StyledText::FromMarkup("<a href='docs://raw'>open</a> docs");

  Dali::Ui::Text::AsyncTextParameters markupParameters;
  markupParameters.text               = fromMarkupStyledText.GetText().CStr();
  markupParameters.fontSize           = 18.0f;
  markupParameters.textColor          = Color::BLACK;
  markupParameters.anchorColor        = Color::CYAN;
  markupParameters.anchorClickedColor = Color::YELLOW;
  markupParameters.textWidth          = 240.0f;
  markupParameters.textHeight         = 80.0f;
  markupParameters.originWidth        = markupParameters.textWidth;
  markupParameters.originHeight       = markupParameters.textHeight;
  markupParameters.maxTextureSize     = 4096;
  markupParameters.requestType        = Dali::Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
  markupParameters.hasStyledTextStyleSnapshot = true;
  markupParameters.styledTextStyleSnapshot    = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(fromMarkupStyledText,
                                                                                                                 96.0f,
                                                                                                                 markupParameters.anchorColor,
                                                                                                                 markupParameters.anchorClickedColor);

  Dali::Ui::Text::AsyncTextRenderInfo markupRenderInfo = loader.RenderText(markupParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(static_cast<uint32_t>(markupRenderInfo.anchorHitRegions.size()), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(markupRenderInfo.anchorHitRegions[0u].characterIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(markupRenderInfo.anchorHitRegions[0u].numberOfCharacters, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(markupRenderInfo.anchorHitRegions[0u].href, std::string("docs://raw"), TEST_LOCATION);
  DALI_TEST_EQUALS(markupRenderInfo.anchorHitRegions[0u].color, Color::CYAN, TEST_LOCATION);
  DALI_TEST_EQUALS(markupRenderInfo.anchorHitRegions[0u].clickedColor, Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(markupRenderInfo.anchorHitRegions[0u].isClicked, false, TEST_LOCATION);
  DALI_TEST_CHECK(!markupRenderInfo.anchorHitRegions[0u].rectangles.empty());

  markupParameters.clickedAnchors.push_back(Dali::Ui::Text::AsyncAnchorClickedState{0u, 4u, "docs://raw"});
  Dali::Ui::Text::AsyncTextRenderInfo clickedMarkupRenderInfo = loader.RenderText(markupParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(static_cast<uint32_t>(clickedMarkupRenderInfo.anchorHitRegions.size()), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(clickedMarkupRenderInfo.anchorHitRegions[0u].isClicked, true, TEST_LOCATION);
  DALI_TEST_EQUALS(clickedMarkupRenderInfo.anchorHitRegions[0u].color, Color::YELLOW, TEST_LOCATION);
  DALI_TEST_EQUALS(clickedMarkupRenderInfo.anchorHitRegions[0u].clickedColor, Color::YELLOW, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncAnchorToPlainClearsStyleP(void)
{
  UiTestApplication application;

  const std::string styledText = "open docs";
  const std::string plainText  = "plain text";

  PublicText::AnchorAttributes attributes;
  attributes.SetHref("docs://open");

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(styledText.c_str());
  DALI_TEST_CHECK(builder.SetSpan(PublicText::AnchorSpan::New(attributes), 0u, 4u));

  Dali::Ui::Text::AsyncTextParameters styledParameters = CreateAsyncRenderParameters(styledText);
  styledParameters.anchorColor                         = Color::GREEN;
  styledParameters.anchorClickedColor                  = Color::MAGENTA;
  styledParameters.hasStyledTextStyleSnapshot          = true;
  styledParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(),
                                                                                                                        96.0f,
                                                                                                                        styledParameters.anchorColor,
                                                                                                                        styledParameters.anchorClickedColor);

  Dali::Ui::Text::AsyncTextLoader     loader           = Dali::Ui::Text::AsyncTextLoader::New();
  Dali::Ui::Text::AsyncTextRenderInfo styledRenderInfo = loader.RenderText(styledParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(static_cast<uint32_t>(styledRenderInfo.anchorHitRegions.size()), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledRenderInfo.isOverlayStyle, true, TEST_LOCATION);
  DALI_TEST_CHECK(styledRenderInfo.overlayStylePixelData);

  Dali::Ui::Text::AsyncTextParameters plainParameters  = CreateAsyncRenderParameters(plainText);
  Dali::Ui::Text::AsyncTextRenderInfo plainRenderInfo  = loader.RenderText(plainParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(static_cast<uint32_t>(plainRenderInfo.anchorHitRegions.size()), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.styleEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.isOverlayStyle, false, TEST_LOCATION);
  DALI_TEST_CHECK(!plainRenderInfo.stylePixelData);
  DALI_TEST_CHECK(!plainRenderInfo.overlayStylePixelData);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncUnderlineToPlainClearsStyleP(void)
{
  UiTestApplication application;

  const std::string styledText = "under text";
  const std::string plainText  = "plain text";

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(styledText.c_str());
  DALI_TEST_CHECK(builder.SetSpan(PublicText::UnderlineSpan::New(CreateUnderline(Color::BLUE, 2.0f)), 0u, 5u));

  Dali::Ui::Text::AsyncTextParameters styledParameters = CreateAsyncRenderParameters(styledText);
  styledParameters.hasStyledTextStyleSnapshot          = true;
  styledParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);

  Dali::Ui::Text::AsyncTextLoader     loader           = Dali::Ui::Text::AsyncTextLoader::New();
  Dali::Ui::Text::AsyncTextRenderInfo styledRenderInfo = loader.RenderText(styledParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(styledRenderInfo.isOverlayStyle, true, TEST_LOCATION);
  DALI_TEST_CHECK(styledRenderInfo.overlayStylePixelData);

  Dali::Ui::Text::AsyncTextParameters plainParameters = CreateAsyncRenderParameters(plainText);
  Dali::Ui::Text::AsyncTextRenderInfo plainRenderInfo = loader.RenderText(plainParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(plainRenderInfo.styleEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.isOverlayStyle, false, TEST_LOCATION);
  DALI_TEST_CHECK(!plainRenderInfo.stylePixelData);
  DALI_TEST_CHECK(!plainRenderInfo.overlayStylePixelData);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncLineThroughToPlainClearsStyleP(void)
{
  UiTestApplication application;

  const std::string styledText = "strike text";
  const std::string plainText  = "plain text";

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(styledText.c_str());
  DALI_TEST_CHECK(builder.SetSpan(PublicText::LineThroughSpan::New(CreateLineThrough(Color::RED, 2.0f)), 0u, 6u));

  Dali::Ui::Text::AsyncTextParameters styledParameters = CreateAsyncRenderParameters(styledText);
  styledParameters.hasStyledTextStyleSnapshot          = true;
  styledParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);

  Dali::Ui::Text::AsyncTextLoader     loader           = Dali::Ui::Text::AsyncTextLoader::New();
  Dali::Ui::Text::AsyncTextRenderInfo styledRenderInfo = loader.RenderText(styledParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(styledRenderInfo.isOverlayStyle, true, TEST_LOCATION);
  DALI_TEST_CHECK(styledRenderInfo.overlayStylePixelData);

  Dali::Ui::Text::AsyncTextParameters plainParameters = CreateAsyncRenderParameters(plainText);
  Dali::Ui::Text::AsyncTextRenderInfo plainRenderInfo = loader.RenderText(plainParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(plainRenderInfo.styleEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.isOverlayStyle, false, TEST_LOCATION);
  DALI_TEST_CHECK(!plainRenderInfo.stylePixelData);
  DALI_TEST_CHECK(!plainRenderInfo.overlayStylePixelData);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncBackgroundToPlainClearsStyleP(void)
{
  UiTestApplication application;

  const std::string styledText = "color text";
  const std::string plainText  = "plain text";

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New(styledText.c_str());
  DALI_TEST_CHECK(builder.SetSpan(PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW)), 0u, 5u));

  Dali::Ui::Text::AsyncTextParameters styledParameters = CreateAsyncRenderParameters(styledText);
  styledParameters.hasStyledTextStyleSnapshot          = true;
  styledParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);

  Dali::Ui::Text::AsyncTextLoader     loader           = Dali::Ui::Text::AsyncTextLoader::New();
  Dali::Ui::Text::AsyncTextRenderInfo styledRenderInfo = loader.RenderText(styledParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(styledRenderInfo.styleEnabled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(styledRenderInfo.styleTextureEnabled, true, TEST_LOCATION);
  DALI_TEST_CHECK(styledRenderInfo.stylePixelData);

  Dali::Ui::Text::AsyncTextParameters plainParameters = CreateAsyncRenderParameters(plainText);
  Dali::Ui::Text::AsyncTextRenderInfo plainRenderInfo = loader.RenderText(plainParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(plainRenderInfo.styleEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.isOverlayStyle, false, TEST_LOCATION);
  DALI_TEST_CHECK(!plainRenderInfo.stylePixelData);
  DALI_TEST_CHECK(!plainRenderInfo.overlayStylePixelData);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncFromMarkupUnderlineToPlainClearsStyleP(void)
{
  UiTestApplication application;

  PublicText::StyledText fromMarkupStyledText           = PublicText::StyledText::FromMarkup("<u>open</u>");
  Dali::Ui::Text::AsyncTextParameters markupParameters = CreateAsyncRenderParameters(fromMarkupStyledText.GetText().CStr());
  markupParameters.hasStyledTextStyleSnapshot          = true;
  markupParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(fromMarkupStyledText, 96.0f);

  Dali::Ui::Text::AsyncTextLoader     loader           = Dali::Ui::Text::AsyncTextLoader::New();
  Dali::Ui::Text::AsyncTextRenderInfo markupRenderInfo = loader.RenderText(markupParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(markupRenderInfo.isOverlayStyle, true, TEST_LOCATION);
  DALI_TEST_CHECK(markupRenderInfo.overlayStylePixelData);

  Dali::Ui::Text::AsyncTextParameters plainParameters = CreateAsyncRenderParameters("open");
  Dali::Ui::Text::AsyncTextRenderInfo plainRenderInfo = loader.RenderText(plainParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(plainRenderInfo.styleEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.isOverlayStyle, false, TEST_LOCATION);
  DALI_TEST_CHECK(!plainRenderInfo.stylePixelData);
  DALI_TEST_CHECK(!plainRenderInfo.overlayStylePixelData);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncFromMarkupLineThroughToPlainClearsStyleP(void)
{
  UiTestApplication application;

  PublicText::StyledText fromMarkupStyledText           = PublicText::StyledText::FromMarkup("<s>open</s>");
  Dali::Ui::Text::AsyncTextParameters markupParameters = CreateAsyncRenderParameters(fromMarkupStyledText.GetText().CStr());
  markupParameters.hasStyledTextStyleSnapshot          = true;
  markupParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(fromMarkupStyledText, 96.0f);

  Dali::Ui::Text::AsyncTextLoader     loader           = Dali::Ui::Text::AsyncTextLoader::New();
  Dali::Ui::Text::AsyncTextRenderInfo markupRenderInfo = loader.RenderText(markupParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(markupRenderInfo.isOverlayStyle, true, TEST_LOCATION);
  DALI_TEST_CHECK(markupRenderInfo.overlayStylePixelData);

  Dali::Ui::Text::AsyncTextParameters plainParameters = CreateAsyncRenderParameters("open");
  Dali::Ui::Text::AsyncTextRenderInfo plainRenderInfo = loader.RenderText(plainParameters, false, Size::ZERO);

  DALI_TEST_EQUALS(plainRenderInfo.styleEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.styleTextureEnabled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(plainRenderInfo.isOverlayStyle, false, TEST_LOCATION);
  DALI_TEST_CHECK(!plainRenderInfo.stylePixelData);
  DALI_TEST_CHECK(!plainRenderInfo.overlayStylePixelData);

  END_TEST;
}

int UtcDaliStyledTextApplierAsyncAnchorTransportCopySafeP(void)
{
  UiTestApplication application;

  Dali::Ui::Text::AsyncAnchorHitRegion region;
  region.characterIndex     = 1u;
  region.numberOfCharacters = 4u;
  region.href               = "docs";
  region.color              = Color::GREEN;
  region.clickedColor       = Color::MAGENTA;
  region.hasColor           = true;
  region.hasClickedColor    = true;
  region.rectangles.emplace_back(10.0f, 20.0f, 30.0f, 40.0f);

  Dali::Ui::Text::AsyncTextRenderInfo renderInfo;
  renderInfo.anchorHitRegions.push_back(region);

  Dali::Ui::Text::AsyncTextRenderInfo copiedRenderInfo = renderInfo;
  renderInfo.anchorHitRegions[0u].href                 = "mutated";
  renderInfo.anchorHitRegions[0u].rectangles[0u].x     = 99.0f;

  DALI_TEST_EQUALS(static_cast<uint32_t>(copiedRenderInfo.anchorHitRegions.size()), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedRenderInfo.anchorHitRegions[0u].href, std::string("docs"), TEST_LOCATION);
  DALI_TEST_EQUALS(copiedRenderInfo.anchorHitRegions[0u].rectangles[0u].x, 10.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedRenderInfo.anchorHitRegions[0u].rectangles[0u].y, 20.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedRenderInfo.anchorHitRegions[0u].rectangles[0u].width, 30.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedRenderInfo.anchorHitRegions[0u].rectangles[0u].height, 40.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  Dali::Ui::Text::AsyncTextParameters parameters;
  parameters.clickedAnchors.push_back(Dali::Ui::Text::AsyncAnchorClickedState{1u, 4u, "docs"});

  Dali::Ui::Text::AsyncTextParameters copiedParameters = parameters;
  parameters.clickedAnchors[0u].href                   = "mutated";

  DALI_TEST_EQUALS(static_cast<uint32_t>(copiedParameters.clickedAnchors.size()), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedParameters.clickedAnchors[0u].characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedParameters.clickedAnchors[0u].numberOfCharacters, 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(copiedParameters.clickedAnchors[0u].href, std::string("docs"), TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextApplierSameUiColorSeparateRunsP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::ForegroundColorSpan         spanA   = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::GREEN));
  PublicText::ForegroundColorSpan         spanB   = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::GREEN));

  DALI_TEST_CHECK(builder.SetSpan(spanA, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(spanB, 4u, 6u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 2u, TEST_LOCATION);
  CheckColorRun(result.foregroundColorRuns[0u], 0u, 2u, Color::GREEN);
  CheckColorRun(result.foregroundColorRuns[1u], 4u, 2u, Color::GREEN);

  END_TEST;
}

int UtcDaliStyledTextApplierSameObjectUpdatedRangeP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::ForegroundColorSpan         span    = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::BLUE));

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(span, 2u, 5u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(result.foregroundColorRuns[0u], 2u, 3u, Color::BLUE);

  END_TEST;
}

int UtcDaliStyledTextApplierSameUnderlineObjectUpdatedRangeP(void)
{
  UiTestApplication application;

  const PublicText::Underline underline = CreateUnderline(Color::CYAN, 2.0f);
  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::UnderlineSpan     span    = PublicText::UnderlineSpan::New(underline);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(span, 2u, 5u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.underlinedCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckUnderlineRun(result.underlinedCharacterRuns[0u], 2u, 3u, underline);

  END_TEST;
}

int UtcDaliStyledTextApplierSameLineThroughObjectUpdatedRangeP(void)
{
  UiTestApplication application;

  const PublicText::LineThrough lineThrough = CreateLineThrough(Color::YELLOW, 3.0f);
  PublicText::StyledTextBuilder builder     = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::LineThroughSpan   span        = PublicText::LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(span, 2u, 5u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.strikethroughCharacterRuns.Count(), 1u, TEST_LOCATION);
  CheckLineThroughRun(result.strikethroughCharacterRuns[0u], 2u, 3u, lineThrough);

  END_TEST;
}

int UtcDaliStyledTextApplierOverlapPreservesRunOrderP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::ForegroundColorSpan         red     = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::RED));
  PublicText::ForegroundColorSpan         blue    = PublicText::ForegroundColorSpan::New(Dali::Ui::UiColor(Color::BLUE));

  DALI_TEST_CHECK(builder.SetSpan(red, 0u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(blue, 2u, 6u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.foregroundColorRuns.Count(), 2u, TEST_LOCATION);
  CheckColorRun(result.foregroundColorRuns[0u], 0u, 4u, Color::RED);
  CheckColorRun(result.foregroundColorRuns[1u], 2u, 4u, Color::BLUE);

  END_TEST;
}

int UtcDaliStyledTextApplierOverlapUnderlinePreservesRunOrderP(void)
{
  UiTestApplication application;

  const PublicText::Underline firstUnderline  = CreateUnderline(Color::GREEN, 2.0f);
  const PublicText::Underline secondUnderline = CreateUnderline(Color::BLUE, 3.0f, PublicText::Underline::Type::DOUBLE);
  PublicText::StyledTextBuilder builder       = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::UnderlineSpan     first         = PublicText::UnderlineSpan::New(firstUnderline);
  PublicText::UnderlineSpan     second        = PublicText::UnderlineSpan::New(secondUnderline);

  DALI_TEST_CHECK(builder.SetSpan(first, 0u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(second, 2u, 6u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.underlinedCharacterRuns.Count(), 2u, TEST_LOCATION);
  CheckUnderlineRun(result.underlinedCharacterRuns[0u], 0u, 4u, firstUnderline);
  CheckUnderlineRun(result.underlinedCharacterRuns[1u], 2u, 4u, secondUnderline);

  END_TEST;
}

int UtcDaliStyledTextApplierOverlapLineThroughPreservesRunOrderP(void)
{
  UiTestApplication application;

  const PublicText::LineThrough firstLineThrough  = CreateLineThrough(Color::RED, 2.0f);
  const PublicText::LineThrough secondLineThrough = CreateLineThrough(Color::BLUE, 3.0f);
  PublicText::StyledTextBuilder builder           = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::LineThroughSpan   first             = PublicText::LineThroughSpan::New(firstLineThrough);
  PublicText::LineThroughSpan   second            = PublicText::LineThroughSpan::New(secondLineThrough);

  DALI_TEST_CHECK(builder.SetSpan(first, 0u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(second, 2u, 6u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.strikethroughCharacterRuns.Count(), 2u, TEST_LOCATION);
  CheckLineThroughRun(result.strikethroughCharacterRuns[0u], 0u, 4u, firstLineThrough);
  CheckLineThroughRun(result.strikethroughCharacterRuns[1u], 2u, 4u, secondLineThrough);

  END_TEST;
}

int UtcDaliStyledTextApplierOverlapBackgroundPreservesRunOrderP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::BackgroundColorSpan yellow  = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::YELLOW));
  PublicText::BackgroundColorSpan cyan    = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::CYAN));

  DALI_TEST_CHECK(builder.SetSpan(yellow, 0u, 4u));
  DALI_TEST_CHECK(builder.SetSpan(cyan, 2u, 6u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 2u, TEST_LOCATION);
  CheckColorRun(result.backgroundColorRuns[0u], 0u, 4u, Color::YELLOW);
  CheckColorRun(result.backgroundColorRuns[1u], 2u, 4u, Color::CYAN);

  END_TEST;
}

int UtcDaliStyledTextApplierSameBackgroundObjectUpdatedRangeP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder   builder = PublicText::StyledTextBuilder::New("abcdef");
  PublicText::BackgroundColorSpan span    = PublicText::BackgroundColorSpan::New(Dali::Ui::UiColor(Color::MAGENTA));

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(span, 2u, 5u));

  const auto result = StyledTextInternal::StyledTextApplier::BuildTextStyleRunResult(builder.Build(), 96.0f);

  DALI_TEST_EQUALS(result.backgroundColorRuns.Count(), 1u, TEST_LOCATION);
  CheckColorRun(result.backgroundColorRuns[0u], 2u, 3u, Color::MAGENTA);

  END_TEST;
}

int UtcDaliStyledTextApplierImageReplacementSnapshotP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("A[one]B[two]C");

  PublicText::ImageAttributes firstAttributes("icon.png", Vector2(24.0f, 18.0f));
  firstAttributes.SetAlignment(PublicText::ImageAttributes::InlineAlignment::TEXT_CENTER);
  firstAttributes.SetVerticalOffset(2.0f);
  PublicText::ImageSpan first = PublicText::ImageSpan::New(firstAttributes);

  PublicText::ImageAttributes secondAttributes("icon.png", Vector2(20.0f, 20.0f));
  PublicText::ImageSpan second = PublicText::ImageSpan::New(secondAttributes);

  DALI_TEST_CHECK(builder.SetSpan(first, 1u, 6u));
  DALI_TEST_CHECK(builder.SetSpan(second, 7u, 12u));

  const PublicText::ReplacementSourceSnapshot snapshot =
    StyledTextInternal::StyledTextApplier::BuildReplacementSourceSnapshot(builder.Build(), 42u);
  DALI_TEST_EQUALS(snapshot.sourceRevision, 42u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.runs.Count(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(snapshot.hasValidReplacementSource);

  const PublicText::ReplacementRunSnapshot& firstRun = snapshot.runs[0u];
  DALI_TEST_EQUALS(firstRun.type, PublicText::ReplacementType::IMAGE, TEST_LOCATION);
  DALI_TEST_EQUALS(firstRun.logicalCharacterRange.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(firstRun.logicalCharacterRange.numberOfCharacters, 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(firstRun.metrics.width, 24.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(firstRun.metrics.height, 18.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(firstRun.metrics.verticalOffset, 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(firstRun.metrics.verticalAlignment, PublicText::ReplacementVerticalAlignment::TEXT_CENTER, TEST_LOCATION);
  const PublicText::ReplacementRunSnapshot& secondRun = snapshot.runs[1u];
  DALI_TEST_CHECK(secondRun.occurrenceIdentity != firstRun.occurrenceIdentity);
  DALI_TEST_EQUALS(secondRun.image.source, firstRun.image.source, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStyledTextApplierInvalidImagePayloadFallbackP(void)
{
  UiTestApplication application;

  PublicText::StyledTextBuilder builder = PublicText::StyledTextBuilder::New("bad");
  PublicText::ImageAttributes  attributes("", Vector2(-1.0f, 20.0f));
  PublicText::ImageSpan        span = PublicText::ImageSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 3u));

  const PublicText::ReplacementSourceSnapshot snapshot =
    StyledTextInternal::StyledTextApplier::BuildReplacementSourceSnapshot(builder.Build(), 9u);
  DALI_TEST_EQUALS(snapshot.runs.Count(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!snapshot.hasValidReplacementSource);
  attributes.SetSource("icon.png");
  attributes.SetReservedSize(Vector2(12.0f, 12.0f));
  DALI_TEST_EQUALS(snapshot.runs[0u].image.source, "", TEST_LOCATION);

  END_TEST;
}
