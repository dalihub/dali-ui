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

void CheckSemiTransparentRedColor(const UiColor& color)
{
  const Vector4 rgba = color.GetRgba();
  DALI_TEST_EQUALS(rgba.a, 128.0f / 255.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(rgba.r, 1.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(rgba.g, 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(rgba.b, 0.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
}

void CheckUtf32LengthApis(const Dali::String& text, uint32_t expectedUtf32Length, uint32_t expectedByteLength)
{
  DALI_TEST_EQUALS(text.Size(), expectedByteLength, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Text::Utf8ToUtf32Length(text), expectedUtf32Length, TEST_LOCATION);

  StyledText styledText = StyledText::New(text);
  DALI_TEST_EQUALS(styledText.GetUtf32Length(), expectedUtf32Length, TEST_LOCATION);

  StyledTextBuilder builder = StyledTextBuilder::New(text);
  DALI_TEST_EQUALS(builder.GetUtf32Length(), expectedUtf32Length, TEST_LOCATION);

  StyledText snapshot = builder.Build();
  DALI_TEST_EQUALS(snapshot.GetUtf32Length(), expectedUtf32Length, TEST_LOCATION);

  StyledTextBuilder copyBuilder = StyledTextBuilder::FromStyledText(snapshot);
  DALI_TEST_EQUALS(copyBuilder.GetUtf32Length(), expectedUtf32Length, TEST_LOCATION);
}

void CheckUtf32RangeValidation(const Dali::String& text, uint32_t expectedUtf32Length, uint32_t expectedByteLength)
{
  StyledTextBuilder builder = StyledTextBuilder::New(text);
  ForegroundColorSpan         span    = ForegroundColorSpan::New(UiColor(Color::RED));

  CheckUtf32LengthApis(text, expectedUtf32Length, expectedByteLength);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, expectedUtf32Length));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, expectedUtf32Length);

  DALI_TEST_CHECK(!builder.SetSpan(span, 0u, expectedUtf32Length + 1u));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, expectedUtf32Length);

  if(expectedByteLength != expectedUtf32Length)
  {
    DALI_TEST_CHECK(!builder.SetSpan(span, 0u, expectedByteLength));
    DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
    CheckRange(builder, 0u, 0u, expectedUtf32Length);
  }

  DALI_TEST_CHECK(!builder.SetSpan(span, expectedUtf32Length, expectedUtf32Length));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, expectedUtf32Length);
}

void CheckUtf32SubRangeValidation(const Dali::String& text, uint32_t startIndex, uint32_t endIndex)
{
  StyledTextBuilder builder = StyledTextBuilder::New(text);
  ForegroundColorSpan         span    = ForegroundColorSpan::New(UiColor(Color::BLUE));

  DALI_TEST_CHECK(builder.SetSpan(span, startIndex, endIndex));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, startIndex, endIndex);
}

void CheckUtf32Length(const Dali::String& text, uint32_t expectedUtf32Length, uint32_t expectedByteLength)
{
  DALI_TEST_EQUALS(text.Size(), expectedByteLength, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Ui::Text::Utf8ToUtf32Length(text), expectedUtf32Length, TEST_LOCATION);
}

void CheckUtf8ToUtf32Index(const Dali::String& text, uint32_t utf8Index, bool expectedResult, uint32_t expectedUtf32Index)
{
  uint32_t utf32Index = 999u;
  DALI_TEST_EQUALS(Dali::Ui::Text::Utf8ToUtf32Index(text, utf8Index, utf32Index), expectedResult, TEST_LOCATION);
  DALI_TEST_EQUALS(utf32Index, expectedResult ? expectedUtf32Index : 999u, TEST_LOCATION);
}

void CheckUtf8ToUtf32Range(const Dali::String& text, uint32_t utf8StartIndex, uint32_t utf8EndIndex, bool expectedResult, uint32_t expectedUtf32StartIndex, uint32_t expectedUtf32EndIndex)
{
  uint32_t utf32StartIndex = 999u;
  uint32_t utf32EndIndex   = 777u;
  DALI_TEST_EQUALS(Dali::Ui::Text::Utf8ToUtf32Range(text, utf8StartIndex, utf8EndIndex, utf32StartIndex, utf32EndIndex), expectedResult, TEST_LOCATION);
  DALI_TEST_EQUALS(utf32StartIndex, expectedResult ? expectedUtf32StartIndex : 999u, TEST_LOCATION);
  DALI_TEST_EQUALS(utf32EndIndex, expectedResult ? expectedUtf32EndIndex : 777u, TEST_LOCATION);
}

void CheckUtf32ToUtf8Index(const Dali::String& text, uint32_t utf32Index, bool expectedResult, uint32_t expectedUtf8Index)
{
  uint32_t utf8Index = 999u;
  DALI_TEST_EQUALS(Dali::Ui::Text::Utf32ToUtf8Index(text, utf32Index, utf8Index), expectedResult, TEST_LOCATION);
  DALI_TEST_EQUALS(utf8Index, expectedResult ? expectedUtf8Index : 999u, TEST_LOCATION);
}

void CheckUtf32ToUtf8Range(const Dali::String& text, uint32_t utf32StartIndex, uint32_t utf32EndIndex, bool expectedResult, uint32_t expectedUtf8StartIndex, uint32_t expectedUtf8EndIndex)
{
  uint32_t utf8StartIndex = 999u;
  uint32_t utf8EndIndex   = 777u;
  DALI_TEST_EQUALS(Dali::Ui::Text::Utf32ToUtf8Range(text, utf32StartIndex, utf32EndIndex, utf8StartIndex, utf8EndIndex), expectedResult, TEST_LOCATION);
  DALI_TEST_EQUALS(utf8StartIndex, expectedResult ? expectedUtf8StartIndex : 999u, TEST_LOCATION);
  DALI_TEST_EQUALS(utf8EndIndex, expectedResult ? expectedUtf8EndIndex : 777u, TEST_LOCATION);
}

