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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <array>
#include <cstddef>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int         WINDOW_WIDTH      = 1100;
constexpr int         WINDOW_HEIGHT     = 650;
constexpr float       HEADER_HEIGHT     = 124.0f;
constexpr float       ID_COLUMN_WIDTH   = 58.0f;
constexpr float       NAME_COLUMN_WIDTH = 148.0f;
constexpr float       SINGLE_ROW_HEIGHT = 44.0f;
constexpr float       MULTI_ROW_HEIGHT  = 96.0f;
constexpr float       TARGET_FONT_SIZE  = 20.0f;
constexpr float       ROW_SPACING       = 8.0f;
constexpr float       COLUMN_SPACING    = 6.0f;
constexpr std::size_t PAGE_COUNT        = 13u;
constexpr uint32_t    WINDOW_COLOR      = 0xE2E8F0;
constexpr uint32_t    HEADER_COLOR      = 0x0F172A;
constexpr uint32_t    HEADER_TEXT_COLOR = 0xF8FAFC;
constexpr uint32_t    HEADER_HINT_COLOR = 0xCBD5E1;
constexpr uint32_t    CHROME_TEXT_COLOR = 0x334155;
constexpr uint32_t    ID_BACKGROUND     = 0xDBEAFE;
constexpr uint32_t    NAME_BACKGROUND   = 0xF1F5F9;
constexpr uint32_t    TARGET_BACKGROUND = 0xFFFFFF;
constexpr uint32_t    TARGET_BORDER     = 0x94A3B8;
constexpr uint32_t    TARGET_TEXT_COLOR = 0x111827;

constexpr const char* CORE_SINGLE_LTR =
  "The quick brown fox keeps walking across the long line while additional English words force "
  "the final ellipsis to move repeatedly during resizing.";

constexpr const char* CORE_SINGLE_RTL =
  "العربية تكشف الكلمات تدريجيا بينما يستمر النص الطويل لإجبار علامة الحذف على التحرك عند تغيير "
  "حجم النافذة العربية تكشف الكلمات تدريجيا بينما يستمر النص الطويل";

constexpr const char* CORE_SINGLE_LTR_RTL =
  "English prefix keeps extending before العربية تكشف الكلمات ويتواصل النص العربي حتى نهاية السطر "
  "العربية تكشف الكلمات ويتواصل النص العربي";

constexpr const char* CORE_SINGLE_RTL_LTR =
  "العربية تكشف الكلمات ثم يبدأ English trailing text and continues for a long distance to exercise "
  "the ellipsis boundary while the window changes width";

constexpr const char* CORE_MULTI_LTR =
  "The quick brown fox keeps walking across the long line while additional English words force the "
  "final ellipsis to move repeatedly during resizing. This paragraph deliberately continues with "
  "more strong left-to-right words so the final visible line changes at many different boundaries. "
  "Every resize should retain English shaping and place the ellipsis at the end of the last "
  "visible line without gaps, overlaps, jumps, or glyphs outside the target control.";

constexpr const char* CORE_MULTI_RTL =
  "العربية تكشف الكلمات تدريجيا بينما يستمر النص الطويل لإجبار علامة الحذف على التحرك عند تغيير حجم "
  "النافذة العربية تكشف الكلمات تدريجيا بينما يستمر النص الطويل لإجبار علامة الحذف على التحرك عند "
  "تغيير حجم النافذة العربية تكشف الكلمات تدريجيا بينما يستمر النص الطويل لإجبار علامة الحذف على "
  "التحرك عند تغيير حجم النافذة العربية تكشف الكلمات تدريجيا بينما يستمر النص الطويل";

constexpr const char* CORE_MULTI_LTR_RTL =
  "English prefix keeps extending before العربية تكشف الكلمات ويتواصل النص العربي حتى نهاية السطر. "
  "More English words establish another long left-to-right segment before العربية تكشف الكلمات "
  "ويتواصل النص العربي حتى نهاية السطر. The paragraph continues in English before العربية تكشف "
  "الكلمات ويتواصل النص العربي حتى نهاية السطر so resizing crosses several directional runs.";

