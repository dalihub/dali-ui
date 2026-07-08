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

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

constexpr float STACK_SPACING       = 8.0f;
constexpr float HEADER_PADDING      = 12.0f;
constexpr float HEADER_ROW_GAP      = 8.0f;
constexpr float HEADER_BADGE_HEIGHT = 30.0f;
constexpr float HEADER_INFO_HEIGHT  = 52.0f;
constexpr float CONTENT_PADDING     = 16.0f;
constexpr float FOOTER_PADDING      = 12.0f;
constexpr float FOOTER_ROW_GAP      = 8.0f;
constexpr float FOOTER_TITLE_WIDTH  = 120.0f;
constexpr float FOOTER_BADGE_HEIGHT = 28.0f;
constexpr float FOOTER_LINE_HEIGHT  = 24.0f;
constexpr float PREVIEW_TITLE_HEIGHT = 28.0f;
constexpr float HEADER_HEIGHT       = HEADER_PADDING + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP + HEADER_INFO_HEIGHT + HEADER_PADDING;
constexpr float FOOTER_HEIGHT       = FOOTER_PADDING + FOOTER_BADGE_HEIGHT + FOOTER_ROW_GAP + FOOTER_LINE_HEIGHT + FOOTER_ROW_GAP + FOOTER_LINE_HEIGHT + FOOTER_PADDING;
constexpr int   WINDOW_WIDTH        = 920;
constexpr int   WINDOW_HEIGHT       = 820;
constexpr std::size_t CASE_COUNT     = 25u;

constexpr uint32_t BADGE_DISABLED_BACKGROUND = 0x1E293B;
constexpr uint32_t BADGE_DISABLED_BORDER     = 0x475569;
constexpr uint32_t BADGE_DISABLED_TEXT       = 0xCBD5E1;
constexpr uint32_t BADGE_READY_BACKGROUND    = 0x075985;
constexpr uint32_t BADGE_READY_BORDER        = 0x38BDF8;
constexpr uint32_t BADGE_APPLY_BACKGROUND    = 0x047857;
constexpr uint32_t BADGE_APPLY_BORDER        = 0x6EE7B7;
constexpr uint32_t BADGE_WARN_BACKGROUND     = 0x7C2D12;
constexpr uint32_t BADGE_WARN_BORDER         = 0xFDBA74;
constexpr uint32_t BADGE_CLEAR_BACKGROUND    = 0x7F1D1D;
constexpr uint32_t BADGE_CLEAR_BORDER        = 0xFCA5A5;
constexpr uint32_t BADGE_ON_TEXT             = 0xF8FAFC;

enum class StyledTextCase
{
  PLAIN_TEXT,
  FOREGROUND_COLOR_SPAN_SINGLE,
  FOREGROUND_COLOR_SPAN_MULTIPLE,
  FOREGROUND_COLOR_SPAN_SAME_OBJECT_UPDATE,
  FOREGROUND_COLOR_SPAN_OVERLAP,
  FOREGROUND_COLOR_SPAN_CLEAR,
  FOREGROUND_COLOR_SPAN_CHANGE_VALUE,
  FOREGROUND_COLOR_SPAN_CHANGE_RANGE,
  BACKGROUND_COLOR_SPAN_SINGLE,
  BACKGROUND_COLOR_SPAN_OVERLAP,
  FOREGROUND_AND_BACKGROUND_COLOR_SPAN,
  UNDERLINE_SPAN_SINGLE,
  LINE_THROUGH_SPAN_SINGLE,
  FOREGROUND_BACKGROUND_DECORATION_SPAN,
  LITERAL_MARKUP_TEXT,
  FONT_SPAN_WEIGHT,
  FONT_SPAN_SLANT,
  FONT_SPAN_SIZE,
  FONT_SPAN_COMBINED,
  FONT_SPAN_FIELD_MERGE,
  FONT_SPAN_LATER_WEIGHT_WINS,
  ANCHOR_SPAN,
  MARKUP_TO_STYLED_TEXT_BASIC,
  MARKUP_TO_STYLED_TEXT_ANCHOR_ENTITY,
  FUTURE_GRADIENT_SPAN_DISABLED,
};

enum class StyledTextValueKind
{
  NONE,
  FOREGROUND,
  BACKGROUND,
  FOREGROUND_AND_BACKGROUND,
  UNDERLINE,
  LINE_THROUGH,
  FONT,
};

struct StyledTextCaseInfo
{
  StyledTextCase      type;
  const char*         title;
  const char*         description;
  bool                enabled;
  StyledTextValueKind valueKind;
  bool                rangeChange;
  bool                clearable;
  bool                sourceToggle;
};

struct PreviewState
{
  std::string text;
  std::string spanMode;
  std::string rangeInfo;
  std::string valueInfo;
};