void CheckUtf8Utf32FullRangeRoundTrip(const Dali::String& text, uint32_t expectedUtf32Length, uint32_t expectedByteLength)
{
  CheckUtf32Length(text, expectedUtf32Length, expectedByteLength);

  uint32_t utf32StartIndex = 999u;
  uint32_t utf32EndIndex   = 777u;
  DALI_TEST_CHECK(Dali::Ui::Text::Utf8ToUtf32Range(text, 0u, text.Size(), utf32StartIndex, utf32EndIndex));
  DALI_TEST_EQUALS(utf32StartIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(utf32EndIndex, expectedUtf32Length, TEST_LOCATION);

  uint32_t utf8StartIndex = 999u;
  uint32_t utf8EndIndex   = 777u;
  DALI_TEST_CHECK(Dali::Ui::Text::Utf32ToUtf8Range(text, 0u, expectedUtf32Length, utf8StartIndex, utf8EndIndex));
  DALI_TEST_EQUALS(utf8StartIndex, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(utf8EndIndex, expectedByteLength, TEST_LOCATION);
}

} // unnamed namespace

int UtcDaliStyledTextDefaultConstructorP(void)
{
  UiTestApplication application;

  StyledText styledText;

  DALI_TEST_CHECK(!styledText);
  DALI_TEST_EQUALS(styledText.GetText(), String(), TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetUtf32Length(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!styledText.GetSpanAt(0u));
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(styledText.IsEmpty());

  StyledTextBuilder emptyBuilder = StyledTextBuilder::New();
  DALI_TEST_CHECK(emptyBuilder);
  DALI_TEST_EQUALS(emptyBuilder.GetText(), Dali::String(), TEST_LOCATION);
  DALI_TEST_EQUALS(emptyBuilder.GetUtf32Length(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(emptyBuilder.GetSpanCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextNewP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::New("Hello");

  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetText(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetUtf32Length(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!styledText.GetSpanAt(1u));
  DALI_TEST_EQUALS(styledText.GetSpanStartIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanEndIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!styledText.IsEmpty());

  END_TEST;
}

int UtcDaliTextUtf8ToUtf32LengthP(void)
{
  UiTestApplication application;

  DALI_TEST_EQUALS(Dali::Ui::Text::Utf8ToUtf32Length(Dali::StringView()), 0u, TEST_LOCATION);
  CheckUtf32Length("ABC", 3u, 3u);
  CheckUtf32Length("가나다", 3u, 9u);
  CheckUtf32Length("A가B", 3u, 5u);
  CheckUtf32Length(Dali::String("A"
                                "\xF0\x9F\x98\x80"
                                "B"),
                   3u,
                   6u);
  CheckUtf32Length(Dali::String("A"
                                "\xE2\x9D\xA4"
                                "\xEF\xB8\x8F"
                                "B"),
                   4u,
                   8u);
  CheckUtf32Length(Dali::String("A"
                                "\xF0\x9F\x91\xA9"
                                "\xE2\x80\x8D"
                                "\xF0\x9F\x92\xBB"
                                "B"),
                   5u,
                   13u);
  CheckUtf32Length(Dali::String("A"
                                "\xF0\x9F\x91\x8D"
                                "\xF0\x9F\x8F\xBD"
                                "B"),
                   4u,
                   10u);
  CheckUtf32Length(Dali::String("A"
                                "\xF0\x9F\x87\xB0"
                                "\xF0\x9F\x87\xB7"
                                "B"),
                   4u,
                   10u);
  CheckUtf32Length(Dali::String("A"
                                "e"
                                "\xCC\x81"
                                "B"),
                   4u,
                   5u);

  const char notNullTerminatedText[] =
  {
    'a',
    'b',
    'c',
    'd',
  };
  DALI_TEST_EQUALS(Dali::Ui::Text::Utf8ToUtf32Length(Dali::StringView(notNullTerminatedText, 3u)), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUtf8ToUtf32IndexP(void)
{
  UiTestApplication application;

  const Dali::String emptyText("");
  CheckUtf8ToUtf32Index(emptyText, 0u, true, 0u);
  CheckUtf8ToUtf32Index(emptyText, 1u, false, 0u);

  const Dali::String text("A"
                          "\xEA\xB0\x80"
                          "B");

  CheckUtf8ToUtf32Index(text, 0u, true, 0u);
  CheckUtf8ToUtf32Index(text, 1u, true, 1u);
  CheckUtf8ToUtf32Index(text, 2u, false, 0u);
  CheckUtf8ToUtf32Index(text, 3u, false, 0u);
  CheckUtf8ToUtf32Index(text, 4u, true, 2u);
  CheckUtf8ToUtf32Index(text, 5u, true, 3u);
  CheckUtf8ToUtf32Index(text, 6u, false, 0u);

  const char truncatedText[] =
  {
    'A',
    static_cast<char>(0xEA),
    static_cast<char>(0xB0),
  };
  uint32_t output = 123u;
  DALI_TEST_CHECK(!Dali::Ui::Text::Utf8ToUtf32Index(Dali::StringView(truncatedText, 3u), 3u, output));
  DALI_TEST_EQUALS(output, 123u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUtf8ToUtf32RangeP(void)
{
  UiTestApplication application;

  const Dali::String emptyText("");
  CheckUtf8ToUtf32Range(emptyText, 0u, 0u, true, 0u, 0u);
  CheckUtf8ToUtf32Range(emptyText, 0u, 1u, false, 0u, 0u);

  const Dali::String text("A"
                          "\xEA\xB0\x80"
                          "B");

  CheckUtf8ToUtf32Range(text, 1u, 4u, true, 1u, 2u);
  CheckUtf8ToUtf32Range(text, 0u, 5u, true, 0u, 3u);
  CheckUtf8ToUtf32Range(text, 1u, 1u, true, 1u, 1u);
  CheckUtf8ToUtf32Range(text, 5u, 5u, true, 3u, 3u);
  CheckUtf8ToUtf32Range(text, 2u, 4u, false, 0u, 0u);
  CheckUtf8ToUtf32Range(text, 1u, 3u, false, 0u, 0u);
  CheckUtf8ToUtf32Range(text, 4u, 1u, false, 0u, 0u);
  CheckUtf8ToUtf32Range(text, 0u, 6u, false, 0u, 0u);

  const char truncatedText[] =
  {
    'A',
    static_cast<char>(0xEA),
    static_cast<char>(0xB0),
  };
  uint32_t start = 123u;
  uint32_t end   = 456u;
  DALI_TEST_CHECK(!Dali::Ui::Text::Utf8ToUtf32Range(Dali::StringView(truncatedText, 3u), 0u, 3u, start, end));
  DALI_TEST_EQUALS(start, 123u, TEST_LOCATION);
  DALI_TEST_EQUALS(end, 456u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUtf32ToUtf8IndexP(void)
{
  UiTestApplication application;

  const Dali::String emptyText("");
  CheckUtf32ToUtf8Index(emptyText, 0u, true, 0u);
  CheckUtf32ToUtf8Index(emptyText, 1u, false, 0u);

  const Dali::String text("A"
                          "\xEA\xB0\x80"
                          "B");

  CheckUtf32ToUtf8Index(text, 0u, true, 0u);
  CheckUtf32ToUtf8Index(text, 1u, true, 1u);
  CheckUtf32ToUtf8Index(text, 2u, true, 4u);
  CheckUtf32ToUtf8Index(text, 3u, true, 5u);
  CheckUtf32ToUtf8Index(text, 4u, false, 0u);

  const char truncatedText[] =
  {
    'A',
    static_cast<char>(0xEA),
    static_cast<char>(0xB0),
  };
  uint32_t output = 123u;
  DALI_TEST_CHECK(!Dali::Ui::Text::Utf32ToUtf8Index(Dali::StringView(truncatedText, 3u), 2u, output));
  DALI_TEST_EQUALS(output, 123u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUtf32ToUtf8RangeP(void)
{
  UiTestApplication application;

  const Dali::String emptyText("");
  CheckUtf32ToUtf8Range(emptyText, 0u, 0u, true, 0u, 0u);
  CheckUtf32ToUtf8Range(emptyText, 0u, 1u, false, 0u, 0u);

  const Dali::String text("A"
                          "\xEA\xB0\x80"
                          "B");

  CheckUtf32ToUtf8Range(text, 1u, 2u, true, 1u, 4u);
  CheckUtf32ToUtf8Range(text, 0u, 3u, true, 0u, 5u);
  CheckUtf32ToUtf8Range(text, 1u, 1u, true, 1u, 1u);
  CheckUtf32ToUtf8Range(text, 3u, 3u, true, 5u, 5u);
  CheckUtf32ToUtf8Range(text, 2u, 1u, false, 0u, 0u);
  CheckUtf32ToUtf8Range(text, 0u, 4u, false, 0u, 0u);

  const char truncatedText[] =
  {
    'A',
    static_cast<char>(0xEA),
    static_cast<char>(0xB0),
  };
  uint32_t start = 123u;
  uint32_t end   = 456u;
  DALI_TEST_CHECK(!Dali::Ui::Text::Utf32ToUtf8Range(Dali::StringView(truncatedText, 3u), 0u, 2u, start, end));
  DALI_TEST_EQUALS(start, 123u, TEST_LOCATION);
  DALI_TEST_EQUALS(end, 456u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTextUtf8Utf32ComplexUnicodeConversionP(void)
{
  UiTestApplication application;

  CheckUtf8Utf32FullRangeRoundTrip(Dali::String("A"
                                                "\xE2\x9D\xA4"
                                                "\xEF\xB8\x8F"
                                                "B"),
                                   4u,
                                   8u);
  CheckUtf8Utf32FullRangeRoundTrip(Dali::String("A"
                                                "\xF0\x9F\x91\xA9"
                                                "\xE2\x80\x8D"
                                                "\xF0\x9F\x92\xBB"
                                                "B"),
                                   5u,
                                   13u);
  CheckUtf8Utf32FullRangeRoundTrip(Dali::String("A"
                                                "\xF0\x9F\x87\xB0"
                                                "\xF0\x9F\x87\xB7"
                                                "B"),
                                   4u,
                                   10u);
  CheckUtf8Utf32FullRangeRoundTrip(Dali::String("A"
                                                "e"
                                                "\xCC\x81"
                                                "B"),
                                   4u,
                                   5u);

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
  DALI_TEST_EQUALS(builder.GetUtf32Length(), 10u, TEST_LOCATION);
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
  DALI_TEST_EQUALS(styledText.GetUtf32Length(), 10u, TEST_LOCATION);
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

  CheckUtf32RangeValidation("ABC", 3u, 3u);
  CheckUtf32RangeValidation("가나다", 3u, 9u);
  CheckUtf32RangeValidation("A가B", 3u, 5u);

  // "A😀B": U+0041 U+1F600 U+0042
  const Dali::String emojiText("A"
                               "\xF0\x9F\x98\x80"
                               "B");
  CheckUtf32RangeValidation(emojiText, 3u, 6u);

  StyledTextBuilder invalidBuilder = StyledTextBuilder::New("abc");
  DALI_TEST_EQUALS(invalidBuilder.GetUtf32Length(), 3u, TEST_LOCATION);
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
  // expected UTF-32 character count: 4
  const Dali::String variationSelectorText("A"
                                           "\xE2\x9D\xA4"
                                           "\xEF\xB8\x8F"
                                           "B");
  const uint32_t variationSelectorExpectedCount = 4u;
  CheckUtf32RangeValidation(variationSelectorText, variationSelectorExpectedCount, 8u);
  CheckUtf32SubRangeValidation(variationSelectorText, 1u, 3u);

  // "A👩‍💻B"
  // U+0041 U+1F469 U+200D U+1F4BB U+0042
  // expected UTF-32 character count: 5, UTF-16 code unit count: 7
  const Dali::String zwjEmojiText("A"
                                  "\xF0\x9F\x91\xA9"
                                  "\xE2\x80\x8D"
                                  "\xF0\x9F\x92\xBB"
                                  "B");
  const uint32_t zwjEmojiExpectedCount = 5u;
  CheckUtf32RangeValidation(zwjEmojiText, zwjEmojiExpectedCount, 13u);
  CheckUtf32SubRangeValidation(zwjEmojiText, 1u, 4u);

  // "👨‍👩‍👧‍👦"
  // U+1F468 U+200D U+1F469 U+200D U+1F467 U+200D U+1F466
  // expected UTF-32 character count: 7
  const Dali::String familyZwJText("\xF0\x9F\x91\xA8"
                                   "\xE2\x80\x8D"
                                   "\xF0\x9F\x91\xA9"
                                   "\xE2\x80\x8D"
                                   "\xF0\x9F\x91\xA7"
                                   "\xE2\x80\x8D"
                                   "\xF0\x9F\x91\xA6");
  const uint32_t familyZwJExpectedCount = 7u;
  CheckUtf32RangeValidation(familyZwJText, familyZwJExpectedCount, 25u);

  // "A👍🏽B"
  // U+0041 U+1F44D U+1F3FD U+0042
  // expected UTF-32 character count: 4
  const Dali::String skinToneText("A"
                                  "\xF0\x9F\x91\x8D"
                                  "\xF0\x9F\x8F\xBD"
                                  "B");
  const uint32_t skinToneExpectedCount = 4u;
  CheckUtf32RangeValidation(skinToneText, skinToneExpectedCount, 10u);
  CheckUtf32SubRangeValidation(skinToneText, 1u, 3u);

  // "A🇰🇷B"
  // U+0041 U+1F1F0 U+1F1F7 U+0042
  // expected UTF-32 character count: 4
  const Dali::String regionalFlagText("A"
                                      "\xF0\x9F\x87\xB0"
                                      "\xF0\x9F\x87\xB7"
                                      "B");
  const uint32_t regionalFlagExpectedCount = 4u;
  CheckUtf32RangeValidation(regionalFlagText, regionalFlagExpectedCount, 10u);
  CheckUtf32SubRangeValidation(regionalFlagText, 1u, 3u);

  // "A1️⃣B"
  // U+0041 U+0031 U+FE0F U+20E3 U+0042
  // expected UTF-32 character count: 5
  const Dali::String keycapText("A"
                                "1"
                                "\xEF\xB8\x8F"
                                "\xE2\x83\xA3"
                                "B");
  const uint32_t keycapExpectedCount = 5u;
  CheckUtf32RangeValidation(keycapText, keycapExpectedCount, 9u);
  CheckUtf32SubRangeValidation(keycapText, 1u, 4u);

  // "AéB"
  // U+0041 U+0065 U+0301 U+0042
  // expected UTF-32 character count: 4
  // Keep the middle character in decomposed form, not precomposed U+00E9.
  const Dali::String combiningMarkText("A"
                                       "e"
                                       "\xCC\x81"
                                       "B");
  const uint32_t combiningMarkExpectedCount = 4u;
  CheckUtf32RangeValidation(combiningMarkText, combiningMarkExpectedCount, 5u);
  CheckUtf32SubRangeValidation(combiningMarkText, 1u, 3u);

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

  DALI_TEST_EQUALS(builder.GetUtf32Length(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 1u));
  builder.AppendText("가😀");
  DALI_TEST_EQUALS(builder.GetText(), "abc가😀", TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetUtf32Length(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(builder, 0u, 0u, 1u);

  builder.SetText("가나다");
  DALI_TEST_EQUALS(builder.GetText(), "가나다", TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetUtf32Length(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetSpanCount(), 0u, TEST_LOCATION);

  DALI_TEST_CHECK(builder.SetSpan(span, 0u, 3u));
  StyledText snapshot = builder.Build();
  DALI_TEST_EQUALS(snapshot.GetUtf32Length(), 3u, TEST_LOCATION);

  builder.SetText("changed");
  DALI_TEST_EQUALS(builder.GetUtf32Length(), 7u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetText(), "가나다", TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetUtf32Length(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(snapshot.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(snapshot.GetSpanAt(0u), span);
  CheckRange(snapshot, 0u, 0u, 3u);

  StyledTextBuilder copyBuilder = StyledTextBuilder::FromStyledText(snapshot);
  DALI_TEST_EQUALS(copyBuilder.GetText(), "가나다", TEST_LOCATION);
  DALI_TEST_EQUALS(copyBuilder.GetUtf32Length(), 3u, TEST_LOCATION);
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

int UtcDaliStyledTextBuilderPushPopSpanP(void)
{
  UiTestApplication application;

  StyledTextBuilder builder = StyledTextBuilder::New();
  DALI_TEST_EQUALS(builder.PushSpan(Span()), StyledTextBuilder::INVALID_SPAN_TOKEN, TEST_LOCATION);
  DALI_TEST_CHECK(!builder.PopSpan());
  DALI_TEST_CHECK(!builder.PopSpan(StyledTextBuilder::INVALID_SPAN_TOKEN));

  ForegroundColorSpan redSpan   = ForegroundColorSpan::New(UiColor(Color::RED));
  ForegroundColorSpan blueSpan  = ForegroundColorSpan::New(UiColor(Color::BLUE));
  ForegroundColorSpan greenSpan = ForegroundColorSpan::New(UiColor(Color::GREEN));

  StyledTextBuilder otherBuilder = StyledTextBuilder::New();
  otherBuilder.PushSpan(ForegroundColorSpan::New(UiColor(Color::YELLOW)));
  const uint32_t otherToken = otherBuilder.PushSpan(ForegroundColorSpan::New(UiColor(Color::MAGENTA)));

  builder.AppendText("A");
  const uint32_t redToken = builder.PushSpan(redSpan);
  DALI_TEST_CHECK(redToken != StyledTextBuilder::INVALID_SPAN_TOKEN);
  builder.AppendText("B");
  const uint32_t blueToken = builder.PushSpan(blueSpan);
  DALI_TEST_CHECK(blueToken != StyledTextBuilder::INVALID_SPAN_TOKEN);
  builder.AppendText("C");
  const uint32_t greenToken = builder.PushSpan(greenSpan);
  DALI_TEST_CHECK(greenToken != StyledTextBuilder::INVALID_SPAN_TOKEN);
  builder.AppendText("D");

  DALI_TEST_CHECK(builder.PopSpan(blueToken));
  DALI_TEST_CHECK(!builder.PopSpan(blueToken));
  DALI_TEST_CHECK(!builder.PopSpan(greenToken));
  DALI_TEST_CHECK(!builder.PopSpan(otherToken));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 2u, TEST_LOCATION);

  CheckSpanIdentity(builder.GetSpanAt(0u), blueSpan);
  CheckRange(builder, 0u, 2u, 4u);
  CheckSpanIdentity(builder.GetSpanAt(1u), greenSpan);
  CheckRange(builder, 1u, 3u, 4u);

  builder.AppendText("E");
  DALI_TEST_CHECK(builder.PopSpan(redToken));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 3u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(0u), redSpan);
  CheckRange(builder, 0u, 1u, 5u);
  CheckSpanIdentity(builder.GetSpanAt(1u), blueSpan);
  CheckSpanIdentity(builder.GetSpanAt(2u), greenSpan);

  StyledTextBuilder latestBuilder = StyledTextBuilder::New();
  latestBuilder.AppendText("A");
  const uint32_t latestRedToken = latestBuilder.PushSpan(redSpan);
  latestBuilder.AppendText("B");
  const uint32_t latestBlueToken = latestBuilder.PushSpan(blueSpan);
  latestBuilder.AppendText("C");

  DALI_TEST_CHECK(latestBuilder.PopSpan());
  DALI_TEST_CHECK(!latestBuilder.PopSpan(latestBlueToken));
  DALI_TEST_EQUALS(latestBuilder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(latestBuilder.GetSpanAt(0u), blueSpan);
  CheckRange(latestBuilder, 0u, 2u, 3u);
  DALI_TEST_CHECK(latestBuilder.PopSpan(latestRedToken));
  DALI_TEST_EQUALS(latestBuilder.GetSpanCount(), 2u, TEST_LOCATION);
  CheckSpanIdentity(latestBuilder.GetSpanAt(0u), redSpan);
  CheckRange(latestBuilder, 0u, 1u, 3u);
  CheckSpanIdentity(latestBuilder.GetSpanAt(1u), blueSpan);
  CheckRange(latestBuilder, 1u, 2u, 3u);

  StyledTextBuilder emptyRangeBuilder = StyledTextBuilder::New("A");
  const uint32_t     emptyRangeToken  = emptyRangeBuilder.PushSpan(redSpan);
  DALI_TEST_CHECK(emptyRangeBuilder.PopSpan(emptyRangeToken));
  DALI_TEST_EQUALS(emptyRangeBuilder.GetSpanCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderBuildDoesNotCloseOpenSpansP(void)
{
  UiTestApplication application;

  AnnotationSpan    annotation = AnnotationSpan::New("style", "gradient");
  StyledTextBuilder builder    = StyledTextBuilder::New();
  builder.AppendText("Hello ");

  const uint32_t token = builder.PushSpan(annotation);
  DALI_TEST_CHECK(token != StyledTextBuilder::INVALID_SPAN_TOKEN);
  builder.AppendText("TV");

  StyledText styledText = builder.Build();
  DALI_TEST_EQUALS(styledText.GetText(), "Hello TV", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(0u), annotation);
  CheckRange(styledText, 0u, 6u, 8u);
  CheckSpanIdentity(styledText.GetAnnotationAt(0u), annotation);
  DALI_TEST_EQUALS(styledText.GetAnnotationStartIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationEndIndexAt(0u), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationAt(0u).GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationAt(0u).GetValue(), "gradient", TEST_LOCATION);

  DALI_TEST_EQUALS(builder.GetSpanCount(), 0u, TEST_LOCATION);
  builder.AppendText(" Pro");

  StyledText secondBuild = builder.Build();
  DALI_TEST_EQUALS(secondBuild.GetText(), "Hello TV Pro", TEST_LOCATION);
  DALI_TEST_EQUALS(secondBuild.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(secondBuild.GetAnnotationCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(secondBuild.GetAnnotationAt(0u), annotation);
  DALI_TEST_EQUALS(secondBuild.GetAnnotationStartIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(secondBuild.GetAnnotationEndIndexAt(0u), 12u, TEST_LOCATION);

  DALI_TEST_CHECK(builder.PopSpan(token));
  DALI_TEST_EQUALS(builder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetAnnotationAt(0u), annotation);
  DALI_TEST_EQUALS(builder.GetAnnotationStartIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetAnnotationEndIndexAt(0u), 12u, TEST_LOCATION);
  DALI_TEST_CHECK(!builder.PopSpan(token));

  END_TEST;
}

int UtcDaliStyledTextAnnotationSpanIntegrationP(void)
{
  UiTestApplication application;

  StyledTextBuilder builder = StyledTextBuilder::New("Hello TV");
  ForegroundColorSpan foreground = ForegroundColorSpan::New(UiColor(Color::RED));
  AnnotationSpan firstAnnotation = AnnotationSpan::New("content-type", "brand");
  AnnotationSpan secondAnnotation = AnnotationSpan::New("tone", "greeting");

  DALI_TEST_CHECK(builder.SetSpan(foreground, 0u, 5u));
  DALI_TEST_CHECK(builder.SetSpan(firstAnnotation, 6u, 8u));
  DALI_TEST_CHECK(builder.SetSpan(secondAnnotation, 0u, 5u));
  DALI_TEST_EQUALS(firstAnnotation.GetKey(), "content-type", TEST_LOCATION);
  DALI_TEST_EQUALS(firstAnnotation.GetValue(), "brand", TEST_LOCATION);

  DALI_TEST_EQUALS(builder.GetSpanCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetAnnotationCount(), 2u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetSpanAt(1u), firstAnnotation);
  CheckSpanIdentity(builder.GetAnnotationAt(0u), firstAnnotation);
  DALI_TEST_EQUALS(builder.GetAnnotationStartIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetAnnotationEndIndexAt(0u), 8u, TEST_LOCATION);
  CheckSpanIdentity(builder.GetAnnotationAt(1u), secondAnnotation);
  DALI_TEST_EQUALS(builder.GetAnnotationStartIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetAnnotationEndIndexAt(1u), 5u, TEST_LOCATION);

  DALI_TEST_CHECK(!builder.GetAnnotationAt(2u));
  DALI_TEST_EQUALS(builder.GetAnnotationStartIndexAt(2u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(builder.GetAnnotationEndIndexAt(2u), 0u, TEST_LOCATION);

  StyledText styledText = builder.Build();
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationCount(), 2u, TEST_LOCATION);
  CheckSpanIdentity(styledText.GetSpanAt(1u), firstAnnotation);
  CheckSpanIdentity(styledText.GetAnnotationAt(0u), firstAnnotation);
  DALI_TEST_EQUALS(styledText.GetAnnotationStartIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationEndIndexAt(0u), 8u, TEST_LOCATION);

  StyledTextBuilder copyBuilder = StyledTextBuilder::FromStyledText(styledText);
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationCount(), 2u, TEST_LOCATION);
  CheckSpanIdentity(copyBuilder.GetAnnotationAt(0u), firstAnnotation);

  DALI_TEST_CHECK(copyBuilder.RemoveSpan(firstAnnotation));
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationCount(), 1u, TEST_LOCATION);
  CheckSpanIdentity(copyBuilder.GetAnnotationAt(0u), secondAnnotation);

  DALI_TEST_CHECK(copyBuilder.RemoveSpanAt(1u));
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationCount(), 0u, TEST_LOCATION);

  DALI_TEST_CHECK(copyBuilder.SetSpan(firstAnnotation, 6u, 8u));
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationStartIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationEndIndexAt(0u), 8u, TEST_LOCATION);

  DALI_TEST_CHECK(copyBuilder.SetSpan(firstAnnotation, 0u, 5u));
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationEndIndexAt(0u), 5u, TEST_LOCATION);

  copyBuilder.ClearSpans();
  DALI_TEST_EQUALS(copyBuilder.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(copyBuilder.GetAnnotationCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderAnnotationCacheUpdateP(void)
{
  UiTestApplication application;

  StyledTextBuilder resolveBuilder = StyledTextBuilder::FromMarkup(
    "<annotation style='muted'>Muted</annotation> "
    "<annotation style='accent'>Accent</annotation>");

  const uint32_t annotationCount = resolveBuilder.GetAnnotationCount();
  DALI_TEST_EQUALS(annotationCount, 2u, TEST_LOCATION);

  for(uint32_t index = 0u; index < annotationCount; ++index)
  {
    AnnotationSpan annotation = resolveBuilder.GetAnnotationAt(index);
    const uint32_t startIndex = resolveBuilder.GetAnnotationStartIndexAt(index);
    const uint32_t endIndex   = resolveBuilder.GetAnnotationEndIndexAt(index);

    if(annotation.GetKey() == "style")
    {
      if(annotation.GetValue() == "muted")
      {
        DALI_TEST_CHECK(resolveBuilder.SetSpan(ForegroundColorSpan::New(UiColor(Color::RED)), startIndex, endIndex));
      }
      else if(annotation.GetValue() == "accent")
      {
        DALI_TEST_CHECK(resolveBuilder.SetSpan(ForegroundColorSpan::New(UiColor(Color::BLUE)), startIndex, endIndex));
      }
    }
  }

  DALI_TEST_EQUALS(resolveBuilder.GetAnnotationCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(resolveBuilder.GetAnnotationAt(0u).GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(resolveBuilder.GetAnnotationAt(0u).GetValue(), "muted", TEST_LOCATION);
  DALI_TEST_EQUALS(resolveBuilder.GetAnnotationAt(1u).GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(resolveBuilder.GetAnnotationAt(1u).GetValue(), "accent", TEST_LOCATION);

  StyledText resolvedText = resolveBuilder.Build();
  DALI_TEST_EQUALS(resolvedText.GetAnnotationCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(resolvedText.GetSpanCount() >= 4u);

  StyledTextBuilder rangeBuilder = StyledTextBuilder::New("Hello TV Pro");
  AnnotationSpan    annotation   = AnnotationSpan::New("style", "muted");

  DALI_TEST_CHECK(rangeBuilder.SetSpan(annotation, 0u, 5u));
  DALI_TEST_EQUALS(rangeBuilder.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeBuilder.GetAnnotationStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeBuilder.GetAnnotationEndIndexAt(0u), 5u, TEST_LOCATION);

  DALI_TEST_CHECK(rangeBuilder.SetSpan(annotation, 6u, 8u));
  DALI_TEST_EQUALS(rangeBuilder.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeBuilder.GetAnnotationStartIndexAt(0u), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(rangeBuilder.GetAnnotationEndIndexAt(0u), 8u, TEST_LOCATION);

  StyledTextBuilder removeBuilder = StyledTextBuilder::New("Hello TV");
  ForegroundColorSpan foreground  = ForegroundColorSpan::New(UiColor(Color::RED));
  AnnotationSpan      first       = AnnotationSpan::New("style", "first");
  AnnotationSpan      second      = AnnotationSpan::New("style", "second");

  DALI_TEST_CHECK(removeBuilder.SetSpan(foreground, 0u, 5u));
  DALI_TEST_CHECK(removeBuilder.SetSpan(first, 0u, 5u));
  DALI_TEST_CHECK(removeBuilder.SetSpan(second, 6u, 8u));
  DALI_TEST_EQUALS(removeBuilder.GetAnnotationCount(), 2u, TEST_LOCATION);

  DALI_TEST_CHECK(removeBuilder.RemoveSpan(foreground));

  DALI_TEST_EQUALS(removeBuilder.GetAnnotationCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(removeBuilder.GetAnnotationAt(0u).GetValue(), "first", TEST_LOCATION);
  DALI_TEST_EQUALS(removeBuilder.GetAnnotationAt(1u).GetValue(), "second", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextBuilderFromMarkupP(void)
{
  UiTestApplication application;

  const char* const plainMarkup = "Hello 가😀";
  StyledText        plainText   = StyledText::FromMarkup(plainMarkup);

  DALI_TEST_EQUALS(plainText.GetText(), plainMarkup, TEST_LOCATION);
  DALI_TEST_EQUALS(plainText.GetUtf32Length(), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(plainText.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plainText.GetAnnotationCount(), 0u, TEST_LOCATION);

  StyledTextBuilder plainBuilder = StyledTextBuilder::FromMarkup(plainMarkup);

  DALI_TEST_EQUALS(plainBuilder.GetText(), plainMarkup, TEST_LOCATION);
  DALI_TEST_EQUALS(plainBuilder.GetUtf32Length(), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(plainBuilder.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(plainBuilder.GetAnnotationCount(), 0u, TEST_LOCATION);

  StyledTextBuilder colorBuilder = StyledTextBuilder::FromMarkup("<color value='red'>가😀B</color>");

  DALI_TEST_EQUALS(colorBuilder.GetText(), "가😀B", TEST_LOCATION);
  DALI_TEST_EQUALS(colorBuilder.GetUtf32Length(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(colorBuilder.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(colorBuilder, 0u, 0u, 3u);
  DALI_TEST_CHECK(ForegroundColorSpan::DownCast(colorBuilder.GetSpanAt(0u)));

  StyledTextBuilder annotationBuilder = StyledTextBuilder::FromMarkup("<annotation style='gradient' role='link'>hello world</annotation>");

  DALI_TEST_EQUALS(annotationBuilder.GetText(), "hello world", TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationAt(0u).GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationAt(0u).GetValue(), "gradient", TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationAt(1u).GetKey(), "role", TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationAt(1u).GetValue(), "link", TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationEndIndexAt(0u), 11u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationStartIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationBuilder.GetAnnotationEndIndexAt(1u), 11u, TEST_LOCATION);
  DALI_TEST_CHECK(AnnotationSpan::DownCast(annotationBuilder.GetSpanAt(0u)));
  DALI_TEST_CHECK(AnnotationSpan::DownCast(annotationBuilder.GetSpanAt(1u)));

  StyledTextBuilder mutableBuilder = StyledTextBuilder::FromMarkup("<u>Hello</u>");
  DALI_TEST_CHECK(!mutableBuilder.PopSpan());

  ForegroundColorSpan foreground = ForegroundColorSpan::New(UiColor(Color::RED));
  DALI_TEST_CHECK(mutableBuilder.SetSpan(foreground, 0u, 5u));

  StyledText mutableText = mutableBuilder.Build();
  DALI_TEST_EQUALS(mutableText.GetText(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(mutableText.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(UnderlineSpan::DownCast(mutableText.GetSpanAt(0u)));
  CheckSpanIdentity(mutableText.GetSpanAt(1u), foreground);

  StyledTextBuilder noAttributeBuilder = StyledTextBuilder::FromMarkup("<annotation>plain</annotation>");

  DALI_TEST_EQUALS(noAttributeBuilder.GetText(), "plain", TEST_LOCATION);
  DALI_TEST_EQUALS(noAttributeBuilder.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(noAttributeBuilder.GetAnnotationCount(), 0u, TEST_LOCATION);

  StyledText emptyAnnotation = StyledText::FromMarkup("<annotation style='muted'></annotation>");
  DALI_TEST_EQUALS(emptyAnnotation.GetText(), "", TEST_LOCATION);
  DALI_TEST_EQUALS(emptyAnnotation.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(emptyAnnotation.GetAnnotationCount(), 0u, TEST_LOCATION);

  StyledText emptyColor = StyledText::FromMarkup("<color value='red'></color>");
  DALI_TEST_EQUALS(emptyColor.GetText(), "", TEST_LOCATION);
  DALI_TEST_EQUALS(emptyColor.GetSpanCount(), 0u, TEST_LOCATION);

  StyledText emptyUnderline = StyledText::FromMarkup("<u></u>");
  DALI_TEST_EQUALS(emptyUnderline.GetText(), "", TEST_LOCATION);
  DALI_TEST_EQUALS(emptyUnderline.GetSpanCount(), 0u, TEST_LOCATION);

  StyledTextBuilder emptyBuilder = StyledTextBuilder::FromMarkup("<annotation style='muted'></annotation>");
  DALI_TEST_EQUALS(emptyBuilder.GetText(), "", TEST_LOCATION);
  DALI_TEST_EQUALS(emptyBuilder.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(emptyBuilder.GetAnnotationCount(), 0u, TEST_LOCATION);

  StyledText emptyAnnotationThenText = StyledText::FromMarkup("<annotation style='muted'></annotation>plain");
  DALI_TEST_EQUALS(emptyAnnotationThenText.GetText(), "plain", TEST_LOCATION);
  DALI_TEST_EQUALS(emptyAnnotationThenText.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(emptyAnnotationThenText.GetAnnotationCount(), 0u, TEST_LOCATION);

  const char* equivalenceMarkup = "<annotation style='gradient' role='link'>hello world</annotation>";
  StyledText styledText = StyledText::FromMarkup(equivalenceMarkup);
  StyledText builderText = StyledTextBuilder::FromMarkup(equivalenceMarkup).Build();

  DALI_TEST_EQUALS(builderText.GetText(), styledText.GetText(), TEST_LOCATION);
  DALI_TEST_EQUALS(builderText.GetSpanCount(), styledText.GetSpanCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(builderText.GetAnnotationCount(), styledText.GetAnnotationCount(), TEST_LOCATION);
  for(uint32_t index = 0u; index < styledText.GetAnnotationCount(); ++index)
  {
    DALI_TEST_EQUALS(builderText.GetAnnotationAt(index).GetKey(), styledText.GetAnnotationAt(index).GetKey(), TEST_LOCATION);
    DALI_TEST_EQUALS(builderText.GetAnnotationAt(index).GetValue(), styledText.GetAnnotationAt(index).GetValue(), TEST_LOCATION);
    DALI_TEST_EQUALS(builderText.GetAnnotationStartIndexAt(index), styledText.GetAnnotationStartIndexAt(index), TEST_LOCATION);
    DALI_TEST_EQUALS(builderText.GetAnnotationEndIndexAt(index), styledText.GetAnnotationEndIndexAt(index), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliStyledTextFromMarkupColorP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::FromMarkup("<color value='red'>Hello</color>");

  DALI_TEST_CHECK(styledText);
  DALI_TEST_EQUALS(styledText.GetText(), "Hello", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 5u);

  ForegroundColorSpan span = ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u));
  DALI_TEST_CHECK(span);
  DALI_TEST_EQUALS(span.GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupAnnotationP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::FromMarkup("<annotation style='gradient' role='link'>hello world</annotation>");

  DALI_TEST_EQUALS(styledText.GetText(), "hello world", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationCount(), 2u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 11u);
  CheckRange(styledText, 1u, 0u, 11u);

  AnnotationSpan styleAnnotation = AnnotationSpan::DownCast(styledText.GetSpanAt(0u));
  AnnotationSpan roleAnnotation  = AnnotationSpan::DownCast(styledText.GetSpanAt(1u));
  DALI_TEST_CHECK(styleAnnotation);
  DALI_TEST_CHECK(roleAnnotation);
  DALI_TEST_EQUALS(styleAnnotation.GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(styleAnnotation.GetValue(), "gradient", TEST_LOCATION);
  DALI_TEST_EQUALS(roleAnnotation.GetKey(), "role", TEST_LOCATION);
  DALI_TEST_EQUALS(roleAnnotation.GetValue(), "link", TEST_LOCATION);
  CheckSpanIdentity(styledText.GetAnnotationAt(0u), styleAnnotation);
  CheckSpanIdentity(styledText.GetAnnotationAt(1u), roleAnnotation);
  DALI_TEST_EQUALS(styledText.GetAnnotationStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationEndIndexAt(0u), 11u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationStartIndexAt(1u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetAnnotationEndIndexAt(1u), 11u, TEST_LOCATION);

  StyledText unicodeText = StyledText::FromMarkup("<annotation style='gradient' role='emoji'>가😀</annotation>");

  DALI_TEST_EQUALS(unicodeText.GetText(), "가😀", TEST_LOCATION);
  DALI_TEST_EQUALS(unicodeText.GetUtf32Length(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(unicodeText.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(unicodeText.GetAnnotationCount(), 2u, TEST_LOCATION);
  CheckRange(unicodeText, 0u, 0u, 2u);
  CheckRange(unicodeText, 1u, 0u, 2u);
  DALI_TEST_EQUALS(unicodeText.GetAnnotationAt(0u).GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(unicodeText.GetAnnotationAt(0u).GetValue(), "gradient", TEST_LOCATION);
  DALI_TEST_EQUALS(unicodeText.GetAnnotationAt(1u).GetKey(), "role", TEST_LOCATION);
  DALI_TEST_EQUALS(unicodeText.GetAnnotationAt(1u).GetValue(), "emoji", TEST_LOCATION);

  StyledText valueText = StyledText::FromMarkup("<annotation value='semantic'>가😀</annotation>");

  DALI_TEST_EQUALS(valueText.GetText(), "가😀", TEST_LOCATION);
  DALI_TEST_EQUALS(valueText.GetUtf32Length(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(valueText.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(valueText.GetAnnotationCount(), 1u, TEST_LOCATION);
  CheckRange(valueText, 0u, 0u, 2u);

  AnnotationSpan valueAnnotation = AnnotationSpan::DownCast(valueText.GetSpanAt(0u));
  DALI_TEST_CHECK(valueAnnotation);
  DALI_TEST_EQUALS(valueAnnotation.GetKey(), "value", TEST_LOCATION);
  DALI_TEST_EQUALS(valueAnnotation.GetValue(), "semantic", TEST_LOCATION);
  CheckSpanIdentity(valueText.GetAnnotationAt(0u), valueAnnotation);
  DALI_TEST_EQUALS(valueText.GetAnnotationStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(valueText.GetAnnotationEndIndexAt(0u), 2u, TEST_LOCATION);

  StyledText noAttribute = StyledText::FromMarkup("<annotation>plain</annotation>");
  DALI_TEST_EQUALS(noAttribute.GetText(), "plain", TEST_LOCATION);
  DALI_TEST_EQUALS(noAttribute.GetSpanCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(noAttribute.GetAnnotationCount(), 0u, TEST_LOCATION);

  StyledText unclosed = StyledText::FromMarkup("<annotation value='tail'>TV");
  DALI_TEST_EQUALS(unclosed.GetText(), "TV", TEST_LOCATION);
  DALI_TEST_EQUALS(unclosed.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(unclosed.GetAnnotationAt(0u).GetKey(), "value", TEST_LOCATION);
  DALI_TEST_EQUALS(unclosed.GetAnnotationAt(0u).GetValue(), "tail", TEST_LOCATION);
  DALI_TEST_EQUALS(unclosed.GetAnnotationStartIndexAt(0u), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(unclosed.GetAnnotationEndIndexAt(0u), 2u, TEST_LOCATION);

  StyledText malformed = StyledText::FromMarkup("<unknown>plain</unknown><annotation value='x'>ok</annotation>");
  DALI_TEST_EQUALS(malformed.GetText(), "plainok", TEST_LOCATION);
  DALI_TEST_EQUALS(malformed.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(malformed.GetAnnotationAt(0u).GetKey(), "value", TEST_LOCATION);
  DALI_TEST_EQUALS(malformed.GetAnnotationAt(0u).GetValue(), "x", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupAlphaHexColorP(void)
{
  UiTestApplication application;

  const char* markups[] =
  {
    "<color value='#80FF0000'>A</color>",
    "<color value='0x80FF0000'>A</color>",
  };

  for(const char* markup : markups)
  {
    StyledText styledText = StyledText::FromMarkup(markup);
    DALI_TEST_EQUALS(styledText.GetText(), "A", TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
    CheckRange(styledText, 0u, 0u, 1u);
    CheckSemiTransparentRedColor(ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor());
  }

  END_TEST;
}

int UtcDaliStyledTextFromMarkupAttributeQuotesP(void)
{
  UiTestApplication application;

  const char* markups[] =
  {
    "<color value='red'>A</color>",
    "<color value=\"red\">A</color>",
    "<color value=red>A</color>",
  };

  for(const char* markup : markups)
  {
    StyledText styledText = StyledText::FromMarkup(markup);
    DALI_TEST_EQUALS(styledText.GetText(), "A", TEST_LOCATION);
    DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
    CheckRange(styledText, 0u, 0u, 1u);
    DALI_TEST_EQUALS(ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor().GetRgba(), Color::RED, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliStyledTextFromMarkupRawSymbolsAndTextEntitiesP(void)
{
  UiTestApplication application;

  StyledText rawText = StyledText::FromMarkup("1 < 2 && 3 > 2");
  DALI_TEST_EQUALS(rawText.GetText(), "1 < 2 && 3 > 2", TEST_LOCATION);
  DALI_TEST_EQUALS(rawText.GetSpanCount(), 0u, TEST_LOCATION);

  StyledText literalText = StyledText::FromMarkup("1 < 2");
  DALI_TEST_EQUALS(literalText.GetText(), "1 < 2", TEST_LOCATION);
  DALI_TEST_EQUALS(literalText.GetSpanCount(), 0u, TEST_LOCATION);

  StyledText simpleEntityText = StyledText::FromMarkup("A &amp; B");
  DALI_TEST_EQUALS(simpleEntityText.GetText(), "A & B", TEST_LOCATION);
  DALI_TEST_EQUALS(simpleEntityText.GetSpanCount(), 0u, TEST_LOCATION);

  StyledText entityText = StyledText::FromMarkup("A &lt; B &amp;&amp; C &gt; D &#60; &#x3C;");
  DALI_TEST_EQUALS(entityText.GetText(), "A < B && C > D < <", TEST_LOCATION);
  DALI_TEST_EQUALS(entityText.GetSpanCount(), 0u, TEST_LOCATION);

  StyledText unknownEntityText = StyledText::FromMarkup("A &unknown; B & C");
  DALI_TEST_EQUALS(unknownEntityText.GetText(), "A &unknown; B & C", TEST_LOCATION);
  DALI_TEST_EQUALS(unknownEntityText.GetSpanCount(), 0u, TEST_LOCATION);

  StyledText entityRecoveryText = StyledText::FromMarkup("A & B &amp; C");
  DALI_TEST_EQUALS(entityRecoveryText.GetText(), "A & B & C", TEST_LOCATION);
  DALI_TEST_EQUALS(entityRecoveryText.GetSpanCount(), 0u, TEST_LOCATION);

  StyledText unknownThenEntityText = StyledText::FromMarkup("&unknown;&amp;");
  DALI_TEST_EQUALS(unknownThenEntityText.GetText(), "&unknown;&", TEST_LOCATION);
  DALI_TEST_EQUALS(unknownThenEntityText.GetSpanCount(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupNestedColorP(void)
{
  UiTestApplication application;

  StyledText textRunStyledText = StyledText::FromMarkup("abc<color value='red'>def</color>ghi");

  DALI_TEST_EQUALS(textRunStyledText.GetText(), "abcdefghi", TEST_LOCATION);
  DALI_TEST_EQUALS(textRunStyledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(textRunStyledText, 0u, 3u, 6u);

  StyledText styledText = StyledText::FromMarkup("<color value='red'>a<color value='blue'>b</color>c</color>");

  DALI_TEST_EQUALS(styledText.GetText(), "abc", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 2u, TEST_LOCATION);

  CheckRange(styledText, 0u, 0u, 3u);
  DALI_TEST_EQUALS(ForegroundColorSpan::DownCast(styledText.GetSpanAt(0u)).GetColor().GetRgba(), Color::RED, TEST_LOCATION);

  CheckRange(styledText, 1u, 1u, 2u);
  DALI_TEST_EQUALS(ForegroundColorSpan::DownCast(styledText.GetSpanAt(1u)).GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupNestedFontOrderP(void)
{
  UiTestApplication application;

  StyledText normalWins = StyledText::FromMarkup("<b><font weight='normal'>x</font></b>");
  DALI_TEST_EQUALS(normalWins.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(normalWins.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(FontSpan::DownCast(normalWins.GetSpanAt(0u)).GetFontAttributes().GetWeight(), FontWeight::BOLD, TEST_LOCATION);
  DALI_TEST_EQUALS(FontSpan::DownCast(normalWins.GetSpanAt(1u)).GetFontAttributes().GetWeight(), FontWeight::NORMAL, TEST_LOCATION);

  StyledText boldWins = StyledText::FromMarkup("<font weight='normal'><b>x</b></font>");
  DALI_TEST_EQUALS(boldWins.GetText(), "x", TEST_LOCATION);
  DALI_TEST_EQUALS(boldWins.GetSpanCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(FontSpan::DownCast(boldWins.GetSpanAt(0u)).GetFontAttributes().GetWeight(), FontWeight::NORMAL, TEST_LOCATION);
  DALI_TEST_EQUALS(FontSpan::DownCast(boldWins.GetSpanAt(1u)).GetFontAttributes().GetWeight(), FontWeight::BOLD, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupDecorationsAndBackgroundP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::FromMarkup("<u color='green' height='2.0f' type='dashed' dash-gap='3' dash-width='4'>under</u><s color='blue' height='5'>strike</s><background color='yellow'>bg</background>");

  DALI_TEST_EQUALS(styledText.GetText(), "understrikebg", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 3u, TEST_LOCATION);

  CheckRange(styledText, 0u, 0u, 5u);
  Underline underline = UnderlineSpan::DownCast(styledText.GetSpanAt(0u)).GetUnderline();
  DALI_TEST_EQUALS(underline.GetColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(underline.GetThickness(), 2.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underline.GetType(), Underline::Type::DASHED, TEST_LOCATION);
  DALI_TEST_EQUALS(underline.GetDashGap(), 3.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(underline.GetDashLength(), 4.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  CheckRange(styledText, 1u, 5u, 11u);
  LineThrough lineThrough = LineThroughSpan::DownCast(styledText.GetSpanAt(1u)).GetLineThrough();
  DALI_TEST_EQUALS(lineThrough.GetColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_EQUALS(lineThrough.GetThickness(), 5.0f, Math::MACHINE_EPSILON_1000, TEST_LOCATION);

  CheckRange(styledText, 2u, 11u, 13u);
  DALI_TEST_EQUALS(BackgroundColorSpan::DownCast(styledText.GetSpanAt(2u)).GetColor().GetRgba(), Color::YELLOW, TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupAnchorP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::FromMarkup("<a href='https://example.com?a=1&amp;b=2' color='red' clicked-color='blue'>link</a>");

  DALI_TEST_EQUALS(styledText.GetText(), "link", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 4u);

  AnchorSpan anchorSpan = AnchorSpan::DownCast(styledText.GetSpanAt(0u));
  DALI_TEST_CHECK(anchorSpan);

  AnchorAttributes attributes = anchorSpan.GetAnchorAttributes();
  DALI_TEST_CHECK(attributes.Has(AnchorAttributes::Attribute::HREF));
  DALI_TEST_CHECK(attributes.Has(AnchorAttributes::Attribute::COLOR));
  DALI_TEST_CHECK(attributes.Has(AnchorAttributes::Attribute::CLICKED_COLOR));
  DALI_TEST_EQUALS(attributes.GetHref(), "https://example.com?a=1&b=2", TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(attributes.GetClickedColor().GetRgba(), Color::BLUE, TEST_LOCATION);

  StyledText quoteEntity = StyledText::FromMarkup("<a href='a&quot;b&apos;c'>x</a>");
  AnchorAttributes quoteAttributes = AnchorSpan::DownCast(quoteEntity.GetSpanAt(0u)).GetAnchorAttributes();
  DALI_TEST_EQUALS(quoteAttributes.GetHref(), "a\"b'c", TEST_LOCATION);

  StyledText emptyHref = StyledText::FromMarkup("<a href=''>x</a>");
  DALI_TEST_EQUALS(emptyHref.GetSpanCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(AnchorSpan::DownCast(emptyHref.GetSpanAt(0u)).GetAnchorAttributes().GetHref(), "", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupUnquotedAnchorHrefP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::FromMarkup("<a href=https://example.com?a=1&amp;b=2>link</a>");

  DALI_TEST_EQUALS(styledText.GetText(), "link", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 4u);

  AnchorSpan anchorSpan = AnchorSpan::DownCast(styledText.GetSpanAt(0u));
  DALI_TEST_CHECK(anchorSpan);
  DALI_TEST_EQUALS(anchorSpan.GetAnchorAttributes().GetHref(), "https://example.com?a=1&b=2", TEST_LOCATION);

  END_TEST;
}

int UtcDaliStyledTextFromMarkupUnicodeRangeP(void)
{
  UiTestApplication application;

  StyledText styledText = StyledText::FromMarkup("<color value='red'>가😀B</color>");

  DALI_TEST_EQUALS(styledText.GetText(), "가😀B", TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetUtf32Length(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(styledText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(styledText, 0u, 0u, 3u);

  StyledText entityText = StyledText::FromMarkup("<u>&lt;가😀</u>");

  DALI_TEST_EQUALS(entityText.GetText(), "<가😀", TEST_LOCATION);
  DALI_TEST_EQUALS(entityText.GetUtf32Length(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(entityText.GetSpanCount(), 1u, TEST_LOCATION);
  CheckRange(entityText, 0u, 0u, 3u);
  DALI_TEST_CHECK(UnderlineSpan::DownCast(entityText.GetSpanAt(0u)));

  StyledText annotationText = StyledText::FromMarkup("가😀<annotation style='x'>나</annotation>다");

  DALI_TEST_EQUALS(annotationText.GetText(), "가😀나다", TEST_LOCATION);
  DALI_TEST_EQUALS(annotationText.GetUtf32Length(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationText.GetAnnotationCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationText.GetAnnotationStartIndexAt(0u), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationText.GetAnnotationEndIndexAt(0u), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(annotationText.GetAnnotationAt(0u).GetKey(), "style", TEST_LOCATION);
  DALI_TEST_EQUALS(annotationText.GetAnnotationAt(0u).GetValue(), "x", TEST_LOCATION);

  END_TEST;
}