constexpr const char* CORE_MULTI_RTL_LTR =
  "العربية تكشف الكلمات ثم يبدأ English trailing text and continues for a long distance to exercise "
  "the ellipsis boundary. العربية تكشف الكلمات ثم يبدأ another English section that keeps extending "
  "across the available width. العربية تكشف الكلمات ثم يبدأ the final English trailing section and "
  "continues until the last visible line must be elided during horizontal window resizing.";

constexpr const char* COMPLEX_SINGLE_1 =
  "English prefix 123 — العربية تكشف الكلمات 456 — English trailing section 789 continues until ellipsis";
constexpr const char* COMPLEX_SINGLE_2 =
  "العربية 123 — English middle section 456 — עברית عربية النهاية العربية تستمر حتى علامة الحذف";
constexpr const char* COMPLEX_SINGLE_3 =
  "English (עברית 123) [العربية 456] — mixed punctuation : ; , . trailing text continues until ellipsis";
constexpr const char* COMPLEX_SINGLE_4 =
  "한국어 순차 표시와 العربية mixed English text 그리고 עברית 123 마지막 영역을 함께 확인합니다 계속되는 문자열";

constexpr const char* COMPLEX_MULTI_1 =
  "English prefix 123 — العربية تكشف الكلمات 456 — English trailing section 789 continues until the "
  "ellipsis boundary. Another English segment 101112 comes before العربية تكشف الكلمات 131415 and "
  "then English trailing text resumes. Resize repeatedly so the final visible boundary crosses the "
  "numbers, punctuation, Arabic run, and English run without a visual jump or overlap.";
constexpr const char* COMPLEX_MULTI_2 =
  "العربية 123 — English middle section 456 — עברית عربية النهاية. العربية تستمر 789 ثم يبدأ another "
  "English middle section 101112 before עברית عربية النهاية. This repeated mixed-direction paragraph "
  "is intentionally long enough to cover many lines so the last visible line always uses END ellipsis.";
constexpr const char* COMPLEX_MULTI_3 =
  "English (עברית 123) [العربية 456] — mixed punctuation : ; , . trailing text continues. Another "
  "group uses [עברית 789], (العربية 101112), and English punctuation ! ? : ; before more trailing "
  "words. Resizing should move the END ellipsis through brackets, numbers, punctuation, and BiDi runs.";
constexpr const char* COMPLEX_MULTI_4 =
  "한국어 순차 표시와 العربية mixed English text 그리고 עברית 123 마지막 영역을 함께 확인합니다. "
  "추가 한국어 문장 뒤에 العربية 456 and another English segment가 오고 עברית 789 구간도 이어집니다. "
  "마지막 visible line에서 ellipsis 위치가 여러 언어와 숫자 경계를 지나도록 문장을 충분히 길게 계속합니다.";

constexpr const char* ALIGNMENT_CORPUS =
  "English 123 starts before العربية 456 and עברית 789, then English trailing punctuation (test) "
  "[more] continues until the END ellipsis";

constexpr const char* ARABIC_JOINING_CORPUS =
  "العربيةالعربيةالعربيةالعربيةالعربيةالعربيةالعربيةالعربيةالعربيةالعربيةالعربيةالعربيةالعربية";

constexpr const char* CLUSTER_CORPUS =
  "office office cafe\u0301 cafe\u0301 👍🏽 👨‍👩‍👧‍👦 trailing office cafe\u0301 family emoji text continues "
  "with office cafe\u0301 👍🏽 👨‍👩‍👧‍👦 until ellipsis";

