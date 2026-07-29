/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
 */

// EXTERNAL INCLUDES
#include <array>
#include <cstdint>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int   WINDOW_WIDTH       = 980;
constexpr int   WINDOW_HEIGHT      = 820;
constexpr std::size_t CASE_COUNT    = 14u;
constexpr float PAGE_PADDING       = 28.0f;
constexpr float PAGE_SPACING       = 12.0f;
constexpr float TITLE_FONT_SIZE    = 25.0f;
constexpr float CASE_FONT_SIZE     = 22.0f;
constexpr float BODY_FONT_SIZE     = 18.0f;
constexpr float RESULT_FONT_SIZE   = 30.0f;
constexpr float FOOTER_FONT_SIZE   = 16.0f;
constexpr uint32_t PAGE_BACKGROUND = 0xF8FAFC;
constexpr uint32_t TEXT_PRIMARY    = 0x0F172A;
constexpr uint32_t TEXT_SECONDARY  = 0x334155;
constexpr uint32_t TEXT_MUTED      = 0x64748B;
constexpr uint32_t RED             = 0xDC2626;
constexpr uint32_t BLUE            = 0x2563EB;
constexpr uint32_t GREEN           = 0x059669;
constexpr uint32_t PURPLE          = 0x7C3AED;
constexpr uint32_t AMBER           = 0xD97706;
constexpr uint32_t BG_SOFT_RED     = 0xFEE2E2;
constexpr uint32_t BG_SOFT_BLUE    = 0xDBEAFE;
constexpr uint32_t BG_SOFT_PURPLE  = 0xF3E8FF;
constexpr uint32_t BG_SOFT_GREEN   = 0xDCFCE7;
constexpr uint32_t BG_SOFT_AMBER   = 0xFEF3C7;
constexpr uint32_t BG_SOFT_CYAN    = 0xCFFAFE;

struct ExampleCase
{
  std::string      title;
  std::string      description;
  std::string      sourceText;
  std::string      stats;
  Text::StyledText result;
};

uint32_t Utf32LengthOf(const std::string& text)
{
  return Text::Utf8ToUtf32Length(Dali::StringView(text.data(), static_cast<uint32_t>(text.size())));
}

std::string RangeText(uint32_t startIndex, uint32_t endIndex)
{
  return "[" + std::to_string(startIndex) + "," + std::to_string(endIndex) + ")";
}

std::string BoolText(bool value)
{
  return value ? "true" : "false";
}

Text::ForegroundColorSpan NewForegroundSpan(uint32_t color)
{
  return Text::ForegroundColorSpan::New(UiColor(color));
}

Text::BackgroundColorSpan NewBackgroundSpan(uint32_t color)
{
  return Text::BackgroundColorSpan::New(UiColor(color));
}

Text::UnderlineSpan NewUnderlineSpan(uint32_t color)
{
  Text::Underline underline;
  underline.SetColor(UiColor(color));
  underline.SetThickness(2.0f);
  underline.SetType(Text::Underline::Type::SOLID);
  return Text::UnderlineSpan::New(underline);
}

Label NewTextLabel(float fontSize, uint32_t color, bool multiline = true)
{
  Label label = Label::New();
  label.SetFontFamily("SamsungOneUI_400");
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  label.SetMultiLine(multiline);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  return label;
}

Text::StyledText BuildFromMarkupOneShotStyledText()
{
  return Text::StyledText::FromMarkup("<color value='red'>Hello</color> StyledText");
}

ExampleCase BuildFromMarkupOneShotCase()
{
  const char* markup = "<color value='red'>Hello</color> StyledText";

  ExampleCase data;
  data.title       = "Case 1: StyledText::FromMarkup one-shot";
  data.description = "Start here when a resource or constant string already uses supported DALi markup.";
  data.sourceText  = markup;
  data.result      = BuildFromMarkupOneShotStyledText();
  data.stats       = "API: Text::StyledText::FromMarkup()\nNo builder is needed for one-shot markup conversion."
               "\nSpan count: " + std::to_string(data.result.GetSpanCount());
  return data;
}

