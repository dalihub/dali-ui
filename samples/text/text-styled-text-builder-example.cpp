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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <array>
#include <cstdint>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int   WINDOW_WIDTH       = 980;
constexpr int   WINDOW_HEIGHT      = 820;
constexpr std::size_t CASE_COUNT    = 6u;
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

ExampleCase BuildIncrementalCase()
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New();

  builder.AppendText("Today: ");

  const uint32_t koreanStart = builder.GetUtf32Length();
  builder.AppendText("가나다");
  const uint32_t koreanEnd = builder.GetUtf32Length();

  builder.AppendText(" ");

  const uint32_t emojiStart = builder.GetUtf32Length();
  builder.AppendText(Dali::String("\xF0\x9F\x98\x80"));
  const uint32_t emojiEnd = builder.GetUtf32Length();

  builder.SetSpan(NewForegroundSpan(RED), koreanStart, koreanEnd);
  builder.SetSpan(NewForegroundSpan(BLUE), emojiStart, emojiEnd);

  const Dali::String text = builder.GetText();

  ExampleCase data;
  data.title       = "Case 1: Incremental builder range";
  data.description = "Scenario: Incremental AppendText(). GetUtf32Length() markers are taken before and after each appended segment.";
  data.sourceText  = text.CStr();
  data.stats       = "UTF-8 bytes: " + std::to_string(text.Size()) +
               "\nUTF-32 length: " + std::to_string(builder.GetUtf32Length()) +
               "\nUTF-32 ranges: 가나다 " + RangeText(koreanStart, koreanEnd) +
               ", 😀 " + RangeText(emojiStart, emojiEnd) +
               "\nNote: GetUtf32Length() is cached by the builder.";
  data.result = builder.Build();
  return data;
}

ExampleCase BuildFullTextSpanCase()
{
  const Dali::String text("Full range: A"
                          "\xE2\x9D\xA4"
                          "\xEF\xB8\x8F"
                          "B 가나다");

  Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(text);
  const uint32_t          length  = builder.GetUtf32Length();
  builder.SetSpan(NewBackgroundSpan(0xDBEAFE), 0u, length);
  builder.SetSpan(NewForegroundSpan(PURPLE), 0u, length);

  ExampleCase data;
  data.title       = "Case 2: Full text span";
  data.description = "Builder already owns the text, so the whole range uses builder.GetUtf32Length().";
  data.sourceText  = text.CStr();
  data.stats       = "UTF-8 bytes: " + std::to_string(text.Size()) +
               "\nUTF-32 length: " + std::to_string(length) +
               "\nFull UTF-32 range: " + RangeText(0u, length);
  data.result = builder.Build();
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
  data.title       = "Case 3: Completed UTF-8 string + std::string::find()";
  data.description = "std::string::find() returns a UTF-8 byte range. Utf8ToUtf32Range() converts it before SetSpan() receives the UTF-32 range.";
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
  const std::string text   = "Back to UTF-8: A"
                           "\xF0\x9F\x91\xA9"
                           "\xE2\x80\x8D"
                           "\xF0\x9F\x92\xBB"
                           "B";
  const std::string target = "\xF0\x9F\x91\xA9"
                             "\xE2\x80\x8D"
                             "\xF0\x9F\x92\xBB";

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
  data.title       = "Case 4: UTF-32 span range back to UTF-8 substring";
  data.description = "Utf8ToUtf32Range() feeds SetSpan(), then Utf32ToUtf8Range() converts the stored span range back to UTF-8 bytes.";
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
    {"\xF0\x9F\x98\x80", BG_SOFT_BLUE},
    {"\xE2\x9D\xA4\xEF\xB8\x8F", BG_SOFT_PURPLE},
    {"\xF0\x9F\x91\xA9\xE2\x80\x8D\xF0\x9F\x92\xBB", BG_SOFT_GREEN},
    {"\xF0\x9F\x87\xB0\xF0\x9F\x87\xB7", BG_SOFT_AMBER},
    {"Ae\xCC\x81""B", BG_SOFT_CYAN},
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
             " | " + std::to_string(Text::Utf8ToUtf32Length(Dali::StringView(rows[rowIndex].text.data(), static_cast<uint32_t>(rows[rowIndex].text.size())))) +
             " | " + RangeText(startIndex, endIndex);
  }

  ExampleCase data;
  data.title       = "Case 5: Complex Unicode table";
  data.description = "UTF-8 byte size and UTF-32 length can differ, including sequences that look compact on screen.";
  data.sourceText  = sourceText;
  data.stats       = stats;
  data.result      = builder.Build();
  return data;
}

ExampleCase BuildBoundaryFailureCase()
{
  const Dali::String text("A"
                          "\xEA\xB0\x80"
                          "B");

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
  data.title       = "Case 6: Boundary failure";
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

ExampleCase BuildCase(std::size_t index)
{
  switch(index)
  {
    case 0u:
      return BuildIncrementalCase();
    case 1u:
      return BuildFullTextSpanCase();
    case 2u:
      return BuildUtf8FindCase();
    case 3u:
      return BuildUtf32BackToUtf8Case();
    case 4u:
      return BuildComplexUnicodeCase();
    default:
      return BuildBoundaryFailureCase();
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
    root.SetPadding(Extents(PAGE_PADDING, PAGE_PADDING, PAGE_PADDING, PAGE_PADDING));
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(PAGE_BACKGROUND));

    mTitleLabel = NewTextLabel(TITLE_FONT_SIZE, TEXT_PRIMARY, false);
    mTitleLabel.SetText("StyledTextBuilder UTF-8 / UTF-32 Range Example");
    mTitleLabel.SetFontFamily("SamsungOneUI_700");

    mCaseTitleLabel = NewTextLabel(CASE_FONT_SIZE, TEXT_PRIMARY, false);
    mCaseTitleLabel.SetFontFamily("SamsungOneUI_700");

    mDescriptionLabel = NewTextLabel(BODY_FONT_SIZE, TEXT_SECONDARY);
    mSourceLabel      = NewTextLabel(BODY_FONT_SIZE, TEXT_SECONDARY);
    mStatsLabel       = NewTextLabel(BODY_FONT_SIZE, TEXT_SECONDARY);
    mResultLabel      = NewTextLabel(RESULT_FONT_SIZE, TEXT_PRIMARY);
    mFooterLabel      = NewTextLabel(FOOTER_FONT_SIZE, TEXT_MUTED, false);

    root.AddChildren({
      mTitleLabel,
      mCaseTitleLabel,
      mDescriptionLabel,
      mSourceLabel,
      mStatsLabel,
      mResultLabel,
      mFooterLabel,
    });

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