constexpr const char* ISOLATE_CORPUS =
  "LTR-prefix [\u2067العربيةالعربيةעברית12345العربية\u2069] trailing English words continue after the "
  "isolated right-to-left sequence. A second isolate [\u2067עברית العربية 67890\u2069] is followed by "
  "more left-to-right text so CHARACTER wrapping and ellipsis cross both isolate boundaries safely.";

constexpr const char* PARAGRAPH_CORPUS =
  "English first paragraph has enough text to wrap across several lines while the window changes width "
  "and moves the final visible boundary toward its paragraph separator.\n"
  "한국어 두 번째 문단과 العربية mixed text 123이 이어지고 paragraph boundary 주변의 ellipsis 위치를 "
  "계속 확인할 수 있도록 문장을 충분히 길게 작성합니다.\n"
  "עברית third paragraph continues with English trailing text and العربية content until the final "
  "visible line is elided near another paragraph boundary.";

constexpr const char* SEMANTIC_LTR_RTL_CORPUS     = "abc אבגדהוזח";
constexpr const char* SEMANTIC_RTL_LTR_CORPUS     = "אבג abcdefgh";
constexpr const char* SEMANTIC_LTR_RTL_LTR_CORPUS = "abc אבג abcdef";
constexpr const char* SEMANTIC_RTL_LTR_RTL_CORPUS = "אבג abc אבגדה";
constexpr const char* SEMANTIC_LTR_NUM_RTL_CORPUS = "abc 123 אבגדה";
constexpr const char* SEMANTIC_RTL_NUM_LTR_CORPUS = "אבג 123 abcdef";

struct EllipsisTestCase
{
  uint32_t              id;
  const char*           name;
  const char*           contentType;
  const char*           text;
  bool                  multiLine;
  LayoutDirection::Type direction;
  Text::Alignment       alignment;
  Text::LineWrapMode    wrapMode;
};

struct TestPage
{
  const char* title;
  const char* mode;
  std::size_t firstCaseIndex;
  std::size_t caseCount;
};