Text::StyledText BuildFromMarkupThenSetSpanStyledText()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup("<u>Hello</u> StyledText");
  builder.SetSpan(NewForegroundSpan(RED), 0u, 5u);
  return builder.Build();
}

ExampleCase BuildFromMarkupThenSetSpanCase()
{
  const char* markup = "<u>Hello</u> StyledText";

  Text::StyledText result = BuildFromMarkupThenSetSpanStyledText();

  ExampleCase data;
  data.title       = "Case 2: StyledTextBuilder::FromMarkup then SetSpan";
  data.description = "Parse markup into a mutable builder, then add an app-defined span before Build().";
  data.sourceText  = markup;
  data.result      = result;
  data.stats       = "Builder source: StyledTextBuilder::FromMarkup()\nExtra foreground range: " + RangeText(0u, 5u) +
               "\nSpan count: " + std::to_string(result.GetSpanCount());
  return data;
}

Text::StyledText BuildPushPopSimpleStyledText()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

  builder.AppendText("Normal ");
  builder.PushSpan(NewForegroundSpan(RED));
  builder.AppendText("Red");
  builder.PopSpan();
  builder.AppendText(" Normal");

  return builder.Build();
}

ExampleCase BuildPushPopSimpleCase()
{
  Text::StyledText result = BuildPushPopSimpleStyledText();

  ExampleCase data;
  data.title       = "Case 3: PushSpan / PopSpan simple";
  data.description = "Recommended builder pattern: append text while a span is open, then close the most recent open span.";
  data.sourceText  = "Normal Red Normal";
  data.result      = result;
  data.stats       = "Manual range indexes: none\nPopSpan() closes the most recently pushed span."
               "\nSpan count: " + std::to_string(result.GetSpanCount());
  return data;
}

Text::StyledText BuildNestedPushPopStyledText()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

  builder.AppendText("Normal ");

  builder.PushSpan(NewForegroundSpan(RED));
  builder.AppendText("Red ");

  builder.PushSpan(NewUnderlineSpan(AMBER));
  builder.AppendText("Red Underline");
  builder.PopSpan();

  builder.AppendText(" Red");
  builder.PopSpan();

  builder.AppendText(" Normal");

  return builder.Build();
}

ExampleCase BuildNestedPushPopCase()
{
  Text::StyledText result = BuildNestedPushPopStyledText();

  ExampleCase data;
  data.title       = "Case 4: Nested PushSpan / PopSpan";
  data.description = "Nested styles can be expressed naturally with the open span stack and no explicit range math.";
  data.sourceText  = "Normal Red Red Underline Red Normal";
  data.result      = result;
  data.stats       = "Manual range indexes: none\nInner underline is popped first; outer foreground stays open."
               "\nSpan count: " + std::to_string(result.GetSpanCount());
  return data;
}

Text::StyledText BuildPushPopTokenStyledText()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

  builder.AppendText("Normal ");

  const uint32_t highlightToken = builder.PushSpan(NewBackgroundSpan(BG_SOFT_AMBER));
  builder.AppendText("Highlighted ");

  builder.PushSpan(NewUnderlineSpan(AMBER));
  builder.AppendText("Highlighted Underline");

  builder.PopSpan(highlightToken);
  builder.AppendText(" Normal");

  return builder.Build();
}

ExampleCase BuildPushPopTokenCase()
{
  Text::StyledText result = BuildPushPopTokenStyledText();

  ExampleCase data;
  data.title       = "Case 5: PushSpan token / PopSpan(token)";
  data.description = "Keep a token for a block span when nested pushes may happen before the block ends.";
  data.sourceText  = "Normal Highlighted Highlighted Underline Normal";
  data.result      = result;
  data.stats       = "PopSpan(token) closes the matching span and any spans above it."
               "\nThis closes underline and background together at the block boundary."
               "\nSpan count: " + std::to_string(result.GetSpanCount());
  return data;
}