constexpr std::array<StyledTextCaseInfo, CASE_COUNT> CASES{{
  {StyledTextCase::PLAIN_TEXT, "Plain text", "No span. Source toggles SetText and StyledText::New for the same text.", true, StyledTextValueKind::NONE, false, false, true},
  {StyledTextCase::FOREGROUND_COLOR_SPAN_SINGLE, "Single ForegroundColorSpan", "One ForegroundColorSpan over the current range. Value, Range and Clear affect this case.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::FOREGROUND_COLOR_SPAN_MULTIPLE, "Multiple ForegroundColorSpan objects", "Two separate ForegroundColorSpan objects. Value updates the pair; Range moves the second span.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::FOREGROUND_COLOR_SPAN_SAME_OBJECT_UPDATE, "Same span object update", "One ForegroundColorSpan object is attached twice; final range wins for that object.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::FOREGROUND_COLOR_SPAN_OVERLAP, "Overlap order", "Red first, variable later span second. Overlap should follow downstream later-wins segmentation.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::FOREGROUND_COLOR_SPAN_CLEAR, "Clear spans", "Starts with a ForegroundColorSpan. Clear removes spans in this case; Reset restores the span.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::FOREGROUND_COLOR_SPAN_CHANGE_VALUE, "Change span value", "Value cycles the ForegroundColorSpan payload while staying in this case.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::FOREGROUND_COLOR_SPAN_CHANGE_RANGE, "Change span range", "Range moves the ForegroundColorSpan range while staying in this case.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::BACKGROUND_COLOR_SPAN_SINGLE, "Single BackgroundColorSpan", "One BackgroundColorSpan over the current range. Value, Range and Clear affect this case.", true, StyledTextValueKind::BACKGROUND, true, true, true},
  {StyledTextCase::BACKGROUND_COLOR_SPAN_OVERLAP, "BackgroundColorSpan overlap", "Yellow first, variable later background second. Overlap should follow later-wins segmentation.", true, StyledTextValueKind::BACKGROUND, true, true, true},
  {StyledTextCase::FOREGROUND_AND_BACKGROUND_COLOR_SPAN, "ForegroundColorSpan + BackgroundColorSpan", "Foreground and background spans share the current range but remain independent categories.", true, StyledTextValueKind::FOREGROUND_AND_BACKGROUND, true, true, true},
  {StyledTextCase::UNDERLINE_SPAN_SINGLE, "Single UnderlineSpan", "One UnderlineSpan over the current range. Value, Range and Clear affect this case.", true, StyledTextValueKind::UNDERLINE, true, true, true},
  {StyledTextCase::LINE_THROUGH_SPAN_SINGLE, "Single LineThroughSpan", "One LineThroughSpan over the current range. Value, Range and Clear affect this case.", true, StyledTextValueKind::LINE_THROUGH, true, true, true},
  {StyledTextCase::FOREGROUND_BACKGROUND_DECORATION_SPAN, "Foreground + Background + Decorations", "ForegroundColorSpan, BackgroundColorSpan, UnderlineSpan and LineThroughSpan share a range as independent categories.", true, StyledTextValueKind::FOREGROUND_AND_BACKGROUND, true, true, true},
  {StyledTextCase::LITERAL_MARKUP_TEXT, "Literal markup text", "StyledText source keeps markup tags as plain text.", true, StyledTextValueKind::NONE, false, false, true},
  {StyledTextCase::FONT_SPAN_WEIGHT, "FontSpan: weight bold", "One FontSpan applies WEIGHT over the current range. Value toggles bold and explicit normal.", true, StyledTextValueKind::FONT, true, true, true},
  {StyledTextCase::FONT_SPAN_SLANT, "FontSpan: slant italic", "One FontSpan applies SLANT over the current range. Value toggles italic and normal.", true, StyledTextValueKind::FONT, true, true, true},
  {StyledTextCase::FONT_SPAN_SIZE, "FontSpan: size in pixels", "One FontSpan applies pixel SIZE over the current range. Value cycles larger pixel sizes.", true, StyledTextValueKind::FONT, true, true, true},
  {StyledTextCase::FONT_SPAN_COMBINED, "FontSpan: combined attributes", "One FontSpan applies family, size, weight and slant together. Family may fall back by platform.", true, StyledTextValueKind::FONT, true, true, true},
  {StyledTextCase::FONT_SPAN_FIELD_MERGE, "FontSpan: field merge", "Family-only and weight-only FontSpans overlap; unset fields do not clear each other.", true, StyledTextValueKind::FONT, true, true, true},
  {StyledTextCase::FONT_SPAN_LATER_WEIGHT_WINS, "FontSpan: later normal wins", "Earlier bold FontSpan is overridden by a later explicit normal weight on the inner range.", true, StyledTextValueKind::NONE, true, true, true},
  {StyledTextCase::ANCHOR_SPAN, "AnchorSpan", "Two StyledText anchors. The first uses Label AnchorColor fallback; the second has explicit color and clicked color.", true, StyledTextValueKind::FOREGROUND, true, true, true},
  {StyledTextCase::MARKUP_TO_STYLED_TEXT_BASIC, "FromMarkup: basic styles", "Converts DALi markup to StyledText. SetText remains plain text.", true, StyledTextValueKind::NONE, false, false, true},
  {StyledTextCase::MARKUP_TO_STYLED_TEXT_ANCHOR_ENTITY, "FromMarkup: anchor + entities", "Converts anchor markup and entities to AnchorSpan and decoded text.", true, StyledTextValueKind::NONE, false, false, true},
  {StyledTextCase::FUTURE_GRADIENT_SPAN_DISABLED, "Future GradientSpan", "Disabled placeholder. GradientSpan is not implemented in this phase.", false, StyledTextValueKind::NONE, false, false, false},
}};

constexpr const char* FROM_MARKUP_BASIC_TEXT =
  "<color value='red'>Red</color> <font weight='bold' size='34'>Font</font> <b>Bold</b> <i>Italic</i>\n"
  "<u color='green' height='2' type='dashed' dash-gap='3' dash-width='5'>Under</u> <s color='blue' height='3'>Strike</s> <background color='yellow'>BG</background>";

constexpr const char* FROM_MARKUP_ANCHOR_ENTITY_TEXT =
  "<a href=https://example.com?a=1&amp;b=2 color=blue clicked-color='red'>entity link</a> | A &lt; B &amp;&amp; C &gt; D | raw 1 < 2 && 3 > 2";

constexpr std::array<uint32_t, 4u> COLOR_VALUES{{
  0xEF4444,
  0x16A34A,
  0x2563EB,
  0xEAB308,
}};

constexpr std::array<const char*, 4u> COLOR_NAMES{{
  "red",
  "green",
  "blue",
  "yellow",
}};

constexpr std::array<uint32_t, 4u> BACKGROUND_VALUES{{
  0xFECACA,
  0xBBF7D0,
  0xBFDBFE,
  0xFEF08A,
}};

constexpr std::array<const char*, 4u> BACKGROUND_NAMES{{
  "soft red",
  "soft green",
  "soft blue",
  "soft yellow",
}};

constexpr std::array<float, 4u> FONT_SIZE_VALUES{{
  28.0f,
  34.0f,
  42.0f,
  50.0f,
}};

Label CreateLabel(const char* text, float fontSize, const UiColor& textColor)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(textColor);
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

void ConfigureHudBadge(Label label, float height, const UiColor& backgroundColor, const UiColor& borderlineColor, const UiColor& textColor, Text::Alignment horizontalAlignment, bool multiline = false)
{
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(height);
  label.SetTextColor(textColor);
  label.SetMultiLine(multiline);
  label.SetHorizontalTextAlignment(horizontalAlignment);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetBackgroundColor(backgroundColor);
  label.SetCornerRadius(7.0f);
  label.SetBorderlineWidth(1.0f);
  label.SetBorderlineOffset(-1.0f);
  label.SetBorderlineColor(borderlineColor);
  label.SetPadding(Extents(10.0f, 10.0f, 0.0f, 0.0f));
}

void ConfigurePreviewLabel(Label label, const UiColor& backgroundColor)
{
  label.SetFontSize(36.0f);
  label.SetTextColor(UiColor(0x111827));
  label.SetBackgroundColor(backgroundColor);
  label.SetPadding(Extents(14.0f, 14.0f, 12.0f, 12.0f));
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  label.SetMultiLine(true);
  label.SetCornerRadius(6.0f);
  label.SetBorderlineWidth(1.0f);
  label.SetBorderlineOffset(-1.0f);
  label.SetBorderlineColor(UiColor(0xCBD5E1));
  label.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
}

std::string HexColor(uint32_t value)
{
  char buffer[16];
  std::snprintf(buffer, sizeof(buffer), "#%06X", value & 0xFFFFFFu);
  return buffer;
}

std::string RangeText(uint32_t start, uint32_t end)
{
  std::ostringstream range;
  range << "[" << start << "," << end << ")";
  return range.str();
}

} // namespace

class StyledTextSampleController : public ConnectionTracker
{
public:
  explicit StyledTextSampleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &StyledTextSampleController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetPositionSize(PositionSize(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    window.SetBackgroundColor(UiColor(0xF8FAFC));

    mWindowWidth = static_cast<float>(WINDOW_WIDTH);

    mRoot = StackLayout::New(StackOrientation::VERTICAL);
    mRoot.SetSpacing(0.0f);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    mRoot.SetBackgroundColor(UiColor(0xF8FAFC));

    mHeaderRoot = AbsoluteLayout::New();
    mHeaderRoot.SetRequestedWidth(MATCH_PARENT);
    mHeaderRoot.SetRequestedHeight(HEADER_HEIGHT);
    mHeaderRoot.SetBackgroundColor(UiColor(0x111827));
    mHeaderRoot.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    mContentRoot = StackLayout::New(StackOrientation::VERTICAL);
    mContentRoot.SetSpacing(STACK_SPACING);
    mContentRoot.SetRequestedWidth(MATCH_PARENT);
    mContentRoot.SetRequestedHeight(MATCH_PARENT);
    mContentRoot.SetBackgroundColor(UiColor(0xF8FAFC));
    mContentRoot.SetPadding(Extents(CONTENT_PADDING, CONTENT_PADDING, CONTENT_PADDING, CONTENT_PADDING));
    mContentRoot.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    mFooterRoot = AbsoluteLayout::New();
    mFooterRoot.SetRequestedWidth(MATCH_PARENT);
    mFooterRoot.SetRequestedHeight(FOOTER_HEIGHT);
    mFooterRoot.SetBackgroundColor(UiColor(0x111827));
    mFooterRoot.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));

    CreateHeaderBadges();
    CreatePreviewLabels();
    CreateFooterBadges();

    mHeaderRoot.AddChildren({
      mCaseBadge,
      mPreviousBadge,
      mNextBadge,
      mSourceBadge,
      mResetBadge,
      mValueBadge,
      mRangeBadge,
      mClearBadge,
      mExpectedBadge,
    });

    mContentRoot.AddChildren({
      mSyncTitleBadge,
      mPreviewLabel,
      mAsyncTitleBadge,
      mAsyncPreviewLabel,
    });

    mFooterRoot.AddChildren({
      mMenuTitleLabel,
      mCaseListLabel,
      mHelpLabel,
      mViewHelpLabel,
    });

    mRoot.AddChildren({
      mHeaderRoot,
      mContentRoot,
      mFooterRoot,
    });

    ConnectHeaderActions();
    UpdateHeaderLayout();
    UpdateFooterLayout();

    window.Add(mRoot);
    window.KeyEventSignal().Connect(this, &StyledTextSampleController::OnKeyEvent);
    window.ResizedSignal().Connect(this, &StyledTextSampleController::OnWindowResized);

