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

#include <stdlib.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Text;

void utc_dali_styled_text_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_styled_text_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

void CheckSpanIdentity(const Span& actual, const Span& expected)
{
  DALI_TEST_EQUALS(actual.GetObjectPtr(), expected.GetObjectPtr(), TEST_LOCATION);
}

void CheckRange(const StyledTextBuilder& builder, uint32_t index, uint32_t startIndex, uint32_t endIndex)
{
  DALI_TEST_EQUALS(builder.GetSpanStartIndexAt(index), startIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetSpanEndIndexAt(index), endIndex, TEST_LOCATION);
}

void CheckRange(const StyledText& styledText, uint32_t index, uint32_t startIndex, uint32_t endIndex)
{
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(index), startIndex, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(index), endIndex, TEST_LOCATION);
}

void CheckCodePointRangeValidation(const Dali::String& text, uint32_t expectedCount, uint32_t additionalInvalidEnd = 0u)
{
  StyledTextBuilder builder = StyledTextBuilder::New(text);
  ForegroundColorSpan         span    = ForegroundColorSpan::New(UiColor(Color::RED));

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, expectedCount));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, expectedCount);

  DALI_TEST_CHECK(!builder.SetSpan(span, 0u, expectedCount + 1u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, expectedCount);

  if(additionalInvalidEnd > expectedCount + 1u)
  {
    DALI_TEST_CHECK(!builder.SetSpan(span, 0u, additionalInvalidEnd));
    DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
    CheckRange(builder, 0u, 0u, expectedCount);
  }

  DALI_TEST_CHECK(!builder.SetSpan(span, expectedCount, expectedCount));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, expectedCount);
}

void CheckCodePointSubRangeValidation(const Dali::String& text, uint32_t startIndex, uint32_t endIndex)
{
  StyledTextBuilder builder = StyledTextBuilder::New(text);
  ForegroundColorSpan         span    = ForegroundColorSpan::New(UiColor(Color::BLUE));

  DALI_TEST_CHECK(builder.SetSpan(span, startIndex, endIndex));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, startIndex, endIndex);
}

} // unnamed namespace

int UtcDaliStyledTextDefaultConstructorP(void)
{
  UiTestApplication application;

  StyledText styledText;

  DALI_TEST_CHECK(!styledText);
  DALI_TEST_EQUALS(styledText.GetText(), String(), TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!styledText.GetSpanAt(0u));
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(styledText.IsEmpty());

  END_TEST;
}

int UtcDaliStyledTextNewP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::New("Hello");

  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetText(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!styledText.GetSpanAt(1u));
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!styledText.IsEmpty());

  END_TEST;
}