Text::StyledText BuildNonMutatingBuildStyledText()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

  const uint32_t token = builder.PushSpan(NewForegroundSpan(BLUE));

  builder.AppendText("First snapshot");
  builder.Build(); // Creates a snapshot without closing the open span in the builder.

  builder.AppendText(" continues");
  Text::StyledText result = builder.Build();

  builder.PopSpan(token);
  return result;
}

ExampleCase BuildNonMutatingBuildCase()
{
  Text::StyledText result = BuildNonMutatingBuildStyledText();

  ExampleCase data;
  data.title       = "Case 6: Build() does not close open spans";
  data.description = "Build() creates a snapshot. It does not mutate the builder or pop open spans.";
  data.sourceText  = "First snapshot continues";
  data.result      = result;
  data.stats       = "The helper calls Build(), appends more text, then calls Build() again."
               "\nThe open blue span is still active for the second snapshot."
               "\nSpan count: " + std::to_string(result.GetSpanCount());
  return data;
}

Text::StyledText BuildAnnotationResolveStyledText()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(
    "<annotation style='muted' role='summary'>Muted</annotation> Normal "
    "<annotation style='accent' role='action' background='soft'>Accent</annotation>");

  const uint32_t annotationCount = builder.GetAnnotationCount();
  for(uint32_t annotationIndex = 0u; annotationIndex < annotationCount; ++annotationIndex)
  {
    const Text::AnnotationSpan annotation = builder.GetAnnotationAt(annotationIndex);
    const uint32_t             startIndex = builder.GetAnnotationStartIndexAt(annotationIndex);
    const uint32_t             endIndex   = builder.GetAnnotationEndIndexAt(annotationIndex);

    if(annotation.GetKey() == "style")
    {
      if(annotation.GetValue() == "muted")
      {
        builder.SetSpan(NewForegroundSpan(TEXT_MUTED), startIndex, endIndex);
      }
      else if(annotation.GetValue() == "accent")
      {
        builder.SetSpan(NewForegroundSpan(PURPLE), startIndex, endIndex);
      }
    }
    else if(annotation.GetKey() == "background")
    {
      if(annotation.GetValue() == "soft")
      {
        builder.SetSpan(NewBackgroundSpan(BG_SOFT_PURPLE), startIndex, endIndex);
      }
    }
  }

  return builder.Build();
}

ExampleCase BuildAnnotationResolveCase()
{
  const char* markup =
    "<annotation style='muted' role='summary'>Muted</annotation> Normal "
    "<annotation style='accent' role='action' background='soft'>Accent</annotation>";

  Text::StyledText result = BuildAnnotationResolveStyledText();

  ExampleCase data;
  data.title       = "Case 7: Resolve annotation markup";
  data.description = "Annotation markup is semantic metadata. App or theme code resolves key/value pairs into concrete visual spans.";
  data.sourceText  = markup;
  data.result      = result;
  data.stats       = "Each attribute in one annotation tag becomes a separate AnnotationSpan."
               "\nAnnotation loop caches GetAnnotationCount() before the loop."
               "\nEach matching visual range receives a new span instance."
               "\nAnnotation count: " + std::to_string(result.GetAnnotationCount()) +
               "\nSpan count after resolve: " + std::to_string(result.GetSpanCount());
  return data;
}

Text::StyledText BuildDirectRangeStyledText()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New("Hello StyledText");
  builder.SetSpan(NewForegroundSpan(GREEN), 0u, 5u);
  builder.SetSpan(NewBackgroundSpan(BG_SOFT_BLUE), 6u, builder.GetUtf32Length());
  return builder.Build();
}

ExampleCase BuildDirectRangeCase()
{
  const Dali::String text("Hello StyledText");
  Text::StyledText   result = BuildDirectRangeStyledText();

  ExampleCase data;
  data.title       = "Case 8: Direct SetSpan with known UTF-32 range";
  data.description = "Use SetSpan() directly when the UTF-32 range is already known.";
  data.sourceText  = text.CStr();
  data.result      = result;
  data.stats       = "Foreground range for Hello: " + RangeText(0u, 5u) +
               "\nBackground range for StyledText: " + RangeText(6u, result.GetUtf32Length()) +
               "\nSpan count: " + std::to_string(result.GetSpanCount());
  return data;
}