    ShowCase(0u);
  }

  void CreateHeaderBadges()
  {
    mCaseBadge = CreateLabel("", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mCaseBadge, HEADER_BADGE_HEIGHT, UiColor(0x1D4ED8), UiColor(0x93C5FD), UiColor(BADGE_ON_TEXT), Text::Alignment::START);

    mPreviousBadge = CreateLabel("PREV", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mPreviousBadge, HEADER_BADGE_HEIGHT, UiColor(0x1E3A8A), UiColor(0x93C5FD), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mNextBadge = CreateLabel("NEXT", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mNextBadge, HEADER_BADGE_HEIGHT, UiColor(0x1E3A8A), UiColor(0x93C5FD), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mSourceBadge = CreateLabel("", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mSourceBadge, HEADER_BADGE_HEIGHT, UiColor(BADGE_READY_BACKGROUND), UiColor(BADGE_READY_BORDER), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mResetBadge = CreateLabel("RESET", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mResetBadge, HEADER_BADGE_HEIGHT, UiColor(BADGE_WARN_BACKGROUND), UiColor(BADGE_WARN_BORDER), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mValueBadge = CreateLabel("", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mValueBadge, HEADER_BADGE_HEIGHT, UiColor(BADGE_APPLY_BACKGROUND), UiColor(BADGE_APPLY_BORDER), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mRangeBadge = CreateLabel("", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mRangeBadge, HEADER_BADGE_HEIGHT, UiColor(0x4C1D95), UiColor(0xC4B5FD), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mClearBadge = CreateLabel("", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mClearBadge, HEADER_BADGE_HEIGHT, UiColor(BADGE_CLEAR_BACKGROUND), UiColor(BADGE_CLEAR_BORDER), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mExpectedBadge = CreateLabel("", 12.0f, UiColor(BADGE_DISABLED_TEXT));
    ConfigureHudBadge(mExpectedBadge, HEADER_INFO_HEIGHT, UiColor(0x0F172A), UiColor(0x334155), UiColor(BADGE_DISABLED_TEXT), Text::Alignment::START, true);
  }

  void CreatePreviewLabels()
  {
    mSyncTitleBadge = CreateLabel("", 13.0f, UiColor(0x0F172A));
    ConfigureHudBadge(mSyncTitleBadge, PREVIEW_TITLE_HEIGHT, UiColor(0xE2E8F0), UiColor(0xCBD5E1), UiColor(0x0F172A), Text::Alignment::START);

    mPreviewLabel = CreateLabel("", 36.0f, UiColor(0x111827));
    ConfigurePreviewLabel(mPreviewLabel, UiColor(0xFFFFFF));
    mPreviewLabel.SetAsyncRendering(false);
    mPreviewLabel.SetAnchorColor(UiColor(0x0EA5E9));
    mPreviewLabel.SetAnchorClickedColor(UiColor(0xBE185D));
    mPreviewLabel.AnchorClickedSignal().Connect(this, &StyledTextSampleController::OnAnchorClicked);

    mAsyncTitleBadge = CreateLabel("", 13.0f, UiColor(0x0F172A));
    ConfigureHudBadge(mAsyncTitleBadge, PREVIEW_TITLE_HEIGHT, UiColor(0xDBEAFE), UiColor(0x93C5FD), UiColor(0x0F172A), Text::Alignment::START);

    mAsyncPreviewLabel = CreateLabel("", 36.0f, UiColor(0x111827));
    ConfigurePreviewLabel(mAsyncPreviewLabel, UiColor(0xEFF6FF));
    mAsyncPreviewLabel.SetAsyncRendering(true);
    mAsyncPreviewLabel.SetAnchorColor(UiColor(0x0EA5E9));
    mAsyncPreviewLabel.SetAnchorClickedColor(UiColor(0xBE185D));
    mAsyncPreviewLabel.AnchorClickedSignal().Connect(this, &StyledTextSampleController::OnAnchorClicked);
  }

  void CreateFooterBadges()
  {
    mMenuTitleLabel = CreateLabel("CONTROLS", 13.0f, UiColor(BADGE_ON_TEXT));
    ConfigureHudBadge(mMenuTitleLabel, FOOTER_BADGE_HEIGHT, UiColor(0x1D4ED8), UiColor(0x93C5FD), UiColor(BADGE_ON_TEXT), Text::Alignment::CENTER);

    mCaseListLabel = CreateLabel("", 12.0f, UiColor(BADGE_DISABLED_TEXT));
    ConfigureHudBadge(mCaseListLabel, FOOTER_BADGE_HEIGHT, UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT), Text::Alignment::START);

    mHelpLabel = CreateLabel(
      "ACTIONS  Click badges | V Value | R Range | C Clear | T Source | X Reset | ESC Quit",
      12.0f,
      UiColor(BADGE_DISABLED_TEXT));
    ConfigureHudBadge(mHelpLabel, FOOTER_LINE_HEIGHT, UiColor(0x0F172A), UiColor(0x334155), UiColor(BADGE_DISABLED_TEXT), Text::Alignment::START);

    mViewHelpLabel = CreateLabel(
      "VIEW  Left/Right Case | 1-9/0 Jump | Disabled badges are unavailable for the current case/source",
      12.0f,
      UiColor(BADGE_DISABLED_TEXT));
    ConfigureHudBadge(mViewHelpLabel, FOOTER_LINE_HEIGHT, UiColor(0x111827), UiColor(0x334155), UiColor(BADGE_DISABLED_TEXT), Text::Alignment::START);
    UpdateFooterAnchorStatus();
  }

  void ConnectHeaderActions()
  {
    mCaseBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      NextCase();
    });
    mPreviousBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      PreviousCase();
    });
    mNextBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      NextCase();
    });
    mSourceBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ToggleSource();
    });
    mResetBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ResetCurrentCase();
    });
    mValueBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ChangeCurrentValue();
    });
    mRangeBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ChangeCurrentRange();
    });
    mClearBadge.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      ClearCurrentSpans();
    });
  }

  void SetHudBadge(Label label, const std::string& text, const UiColor& backgroundColor, const UiColor& borderlineColor, const UiColor& textColor = UiColor(BADGE_ON_TEXT))
  {
    label.SetText(text.c_str());
    label.SetBackgroundColor(backgroundColor);
    label.SetBorderlineColor(borderlineColor);
    label.SetTextColor(textColor);
  }

  void SetActionBadge(Label label, const std::string& text, bool enabled, const UiColor& backgroundColor, const UiColor& borderlineColor)
  {
    if(enabled)
    {
      SetHudBadge(label, text, backgroundColor, borderlineColor, UiColor(BADGE_ON_TEXT));
    }
    else
    {
      SetHudBadge(label, text, UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
    }
    label.SetEnabled(enabled);
  }

  void SetBadgeBounds(Label label, float x, float y, float width, float height)
  {
    label.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, width, height)));
  }

  void UpdateHeaderLayout()
  {
    const float contentWidth = std::max(1.0f, mWindowWidth - (HEADER_PADDING * 2.0f));
    const float row1Y        = HEADER_PADDING;
    const float row2Y        = row1Y + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP;
    const float infoY        = row2Y + HEADER_BADGE_HEIGHT + HEADER_ROW_GAP;

    const float prevWidth   = 66.0f;
    const float nextWidth   = 66.0f;
    const float sourceWidth = 152.0f;
    const float resetWidth  = 88.0f;
    const float caseWidth   = std::max(180.0f, contentWidth - prevWidth - nextWidth - sourceWidth - resetWidth - (HEADER_ROW_GAP * 4.0f));

    float x = HEADER_PADDING;
    SetBadgeBounds(mCaseBadge, x, row1Y, caseWidth, HEADER_BADGE_HEIGHT);
    x += caseWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mPreviousBadge, x, row1Y, prevWidth, HEADER_BADGE_HEIGHT);
    x += prevWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mNextBadge, x, row1Y, nextWidth, HEADER_BADGE_HEIGHT);
    x += nextWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mSourceBadge, x, row1Y, sourceWidth, HEADER_BADGE_HEIGHT);
    x += sourceWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mResetBadge, x, row1Y, resetWidth, HEADER_BADGE_HEIGHT);

    const float actionWidth = (contentWidth - (HEADER_ROW_GAP * 2.0f)) / 3.0f;
    x = HEADER_PADDING;
    SetBadgeBounds(mValueBadge, x, row2Y, actionWidth, HEADER_BADGE_HEIGHT);
    x += actionWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mRangeBadge, x, row2Y, actionWidth, HEADER_BADGE_HEIGHT);
    x += actionWidth + HEADER_ROW_GAP;
    SetBadgeBounds(mClearBadge, x, row2Y, actionWidth, HEADER_BADGE_HEIGHT);

    SetBadgeBounds(mExpectedBadge, HEADER_PADDING, infoY, contentWidth, HEADER_INFO_HEIGHT);
  }

  void UpdateFooterLayout()
  {
    const float contentWidth = std::max(1.0f, mWindowWidth - (FOOTER_PADDING * 2.0f));
    const float caseX        = FOOTER_PADDING + FOOTER_TITLE_WIDTH + FOOTER_ROW_GAP;
    const float caseWidth    = std::max(1.0f, contentWidth - FOOTER_TITLE_WIDTH - FOOTER_ROW_GAP);
    const float helpY        = FOOTER_PADDING + FOOTER_BADGE_HEIGHT + FOOTER_ROW_GAP;
    const float viewY        = helpY + FOOTER_LINE_HEIGHT + FOOTER_ROW_GAP;

    mMenuTitleLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(FOOTER_PADDING, FOOTER_PADDING, FOOTER_TITLE_WIDTH, FOOTER_BADGE_HEIGHT)));
    mCaseListLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(caseX, FOOTER_PADDING, caseWidth, FOOTER_BADGE_HEIGHT)));
    mHelpLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(FOOTER_PADDING, helpY, contentWidth, FOOTER_LINE_HEIGHT)));
    mViewHelpLabel.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(FOOTER_PADDING, viewY, contentWidth, FOOTER_LINE_HEIGHT)));
  }

  void OnWindowResized(Window, Window::WindowSize windowSize)
  {
    mWindowWidth = static_cast<float>(windowSize.GetWidth());
    UpdateHeaderLayout();
    UpdateFooterLayout();
  }

  Text::ForegroundColorSpan NewForegroundColorSpan(std::size_t colorIndex) const
  {
    return Text::ForegroundColorSpan::New(UiColor(COLOR_VALUES[colorIndex % COLOR_VALUES.size()]));
  }

  Text::BackgroundColorSpan NewBackgroundColorSpan(std::size_t colorIndex) const
  {
    return Text::BackgroundColorSpan::New(UiColor(BACKGROUND_VALUES[colorIndex % BACKGROUND_VALUES.size()]));
  }

  Text::Underline::Type UnderlineTypeForValue(std::size_t valueIndex) const
  {
    switch(valueIndex % 3u)
    {
      case 1u:
        return Text::Underline::Type::DASHED;
      case 2u:
        return Text::Underline::Type::DOUBLE;
      case 0u:
      default:
        return Text::Underline::Type::SOLID;
    }
  }

  const char* UnderlineTypeName(std::size_t valueIndex) const
  {
    switch(valueIndex % 3u)
    {
      case 1u:
        return "dashed";
      case 2u:
        return "double";
      case 0u:
      default:
        return "solid";
    }
  }

  Text::UnderlineSpan NewUnderlineSpan(std::size_t colorIndex, Text::Underline::Type type = Text::Underline::Type::SOLID) const
  {
    Text::Underline underline;
    underline.SetColor(UiColor(COLOR_VALUES[colorIndex % COLOR_VALUES.size()]));
    underline.SetThickness(type == Text::Underline::Type::DOUBLE ? 2.0f : 2.5f);
    underline.SetType(type);
    underline.SetDashLength(6.0f);
    underline.SetDashGap(3.0f);
    return Text::UnderlineSpan::New(underline);
  }

  float LineThroughThicknessForValue(std::size_t valueIndex) const
  {
    switch(valueIndex % 4u)
    {
      case 1u:
        return 1.5f;
      case 2u:
        return 2.0f;
      case 3u:
        return 3.0f;
      case 0u:
      default:
        return 2.5f;
    }
  }

  const char* LineThroughThicknessName(std::size_t valueIndex) const
  {
    switch(valueIndex % 4u)
    {
      case 1u:
        return "1.5";
      case 2u:
        return "2.0";
      case 3u:
        return "3.0";
      case 0u:
      default:
        return "2.5";
    }
  }

  Text::LineThroughSpan NewLineThroughSpan(std::size_t colorIndex, float thickness = 2.5f) const
  {
    Text::LineThrough lineThrough;
    lineThrough.SetColor(UiColor(COLOR_VALUES[colorIndex % COLOR_VALUES.size()]));
    lineThrough.SetThickness(thickness);
    return Text::LineThroughSpan::New(lineThrough);
  }

  Text::FontWeight FontWeightForValue(std::size_t valueIndex) const
  {
    return (valueIndex % 2u == 0u) ? Text::FontWeight::BOLD : Text::FontWeight::NORMAL;
  }

  const char* FontWeightName(std::size_t valueIndex) const
  {
    return (valueIndex % 2u == 0u) ? "bold" : "normal";
  }

  Text::FontSlant FontSlantForValue(std::size_t valueIndex) const
  {
    return (valueIndex % 2u == 0u) ? Text::FontSlant::ITALIC : Text::FontSlant::NORMAL;
  }

  const char* FontSlantName(std::size_t valueIndex) const
  {
    return (valueIndex % 2u == 0u) ? "italic" : "normal";
  }

  float FontSizeForValue(std::size_t valueIndex) const
  {
    return FONT_SIZE_VALUES[valueIndex % FONT_SIZE_VALUES.size()];
  }

  std::string FontSizeText(std::size_t valueIndex) const
  {
    std::ostringstream text;
    text << FontSizeForValue(valueIndex) << "px";
    return text.str();
  }

  Text::FontSpan NewFontSpan(const Text::FontAttributes& attributes) const
  {
    return Text::FontSpan::New(attributes);
  }

  Text::AnchorSpan NewAnchorSpan(const char* href, bool explicitColor) const
  {
    Text::AnchorAttributes attributes;
    attributes.SetHref(href);
    if(explicitColor)
    {
      attributes.SetColor(UiColor(COLOR_VALUES[mColorIndex % COLOR_VALUES.size()]));
      attributes.SetClickedColor(UiColor(0xDC2626));
    }
    return Text::AnchorSpan::New(attributes);
  }

  std::size_t CurrentUnderlineValueIndex() const
  {
    return mDecorationIndex + 1u;
  }

  std::size_t CurrentLineThroughValueIndex() const
  {
    return mDecorationIndex;
  }

  std::size_t CurrentFontValueIndex() const
  {
    return mDecorationIndex;
  }

  std::pair<uint32_t, uint32_t> CurrentRange(uint32_t baseStart, uint32_t baseEnd, uint32_t altStart, uint32_t altEnd) const
  {
    return mRangeVariant ? std::make_pair(altStart, altEnd) : std::make_pair(baseStart, baseEnd);
  }

  std::size_t LaterColorIndex() const
  {
    return (mColorIndex + 2u) % COLOR_VALUES.size();
  }

  std::size_t LaterBackgroundIndex() const
  {
    return (mBackgroundIndex + 2u) % BACKGROUND_VALUES.size();
  }

  Text::StyledText FinishBuilder(Text::StyledTextBuilder builder, PreviewState& state) const
  {
    if(mCleared)
    {
      builder.ClearSpans();
      state.spanMode  = "cleared from " + state.spanMode;
      state.rangeInfo = "range: cleared after " + state.rangeInfo;
      state.valueInfo = "value: no active span";
    }
    return builder.Build();
  }

  Text::StyledText BuildCurrentStyledText(PreviewState& state)
  {
    switch(CurrentCase().type)
    {
      case StyledTextCase::PLAIN_TEXT:
      {
        state.text      = "StyledText Span Example";
        state.spanMode  = "none";
        state.rangeInfo = "range: n/a";
        state.valueInfo = "value: Label default color";
        return Text::StyledText::New(state.text.c_str());
      }
      case StyledTextCase::FOREGROUND_COLOR_SPAN_SINGLE:
      {
        state.text = "Hello StyledText ForegroundColorSpan";
        auto range = CurrentRange(6u, 16u, 0u, 5u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewForegroundColorSpan(mColorIndex), range.first, range.second);
        state.spanMode  = "ForegroundColorSpan single";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentColorValueInfo("value");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_COLOR_SPAN_MULTIPLE:
      {
        state.text = "Multiple ForegroundColorSpan ranges";
        auto secondRange = CurrentRange(9u, 18u, 16u, 24u);
        const std::size_t firstIndex  = mColorIndex;
        const std::size_t secondIndex = (mColorIndex + 2u) % COLOR_VALUES.size();
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewForegroundColorSpan(firstIndex), 0u, 8u);
        builder.SetSpan(NewForegroundColorSpan(secondIndex), secondRange.first, secondRange.second);
        state.spanMode  = "ForegroundColorSpan multiple objects";
        state.rangeInfo = "ranges: " + RangeText(0u, 8u) + ", " + RangeText(secondRange.first, secondRange.second);
        state.valueInfo = std::string("value: ") + COLOR_NAMES[firstIndex] + " + " + COLOR_NAMES[secondIndex];
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_COLOR_SPAN_SAME_OBJECT_UPDATE:
      {
        state.text = "Same object updates range";
        auto finalRange = CurrentRange(5u, 11u, 12u, 17u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        Text::ForegroundColorSpan         span    = NewForegroundColorSpan(mColorIndex);
        builder.SetSpan(span, 0u, 4u);
        builder.SetSpan(span, finalRange.first, finalRange.second);
        state.spanMode  = "same ForegroundColorSpan object update";
        state.rangeInfo = "initial " + RangeText(0u, 4u) + ", final " + RangeText(finalRange.first, finalRange.second);
        state.valueInfo = CurrentColorValueInfo("value");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_COLOR_SPAN_OVERLAP:
      {
        state.text = "Overlap later wins sample";
        auto laterRange = CurrentRange(8u, 20u, 3u, 18u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(0xEF4444)), 0u, 14u);
        builder.SetSpan(NewForegroundColorSpan(LaterColorIndex()), laterRange.first, laterRange.second);
        state.spanMode  = "overlap ForegroundColorSpan";
        state.rangeInfo = "order: red " + RangeText(0u, 14u) + ", later " + RangeText(laterRange.first, laterRange.second);
        state.valueInfo = CurrentColorValueInfo("later value", LaterColorIndex());
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_COLOR_SPAN_CLEAR:
      {
        state.text = "Clear spans returns plain StyledText";
        auto range = CurrentRange(0u, 11u, 12u, 23u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewForegroundColorSpan(mColorIndex), range.first, range.second);
        state.spanMode  = "ForegroundColorSpan before clear";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentColorValueInfo("value");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_COLOR_SPAN_CHANGE_VALUE:
      {
        state.text = "Change ForegroundColorSpan value";
        auto range = CurrentRange(7u, 16u, 0u, 6u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewForegroundColorSpan(mColorIndex), range.first, range.second);
        state.spanMode  = "ForegroundColorSpan value update";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentColorValueInfo("value");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_COLOR_SPAN_CHANGE_RANGE:
      {
        state.text = "Change span range sample";
        auto range = CurrentRange(7u, 17u, 0u, 6u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewForegroundColorSpan(mColorIndex), range.first, range.second);
        state.spanMode  = "ForegroundColorSpan range update";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentColorValueInfo("value");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::BACKGROUND_COLOR_SPAN_SINGLE:
      {
        state.text = "BackgroundColorSpan sample";
        auto range = CurrentRange(0u, 20u, 9u, 26u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewBackgroundColorSpan(mBackgroundIndex), range.first, range.second);
        state.spanMode  = "BackgroundColorSpan single";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentBackgroundValueInfo("value");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::BACKGROUND_COLOR_SPAN_OVERLAP:
      {
        state.text = "Background overlap sample";
        auto laterRange = CurrentRange(10u, 24u, 3u, 21u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(Text::BackgroundColorSpan::New(UiColor(0xFEF08A)), 0u, 16u);
        builder.SetSpan(NewBackgroundColorSpan(LaterBackgroundIndex()), laterRange.first, laterRange.second);
        state.spanMode  = "overlap BackgroundColorSpan";
        state.rangeInfo = "order: yellow " + RangeText(0u, 16u) + ", later " + RangeText(laterRange.first, laterRange.second);
        state.valueInfo = CurrentBackgroundValueInfo("later value", LaterBackgroundIndex());
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_AND_BACKGROUND_COLOR_SPAN:
      {
        state.text = "Foreground background";
        auto range = CurrentRange(0u, 10u, 11u, 21u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewForegroundColorSpan(mColorIndex), range.first, range.second);
        builder.SetSpan(NewBackgroundColorSpan(mBackgroundIndex), range.first, range.second);
        state.spanMode  = "ForegroundColorSpan + BackgroundColorSpan";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentColorValueInfo("fg") + " | " + CurrentBackgroundValueInfo("bg");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::UNDERLINE_SPAN_SINGLE:
      {
        state.text = "UnderlineSpan sample";
        auto range = CurrentRange(0u, 13u, 14u, 20u);
        const std::size_t valueIndex = CurrentUnderlineValueIndex();
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewUnderlineSpan(valueIndex, UnderlineTypeForValue(valueIndex)), range.first, range.second);
        state.spanMode  = "UnderlineSpan single";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentUnderlineValueInfo("value", valueIndex);
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::LINE_THROUGH_SPAN_SINGLE:
      {
        state.text = "LineThroughSpan sample";
        auto range = CurrentRange(0u, 15u, 16u, 22u);
        const std::size_t valueIndex = CurrentLineThroughValueIndex();
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewLineThroughSpan(valueIndex, LineThroughThicknessForValue(valueIndex)), range.first, range.second);
        state.spanMode  = "LineThroughSpan single";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentLineThroughValueInfo("value", valueIndex);
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FOREGROUND_BACKGROUND_DECORATION_SPAN:
      {
        state.text = "Decorated StyledText";
        auto range = CurrentRange(0u, 9u, 10u, 20u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewForegroundColorSpan(mColorIndex), range.first, range.second);
        builder.SetSpan(NewBackgroundColorSpan(mBackgroundIndex), range.first, range.second);
        builder.SetSpan(NewUnderlineSpan(2u, Text::Underline::Type::DOUBLE), range.first, range.second);
        builder.SetSpan(NewLineThroughSpan(3u), range.first, range.second);
        state.spanMode  = "ForegroundColorSpan + BackgroundColorSpan + UnderlineSpan + LineThroughSpan";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentColorValueInfo("fg") + " | " + CurrentBackgroundValueInfo("bg") + " | fixed decorations";
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::LITERAL_MARKUP_TEXT:
      {
        state.text      = "<color value='red'>Markup Should Not Parse</color>";
        state.spanMode  = "none; StyledText source uses literal text";
        state.rangeInfo = "range: n/a";
        state.valueInfo = "value: literal markup text";
        return Text::StyledText::New(state.text.c_str());
      }
      case StyledTextCase::FONT_SPAN_WEIGHT:
      {
        state.text = "Weight span sample";
        auto range = CurrentRange(0u, 6u, 7u, 11u);
        const std::size_t valueIndex = CurrentFontValueIndex();
        Text::FontAttributes attributes;
        attributes.SetWeight(FontWeightForValue(valueIndex));
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewFontSpan(attributes), range.first, range.second);
        state.spanMode  = "FontSpan WEIGHT";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentFontValueInfo("value", valueIndex);
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FONT_SPAN_SLANT:
      {
        state.text = "Slant italic sample";
        auto range = CurrentRange(0u, 6u, 7u, 13u);
        const std::size_t valueIndex = CurrentFontValueIndex();
        Text::FontAttributes attributes;
        attributes.SetSlant(FontSlantForValue(valueIndex));
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewFontSpan(attributes), range.first, range.second);
        state.spanMode  = "FontSpan SLANT";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentFontValueInfo("value", valueIndex);
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FONT_SPAN_SIZE:
      {
        state.text = "Font size pixel span";
        auto range = CurrentRange(0u, 9u, 10u, 15u);
        const std::size_t valueIndex = CurrentFontValueIndex();
        Text::FontAttributes attributes;
        attributes.SetSize(FontSizeForValue(valueIndex));
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewFontSpan(attributes), range.first, range.second);
        state.spanMode  = "FontSpan SIZE";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentFontValueInfo("value", valueIndex);
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FONT_SPAN_COMBINED:
      {
        state.text = "Combined FontSpan sample";
        auto range = CurrentRange(0u, 8u, 9u, 17u);
        const std::size_t valueIndex = CurrentFontValueIndex();
        Text::FontAttributes attributes;
        attributes.SetFamily("Ubuntu Mono");
        attributes.SetSize(FontSizeForValue(valueIndex));
        attributes.SetWeight(FontWeightForValue(valueIndex));
        attributes.SetSlant(FontSlantForValue(valueIndex));
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewFontSpan(attributes), range.first, range.second);
        state.spanMode  = "FontSpan FAMILY + SIZE + WEIGHT + SLANT";
        state.rangeInfo = "range: " + RangeText(range.first, range.second);
        state.valueInfo = CurrentFontValueInfo("value", valueIndex) + " | family: Ubuntu Mono, fallback may apply by platform";
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FONT_SPAN_FIELD_MERGE:
      {
        state.text = "Family and weight merge";
        auto weightRange = CurrentRange(11u, 17u, 7u, 17u);
        const std::size_t valueIndex = CurrentFontValueIndex();
        Text::FontAttributes familyAttributes;
        familyAttributes.SetFamily("Ubuntu Mono");
        Text::FontAttributes weightAttributes;
        weightAttributes.SetWeight(FontWeightForValue(valueIndex));
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewFontSpan(familyAttributes), 0u, 17u);
        builder.SetSpan(NewFontSpan(weightAttributes), weightRange.first, weightRange.second);
        state.spanMode  = "FontSpan family-only + weight-only";
        state.rangeInfo = "family " + RangeText(0u, 17u) + ", weight " + RangeText(weightRange.first, weightRange.second);
        state.valueInfo = CurrentFontValueInfo("weight", valueIndex) + " | family: Ubuntu Mono, fallback may apply by platform";
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::FONT_SPAN_LATER_WEIGHT_WINS:
      {
        state.text = "Bold normal wins";
        auto normalRange = CurrentRange(5u, 11u, 0u, 4u);
        Text::FontAttributes boldAttributes;
        boldAttributes.SetWeight(Text::FontWeight::BOLD);
        Text::FontAttributes normalAttributes;
        normalAttributes.SetWeight(Text::FontWeight::NORMAL);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewFontSpan(boldAttributes), 0u, 16u);
        builder.SetSpan(NewFontSpan(normalAttributes), normalRange.first, normalRange.second);
        state.spanMode  = "FontSpan later explicit WEIGHT=NORMAL";
        state.rangeInfo = "bold " + RangeText(0u, 16u) + ", later normal " + RangeText(normalRange.first, normalRange.second);
        state.valueInfo = "value: inner range uses explicit normal";
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::ANCHOR_SPAN:
      {
        state.text = "Fallback explicit links";
        auto explicitRange = CurrentRange(9u, 17u, 18u, 23u);
        Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(state.text.c_str());
        builder.SetSpan(NewAnchorSpan("anchor://fallback", false), 0u, 8u);
        builder.SetSpan(NewAnchorSpan("anchor://explicit", true), explicitRange.first, explicitRange.second);
        state.spanMode  = "AnchorSpan fallback + explicit";
        state.rangeInfo = "fallback " + RangeText(0u, 8u) + ", explicit " + RangeText(explicitRange.first, explicitRange.second);
        state.valueInfo = "fallback: Label AnchorColor #0EA5E9 | explicit " + CurrentColorValueInfo("color");
        return FinishBuilder(builder, state);
      }
      case StyledTextCase::MARKUP_TO_STYLED_TEXT_BASIC:
      {
        state.text      = FROM_MARKUP_BASIC_TEXT;
        state.spanMode  = "StyledText::FromMarkup";
        state.rangeInfo = "markup converted to StyledText spans";
        state.valueInfo = "color/font/b/i/u/s/background";
        return Text::StyledText::FromMarkup(state.text.c_str());
      }
      case StyledTextCase::MARKUP_TO_STYLED_TEXT_ANCHOR_ENTITY:
      {
        state.text      = FROM_MARKUP_ANCHOR_ENTITY_TEXT;
        state.spanMode  = "StyledText::FromMarkup";
        state.rangeInfo = "anchor markup converted to AnchorSpan";
        state.valueInfo = "href entity decode + unquoted attrs";
        return Text::StyledText::FromMarkup(state.text.c_str());
      }
      default:
      {
        state.text      = CurrentCase().title;
        state.spanMode  = "disabled future span";
        state.rangeInfo = "range: n/a";
        state.valueInfo = "value: n/a";
        return Text::StyledText::New(state.text.c_str());
      }
    }
  }

  void ApplyCurrentCase()
  {
    PreviewState state;
    const StyledTextCaseInfo& info = CurrentCase();

    if(!info.enabled)
    {
      state.text      = std::string(info.title) + "\n\n" + info.description;
      state.spanMode  = "disabled future span";
      state.rangeInfo = "range: n/a";
      state.valueInfo = "value: n/a";
      ApplyPlainTextToPreviews(state.text.c_str());
    }
    else if(mPlainSource)
    {
      state.text      = CurrentDisplayText();
      state.spanMode  = "source: SetText() plain path";
      state.rangeInfo = "range: n/a on plain source";
      state.valueInfo = "value: no active StyledText spans";
      ApplyPlainTextToPreviews(state.text.c_str());
    }
    else
    {
      Text::StyledText styledText = BuildCurrentStyledText(state);
      ApplyStyledTextToPreviews(styledText);
    }

    UpdateBadges(state);
    LogStatus(state);
  }

  void ApplyPlainTextToPreviews(const Dali::String& text)
  {
    mPreviewLabel.SetAsyncRendering(false);
    mPreviewLabel.SetText(text);

    mAsyncPreviewLabel.SetAsyncRendering(true);
    mAsyncPreviewLabel.SetText(text);
  }

  void ApplyStyledTextToPreviews(const Text::StyledText& styledText)
  {
    mPreviewLabel.SetAsyncRendering(false);
    mPreviewLabel.SetStyledText(styledText);

    mAsyncPreviewLabel.SetAsyncRendering(true);
    mAsyncPreviewLabel.SetStyledText(styledText);
  }

  void UpdateBadges(const PreviewState& state)
  {
    const StyledTextCaseInfo& info = CurrentCase();

    std::ostringstream caseText;
    caseText << "CASE " << (mCaseIndex + 1u) << "/" << CASES.size() << "  " << info.title;
    SetHudBadge(mCaseBadge, caseText.str(), info.enabled ? UiColor(0x1D4ED8) : UiColor(BADGE_DISABLED_BACKGROUND), info.enabled ? UiColor(0x93C5FD) : UiColor(BADGE_DISABLED_BORDER), info.enabled ? UiColor(BADGE_ON_TEXT) : UiColor(BADGE_DISABLED_TEXT));
    mCaseBadge.SetEnabled(true);

    SetActionBadge(mPreviousBadge, "PREV", true, UiColor(0x1E3A8A), UiColor(0x93C5FD));
    SetActionBadge(mNextBadge, "NEXT", true, UiColor(0x1E3A8A), UiColor(0x93C5FD));
    SetActionBadge(mSourceBadge, SourceBadgeText(), CanToggleSource(), UiColor(BADGE_READY_BACKGROUND), UiColor(BADGE_READY_BORDER));
    SetActionBadge(mResetBadge, "RESET", true, UiColor(BADGE_WARN_BACKGROUND), UiColor(BADGE_WARN_BORDER));

    SetActionBadge(mValueBadge, ValueBadgeText(), CanChangeValue(), UiColor(BADGE_APPLY_BACKGROUND), UiColor(BADGE_APPLY_BORDER));
    SetActionBadge(mRangeBadge, mRangeVariant ? "RANGE Alt" : "RANGE Base", CanChangeRange(), UiColor(0x4C1D95), UiColor(0xC4B5FD));
    SetActionBadge(mClearBadge, ClearBadgeText(), CanClear(), UiColor(BADGE_CLEAR_BACKGROUND), UiColor(BADGE_CLEAR_BORDER));

    std::ostringstream expected;
    expected << info.description << "\n"
             << "Source: " << (mPlainSource ? "SetText" : "StyledText")
             << " | Sync+Async labels visible | " << state.spanMode
             << " | " << state.rangeInfo << " | " << state.valueInfo;
    SetHudBadge(mExpectedBadge, expected.str(), UiColor(0x0F172A), UiColor(0x334155), UiColor(BADGE_DISABLED_TEXT));

    const std::string sourceText = SourceTitleText();
    SetHudBadge(mSyncTitleBadge, "SYNC LABEL  SetAsyncRendering(false) | " + sourceText, UiColor(0xE2E8F0), UiColor(0xCBD5E1), UiColor(0x0F172A));
    std::string asyncTitle = "ASYNC LABEL  SetAsyncRendering(true) | " + sourceText;
    if(IsAnchorClickCase() && !mPlainSource)
    {
      asyncTitle += " | async anchor click enabled";
    }
    SetHudBadge(mAsyncTitleBadge, asyncTitle, UiColor(0xDBEAFE), UiColor(0x93C5FD), UiColor(0x0F172A));

    std::ostringstream caseList;
    caseList << "CASES  prev: " << CASES[(mCaseIndex + CASES.size() - 1u) % CASES.size()].title
             << " | current: " << info.title
             << " | next: " << CASES[(mCaseIndex + 1u) % CASES.size()].title;
    SetHudBadge(mCaseListLabel, caseList.str(), UiColor(BADGE_DISABLED_BACKGROUND), UiColor(BADGE_DISABLED_BORDER), UiColor(BADGE_DISABLED_TEXT));
    UpdateFooterAnchorStatus();
  }

  std::string ValueBadgeText() const
  {
    if(mCleared)
    {
      return "VALUE Cleared";
    }
    if(mPlainSource || !CurrentCase().enabled || CurrentCase().valueKind == StyledTextValueKind::NONE)
    {
      return "VALUE N/A";
    }

    switch(CurrentCase().valueKind)
    {
      case StyledTextValueKind::FOREGROUND:
      {
        const std::size_t index = UsesLaterForegroundValue() ? LaterColorIndex() : mColorIndex;
        return std::string("VALUE ") + COLOR_NAMES[index];
      }
      case StyledTextValueKind::BACKGROUND:
      {
        const std::size_t index = UsesLaterBackgroundValue() ? LaterBackgroundIndex() : mBackgroundIndex;
        return std::string("VALUE ") + BACKGROUND_NAMES[index];
      }
      case StyledTextValueKind::FOREGROUND_AND_BACKGROUND:
      {
        return std::string("VALUE ") + COLOR_NAMES[mColorIndex] + "/" + BACKGROUND_NAMES[mBackgroundIndex];
      }
      case StyledTextValueKind::UNDERLINE:
      {
        return std::string("VALUE ") + UnderlineValueText(CurrentUnderlineValueIndex());
      }
      case StyledTextValueKind::LINE_THROUGH:
      {
        return std::string("VALUE ") + LineThroughValueText(CurrentLineThroughValueIndex());
      }
      case StyledTextValueKind::FONT:
      {
        return std::string("VALUE ") + FontValueText(CurrentFontValueIndex());
      }
      case StyledTextValueKind::NONE:
      default:
      {
        return "VALUE N/A";
      }
    }
  }

  bool IsFromMarkupCase() const
  {
    return (CurrentCase().type == StyledTextCase::MARKUP_TO_STYLED_TEXT_BASIC) ||
           (CurrentCase().type == StyledTextCase::MARKUP_TO_STYLED_TEXT_ANCHOR_ENTITY);
  }

  bool IsAnchorClickCase() const
  {
    return (CurrentCase().type == StyledTextCase::ANCHOR_SPAN) ||
           (CurrentCase().type == StyledTextCase::MARKUP_TO_STYLED_TEXT_ANCHOR_ENTITY);
  }

  std::string SourceBadgeText() const
  {
    if(mPlainSource)
    {
      return "SOURCE SetText";
    }
    return IsFromMarkupCase() ? "SOURCE FromMarkup" : "SOURCE StyledText";
  }

  std::string SourceTitleText() const
  {
    if(mPlainSource)
    {
      return "SetText plain source";
    }
    return IsFromMarkupCase() ? "SetStyledText(StyledText::FromMarkup)" : "SetStyledText source";
  }

  std::string ClearBadgeText() const
  {
    if(mPlainSource || !CurrentCase().enabled || !CurrentCase().clearable)
    {
      return "CLEAR N/A";
    }
    return mCleared ? "CLEAR Done" : "CLEAR";
  }

  bool UsesLaterForegroundValue() const
  {
    return CurrentCase().type == StyledTextCase::FOREGROUND_COLOR_SPAN_OVERLAP;
  }

  bool UsesLaterBackgroundValue() const
  {
    return CurrentCase().type == StyledTextCase::BACKGROUND_COLOR_SPAN_OVERLAP;
  }

  bool CanChangeValue() const
  {
    const StyledTextCaseInfo& info = CurrentCase();
    return info.enabled && !mPlainSource && !mCleared && info.valueKind != StyledTextValueKind::NONE;
  }

  bool CanChangeRange() const
  {
    const StyledTextCaseInfo& info = CurrentCase();
    return info.enabled && !mPlainSource && !mCleared && info.rangeChange;
  }

  bool CanClear() const
  {
    const StyledTextCaseInfo& info = CurrentCase();
    return info.enabled && !mPlainSource && !mCleared && info.clearable;
  }

  bool CanToggleSource() const
  {
    const StyledTextCaseInfo& info = CurrentCase();
    return info.enabled && info.sourceToggle;
  }

  void ChangeCurrentValue()
  {
    if(!CanChangeValue())
    {
      return;
    }

    switch(CurrentCase().valueKind)
    {
      case StyledTextValueKind::FOREGROUND:
      {
        mColorIndex = (mColorIndex + 1u) % COLOR_VALUES.size();
        break;
      }
      case StyledTextValueKind::BACKGROUND:
      {
        mBackgroundIndex = (mBackgroundIndex + 1u) % BACKGROUND_VALUES.size();
        break;
      }
      case StyledTextValueKind::FOREGROUND_AND_BACKGROUND:
      {
        mColorIndex      = (mColorIndex + 1u) % COLOR_VALUES.size();
        mBackgroundIndex = (mBackgroundIndex + 1u) % BACKGROUND_VALUES.size();
        break;
      }
      case StyledTextValueKind::UNDERLINE:
      case StyledTextValueKind::LINE_THROUGH:
      case StyledTextValueKind::FONT:
      {
        mDecorationIndex = (mDecorationIndex + 1u) % COLOR_VALUES.size();
        break;
      }
      case StyledTextValueKind::NONE:
      default:
      {
        break;
      }
    }
    ApplyCurrentCase();
  }

  void ChangeCurrentRange()
  {
    if(!CanChangeRange())
    {
      return;
    }
    mRangeVariant = !mRangeVariant;
    ApplyCurrentCase();
  }

  void ClearCurrentSpans()
  {
    if(!CanClear())
    {
      return;
    }
    mCleared = true;
    ApplyCurrentCase();
  }

  void ToggleSource()
  {
    if(!CanToggleSource())
    {
      return;
    }
    mPlainSource = !mPlainSource;
    ApplyCurrentCase();
  }

  void ResetCurrentCase()
  {
    ResetCurrentCaseState();
    ApplyCurrentCase();
  }

  void ResetCurrentCaseState()
  {
    mColorIndex      = 0u;
    mBackgroundIndex = 0u;
    mDecorationIndex = 0u;
    mRangeVariant    = false;
    mCleared         = false;
    mPlainSource     = false;
    mLastAnchorHref.clear();
  }

  void ShowCase(std::size_t index)
  {
    mCaseIndex = index % CASES.size();
    ResetCurrentCaseState();
    ApplyCurrentCase();
  }

  void NextCase()
  {
    ShowCase(mCaseIndex + 1u);
  }

  void PreviousCase()
  {
    ShowCase((mCaseIndex + CASES.size() - 1u) % CASES.size());
  }

  std::string CurrentColorValueInfo(const char* label) const
  {
    return CurrentColorValueInfo(label, mColorIndex);
  }

  std::string CurrentColorValueInfo(const char* label, std::size_t colorIndex) const
  {
    const std::size_t index = colorIndex % COLOR_VALUES.size();
    return std::string(label) + ": " + COLOR_NAMES[index] + " (" + HexColor(COLOR_VALUES[index]) + ")";
  }

  std::string CurrentBackgroundValueInfo(const char* label) const
  {
    return CurrentBackgroundValueInfo(label, mBackgroundIndex);
  }

  std::string CurrentBackgroundValueInfo(const char* label, std::size_t colorIndex) const
  {
    const std::size_t index = colorIndex % BACKGROUND_VALUES.size();
    return std::string(label) + ": " + BACKGROUND_NAMES[index] + " (" + HexColor(BACKGROUND_VALUES[index]) + ")";
  }

  std::string UnderlineValueText(std::size_t valueIndex) const
  {
    const std::size_t colorIndex = valueIndex % COLOR_VALUES.size();
    return std::string(COLOR_NAMES[colorIndex]) + "/" + UnderlineTypeName(valueIndex);
  }

  std::string CurrentUnderlineValueInfo(const char* label, std::size_t valueIndex) const
  {
    const std::size_t colorIndex = valueIndex % COLOR_VALUES.size();
    return std::string(label) + ": " + COLOR_NAMES[colorIndex] + " " + UnderlineTypeName(valueIndex) + " (" + HexColor(COLOR_VALUES[colorIndex]) + ")";
  }

  std::string LineThroughValueText(std::size_t valueIndex) const
  {
    const std::size_t colorIndex = valueIndex % COLOR_VALUES.size();
    return std::string(COLOR_NAMES[colorIndex]) + "/thick " + LineThroughThicknessName(valueIndex);
  }

  std::string CurrentLineThroughValueInfo(const char* label, std::size_t valueIndex) const
  {
    const std::size_t colorIndex = valueIndex % COLOR_VALUES.size();
    return std::string(label) + ": " + COLOR_NAMES[colorIndex] + " thickness " + LineThroughThicknessName(valueIndex) + " (" + HexColor(COLOR_VALUES[colorIndex]) + ")";
  }

  std::string FontValueText(std::size_t valueIndex) const
  {
    switch(CurrentCase().type)
    {
      case StyledTextCase::FONT_SPAN_WEIGHT:
      case StyledTextCase::FONT_SPAN_FIELD_MERGE:
      {
        return FontWeightName(valueIndex);
      }
      case StyledTextCase::FONT_SPAN_SLANT:
      {
        return FontSlantName(valueIndex);
      }
      case StyledTextCase::FONT_SPAN_SIZE:
      {
        return FontSizeText(valueIndex);
      }
      case StyledTextCase::FONT_SPAN_COMBINED:
      {
        return std::string("Ubuntu Mono/") + FontSizeText(valueIndex) + "/" + FontWeightName(valueIndex) + "/" + FontSlantName(valueIndex);
      }
      case StyledTextCase::FONT_SPAN_LATER_WEIGHT_WINS:
      {
        return "inner normal";
      }
      case StyledTextCase::ANCHOR_SPAN:
      {
        return std::string("explicit ") + COLOR_NAMES[valueIndex % COLOR_VALUES.size()];
      }
      default:
      {
        return "font";
      }
    }
  }

  std::string CurrentFontValueInfo(const char* label, std::size_t valueIndex) const
  {
    return std::string(label) + ": " + FontValueText(valueIndex);
  }

  std::string CurrentDisplayText() const
  {
    switch(CurrentCase().type)
    {
      case StyledTextCase::PLAIN_TEXT:
        return "StyledText Span Example";
      case StyledTextCase::FOREGROUND_COLOR_SPAN_SINGLE:
        return "Hello StyledText ForegroundColorSpan";
      case StyledTextCase::FOREGROUND_COLOR_SPAN_MULTIPLE:
        return "Multiple ForegroundColorSpan ranges";
      case StyledTextCase::FOREGROUND_COLOR_SPAN_SAME_OBJECT_UPDATE:
        return "Same object updates range";
      case StyledTextCase::FOREGROUND_COLOR_SPAN_OVERLAP:
        return "Overlap later wins sample";
      case StyledTextCase::FOREGROUND_COLOR_SPAN_CLEAR:
        return "Clear spans returns plain StyledText";
      case StyledTextCase::FOREGROUND_COLOR_SPAN_CHANGE_VALUE:
        return "Change ForegroundColorSpan value";
      case StyledTextCase::FOREGROUND_COLOR_SPAN_CHANGE_RANGE:
        return "Change span range sample";
      case StyledTextCase::BACKGROUND_COLOR_SPAN_SINGLE:
        return "BackgroundColorSpan sample";
      case StyledTextCase::BACKGROUND_COLOR_SPAN_OVERLAP:
        return "Background overlap sample";
      case StyledTextCase::FOREGROUND_AND_BACKGROUND_COLOR_SPAN:
        return "Foreground background";
      case StyledTextCase::UNDERLINE_SPAN_SINGLE:
        return "UnderlineSpan sample";
      case StyledTextCase::LINE_THROUGH_SPAN_SINGLE:
        return "LineThroughSpan sample";
      case StyledTextCase::FOREGROUND_BACKGROUND_DECORATION_SPAN:
        return "Decorated StyledText";
      case StyledTextCase::LITERAL_MARKUP_TEXT:
        return "<color value='red'>Markup Should Not Parse</color>";
      case StyledTextCase::FONT_SPAN_WEIGHT:
        return "Weight span sample";
      case StyledTextCase::FONT_SPAN_SLANT:
        return "Slant italic sample";
      case StyledTextCase::FONT_SPAN_SIZE:
        return "Font size pixel span";
      case StyledTextCase::FONT_SPAN_COMBINED:
        return "Combined FontSpan sample";
      case StyledTextCase::FONT_SPAN_FIELD_MERGE:
        return "Family and weight merge";
      case StyledTextCase::FONT_SPAN_LATER_WEIGHT_WINS:
        return "Bold normal wins";
      case StyledTextCase::ANCHOR_SPAN:
        return "Fallback explicit links";
      case StyledTextCase::MARKUP_TO_STYLED_TEXT_BASIC:
        return FROM_MARKUP_BASIC_TEXT;
      case StyledTextCase::MARKUP_TO_STYLED_TEXT_ANCHOR_ENTITY:
        return FROM_MARKUP_ANCHOR_ENTITY_TEXT;
      default:
        return CurrentCase().title;
    }
  }

  const StyledTextCaseInfo& CurrentCase() const
  {
    return CASES[mCaseIndex % CASES.size()];
  }

  void JumpByNumericKey(const Dali::String& keyName)
  {
    const std::string key(keyName.CStr());
    if(key == "0")
    {
      ShowCase(9u);
    }
    else if(key.size() == 1u && key[0] >= '1' && key[0] <= '9')
    {
      ShowCase(static_cast<std::size_t>(key[0] - '1'));
    }
  }

  void OnAnchorClicked(View, const Dali::String& href)
  {
    mLastAnchorHref = std::string(href.CStr(), href.Size());
    std::printf("[text-styled-text-example] Anchor clicked href=%s\n", mLastAnchorHref.c_str());
    UpdateFooterAnchorStatus();
  }

  void UpdateFooterAnchorStatus()
  {
    std::string text = "VIEW  Left/Right Case | 1-9/0 Jump | Disabled badges are unavailable for the current case/source";
    if(!mLastAnchorHref.empty())
    {
      text = "ANCHOR CLICK  href=" + mLastAnchorHref;
    }
    SetHudBadge(mViewHelpLabel, text, UiColor(0x111827), UiColor(0x334155), UiColor(BADGE_DISABLED_TEXT));
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
    if(IsKey(event, Dali::DALI_KEY_CURSOR_LEFT))
    {
      PreviousCase();
    }
    else if(IsKey(event, Dali::DALI_KEY_CURSOR_RIGHT))
    {
      NextCase();
    }
    else if(keyName == "c" || keyName == "C")
    {
      ClearCurrentSpans();
    }
    else if(keyName == "t" || keyName == "T" || keyName == "s" || keyName == "S")
    {
      ToggleSource();
    }
    else if(keyName == "v" || keyName == "V")
    {
      ChangeCurrentValue();
    }
    else if(keyName == "r" || keyName == "R")
    {
      ChangeCurrentRange();
    }
    else if(keyName == "x" || keyName == "X")
    {
      ResetCurrentCase();
    }
    else
    {
      JumpByNumericKey(keyName);
    }
  }

  void LogStatus(const PreviewState& state) const
  {
    std::ostringstream status;
    status << "Case " << (mCaseIndex + 1u) << "/" << CASES.size() << ": " << CurrentCase().title << "\n";
    status << CurrentCase().description << "\n";
    status << "Source: " << (mPlainSource ? "SetText" : "StyledText")
           << " | Sync label: async=false | Async label: async=true\n";
    status << "Span mode: " << state.spanMode << "\n";
    status << state.rangeInfo << " | " << state.valueInfo;
    std::printf("[text-styled-text-example] %s\n", status.str().c_str());
  }

private:
  Application& mApplication;

  StackLayout    mRoot;
  AbsoluteLayout mHeaderRoot;
  StackLayout    mContentRoot;
  AbsoluteLayout mFooterRoot;

  Label mCaseBadge;
  Label mPreviousBadge;
  Label mNextBadge;
  Label mSourceBadge;
  Label mResetBadge;
  Label mValueBadge;
  Label mRangeBadge;
  Label mClearBadge;
  Label mExpectedBadge;

  Label mSyncTitleBadge;
  Label mPreviewLabel;
  Label mAsyncTitleBadge;
  Label mAsyncPreviewLabel;

  Label mMenuTitleLabel;
  Label mCaseListLabel;
  Label mHelpLabel;
  Label mViewHelpLabel;

  float       mWindowWidth{static_cast<float>(WINDOW_WIDTH)};
  std::size_t mCaseIndex{0u};
  std::size_t mColorIndex{0u};
  std::size_t mBackgroundIndex{0u};
  std::size_t mDecorationIndex{0u};
  std::string mLastAnchorHref;
  bool        mRangeVariant{false};
  bool        mCleared{false};
  bool        mPlainSource{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  StyledTextSampleController controller(application);
  application.MainLoop();

  return 0;
}