int UtcDaliForegroundColorSpanNewAndDownCastP(void)
{
  UiTestApplication application;

  ForegroundColorSpan foregroundColorSpan = ForegroundColorSpan::New(UiColor(Color::RED));

  DALI_TEST_CHECK(foregroundColorSpan);
  DALI_TEST_EQUALS(foregroundColorSpan.GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  Span baseSpan = Span::DownCast(foregroundColorSpan);
  DALI_TEST_CHECK(baseSpan);
  CheckSpanIdentity(baseSpan, foregroundColorSpan);

  ForegroundColorSpan downcastSpan = ForegroundColorSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(downcastSpan);
  DALI_TEST_EQUALS(downcastSpan.GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  DALI_TEST_CHECK(!Span::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!ForegroundColorSpan::DownCast(BaseHandle()));
  DALI_TEST_EQUALS(ForegroundColorSpan().GetColor().GetRgba(), UiColor().GetRgba(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliBackgroundColorSpanNewAndDownCastP(void)
{
  UiTestApplication application;

  BackgroundColorSpan backgroundColorSpan = BackgroundColorSpan::New(UiColor(Color::YELLOW));

  DALI_TEST_CHECK(backgroundColorSpan);
  DALI_TEST_EQUALS(backgroundColorSpan.GetColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  Span baseSpan = Span::DownCast(backgroundColorSpan);
  DALI_TEST_CHECK(baseSpan);
  CheckSpanIdentity(baseSpan, backgroundColorSpan);

  BackgroundColorSpan downcastSpan = BackgroundColorSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(downcastSpan);
  DALI_TEST_EQUALS(downcastSpan.GetColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  DALI_TEST_CHECK(!BackgroundColorSpan::DownCast(BaseHandle()));
  DALI_TEST_EQUALS(BackgroundColorSpan().GetColor().GetRgba(), UiColor().GetRgba(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliUnderlineSpanNewAndDownCastP(void)
{
  UiTestApplication application;

  Underline underline;
  underline.SetColor(UiColor(Color::MAGENTA));
  underline.SetThickness(2.0f);
  underline.SetType(Underline::Type::DASHED);
  underline.SetDashLength(4.0f);
  underline.SetDashGap(2.0f);

  UnderlineSpan underlineSpan = UnderlineSpan::New(underline);

  DALI_TEST_CHECK(underlineSpan);
  DALI_TEST_CHECK(underlineSpan.GetUnderline() == underline);

  Span baseSpan = Span::DownCast(underlineSpan);
  DALI_TEST_CHECK(baseSpan);
  CheckSpanIdentity(baseSpan, underlineSpan);

  UnderlineSpan downcastSpan = UnderlineSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(downcastSpan);
  DALI_TEST_CHECK(downcastSpan.GetUnderline() == underline);

  DALI_TEST_CHECK(!UnderlineSpan::DownCast(BaseHandle()));
  DALI_TEST_CHECK(UnderlineSpan().GetUnderline() == Underline());

  END_TEST;
}

int UtcDaliLineThroughSpanNewAndDownCastP(void)
{
  UiTestApplication application;

  LineThrough lineThrough;
  lineThrough.SetColor(UiColor(Color::BLUE));
  lineThrough.SetThickness(3.0f);

  LineThroughSpan lineThroughSpan = LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(lineThroughSpan);
  DALI_TEST_CHECK(lineThroughSpan.GetLineThrough() == lineThrough);

  Span baseSpan = Span::DownCast(lineThroughSpan);
  DALI_TEST_CHECK(baseSpan);
  CheckSpanIdentity(baseSpan, lineThroughSpan);

  LineThroughSpan downcastSpan = LineThroughSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(downcastSpan);
  DALI_TEST_CHECK(downcastSpan.GetLineThrough() == lineThrough);

  DALI_TEST_CHECK(!LineThroughSpan::DownCast(BaseHandle()));
  DALI_TEST_CHECK(LineThroughSpan().GetLineThrough() == LineThrough());

  END_TEST;
}

int UtcDaliFontSpanNewAndDownCastP(void)
{
  UiTestApplication application;

  FontAttributes attributes;
  attributes.SetFamily("Ubuntu Mono");
  attributes.SetSize(30.0f);
  attributes.SetWeight(FontWeight::BOLD);
  attributes.SetWidth(FontWidth::CONDENSED);
  attributes.SetSlant(FontSlant::ITALIC);

  FontSpan fontSpan = FontSpan::New(attributes);

  DALI_TEST_CHECK(fontSpan);
  DALI_TEST_CHECK(fontSpan.GetFontAttributes() == attributes);

  Span baseSpan = Span::DownCast(fontSpan);
  DALI_TEST_CHECK(baseSpan);
  CheckSpanIdentity(baseSpan, fontSpan);

  FontSpan downcastSpan = FontSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(downcastSpan);
  DALI_TEST_CHECK(downcastSpan.GetFontAttributes() == attributes);

  DALI_TEST_CHECK(!FontSpan::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!FontSpan().GetFontAttributes().HasAttributes());

  END_TEST;
}

int UtcDaliAnchorSpanNewAndDownCastP(void)
{
  UiTestApplication application;

  AnchorAttributes emptyAttributes;
  DALI_TEST_CHECK(!AnchorSpan::New(emptyAttributes));

  AnchorAttributes attributes;
  attributes.SetHref("");
  attributes.SetColor(UiColor(Color::GREEN));
  attributes.SetClickedColor(UiColor(Color::RED));

  AnchorSpan anchorSpan = AnchorSpan::New(attributes);

  DALI_TEST_CHECK(anchorSpan);
  DALI_TEST_CHECK(anchorSpan.GetAnchorAttributes() == attributes);

  Span baseSpan = Span::DownCast(anchorSpan);
  DALI_TEST_CHECK(baseSpan);
  CheckSpanIdentity(baseSpan, anchorSpan);

  AnchorSpan downcastSpan = AnchorSpan::DownCast(baseSpan);
  DALI_TEST_CHECK(downcastSpan);
  DALI_TEST_CHECK(downcastSpan.GetAnchorAttributes() == attributes);

  DALI_TEST_CHECK(!AnchorSpan::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!AnchorSpan().GetAnchorAttributes().HasAttributes());

  END_TEST;
}

int UtcDaliStyledTextBuilderSetSpanAndBuildP(void)
{
  UiTestApplication application;

  StyledTextBuilder builder = StyledTextBuilder::New("Hello DALi");
  ForegroundColorSpan         span    = ForegroundColorSpan::New(UiColor(Color::BLUE));

  DALI_TEST_CHECK(builder);
  DALI_TEST_EQUALS(builder.GetText(), "Hello DALi", TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetSpanCount(), 0u, TEST_LOCATION);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), span);
  CheckRange(builder, 0u, 0u, 5u);
  DALI_TEST_CHECK(!builder.GetSpanAt(1u));
  DALI_TEST_EQUALS(builder.GetSpanStartIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetSpanEndIndexAt(1u), 0u, TEST_LOCATION);

  StyledText styledText = builder.Build();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetText(), "Hello DALi", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(0u), span);
  CheckRange(styledText, 0u, 0u, 5u);
  DALI_TEST_EQUALS(ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderSetBackgroundColorSpanAndBuildP(void)
{
  UiTestApplication application;

  StyledTextBuilder   builder = StyledTextBuilder::New("Hello DALi");
  BackgroundColorSpan span    = BackgroundColorSpan::New(UiColor(Color::CYAN));

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), span);
  CheckRange(builder, 0u, 0u, 5u);

  StyledText styledText = builder.Build();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(0u), span);
  CheckRange(styledText, 0u, 0u, 5u);
  DALI_TEST_EQUALS(BackgroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor().GetRgba(), Color::CYAN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderSetUnderlineSpanAndBuildP(void)
{
  UiTestApplication application;

  Underline underline;
  underline.SetColor(UiColor(Color::GREEN));
  underline.SetThickness(2.0f);
  underline.SetType(Underline::Type::DOUBLE);

  StyledTextBuilder builder = StyledTextBuilder::New("Hello DALi");
  UnderlineSpan     span    = UnderlineSpan::New(underline);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), span);
  CheckRange(builder, 0u, 0u, 5u);

  StyledText styledText = builder.Build();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(0u), span);
  CheckRange(styledText, 0u, 0u, 5u);
  DALI_TEST_CHECK(UnderlineSpan::DownCast(styledText.GetSpanAt(0u)).GetUnderline() == underline);

  END_TEST;
}

int UtcDaliStyledTextBuilderSetLineThroughSpanAndBuildP(void)
{
  UiTestApplication application;

  LineThrough lineThrough;
  lineThrough.SetColor(UiColor(Color::RED));
  lineThrough.SetThickness(2.5f);

  StyledTextBuilder builder = StyledTextBuilder::New("Hello DALi");
  LineThroughSpan   span    = LineThroughSpan::New(lineThrough);

  DALI_TEST_CHECK(builder.SetSpan(span, 6u, 10u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), span);
  CheckRange(builder, 0u, 6u, 10u);

  StyledText styledText = builder.Build();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(0u), span);
  CheckRange(styledText, 0u, 6u, 10u);
  DALI_TEST_CHECK(LineThroughSpan::DownCast(styledText.GetSpanAt(0u)).GetLineThrough() == lineThrough);

  END_TEST;
}

int UtcDaliStyledTextBuilderSetFontSpanAndBuildP(void)
{
  UiTestApplication application;

  FontAttributes attributes;
  attributes.SetWeight(FontWeight::BOLD);
  attributes.SetSlant(FontSlant::ITALIC);

  StyledTextBuilder builder = StyledTextBuilder::New("Hello DALi");
  FontSpan          span    = FontSpan::New(attributes);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 5u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), span);
  CheckRange(builder, 0u, 0u, 5u);

  StyledText styledText = builder.Build();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(0u), span);
  CheckRange(styledText, 0u, 0u, 5u);
  DALI_TEST_CHECK(FontSpan::DownCast(styledText.GetSpanAt(0u)).GetFontAttributes() == attributes);

  END_TEST;
}

int UtcDaliStyledTextBuilderSetAnchorSpanAndBuildP(void)
{
  UiTestApplication application;

  AnchorAttributes attributes;
  attributes.SetHref("https://www.tizen.org");
  attributes.SetClickedColor(UiColor(Color::MAGENTA));

  StyledTextBuilder builder = StyledTextBuilder::New("Hello DALi");
  AnchorSpan        span    = AnchorSpan::New(attributes);

  DALI_TEST_CHECK(builder.SetSpan(span, 6u, 10u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), span);
  CheckRange(builder, 0u, 6u, 10u);

  StyledText styledText = builder.Build();
  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(0u), span);
  CheckRange(styledText, 0u, 6u, 10u);
  DALI_TEST_CHECK(AnchorSpan::DownCast(styledText.GetSpanAt(0u)).GetAnchorAttributes() == attributes);

  END_TEST;
}