Text::StyledText BuildUtf8ToUtf32RangeStyledText()
{
  const std::string prefix = "Hello ";
  const std::string target = "가😀";
  const std::string suffix = " StyledText";
  const std::string text   = prefix + target + suffix;

  const uint32_t startIndex = Utf32LengthOf(prefix);
  const uint32_t endIndex   = startIndex + Utf32LengthOf(target);

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text.c_str());
  builder.SetSpan(NewForegroundSpan(RED), startIndex, endIndex);
  builder.SetSpan(NewUnderlineSpan(RED), startIndex, endIndex);

  return builder.Build();
}

ExampleCase BuildUtf8ToUtf32RangeCase()
{
  const std::string prefix = "Hello ";
  const std::string target = "가😀";
  const std::string suffix = " StyledText";
  const std::string text   = prefix + target + suffix;

  const uint32_t startIndex = Utf32LengthOf(prefix);
  const uint32_t endIndex   = startIndex + Utf32LengthOf(target);

  Text::StyledText result = BuildUtf8ToUtf32RangeStyledText();

  ExampleCase data;
  data.title       = "Case 9: UTF-8 text with computed UTF-32 range";
  data.description = "Advanced case: compute UTF-32 range indexes from UTF-8 string parts before calling SetSpan().";
  data.sourceText  = text;
  data.result      = result;
  data.stats       = "Target: " + target +
               "\nPrefix UTF-8 bytes: " + std::to_string(prefix.size()) +
               "\nTarget UTF-8 bytes: " + std::to_string(target.size()) +
               "\nTarget UTF-32 range: " + RangeText(startIndex, endIndex) +
               "\nSpan count: " + std::to_string(result.GetSpanCount());
  return data;
}

ExampleCase BuildUtf8FindCase()
{
  const std::string text   = "오늘은 가나다 입니다";
  const std::string target = "가나다";

  const std::size_t found       = text.find(target);
  const bool        targetFound = (found != std::string::npos);
  bool              converted   = false;
  uint32_t          utf8StartIndex  = 0u;
  uint32_t          utf8EndIndex    = 0u;
  uint32_t          utf32StartIndex = 0u;
  uint32_t          utf32EndIndex   = 0u;

  if(targetFound)
  {
    utf8StartIndex = static_cast<uint32_t>(found);
    utf8EndIndex   = utf8StartIndex + static_cast<uint32_t>(target.size());

    converted = Text::Utf8ToUtf32Range(Dali::StringView(text.data(), static_cast<uint32_t>(text.size())),
                                       utf8StartIndex,
                                       utf8EndIndex,
                                       utf32StartIndex,
                                       utf32EndIndex);
  }

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text.c_str());
  if(converted)
  {
    builder.SetSpan(NewForegroundSpan(RED), utf32StartIndex, utf32EndIndex);
    builder.SetSpan(NewUnderlineSpan(RED), utf32StartIndex, utf32EndIndex);
  }

  ExampleCase data;
  data.title       = "Case 10: UTF-8 string find converted to UTF-32 range";
  data.description = "std::string::find() returns a UTF-8 byte range. Convert it before passing the range to SetSpan().";
  data.sourceText  = text;
  data.stats       = "find target: " + target +
               "\ntarget found: " + BoolText(targetFound) +
               "\nconverted: " + BoolText(converted) +
               "\nUTF-8 byte range: " + RangeText(utf8StartIndex, utf8EndIndex) +
               "\nUTF-32 range: " + RangeText(utf32StartIndex, utf32EndIndex);
  data.result = builder.Build();
  return data;
}

