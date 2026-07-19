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

#include <dali-ui-foundation/internal/text/styled-text/markup-to-styled-text.h>
#include <dali-ui-foundation/public-api/text/style/image-attributes.h>
#include <dali-ui-foundation/public-api/text/styled-text/anchor-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/annotation-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/background-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/font-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/foreground-color-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/image-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/line-through-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/replacement-span.h>
#include <dali-ui-foundation/public-api/text/styled-text/underline-span.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <cmath>
#include <string>

using namespace Dali;

namespace
{
namespace StyledTextInternal = Dali::Ui::Internal::Text;
namespace PublicText         = Dali::Ui::Text;

void CheckRange(const PublicText::StyledText& styledText, uint32_t index, uint32_t startIndex, uint32_t endIndex)
{
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(index), startIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(index), endIndex, TEST_LOCATION);
}

void CheckInfo(const StyledTextInternal::MarkupParseInfo& info, uint32_t unsupported, uint32_t malformed, uint32_t invalid)
{
  DALI_TEST_EQUALS(info.unsupportedTagCount, unsupported, TEST_LOCATION);
  DALI_TEST_EQUALS(info.malformedTagCount, malformed, TEST_LOCATION);
  DALI_TEST_EQUALS(info.invalidAttributeCount, invalid, TEST_LOCATION);
}

PublicText::StyledText Parse(const char* markup, StyledTextInternal::MarkupParseInfo& info)
{
  return StyledTextInternal::MarkupToStyledText(markup, &info);
}