int UtcDaliStyledTextBuilderSetSpanUpdateSameObjectP(void)
{
  UiTestApplication application;

  StyledTextBuilder builder = StyledTextBuilder::New("Hello DALi");
  ForegroundColorSpan         spanA   = ForegroundColorSpan::New(UiColor(Color::RED));
  ForegroundColorSpan         spanB   = ForegroundColorSpan::New(UiColor(Color::RED));

  DALI_TEST_CHECK(builder.SetSpan(spanA, 0u, 2u));
  DALI_TEST_CHECK(builder.SetSpan(spanB, 3u, 7u));
  DALI_TEST_CHECK(builder.SetSpan(spanA, 1u, 4u));

  DALI_TEST_EQUALS(builder.GetSpanCount(), 2u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), spanA);
  CheckRange(builder, 0u, 1u, 4u);
  CheckSpanIdentity(builder.GetSpanAt(1u), spanB);
  CheckRange(builder, 1u, 3u, 7u);

  END_TEST;
}

int UtcDaliStyledTextBuilderRangeValidationP(void)
{
  UiTestApplication application;

  ForegroundColorSpan span = ForegroundColorSpan::New(UiColor(Color::GREEN));

  CheckCodePointRangeValidation("ABC", 3u);

  StyledTextBuilder koreanBuilder = StyledTextBuilder::New("가나다");
  DALI_TEST_CHECK(koreanBuilder.SetSpan(span, 0u, 3u));
  DALI_TEST_CHECK(!koreanBuilder.SetSpan(span, 0u, 4u));
  DALI_TEST_EQUALS(koreanBuilder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(koreanBuilder, 0u, 0u, 3u);

  StyledTextBuilder mixedBuilder = StyledTextBuilder::New("Hi가");
  DALI_TEST_CHECK(mixedBuilder.SetSpan(span, 0u, 3u));
  DALI_TEST_CHECK(!mixedBuilder.SetSpan(span, 0u, 4u));
  DALI_TEST_EQUALS(mixedBuilder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(mixedBuilder, 0u, 0u, 3u);

  StyledTextBuilder emojiBuilder = StyledTextBuilder::New("😀A");
  DALI_TEST_CHECK(emojiBuilder.SetSpan(span, 0u, 2u));
  DALI_TEST_CHECK(!emojiBuilder.SetSpan(span, 0u, 3u));
  DALI_TEST_EQUALS(emojiBuilder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(emojiBuilder, 0u, 0u, 2u);

  StyledTextBuilder invalidBuilder = StyledTextBuilder::New("abc");
  DALI_TEST_CHECK(!invalidBuilder.SetSpan(Span(), 0u, 1u));
  DALI_TEST_CHECK(!invalidBuilder.SetSpan(span, 1u, 1u));
  DALI_TEST_CHECK(!invalidBuilder.SetSpan(span, 2u, 1u));
  DALI_TEST_CHECK(!invalidBuilder.SetSpan(span, 0u, 4u));
  DALI_TEST_EQUALS(invalidBuilder.GetSpanCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderComplexUnicodeRangeValidationP(void)
{
  UiTestApplication application;

  // "A❤️B"
  // U+0041 U+2764 U+FE0F U+0042
  // expected code point count: 4
  const Dali::String variationSelectorText("A"
                                           "\xE2\x9D\xA4"
                                           "\xEF\xB8\x8F"
                                           "B");
  const uint32_t variationSelectorExpectedCount = 4u;
  CheckCodePointRangeValidation(variationSelectorText, variationSelectorExpectedCount);
  CheckCodePointSubRangeValidation(variationSelectorText, 1u, 3u);

  // "A👩‍💻B"
  // U+0041 U+1F469 U+200D U+1F4BB U+0042
  // expected code point count: 5, UTF-16 code unit count: 7
  const Dali::String zwjEmojiText("A"
                                  "\xF0\x9F\x91\xA9"
                                  "\xE2\x80\x8D"
                                  "\xF0\x9F\x92\xBB"
                                  "B");
  const uint32_t zwjEmojiExpectedCount = 5u;
  CheckCodePointRangeValidation(zwjEmojiText, zwjEmojiExpectedCount, 7u);
  CheckCodePointSubRangeValidation(zwjEmojiText, 1u, 4u);

  // "👨‍👩‍👧‍👦"
  // U+1F468 U+200D U+1F469 U+200D U+1F467 U+200D U+1F466
  // expected code point count: 7
  const Dali::String familyZwJText("\xF0\x9F\x91\xA8"
                                   "\xE2\x80\x8D"
                                   "\xF0\x9F\x91\xA9"
                                   "\xE2\x80\x8D"
                                   "\xF0\x9F\x91\xA7"
                                   "\xE2\x80\x8D"
                                   "\xF0\x9F\x91\xA6");
  const uint32_t familyZwJExpectedCount = 7u;
  CheckCodePointRangeValidation(familyZwJText, familyZwJExpectedCount);

  // "A👍🏽B"
  // U+0041 U+1F44D U+1F3FD U+0042
  // expected code point count: 4
  const Dali::String skinToneText("A"
                                  "\xF0\x9F\x91\x8D"
                                  "\xF0\x9F\x8F\xBD"
                                  "B");
  const uint32_t skinToneExpectedCount = 4u;
  CheckCodePointRangeValidation(skinToneText, skinToneExpectedCount);
  CheckCodePointSubRangeValidation(skinToneText, 1u, 3u);

  // "A🇰🇷B"
  // U+0041 U+1F1F0 U+1F1F7 U+0042
  // expected code point count: 4
  const Dali::String regionalFlagText("A"
                                      "\xF0\x9F\x87\xB0"
                                      "\xF0\x9F\x87\xB7"
                                      "B");
  const uint32_t regionalFlagExpectedCount = 4u;
  CheckCodePointRangeValidation(regionalFlagText, regionalFlagExpectedCount);
  CheckCodePointSubRangeValidation(regionalFlagText, 1u, 3u);

  // "A1️⃣B"
  // U+0041 U+0031 U+FE0F U+20E3 U+0042
  // expected code point count: 5
  const Dali::String keycapText("A"
                                "1"
                                "\xEF\xB8\x8F"
                                "\xE2\x83\xA3"
                                "B");
  const uint32_t keycapExpectedCount = 5u;
  CheckCodePointRangeValidation(keycapText, keycapExpectedCount);
  CheckCodePointSubRangeValidation(keycapText, 1u, 4u);

  // "AéB"
  // U+0041 U+0065 U+0301 U+0042
  // expected code point count: 4
  // Keep the middle character in decomposed form, not precomposed U+00E9.
  const Dali::String combiningMarkText("A"
                                       "e"
                                       "\xCC\x81"
                                       "B");
  const uint32_t combiningMarkExpectedCount = 4u;
  CheckCodePointRangeValidation(combiningMarkText, combiningMarkExpectedCount);
  CheckCodePointSubRangeValidation(combiningMarkText, 1u, 3u);

  END_TEST;
}

int UtcDaliStyledTextBuilderRemoveAndClearP(void)
{
  UiTestApplication application;

  StyledTextBuilder builder = StyledTextBuilder::New("Hello DALi");
  ForegroundColorSpan         spanA   = ForegroundColorSpan::New(UiColor(Color::RED));
  ForegroundColorSpan         spanB   = ForegroundColorSpan::New(UiColor(Color::BLUE));

  DALI_TEST_CHECK(builder.SetSpan(spanA, 0u, 5u));
  DALI_TEST_CHECK(builder.SetSpan(spanB, 6u, 10u));

  DALI_TEST_CHECK(builder.RemoveSpan(spanA));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), spanB);
  CheckRange(builder, 0u, 6u, 10u);

  DALI_TEST_CHECK(!builder.RemoveSpan(spanA));
  DALI_TEST_CHECK(!builder.RemoveSpan(Span()));
  DALI_TEST_CHECK(!builder.RemoveSpanAt(1u));

  DALI_TEST_CHECK(builder.RemoveSpanAt(0u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 0u, TEST_LOCATION);

  DALI_TEST_CHECK(builder.SetSpan(spanA, 0u, 5u));
  DALI_TEST_CHECK(builder.SetSpan(spanB, 6u, 10u));
  builder.ClearSpans();
  DALI_TEST_EQUALS(builder.GetSpanCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderTextMutationAndSnapshotP(void)
{
  UiTestApplication application;

  StyledTextBuilder builder = StyledTextBuilder::New("abc");
  ForegroundColorSpan         span    = ForegroundColorSpan::New(UiColor(Color::YELLOW));

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 1u));
  builder.AppendText("def");
  DALI_TEST_EQUALS(builder.GetText(), "abcdef", TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, 1u);

  builder.SetText("xyz");
  DALI_TEST_EQUALS(builder.GetText(), "xyz", TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetSpanCount(), 0u, TEST_LOCATION);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 3u));
  StyledText snapshot = builder.Build();

  builder.SetText("changed");
  DALI_TEST_EQUALS(snapshot.GetText(), "xyz", TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(snapshot.GetSpanAt(0u), span);
  CheckRange(snapshot, 0u, 0u, 3u);

  StyledTextBuilder copyBuilder = StyledTextBuilder::FromStyledText(snapshot);
  DALI_TEST_EQUALS(copyBuilder.GetText(), "xyz", TEST_LOCATION);
  DALI_TEST_EQUALS(copyBuilder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(copyBuilder.GetSpanAt(0u), span);
  CheckRange(copyBuilder, 0u, 0u, 3u);

  DALI_TEST_CHECK(copyBuilder.SetSpan(span, 1u, 2u));
  CheckRange(copyBuilder, 0u, 1u, 2u);
  CheckRange(snapshot, 0u, 0u, 3u);

  END_TEST;
}

int UtcDaliStyledTextBuilderFromStyledTextPreservesFontSpanP(void)
{
  UiTestApplication application;

  FontAttributes attributes;
  attributes.SetFamily("");
  attributes.SetWeight(FontWeight::NORMAL);

  StyledTextBuilder builder = StyledTextBuilder::New("Font span");
  FontSpan          span    = FontSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 4u));

  StyledText        snapshot    = builder.Build();
  StyledTextBuilder copyBuilder = StyledTextBuilder::FromStyledText(snapshot);
  StyledText        copy        = copyBuilder.Build();

  DALI_TEST_EQUALS(copy.GetText(), "Font span", TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSpanStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSpanEndIndexAt(0u), 4u, TEST_LOCATION);

  FontAttributes copiedAttributes = FontSpan::DownCast(copy.GetSpanAt(0u)).GetFontAttributes();
  DALI_TEST_CHECK(copiedAttributes == attributes);
  DALI_TEST_CHECK(copiedAttributes.Has(FontAttributes::Attribute::FAMILY));
  DALI_TEST_CHECK(copiedAttributes.Has(FontAttributes::Attribute::WEIGHT));
  DALI_TEST_EQUALS(copiedAttributes.GetFamily(), Dali::String(""), TEST_LOCATION);
  DALI_TEST_EQUALS(copiedAttributes.GetWeight(), FontWeight::NORMAL, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderFromStyledTextPreservesAnchorSpanP(void)
{
  UiTestApplication application;

  AnchorAttributes attributes;
  attributes.SetHref("");
  attributes.SetColor(UiColor(Color::GREEN));

  StyledTextBuilder builder = StyledTextBuilder::New("Anchor span");
  AnchorSpan        span    = AnchorSpan::New(attributes);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 6u));

  StyledText        snapshot    = builder.Build();
  StyledTextBuilder copyBuilder = StyledTextBuilder::FromStyledText(snapshot);
  StyledText        copy        = copyBuilder.Build();

  DALI_TEST_EQUALS(copy.GetText(), "Anchor span", TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSpanStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetSpanEndIndexAt(0u), 6u, TEST_LOCATION);

  AnchorAttributes copiedAttributes = AnchorSpan::DownCast(copy.GetSpanAt(0u)).GetAnchorAttributes();
  DALI_TEST_CHECK(copiedAttributes == attributes);
  DALI_TEST_CHECK(copiedAttributes.Has(AnchorAttributes::Attribute::HREF));
  DALI_TEST_CHECK(copiedAttributes.Has(AnchorAttributes::Attribute::COLOR));
  DALI_TEST_EQUALS(copiedAttributes.GetHref(), Dali::String(""), TEST_LOCATION);
  DALI_TEST_EQUALS(copiedAttributes.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);

  END_TEST;
}