ExampleCase BuildUtf32BackToUtf8Case()
{
  const std::string text   = "Back to UTF-8: A👩‍💻B";
  const std::string target = "👩‍💻";

  const std::size_t found       = text.find(target);
  const bool        targetFound = (found != std::string::npos);
  bool              forwardConverted  = false;
  uint32_t          utf8StartFromFind  = 0u;
  uint32_t          utf8EndFromFind    = 0u;
  uint32_t          utf32StartIndex    = 0u;
  uint32_t          utf32EndIndex      = 0u;

  if(targetFound)
  {
    utf8StartFromFind = static_cast<uint32_t>(found);
    utf8EndFromFind   = utf8StartFromFind + static_cast<uint32_t>(target.size());

    forwardConverted = Text::Utf8ToUtf32Range(Dali::StringView(text.data(), static_cast<uint32_t>(text.size())),
                                              utf8StartFromFind,
                                              utf8EndFromFind,
                                              utf32StartIndex,
                                              utf32EndIndex);
  }

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text.c_str());
  if(forwardConverted)
  {
    builder.SetSpan(NewForegroundSpan(BLUE), utf32StartIndex, utf32EndIndex);
  }
  Text::StyledText styledText = builder.Build();

  bool     backwardConverted     = false;
  uint32_t storedUtf32StartIndex = 0u;
  uint32_t storedUtf32EndIndex   = 0u;
  uint32_t utf8StartIndex        = 0u;
  uint32_t utf8EndIndex          = 0u;

  if(styledText.GetSpanCount() > 0u)
  {
    storedUtf32StartIndex = styledText.GetSpanStartIndexAt(0u);
    storedUtf32EndIndex   = styledText.GetSpanEndIndexAt(0u);

    backwardConverted = Text::Utf32ToUtf8Range(styledText.GetText(),
                                               storedUtf32StartIndex,
                                               storedUtf32EndIndex,
                                               utf8StartIndex,
                                               utf8EndIndex);
  }

  const std::string extracted = backwardConverted ? text.substr(utf8StartIndex, utf8EndIndex - utf8StartIndex) : "";

  ExampleCase data;
  data.title       = "Case 11: Stored UTF-32 range converted back to UTF-8";
  data.description = "A stored span range is UTF-32. Convert it back when you need a UTF-8 substring range.";
  data.sourceText  = text;
  data.stats       = "target found: " + BoolText(targetFound) +
               "\nfind UTF-8 byte range: " + RangeText(utf8StartFromFind, utf8EndFromFind) +
               "\nforward converted: " + BoolText(forwardConverted) +
               "\nSetSpan UTF-32 range: " + RangeText(utf32StartIndex, utf32EndIndex) +
               "\nStored UTF-32 range: " + RangeText(storedUtf32StartIndex, storedUtf32EndIndex) +
               "\nbackward converted: " + BoolText(backwardConverted) +
               "\nConverted UTF-8 byte range: " + RangeText(utf8StartIndex, utf8EndIndex) +
               "\nExtracted UTF-8 text: " + extracted;
  data.result = styledText;
  return data;
}

ExampleCase BuildComplexUnicodeCase()
{
  struct Row
  {
    std::string text;
    uint32_t    color;
  };

  const std::array<Row, 6u> rows{{
    {"가나다", BG_SOFT_RED},
    {"😀", BG_SOFT_BLUE},
    {"❤️", BG_SOFT_PURPLE},
    {"👩‍💻", BG_SOFT_GREEN},
    {"🇰🇷", BG_SOFT_AMBER},
    {"AéB", BG_SOFT_CYAN},
  }};

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
  std::string             stats("Text | UTF-8 bytes | UTF-32 length | selected UTF-32 range");
  std::string             sourceText;

  for(std::size_t rowIndex = 0u; rowIndex < rows.size(); ++rowIndex)
  {
    if(rowIndex > 0u)
    {
      builder.AppendText("   ");
      sourceText += "   ";
    }

    const uint32_t startIndex = builder.GetUtf32Length();
    builder.AppendText(rows[rowIndex].text.c_str());
    const uint32_t endIndex = builder.GetUtf32Length();
    builder.SetSpan(NewBackgroundSpan(rows[rowIndex].color), startIndex, endIndex);

    sourceText += rows[rowIndex].text;
    stats += "\n" + rows[rowIndex].text +
             " | " + std::to_string(rows[rowIndex].text.size()) +
             " | " + std::to_string(Utf32LengthOf(rows[rowIndex].text)) +
             " | " + RangeText(startIndex, endIndex);
  }

  ExampleCase data;
  data.title       = "Case 12: Complex Unicode ranges";
  data.description = "UTF-8 byte size and UTF-32 length can differ, including emoji sequences that look compact on screen.";
  data.sourceText  = sourceText;
  data.stats       = stats;
  data.result      = builder.Build();
  return data;
}