void CheckColor(const Dali::Ui::UiColor& color, const Vector4& expected)
{
  const Vector4 rgba = color.GetRgba();
  DALI_TEST_EQUALS(rgba.r, expected.r, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(rgba.g, expected.g, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(rgba.b, expected.b, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(rgba.a, expected.a, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
}

void CheckOpaqueRed(const Dali::Ui::UiColor& color)
{
  CheckColor(color, Color::RED);
}

void CheckSemiTransparentRed(const Dali::Ui::UiColor& color)
{
  CheckColor(color, Vector4(1.0f, 0.0f, 0.0f, 128.0f / 255.0f));
}

} // unnamed namespace

void utc_dali_markup_to_styled_text_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_markup_to_styled_text_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliMarkupToStyledTextUnsupportedTagsP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = StyledTextInternal::MarkupToStyledText("<p>Hello</p><foo>World</foo>", &info);

  DALI_TEST_EQUALS(styledText.GetText(), "HelloWorld", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(info, 2u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextMissingHrefP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = StyledTextInternal::MarkupToStyledText("<a>x</a>", &info);

  DALI_TEST_EQUALS(styledText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(info, 0u, 0u, 1u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextEmptyHrefP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = StyledTextInternal::MarkupToStyledText("<a href=''>x</a>", &info);

  DALI_TEST_EQUALS(styledText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 1u);
  DALI_TEST_CHECK(PublicText::AnchorSpan::DownCast(styledText.GetSpanAt(0u)));
  DALI_TEST_EQUALS(PublicText::AnchorSpan::DownCast(styledText.GetSpanAt(0u)).GetAnchorAttributes().GetHref(), "", TEST_LOCATION);
  CheckInfo(info, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextUnclosedTagP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = StyledTextInternal::MarkupToStyledText("<u>hello", &info);

  DALI_TEST_EQUALS(styledText.GetText(), "hello", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 5u);
  DALI_TEST_CHECK(PublicText::UnderlineSpan::DownCast(styledText.GetSpanAt(0u)));
  CheckInfo(info, 0u, 1u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextInvalidAttributesP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = StyledTextInternal::MarkupToStyledText("<color value='not-a-color'>x</color><u color='nope' height='bad'>y</u>", &info);

  DALI_TEST_EQUALS(styledText.GetText(), "xy", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(PublicText::UnderlineSpan::DownCast(styledText.GetSpanAt(0u)));
  CheckRange(styledText, 0u, 1u, 2u);
  CheckInfo(info, 0u, 0u, 3u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextMalformedCloseP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = StyledTextInternal::MarkupToStyledText("<color value='red'>x</u>", &info);

  DALI_TEST_EQUALS(styledText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 1u);
  DALI_TEST_CHECK(PublicText::ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)));
  CheckInfo(info, 0u, 2u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextNullNumericEntityP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = StyledTextInternal::MarkupToStyledText("A &#0; B &#x0; C", &info);

  DALI_TEST_EQUALS(styledText.GetText(), "A &#0; B &#x0; C", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(info, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextEntityRecoveryP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = Parse("A & B &amp; C", info);
  DALI_TEST_EQUALS(styledText.GetText(), "A & B & C", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(info, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo unknownInfo;
  PublicText::StyledText unknownText = Parse("&unknown;&amp;", unknownInfo);
  DALI_TEST_EQUALS(unknownText.GetText(), "&unknown;&", TEST_LOCATION);
  DALI_TEST_EQUALS(unknownText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(unknownInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo invalidNumericInfo;
  PublicText::StyledText invalidNumericText = Parse("A &#xD800; B &#x110000; C", invalidNumericInfo);
  DALI_TEST_EQUALS(invalidNumericText.GetText(), "A &#xD800; B &#x110000; C", TEST_LOCATION);
  DALI_TEST_EQUALS(invalidNumericText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(invalidNumericInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo anchorInfo;
  PublicText::StyledText anchorText = Parse("<a href='a&b&amp;c'>x</a>", anchorInfo);
  DALI_TEST_EQUALS(anchorText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(anchorText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::AnchorSpan::DownCast(anchorText.GetSpanAt(0u)).GetAnchorAttributes().GetHref(), "a&b&c", TEST_LOCATION);
  CheckInfo(anchorInfo, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextRawSymbolsP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo rawInfo;
  PublicText::StyledText rawText = Parse("1 < 2 && 3 > 2", rawInfo);
  DALI_TEST_EQUALS(rawText.GetText(), "1 < 2 && 3 > 2", TEST_LOCATION);
  DALI_TEST_EQUALS(rawText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(rawInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo arrowInfo;
  PublicText::StyledText arrowText = Parse("value <- arrow -> end", arrowInfo);
  DALI_TEST_EQUALS(arrowText.GetText(), "value <- arrow -> end", TEST_LOCATION);
  DALI_TEST_EQUALS(arrowText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(arrowInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo unsupportedInfo;
  PublicText::StyledText unsupportedText = Parse("<foo>Hello</foo>", unsupportedInfo);
  DALI_TEST_EQUALS(unsupportedText.GetText(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(unsupportedText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(unsupportedInfo, 1u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextAttributeParsingP(void)
{
  UiTestApplication application;

  const char* markups[] =
  {
    "<color value='red'>A</color>",
    "<color value=\"red\">A</color>",
    "<color value=red>A</color>",
    "<color  value = 'red' >A</color>",
    "<COLOR VALUE='red'>A</COLOR>",
  };

  for(const char* markup : markups)
  {
    StyledTextInternal::MarkupParseInfo info;
    PublicText::StyledText styledText = Parse(markup, info);
    DALI_TEST_EQUALS(styledText.GetText(), "A", TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
    CheckOpaqueRed(PublicText::ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor());
    CheckInfo(info, 0u, 0u, 0u);
  }

  StyledTextInternal::MarkupParseInfo duplicateInfo;
  PublicText::StyledText duplicateText = Parse("<color value='red' value='blue'>A</color>", duplicateInfo);
  DALI_TEST_EQUALS(duplicateText.GetText(), "A", TEST_LOCATION);
  DALI_TEST_EQUALS(duplicateText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckColor(PublicText::ForegroundColorSpan::DownCast(duplicateText.GetSpanAt(0u)).GetColor(), Color::BLUE);
  CheckInfo(duplicateInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo unknownAttributeInfo;
  PublicText::StyledText unknownAttributeText = Parse("<color value='red' unknown='x'>A</color>", unknownAttributeInfo);
  DALI_TEST_EQUALS(unknownAttributeText.GetText(), "A", TEST_LOCATION);
  DALI_TEST_EQUALS(unknownAttributeText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckOpaqueRed(PublicText::ForegroundColorSpan::DownCast(unknownAttributeText.GetSpanAt(0u)).GetColor());
  CheckInfo(unknownAttributeInfo, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextColorCompatibilityP(void)
{
  UiTestApplication application;

  const char* opaqueRedMarkups[] =
  {
    "<color value='#F00'>A</color>",
    "<color value='#FF0000'>A</color>",
    "<color value='RED'>A</color>",
  };

  for(const char* markup : opaqueRedMarkups)
  {
    StyledTextInternal::MarkupParseInfo info;
    PublicText::StyledText styledText = Parse(markup, info);
    DALI_TEST_EQUALS(styledText.GetText(), "A", TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
    CheckOpaqueRed(PublicText::ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor());
    CheckInfo(info, 0u, 0u, 0u);
  }

  const char* alphaRedMarkups[] =
  {
    "<color value='#80FF0000'>A</color>",
    "<color value='0x80FF0000'>A</color>",
  };

  for(const char* markup : alphaRedMarkups)
  {
    StyledTextInternal::MarkupParseInfo info;
    PublicText::StyledText styledText = Parse(markup, info);
    DALI_TEST_EQUALS(styledText.GetText(), "A", TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
    CheckSemiTransparentRed(PublicText::ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor());
    CheckInfo(info, 0u, 0u, 0u);
  }

  const char* invalidColorMarkups[] =
  {
    "<color value='#GGG'>A</color>",
    "<color value='not-a-color'>A</color>",
  };

  for(const char* markup : invalidColorMarkups)
  {
    StyledTextInternal::MarkupParseInfo info;
    PublicText::StyledText styledText = Parse(markup, info);
    DALI_TEST_EQUALS(styledText.GetText(), "A", TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
    CheckInfo(info, 0u, 0u, 1u);
  }

  StyledTextInternal::MarkupParseInfo backgroundInfo;
  PublicText::StyledText backgroundText = Parse("<background color='not-a-color'>A</background>", backgroundInfo);
  DALI_TEST_EQUALS(backgroundText.GetText(), "A", TEST_LOCATION);
  DALI_TEST_EQUALS(backgroundText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(backgroundInfo, 0u, 0u, 1u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextFontPolicyP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo fontInfo;
  PublicText::StyledText fontText = Parse("<font family='Samsung One' size='30' weight='bold' width='condensed' slant='italic'>A</font>", fontInfo);
  DALI_TEST_EQUALS(fontText.GetText(), "A", TEST_LOCATION);
  DALI_TEST_EQUALS(fontText.GetSpanCount(), 1u, TEST_LOCATION);
  PublicText::FontAttributes attributes = PublicText::FontSpan::DownCast(fontText.GetSpanAt(0u)).GetFontAttributes();
  DALI_TEST_CHECK(attributes.Has(PublicText::FontAttributes::Attribute::FAMILY));
  DALI_TEST_CHECK(attributes.Has(PublicText::FontAttributes::Attribute::SIZE));
  DALI_TEST_CHECK(attributes.Has(PublicText::FontAttributes::Attribute::WEIGHT));
  DALI_TEST_CHECK(attributes.Has(PublicText::FontAttributes::Attribute::WIDTH));
  DALI_TEST_CHECK(attributes.Has(PublicText::FontAttributes::Attribute::SLANT));
  DALI_TEST_EQUALS(attributes.GetFamily(), "Samsung One", TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetSize(), 30.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetWeight(), PublicText::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetWidth(), PublicText::FontWidth::CONDENSED, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetSlant(), PublicText::FontSlant::ITALIC, TEST_LOCATION);
  CheckInfo(fontInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo enumInfo;
  PublicText::StyledText enumText = Parse("<font weight='BOLD' slant='ITALIC'>A</font>", enumInfo);
  PublicText::FontAttributes enumAttributes = PublicText::FontSpan::DownCast(enumText.GetSpanAt(0u)).GetFontAttributes();
  DALI_TEST_EQUALS(enumAttributes.GetWeight(), PublicText::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(enumAttributes.GetSlant(), PublicText::FontSlant::ITALIC, TEST_LOCATION);
  CheckInfo(enumInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo normalWinsInfo;
  PublicText::StyledText normalWinsText = Parse("<b><font weight='normal'>x</font></b>", normalWinsInfo);
  DALI_TEST_EQUALS(normalWinsText.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::FontSpan::DownCast(normalWinsText.GetSpanAt(0u)).GetFontAttributes().GetWeight(), PublicText::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::FontSpan::DownCast(normalWinsText.GetSpanAt(1u)).GetFontAttributes().GetWeight(), PublicText::FontWeight::NORMAL, TEST_LOCATION);
  CheckInfo(normalWinsInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo boldWinsInfo;
  PublicText::StyledText boldWinsText = Parse("<font weight='normal'><b>x</b></font>", boldWinsInfo);
  DALI_TEST_EQUALS(boldWinsText.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::FontSpan::DownCast(boldWinsText.GetSpanAt(0u)).GetFontAttributes().GetWeight(), PublicText::FontWeight::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::FontSpan::DownCast(boldWinsText.GetSpanAt(1u)).GetFontAttributes().GetWeight(), PublicText::FontWeight::BOLD, TEST_LOCATION);
  CheckInfo(boldWinsInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo invalidInfo;
  PublicText::StyledText invalidText = Parse("<font weight='bad'>A</font>", invalidInfo);
  DALI_TEST_EQUALS(invalidText.GetText(), "A", TEST_LOCATION);
  DALI_TEST_EQUALS(invalidText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(invalidInfo, 0u, 0u, 1u);

  StyledTextInternal::MarkupParseInfo infInfo;
  PublicText::StyledText infText = Parse("<font size='inf'>A</font>", infInfo);
  DALI_TEST_EQUALS(infText.GetText(), "A", TEST_LOCATION);
  DALI_TEST_EQUALS(infText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(infInfo, 0u, 0u, 1u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextDecorationPolicyP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo underlineInfo;
  PublicText::StyledText underlineText = Parse("<u color='red'><u height='3'>x</u></u>", underlineInfo);
  DALI_TEST_EQUALS(underlineText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(underlineText.GetSpanCount(), 2u, TEST_LOCATION);
  PublicText::Underline outerUnderline = PublicText::UnderlineSpan::DownCast(underlineText.GetSpanAt(0u)).GetUnderline();
  PublicText::Underline innerUnderline = PublicText::UnderlineSpan::DownCast(underlineText.GetSpanAt(1u)).GetUnderline();
  CheckOpaqueRed(outerUnderline.GetColor());
  CheckOpaqueRed(innerUnderline.GetColor());
  DALI_TEST_EQUALS(innerUnderline.GetThickness(), 3.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  CheckInfo(underlineInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo lineThroughInfo;
  PublicText::StyledText lineThroughText = Parse("<s color='blue'><s height='4'>x</s></s>", lineThroughInfo);
  DALI_TEST_EQUALS(lineThroughText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(lineThroughText.GetSpanCount(), 2u, TEST_LOCATION);
  PublicText::LineThrough innerLineThrough = PublicText::LineThroughSpan::DownCast(lineThroughText.GetSpanAt(1u)).GetLineThrough();
  CheckColor(innerLineThrough.GetColor(), Color::BLUE);
  DALI_TEST_EQUALS(innerLineThrough.GetThickness(), 4.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  CheckInfo(lineThroughInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo invalidInfo;
  PublicText::StyledText invalidText = Parse("<u color='bad' height='2'>x</u>", invalidInfo);
  DALI_TEST_EQUALS(invalidText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(invalidText.GetSpanCount(), 1u, TEST_LOCATION);
  PublicText::Underline invalidUnderline = PublicText::UnderlineSpan::DownCast(invalidText.GetSpanAt(0u)).GetUnderline();
  DALI_TEST_EQUALS(invalidUnderline.GetThickness(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  CheckInfo(invalidInfo, 0u, 0u, 1u);

  StyledTextInternal::MarkupParseInfo nanInfo;
  PublicText::StyledText nanText = Parse("<u height='nan'>x</u>", nanInfo);
  DALI_TEST_EQUALS(nanText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(nanText.GetSpanCount(), 1u, TEST_LOCATION);
  PublicText::Underline nanUnderline = PublicText::UnderlineSpan::DownCast(nanText.GetSpanAt(0u)).GetUnderline();
  DALI_TEST_CHECK(std::isfinite(nanUnderline.GetThickness()));
  CheckInfo(nanInfo, 0u, 0u, 1u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextAnchorPolicyP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo hrefInfo;
  PublicText::StyledText hrefText = Parse("<a href='a&b&amp;c'>x</a>", hrefInfo);
  DALI_TEST_EQUALS(hrefText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(hrefText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::AnchorSpan::DownCast(hrefText.GetSpanAt(0u)).GetAnchorAttributes().GetHref(), "a&b&c", TEST_LOCATION);
  CheckInfo(hrefInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo colorInfo;
  PublicText::StyledText colorText = Parse("<a href='x' color='bad' clicked-color='blue'>x</a>", colorInfo);
  DALI_TEST_EQUALS(colorText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(colorText.GetSpanCount(), 1u, TEST_LOCATION);
  PublicText::AnchorAttributes colorAttributes = PublicText::AnchorSpan::DownCast(colorText.GetSpanAt(0u)).GetAnchorAttributes();
  DALI_TEST_CHECK(colorAttributes.Has(PublicText::AnchorAttributes::Attribute::HREF));
  DALI_TEST_CHECK(!colorAttributes.Has(PublicText::AnchorAttributes::Attribute::COLOR));
  DALI_TEST_CHECK(colorAttributes.Has(PublicText::AnchorAttributes::Attribute::CLICKED_COLOR));
  DALI_TEST_EQUALS(colorAttributes.GetHref(), "x", TEST_LOCATION);
  CheckColor(colorAttributes.GetClickedColor(), Color::BLUE);
  CheckInfo(colorInfo, 0u, 0u, 1u);

  StyledTextInternal::MarkupParseInfo missingHrefInfo;
  PublicText::StyledText missingHrefText = Parse("<a color='red'>x</a>", missingHrefInfo);
  DALI_TEST_EQUALS(missingHrefText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(missingHrefText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(missingHrefInfo, 0u, 0u, 1u);

  StyledTextInternal::MarkupParseInfo emptyHrefInfo;
  PublicText::StyledText emptyHrefText = Parse("<a href=''>x</a>", emptyHrefInfo);
  DALI_TEST_EQUALS(emptyHrefText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(emptyHrefText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::AnchorSpan::DownCast(emptyHrefText.GetSpanAt(0u)).GetAnchorAttributes().GetHref(), "", TEST_LOCATION);
  CheckInfo(emptyHrefInfo, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextAnnotationPolicyP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = Parse("<annotation style='gradient' role='link'>hello world</annotation>", info);

  DALI_TEST_EQUALS(styledText.GetText(), "hello world", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationCount(), 2u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 11u);
  CheckRange(styledText, 1u, 0u, 11u);

  PublicText::AnnotationSpan style = PublicText::AnnotationSpan::DownCast(styledText.GetSpanAt(0u));
  PublicText::AnnotationSpan role  = PublicText::AnnotationSpan::DownCast(styledText.GetSpanAt(1u));
  DALI_TEST_CHECK(style);
  DALI_TEST_CHECK(role);
  DALI_TEST_EQUALS(style.GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetValue(), "gradient", TEST_LOCATION);
  DALI_TEST_EQUALS(role.GetKey(), "role", TEST_LOCATION);
  DALI_TEST_EQUALS(role.GetValue(), "link", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationAt(0u).GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationAt(1u).GetKey(), "role", TEST_LOCATION);
  CheckInfo(info, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo valueInfo;
  PublicText::StyledText valueText = Parse("<annotation value='semantic'>x</annotation>", valueInfo);
  DALI_TEST_EQUALS(valueText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(valueText.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(valueText.GetAnnotationAt(0u).GetKey(), "value", TEST_LOCATION);
  DALI_TEST_EQUALS(valueText.GetAnnotationAt(0u).GetValue(), "semantic", TEST_LOCATION);
  CheckInfo(valueInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo noAttributeInfo;
  PublicText::StyledText noAttributeText = Parse("<annotation>plain</annotation>", noAttributeInfo);
  DALI_TEST_EQUALS(noAttributeText.GetText(), "plain", TEST_LOCATION);
  DALI_TEST_EQUALS(noAttributeText.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(noAttributeText.GetAnnotationCount(), 0u, TEST_LOCATION);
  CheckInfo(noAttributeInfo, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextMalformedRecoveryP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo unclosedInfo;
  PublicText::StyledText unclosedText = Parse("<color value='red'>x", unclosedInfo);
  DALI_TEST_EQUALS(unclosedText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(unclosedText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(unclosedText, 0u, 0u, 1u);
  CheckOpaqueRed(PublicText::ForegroundColorSpan::DownCast(unclosedText.GetSpanAt(0u)).GetColor());
  CheckInfo(unclosedInfo, 0u, 1u, 0u);

  StyledTextInternal::MarkupParseInfo mismatchedInfo;
  PublicText::StyledText mismatchedText = Parse("<color value='red'>x</u>", mismatchedInfo);
  DALI_TEST_EQUALS(mismatchedText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(mismatchedText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckOpaqueRed(PublicText::ForegroundColorSpan::DownCast(mismatchedText.GetSpanAt(0u)).GetColor());
  CheckInfo(mismatchedInfo, 0u, 2u, 0u);

  StyledTextInternal::MarkupParseInfo truncatedInfo;
  PublicText::StyledText truncatedText = Parse("<color value='red'", truncatedInfo);
  DALI_TEST_EQUALS(truncatedText.GetText(), "<color value='red'", TEST_LOCATION);
  DALI_TEST_EQUALS(truncatedText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(truncatedInfo, 0u, 1u, 0u);

  StyledTextInternal::MarkupParseInfo closeOnlyInfo;
  PublicText::StyledText closeOnlyText = Parse("</u>x", closeOnlyInfo);
  DALI_TEST_EQUALS(closeOnlyText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(closeOnlyText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(closeOnlyInfo, 0u, 1u, 0u);

  StyledTextInternal::MarkupParseInfo nestedMismatchInfo;
  PublicText::StyledText nestedMismatchText = Parse("<b><i>x</b></i>", nestedMismatchInfo);
  DALI_TEST_EQUALS(nestedMismatchText.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(nestedMismatchText.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::FontSpan::DownCast(nestedMismatchText.GetSpanAt(0u)).GetFontAttributes().GetWeight(), PublicText::FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::FontSpan::DownCast(nestedMismatchText.GetSpanAt(1u)).GetFontAttributes().GetSlant(), PublicText::FontSlant::ITALIC, TEST_LOCATION);
  CheckInfo(nestedMismatchInfo, 0u, 1u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextCodePointRangeWithEntityP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = Parse("<u>&lt;가😀</u>", info);

  DALI_TEST_EQUALS(styledText.GetText(), "<가😀", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 3u);
  DALI_TEST_CHECK(PublicText::UnderlineSpan::DownCast(styledText.GetSpanAt(0u)));
  CheckInfo(info, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextImageFormsP(void)
{
  UiTestApplication application;

  const char* markups[] =
  {
    "<img src='icon.png' width='24' height='18'/>",
    "<img src='icon.png' width='24' height='18'>",
    "<img src='icon.png' width='24' height='18'></img>",
    "<img src='icon.png' width='24' height='18'>Hello</img>",
  };
  const char* suffixes[] = {"", "", "", "Hello"};
  const uint32_t expectedUtf32Lengths[] = {1u, 1u, 1u, 6u};

  for(uint32_t index = 0u; index < 4u; ++index)
  {
    StyledTextInternal::MarkupParseInfo info;
    PublicText::StyledText styledText = Parse(markups[index], info);

    const std::string expected = std::string(PublicText::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER) + suffixes[index];
    DALI_TEST_EQUALS(styledText.GetText(), expected.c_str(), TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetUtf32Length(), expectedUtf32Lengths[index], TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
    CheckRange(styledText, 0u, 0u, 1u);

    PublicText::ImageSpan imageSpan = PublicText::ImageSpan::DownCast(styledText.GetSpanAt(0u));
    DALI_TEST_CHECK(imageSpan);
    const PublicText::ImageAttributes attributes = imageSpan.GetImageAttributes();
    DALI_TEST_EQUALS(attributes.GetSource(), "icon.png", TEST_LOCATION);
    DALI_TEST_EQUALS(attributes.GetReservedSize(), Vector2(24.0f, 18.0f), TEST_LOCATION);
    CheckInfo(info, 0u, 0u, 0u);
  }

  StyledTextInternal::MarkupParseInfo numericInfo;
  PublicText::StyledText numericText = Parse(
    "<img src='decimal.png' width='24.5' height='0.5'/>"
    "<img src='exponent.png' width='1e2' height='24'/>",
    numericInfo);
  const std::string numericExpected = std::string(PublicText::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER) +
                                      PublicText::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER;
  DALI_TEST_EQUALS(numericText.GetText(), numericExpected.c_str(), TEST_LOCATION);
  DALI_TEST_EQUALS(numericText.GetSpanCount(), 2u, TEST_LOCATION);
  CheckRange(numericText, 0u, 0u, 1u);
  CheckRange(numericText, 1u, 1u, 2u);
  DALI_TEST_EQUALS(PublicText::ImageSpan::DownCast(numericText.GetSpanAt(0u)).GetImageAttributes().GetReservedSize(), Vector2(24.5f, 0.5f), TEST_LOCATION);
  DALI_TEST_EQUALS(PublicText::ImageSpan::DownCast(numericText.GetSpanAt(1u)).GetImageAttributes().GetReservedSize(), Vector2(100.0f, 24.0f), TEST_LOCATION);
  CheckInfo(numericInfo, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextImageUtf32RangeP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = Parse("한😀<img src='icon.png' width='24' height='18'/>끝", info);

  const std::string expected = std::string("한😀") + PublicText::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER + "끝";
  DALI_TEST_EQUALS(styledText.GetText(), expected.c_str(), TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetUtf32Length(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 2u, 3u);

  PublicText::ImageSpan imageSpan = PublicText::ImageSpan::DownCast(styledText.GetSpanAt(0u));
  DALI_TEST_CHECK(imageSpan);
  DALI_TEST_EQUALS(imageSpan.GetImageAttributes().GetSource(), "icon.png", TEST_LOCATION);
  DALI_TEST_EQUALS(imageSpan.GetImageAttributes().GetReservedSize(), Vector2(24.0f, 18.0f), TEST_LOCATION);
  CheckInfo(info, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextImageCompositionP(void)
{
  UiTestApplication application;

  StyledTextInternal::MarkupParseInfo info;
  PublicText::StyledText styledText = Parse(
    "<color value='red'>A<img height='18' src='icon&amp;one.png' width='24'/>B</color>"
    "<img src='two.png' width='32' height='20'/>",
    info);

  const std::string expected = std::string("A") + PublicText::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER +
                               "B" + PublicText::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER;
  DALI_TEST_EQUALS(styledText.GetText(), expected.c_str(), TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetUtf32Length(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(PublicText::ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)));
  CheckRange(styledText, 0u, 0u, 3u);

  PublicText::ImageSpan firstImage = PublicText::ImageSpan::DownCast(styledText.GetSpanAt(1u));
  PublicText::ImageSpan secondImage = PublicText::ImageSpan::DownCast(styledText.GetSpanAt(2u));
  DALI_TEST_CHECK(firstImage);
  DALI_TEST_CHECK(secondImage);
  CheckRange(styledText, 1u, 1u, 2u);
  CheckRange(styledText, 2u, 3u, 4u);
  DALI_TEST_EQUALS(firstImage.GetImageAttributes().GetSource(), "icon&one.png", TEST_LOCATION);
  DALI_TEST_EQUALS(secondImage.GetImageAttributes().GetReservedSize(), Vector2(32.0f, 20.0f), TEST_LOCATION);
  CheckInfo(info, 0u, 0u, 0u);

  END_TEST;
}

int UtcDaliMarkupToStyledTextInvalidImageP(void)
{
  UiTestApplication application;

  const char* markups[] =
  {
    "<img width='24' height='18'>Hello</img>",
    "<img src='' width='24' height='18'>Hello</img>",
    "<img src='icon.png' height='18'>Hello</img>",
    "<img src='icon.png' width='24'>Hello</img>",
    "<img src='icon.png' width='' height='18'>Hello</img>",
    "<img src='icon.png' width='24' height=''>Hello</img>",
    "<img src='icon.png' width='0' height='18'>Hello</img>",
    "<img src='icon.png' width='24' height='-1'>Hello</img>",
    "<img src='icon.png' width='nan' height='18'>Hello</img>",
    "<img src='icon.png' width='24' height='nan'>Hello</img>",
    "<img src='icon.png' width='inf' height='18'>Hello</img>",
    "<img src='icon.png' width='24' height='inf'>Hello</img>",
    "<img src='icon.png' width='infinity' height='18'>Hello</img>",
    "<img src='icon.png' width='1e999' height='18'>Hello</img>",
    "<img src='icon.png' width='24px' height='18'>Hello</img>",
  };

  for(const char* markup : markups)
  {
    StyledTextInternal::MarkupParseInfo info;
    PublicText::StyledText styledText = Parse(markup, info);
    DALI_TEST_EQUALS(styledText.GetText(), "Hello", TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
    DALI_TEST_CHECK(info.invalidAttributeCount > 0u);
    DALI_TEST_EQUALS(info.unsupportedTagCount, 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(info.malformedTagCount, 0u, TEST_LOCATION);
  }

  StyledTextInternal::MarkupParseInfo invalidSelfClosingInfo;
  PublicText::StyledText invalidSelfClosingText = Parse("before<img src='icon.png' width='nan' height='18'/>after", invalidSelfClosingInfo);
  DALI_TEST_EQUALS(invalidSelfClosingText.GetText(), "beforeafter", TEST_LOCATION);
  DALI_TEST_EQUALS(invalidSelfClosingText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(invalidSelfClosingInfo, 0u, 0u, 1u);

  StyledTextInternal::MarkupParseInfo closeInfo;
  PublicText::StyledText closeText = Parse("before</img>after", closeInfo);
  DALI_TEST_EQUALS(closeText.GetText(), "beforeafter", TEST_LOCATION);
  DALI_TEST_EQUALS(closeText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(closeInfo, 0u, 0u, 0u);

  StyledTextInternal::MarkupParseInfo unsupportedInfo;
  PublicText::StyledText unsupportedText = Parse("<image src='icon.png' width='24' height='18'/>text", unsupportedInfo);
  DALI_TEST_EQUALS(unsupportedText.GetText(), "text", TEST_LOCATION);
  DALI_TEST_EQUALS(unsupportedText.GetSpanCount(), 0u, TEST_LOCATION);
  CheckInfo(unsupportedInfo, 1u, 0u, 0u);

  END_TEST;
}