constexpr std::array<EllipsisTestCase, 56u> TEST_CASES{{
  {1u, "LTR ONLY", "LTR_ONLY", CORE_SINGLE_LTR, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {2u, "RTL ONLY", "RTL_ONLY", CORE_SINGLE_RTL, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {3u, "LTR -> RTL", "LTR_RTL", CORE_SINGLE_LTR_RTL, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {4u, "RTL -> LTR", "RTL_LTR", CORE_SINGLE_RTL_LTR, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {5u, "LTR ONLY", "LTR_ONLY", CORE_SINGLE_LTR, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {6u, "RTL ONLY", "RTL_ONLY", CORE_SINGLE_RTL, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {7u, "LTR -> RTL", "LTR_RTL", CORE_SINGLE_LTR_RTL, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {8u, "RTL -> LTR", "RTL_LTR", CORE_SINGLE_RTL_LTR, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {9u, "LTR ONLY", "LTR_ONLY", CORE_MULTI_LTR, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {10u, "RTL ONLY", "RTL_ONLY", CORE_MULTI_RTL, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {11u, "LTR -> RTL", "LTR_RTL", CORE_MULTI_LTR_RTL, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {12u, "RTL -> LTR", "RTL_LTR", CORE_MULTI_RTL_LTR, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {13u, "LTR ONLY", "LTR_ONLY", CORE_MULTI_LTR, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {14u, "RTL ONLY", "RTL_ONLY", CORE_MULTI_RTL, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {15u, "LTR -> RTL", "LTR_RTL", CORE_MULTI_LTR_RTL, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {16u, "RTL -> LTR", "RTL_LTR", CORE_MULTI_RTL_LTR, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {17u, "LTR-RTL-LTR + #", "MIXED_NUMBERS", COMPLEX_SINGLE_1, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {18u, "RTL-LTR-RTL + #", "MIXED_NUMBERS", COMPLEX_SINGLE_2, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {19u, "BRACKETS / PUNCT", "BRACKETS_PUNCT", COMPLEX_SINGLE_3, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {20u, "KO / AR / EN / HE", "FOUR_SCRIPTS", COMPLEX_SINGLE_4, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {21u, "LTR-RTL-LTR + #", "MIXED_NUMBERS", COMPLEX_SINGLE_1, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {22u, "RTL-LTR-RTL + #", "MIXED_NUMBERS", COMPLEX_SINGLE_2, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {23u, "BRACKETS / PUNCT", "BRACKETS_PUNCT", COMPLEX_SINGLE_3, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {24u, "KO / AR / EN / HE", "FOUR_SCRIPTS", COMPLEX_SINGLE_4, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {25u, "LTR-RTL-LTR + #", "MIXED_NUMBERS", COMPLEX_MULTI_1, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {26u, "RTL-LTR-RTL + #", "MIXED_NUMBERS", COMPLEX_MULTI_2, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {27u, "BRACKETS / PUNCT", "BRACKETS_PUNCT", COMPLEX_MULTI_3, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {28u, "KO / AR / EN / HE", "FOUR_SCRIPTS", COMPLEX_MULTI_4, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {29u, "LTR-RTL-LTR + #", "MIXED_NUMBERS", COMPLEX_MULTI_1, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {30u, "RTL-LTR-RTL + #", "MIXED_NUMBERS", COMPLEX_MULTI_2, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {31u, "BRACKETS / PUNCT", "BRACKETS_PUNCT", COMPLEX_MULTI_3, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {32u, "KO / AR / EN / HE", "FOUR_SCRIPTS", COMPLEX_MULTI_4, true, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {33u, "MIXED / START", "ALIGN_START", ALIGNMENT_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {34u, "MIXED / CENTER", "ALIGN_CENTER", ALIGNMENT_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::CENTER, Text::LineWrapMode::WORD},
  {35u, "MIXED / END", "ALIGN_END", ALIGNMENT_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::END, Text::LineWrapMode::WORD},
  {36u, "LTR TEXT / END", "ALIGN_END_LTR_TEXT", CORE_SINGLE_LTR, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::END, Text::LineWrapMode::WORD},

  {37u, "MIXED / START", "ALIGN_START", ALIGNMENT_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {38u, "MIXED / CENTER", "ALIGN_CENTER", ALIGNMENT_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::CENTER, Text::LineWrapMode::WORD},
  {39u, "MIXED / END", "ALIGN_END", ALIGNMENT_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::END, Text::LineWrapMode::WORD},
  {40u, "RTL TEXT / END", "ALIGN_END_RTL_TEXT", CORE_SINGLE_RTL, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::END, Text::LineWrapMode::WORD},

  {41u, "ARABIC JOINING", "ARABIC_JOINING", ARABIC_JOINING_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {42u, "CLUSTER / EMOJI", "LIGATURE_COMBINING_EMOJI", CLUSTER_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {43u, "RLI / PDI / CHAR", "BIDI_ISOLATE", ISOLATE_CORPUS, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::CHARACTER},
  {44u, "PARAGRAPHS", "PARAGRAPH_BOUNDARY", PARAGRAPH_CORPUS, true, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {45u, "LTR -> RTL", "SEMANTIC_LTR_RTL", SEMANTIC_LTR_RTL_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {46u, "RTL -> LTR", "SEMANTIC_RTL_LTR", SEMANTIC_RTL_LTR_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {47u, "LTR-RTL-LTR", "SEMANTIC_LTR_RTL_LTR", SEMANTIC_LTR_RTL_LTR_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {48u, "RTL-LTR-RTL", "SEMANTIC_RTL_LTR_RTL", SEMANTIC_RTL_LTR_RTL_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {49u, "LTR-#-RTL", "SEMANTIC_LTR_NUM_RTL", SEMANTIC_LTR_NUM_RTL_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {50u, "RTL-#-LTR", "SEMANTIC_RTL_NUM_LTR", SEMANTIC_RTL_NUM_LTR_CORPUS, false, LayoutDirection::LEFT_TO_RIGHT, Text::Alignment::START, Text::LineWrapMode::WORD},

  {51u, "LTR -> RTL", "SEMANTIC_LTR_RTL", SEMANTIC_LTR_RTL_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {52u, "RTL -> LTR", "SEMANTIC_RTL_LTR", SEMANTIC_RTL_LTR_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {53u, "LTR-RTL-LTR", "SEMANTIC_LTR_RTL_LTR", SEMANTIC_LTR_RTL_LTR_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {54u, "RTL-LTR-RTL", "SEMANTIC_RTL_LTR_RTL", SEMANTIC_RTL_LTR_RTL_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {55u, "LTR-#-RTL", "SEMANTIC_LTR_NUM_RTL", SEMANTIC_LTR_NUM_RTL_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
  {56u, "RTL-#-LTR", "SEMANTIC_RTL_NUM_LTR", SEMANTIC_RTL_NUM_LTR_CORPUS, false, LayoutDirection::RIGHT_TO_LEFT, Text::Alignment::START, Text::LineWrapMode::WORD},
}};

constexpr std::array<TestPage, PAGE_COUNT> TEST_PAGES{{
  {"CORE SINGLE / FORCED LTR", "SINGLE forced=LTR", 0u, 4u},
  {"CORE SINGLE / FORCED RTL", "SINGLE forced=RTL", 4u, 4u},
  {"CORE MULTI / FORCED LTR", "MULTI forced=LTR wrap=WORD", 8u, 4u},
  {"CORE MULTI / FORCED RTL", "MULTI forced=RTL wrap=WORD", 12u, 4u},
  {"COMPLEX BIDI SINGLE / FORCED LTR", "SINGLE complex forced=LTR", 16u, 4u},
  {"COMPLEX BIDI SINGLE / FORCED RTL", "SINGLE complex forced=RTL", 20u, 4u},
  {"COMPLEX BIDI MULTI / FORCED LTR", "MULTI complex forced=LTR", 24u, 4u},
  {"COMPLEX BIDI MULTI / FORCED RTL", "MULTI complex forced=RTL", 28u, 4u},
  {"ALIGNMENT / FORCED LTR", "SINGLE forced=LTR alignment", 32u, 4u},
  {"ALIGNMENT / FORCED RTL", "SINGLE forced=RTL alignment", 36u, 4u},
  {"SHAPING / CLUSTER / PARAGRAPH", "SINGLE+MULTI shaping stress", 40u, 4u},
  {"BIDI SEMANTIC PROBES / FORCED LTR", "SINGLE semantic-probe forced=LTR", 44u, 6u},
  {"BIDI SEMANTIC PROBES / FORCED RTL", "SINGLE semantic-probe forced=RTL", 50u, 6u},
}};

constexpr bool HasExpectedCaseIds()
{
  for(std::size_t index = 0u; index < TEST_CASES.size(); ++index)
  {
    if(TEST_CASES[index].id != index + 1u)
    {
      return false;
    }
  }
  return true;
}

constexpr bool HasValidPageRanges()
{
  std::size_t nextCaseIndex = 0u;
  for(const TestPage& page : TEST_PAGES)
  {
    if(page.firstCaseIndex != nextCaseIndex || page.caseCount == 0u ||
       page.firstCaseIndex + page.caseCount > TEST_CASES.size())
    {
      return false;
    }
    nextCaseIndex += page.caseCount;
  }
  return nextCaseIndex == TEST_CASES.size();
}

static_assert(HasExpectedCaseIds(), "Ellipsis case IDs must remain globally unique from #001 through #056");
static_assert(HasValidPageRanges(), "Every ellipsis case must belong to exactly one contiguous page range");

const char* DirectionName(LayoutDirection::Type direction)
{
  return direction == LayoutDirection::RIGHT_TO_LEFT ? "RTL" : "LTR";
}

const char* AlignmentName(Text::Alignment alignment)
{
  switch(alignment)
  {
    case Text::Alignment::START:
      return "START";
    case Text::Alignment::CENTER:
      return "CENTER";
    case Text::Alignment::END:
      return "END";
  }
  return "UNKNOWN";
}

const char* WrapModeName(Text::LineWrapMode mode)
{
  switch(mode)
  {
    case Text::LineWrapMode::WORD:
      return "WORD";
    case Text::LineWrapMode::CHARACTER:
      return "CHARACTER";
    case Text::LineWrapMode::HYPHENATION:
      return "HYPHENATION";
    case Text::LineWrapMode::MIXED:
      return "MIXED";
  }
  return "UNKNOWN";
}

void SetChromeDirection(View view)
{
  view.SetLayoutDirection(LayoutDirection::LEFT_TO_RIGHT);
}

void SetChromeDirection(Label label)
{
  SetChromeDirection(View(label));
  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
}

Label NewChromeLabel(const char* text, float fontSize, uint32_t textColor, uint32_t backgroundColor)
{
  Label label = Label::New(text);
  SetChromeDirection(label);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(textColor));
  label.SetBackgroundColor(UiColor(backgroundColor));
  label.SetMultiLine(false);
  label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
  label.SetHorizontalTextAlignment(Text::Alignment::START);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}
} // unnamed namespace

class TextEllipsisController : public ConnectionTracker
{
public:
  explicit TextEllipsisController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextEllipsisController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(WINDOW_COLOR));
    window.KeyEventSignal().Connect(this, &TextEllipsisController::OnKeyEvent);
    window.ResizedSignal().Connect(this, &TextEllipsisController::OnWindowResized);

    mWindowWidth  = WINDOW_WIDTH;
    mWindowHeight = WINDOW_HEIGHT;

    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    SetChromeDirection(mRoot);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetSpacing(0.0f);
    mRoot.SetBackgroundColor(UiColor(WINDOW_COLOR));

    CreateHeader();
    mRoot.Add(mHeader);
    window.Add(mRoot);

    ShowPage(0u);
  }

  void CreateHeader()
  {
    mHeader = StackLayout::New(StackOrientation::VERTICAL);
    SetChromeDirection(mHeader);
    mHeader.SetRequestedWidth(MATCH_PARENT);
    mHeader.SetRequestedHeight(HEADER_HEIGHT);
    mHeader.SetSpacing(4.0f);
    mHeader.SetPadding(Extents(static_cast<int16_t>(12u),
                               static_cast<int16_t>(12u),
                               static_cast<int16_t>(8u),
                               static_cast<int16_t>(8u)));
    mHeader.SetBackgroundColor(UiColor(HEADER_COLOR));

    mHeaderTitle = NewChromeLabel("", 22.0f, HEADER_TEXT_COLOR, HEADER_COLOR);
    mHeaderTitle.SetRequestedWidth(MATCH_PARENT);
    mHeaderTitle.SetRequestedHeight(30.0f);

    mHeaderStatus = NewChromeLabel("", 16.0f, HEADER_TEXT_COLOR, HEADER_COLOR);
    mHeaderStatus.SetRequestedWidth(MATCH_PARENT);
    mHeaderStatus.SetRequestedHeight(27.0f);

    mHeaderHelp = NewChromeLabel(
      "Resize horizontally | Left/Right or PageUp/PageDown: page | Home/End | ESC: exit\n"
      "Target: 20 px | solid | END ellipsis | Reveal/Gradient/Marquee off",
      13.0f,
      HEADER_HINT_COLOR,
      HEADER_COLOR);
    mHeaderHelp.SetMultiLine(true);
    mHeaderHelp.SetLineWrapMode(Text::LineWrapMode::WORD);
    mHeaderHelp.SetRequestedWidth(MATCH_PARENT);
    mHeaderHelp.SetRequestedHeight(42.0f);

    mHeader.Add(mHeaderTitle);
    mHeader.Add(mHeaderStatus);
    mHeader.Add(mHeaderHelp);
  }

  Label CreateIdLabel(uint32_t id) const
  {
    std::ostringstream text;
    text << '#' << std::setw(3) << std::setfill('0') << id;

    Label label = NewChromeLabel(text.str().c_str(), 16.0f, 0x1E3A8A, ID_BACKGROUND);
    label.SetRequestedWidth(ID_COLUMN_WIDTH);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetBorderlineWidth(1.0f);
    label.SetBorderlineOffset(-1.0f);
    label.SetBorderlineColor(UiColor(0x93C5FD));
    label.SetCornerRadius(5.0f);
    return label;
  }

  Label CreateNameLabel(const EllipsisTestCase& test) const
  {
    Label label = NewChromeLabel(test.name, 13.0f, CHROME_TEXT_COLOR, NAME_BACKGROUND);
    label.SetRequestedWidth(NAME_COLUMN_WIDTH);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetPadding(Extents(static_cast<int16_t>(8u),
                             static_cast<int16_t>(6u),
                             static_cast<int16_t>(0u),
                             static_cast<int16_t>(0u)));
    label.SetBorderlineWidth(1.0f);
    label.SetBorderlineOffset(-1.0f);
    label.SetBorderlineColor(UiColor(0xCBD5E1));
    label.SetCornerRadius(5.0f);
    return label;
  }

  Label CreateTargetLabel(const EllipsisTestCase& test) const
  {
    Label label = Label::New(test.text);

    // Only the target receives the test direction. INHERIT makes the text
    // controller consume this Label actor's explicitly forced effective
    // layout direction instead of resolving direction from the text content.
    label.SetLayoutDirection(test.direction);
    label.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
    label.SetFontSize(TARGET_FONT_SIZE);
    label.SetTextColor(UiColor(TARGET_TEXT_COLOR));
    label.SetTextGradient(Gradient::Base::None());
    label.SetTextReveal(Text::Reveal::None());
    label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::MANUAL);
    label.SetMultiLine(test.multiLine);
    label.SetLineWrapMode(test.wrapMode);
    label.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    label.SetHorizontalTextAlignment(test.alignment);
    label.SetVerticalTextAlignment(test.multiLine ? Text::Alignment::START : Text::Alignment::CENTER);
    label.SetLineHeightMode(Text::LineHeightMode::RELATIVE);
    label.SetLineHeight(1.15f);
    label.SetPadding(Extents(static_cast<int16_t>(8u),
                             static_cast<int16_t>(8u),
                             static_cast<int16_t>(4u),
                             static_cast<int16_t>(4u)));
    label.SetBackgroundColor(UiColor(TARGET_BACKGROUND));
    label.SetBorderlineWidth(1.0f);
    label.SetBorderlineOffset(-1.0f);
    label.SetBorderlineColor(UiColor(TARGET_BORDER));
    label.SetCornerRadius(5.0f);
    label.SetRequestedWidth(0.0f);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    return label;
  }

  StackLayout CreateTestRow(const EllipsisTestCase& test) const
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    SetChromeDirection(row);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(test.multiLine ? MULTI_ROW_HEIGHT : SINGLE_ROW_HEIGHT);
    row.SetSpacing(COLUMN_SPACING);
    row.Add(CreateIdLabel(test.id));
    row.Add(CreateNameLabel(test));
    row.Add(CreateTargetLabel(test));
    return row;
  }

  StackLayout CreatePageContent(const TestPage& page) const
  {
    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    SetChromeDirection(content);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(0.0f);
    content.SetSpacing(ROW_SPACING);
    content.SetPadding(Extents(static_cast<int16_t>(12u),
                               static_cast<int16_t>(12u),
                               static_cast<int16_t>(12u),
                               static_cast<int16_t>(12u)));
    content.SetBackgroundColor(UiColor(WINDOW_COLOR));
    content.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    for(std::size_t caseOffset = 0u; caseOffset < page.caseCount; ++caseOffset)
    {
      content.Add(CreateTestRow(TEST_CASES[page.firstCaseIndex + caseOffset]));
    }
    return content;
  }

  void ShowPage(std::size_t pageIndex)
  {
    mPageIndex = pageIndex % TEST_PAGES.size();
    if(mPageContent)
    {
      mRoot.Remove(mPageContent, RemovePolicy::IMMEDIATE);
      mPageContent.Reset();
    }

    const TestPage& page = TEST_PAGES[mPageIndex];
    mPageContent         = CreatePageContent(page);
    mRoot.Add(mPageContent);
    UpdateHeader();
    PrintPageMetadata(page);
  }

  void UpdateHeader()
  {
    const TestPage& page = TEST_PAGES[mPageIndex];

    std::ostringstream title;
    title << "Page " << (mPageIndex + 1u) << " / " << TEST_PAGES.size()
          << "  |  DALi UI END Ellipsis Visual Test";
    mHeaderTitle.SetText(title.str().c_str());

    std::ostringstream status;
    status << page.title << "  |  Window: " << mWindowWidth << " x " << mWindowHeight;
    mHeaderStatus.SetText(status.str().c_str());
  }

  void PrintPageMetadata(const TestPage& page) const
  {
    std::printf("ELLIPSIS_TEST page=%zu/%zu mode=%s\n", mPageIndex + 1u, TEST_PAGES.size(), page.mode);
    for(std::size_t caseOffset = 0u; caseOffset < page.caseCount; ++caseOffset)
    {
      const EllipsisTestCase& test = TEST_CASES[page.firstCaseIndex + caseOffset];
      std::printf("#%03u content=%s line=%s forced=%s wrap=%s align=%s\n",
                  test.id,
                  test.contentType,
                  test.multiLine ? "MULTI" : "SINGLE",
                  DirectionName(test.direction),
                  WrapModeName(test.wrapMode),
                  AlignmentName(test.alignment));
    }
    std::fflush(stdout);
  }

  void OnWindowResized(Window, Window::WindowSize windowSize)
  {
    mWindowWidth  = static_cast<uint32_t>(windowSize.GetWidth());
    mWindowHeight = static_cast<uint32_t>(windowSize.GetHeight());
    UpdateHeader();
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

    const Dali::String& keyName = event.GetKeyName();
    if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT) || keyName == "Right" || keyName == "KP_Right" ||
       keyName == "Page_Down" || keyName == "PageDown" || keyName == "Next")
    {
      ShowPage((mPageIndex + 1u) % TEST_PAGES.size());
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT) || keyName == "Left" || keyName == "KP_Left" ||
            keyName == "Page_Up" || keyName == "PageUp" || keyName == "Prior")
    {
      ShowPage((mPageIndex + TEST_PAGES.size() - 1u) % TEST_PAGES.size());
    }
    else if(keyName == "Home" || keyName == "KP_Home")
    {
      ShowPage(0u);
    }
    else if(keyName == "End" || keyName == "KP_End")
    {
      ShowPage(TEST_PAGES.size() - 1u);
    }
  }

private:
  Application& mApplication;
  StackLayout  mRoot;
  StackLayout  mHeader;
  StackLayout  mPageContent;
  Label        mHeaderTitle;
  Label        mHeaderStatus;
  Label        mHeaderHelp;
  std::size_t  mPageIndex{0u};
  uint32_t     mWindowWidth{WINDOW_WIDTH};
  uint32_t     mWindowHeight{WINDOW_HEIGHT};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextEllipsisController controller(application);
  application.MainLoop();
  return 0;
}