ExampleCase BuildBoundaryFailureCase()
{
  const Dali::String text("A가B");

  uint32_t indexOutputBefore = 123u;
  uint32_t indexOutput       = indexOutputBefore;
  const bool indexConverted  = Text::Utf8ToUtf32Index(text, 2u, indexOutput);

  uint32_t rangeStartBefore = 456u;
  uint32_t rangeEndBefore   = 789u;
  uint32_t rangeStart       = rangeStartBefore;
  uint32_t rangeEnd         = rangeEndBefore;
  const bool rangeConverted = Text::Utf8ToUtf32Range(text, 2u, 4u, rangeStart, rangeEnd);

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text);
  builder.SetSpan(NewForegroundSpan(GREEN), 1u, 2u);

  ExampleCase data;
  data.title       = "Case 13: Invalid UTF-8 boundary conversion";
  data.description = "A UTF-8 byte index inside a multi-byte sequence is not a valid conversion boundary.";
  data.sourceText  = text.CStr();
  data.stats       = "Utf8ToUtf32Index(text, 2): " + BoolText(indexConverted) +
               "\nIndex output unchanged: " + BoolText(indexOutput == indexOutputBefore) +
               "\nUtf8ToUtf32Range(text, 2, 4): " + BoolText(rangeConverted) +
               "\nRange output unchanged: " + BoolText((rangeStart == rangeStartBefore) && (rangeEnd == rangeEndBefore)) +
               "\nValid span for 가 uses UTF-32 range " + RangeText(1u, 2u);
  data.result = builder.Build();
  return data;
}

ExampleCase BuildImageSpanCase()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();
  builder.AppendText("Before ");

  const uint32_t imageIndex = builder.GetUtf32Length();
  builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);

  builder.AppendText(" after");

  Text::ImageAttributes imageAttributes(RESOURCES_DIR "flag_kr.png", Vector2(64.0f, 40.0f));
  imageAttributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);

  Text::ImageSpan imageSpan = Text::ImageSpan::New(imageAttributes);
  builder.SetSpan(imageSpan, imageIndex, imageIndex + 1u);

  ExampleCase data;
  data.title       = "Case 14: Recommended ImageSpan authoring";
  data.description = "Append one U+FFFC, remember its UTF-32 index, then attach one ImageSpan to [index,index+1).";
  data.sourceText  = "Before [U+FFFC] after";
  data.stats       = "Canonical source unit: U+FFFC OBJECT REPLACEMENT CHARACTER"
               "\nImageSpan UTF-32 range: " + RangeText(imageIndex, imageIndex + 1u) +
               "\nReserved size: 64x40 logical pixels"
               "\nSpan count: 1";
  data.result = builder.Build();
  return data;
}

ExampleCase BuildCase(std::size_t index)
{
  switch(index)
  {
    case 0u:
      return BuildFromMarkupOneShotCase();
    case 1u:
      return BuildFromMarkupThenSetSpanCase();
    case 2u:
      return BuildPushPopSimpleCase();
    case 3u:
      return BuildNestedPushPopCase();
    case 4u:
      return BuildPushPopTokenCase();
    case 5u:
      return BuildNonMutatingBuildCase();
    case 6u:
      return BuildAnnotationResolveCase();
    case 7u:
      return BuildDirectRangeCase();
    case 8u:
      return BuildUtf8ToUtf32RangeCase();
    case 9u:
      return BuildUtf8FindCase();
    case 10u:
      return BuildUtf32BackToUtf8Case();
    case 11u:
      return BuildComplexUnicodeCase();
    case 12u:
      return BuildBoundaryFailureCase();
    default:
      return BuildImageSpanCase();
  }
}

} // namespace

