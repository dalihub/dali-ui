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

#include <dali-ui-foundation/internal/text/async-text/async-text-loader.h>
#include <dali-ui-foundation/internal/text/logical-model-impl.h>
#include <dali-ui-foundation/internal/text/multi-language-helper-functions.h>
#include <dali-ui-foundation/internal/text/styled-text/styled-text-applier.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/styled-text-builder.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

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
  baseParameters.text         = plainText;
  baseParameters.fontSize     = 16.0f;
  baseParameters.textColor    = Color::BLACK;
  baseParameters.enableMarkup = false;

  Dali::Ui::Text::AsyncTextParameters styledParameters = baseParameters;
  styledParameters.hasStyledTextStyleSnapshot          = true;
  styledParameters.styledTextStyleSnapshot             = StyledTextInternal::StyledTextApplier::BuildTextStyleRunSnapshot(builder.Build(), 96.0f);

  Dali::Ui::Text::AsyncTextLoader loader = Dali::Ui::Text::AsyncTextLoader::New();
  const Size                       baseSize   = loader.ComputeNaturalSize(baseParameters);
  const Size                       styledSize = loader.ComputeNaturalSize(styledParameters);

  DALI_TEST_CHECK(!styledParameters.enableMarkup);
  DALI_TEST_CHECK(baseSize.height > 0.0f);
  DALI_TEST_CHECK(styledSize.height > baseSize.height);

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