class TextStyledTextBuilderController : public ConnectionTracker
{
public:
  explicit TextStyledTextBuilderController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextStyledTextBuilderController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(PAGE_BACKGROUND));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(PAGE_SPACING);
    root.SetPadding(Extents(static_cast<int16_t>(PAGE_PADDING), static_cast<int16_t>(PAGE_PADDING), static_cast<int16_t>(PAGE_PADDING), static_cast<int16_t>(PAGE_PADDING)));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(PAGE_BACKGROUND));

    mTitleLabel = NewTextLabel(TITLE_FONT_SIZE, TEXT_PRIMARY, false);
    mTitleLabel.SetText("StyledTextBuilder App Usage Guide");
    mTitleLabel.SetFontFamily("SamsungOneUI_700");

    mCaseTitleLabel = NewTextLabel(CASE_FONT_SIZE, TEXT_PRIMARY, false);
    mCaseTitleLabel.SetFontFamily("SamsungOneUI_700");

    mDescriptionLabel = NewTextLabel(BODY_FONT_SIZE, TEXT_SECONDARY);
    mSourceLabel      = NewTextLabel(BODY_FONT_SIZE, TEXT_SECONDARY);
    mStatsLabel       = NewTextLabel(BODY_FONT_SIZE, TEXT_SECONDARY);
    mResultLabel      = NewTextLabel(RESULT_FONT_SIZE, TEXT_PRIMARY);
    mFooterLabel      = NewTextLabel(FOOTER_FONT_SIZE, TEXT_MUTED, false);

    root.Add(mTitleLabel);
    root.Add(mCaseTitleLabel);
    root.Add(mDescriptionLabel);
    root.Add(mSourceLabel);
    root.Add(mStatsLabel);
    root.Add(mResultLabel);
    root.Add(mFooterLabel);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &TextStyledTextBuilderController::OnKeyEvent);
    Refresh();
  }

  void Refresh()
  {
    const ExampleCase data = BuildCase(mCaseIndex);

    mCaseTitleLabel.SetText(data.title.c_str());
    mDescriptionLabel.SetText(data.description.c_str());
    mSourceLabel.SetText(("Source text: " + data.sourceText).c_str());
    mStatsLabel.SetText(data.stats.c_str());
    mResultLabel.SetStyledText(data.result);
    mFooterLabel.SetText(("Case " + std::to_string(mCaseIndex + 1u) + "/" + std::to_string(CASE_COUNT) + "  |  Left/Right or N/P: case  |  R: rebuild  |  Esc/Back: quit").c_str());
  }

  void ShowPreviousCase()
  {
    mCaseIndex = (mCaseIndex == 0u) ? (CASE_COUNT - 1u) : (mCaseIndex - 1u);
    Refresh();
  }

  void ShowNextCase()
  {
    mCaseIndex = (mCaseIndex + 1u) % CASE_COUNT;
    Refresh();
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT) ||
       event.GetKeyName() == "p" ||
       event.GetKeyName() == "P")
    {
      ShowPreviousCase();
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT) ||
       event.GetKeyName() == "n" ||
       event.GetKeyName() == "N")
    {
      ShowNextCase();
      return;
    }

    if(event.GetKeyName() == "r" || event.GetKeyName() == "R")
    {
      Refresh();
    }
  }

private:
  Application& mApplication;
  Label        mTitleLabel;
  Label        mCaseTitleLabel;
  Label        mDescriptionLabel;
  Label        mSourceLabel;
  Label        mStatsLabel;
  Label        mResultLabel;
  Label        mFooterLabel;
  std::size_t  mCaseIndex{0u};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextStyledTextBuilderController controller(application);
  application.MainLoop();
  return 0;
}
