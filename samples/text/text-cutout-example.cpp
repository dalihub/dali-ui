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
 */

// EXTERNAL INCLUDES
#include <dali/devel-api/adaptor-framework/application.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float       GAP            = 8.f;
constexpr float       BUTTON_HEIGHT  = 52.f;
constexpr unsigned    STRIP_COUNT    = 8;
constexpr uint32_t    STRIP_COLORS[] = {0xFF557A, 0xFF9966, 0xF5CE68, 0x61D8AA, 0x43BED9, 0x637FE8, 0xA176E8, 0xE479C5};
constexpr float       ALPHAS[]       = {0.f, .5f, 1.f};
constexpr float       FONT_SIZES[]   = {32.f, 60.f, 84.f};
constexpr uint32_t    COLORS[]       = {0x17243B, 0x88394F, 0x165C55, 0x304D88};
constexpr const char* COLOR_NAMES[]  = {"Navy", "Rose", "Forest", "Blue"};

// Content pages contain strings only. Layout, effects and rendering settings
// belong to the playground, and changing a page must not replace them.
struct Content
{
  const char* name;
  const char* text;
  const char* hint;
};
constexpr Content CONTENTS[] = {
  {"Headline", "CUTOUT", "Switch text pages; keep your settings."},
  {"Sentence", "Let the colors shine through.", "Try Auto fit to keep the full sentence visible."},
  {"Paragraph", "A moving world of color lives behind these letters. Change the text, keep your settings, and see how each shape opens a different view.", "Try Multiline for this longer paragraph."},
  {"Line breaks", "MAKE\nROOM FOR\nCOLOR", "Explicit line breaks follow the Multiline setting."},
  {"Numbers", "0123456789  12:34  98.6%", "Digits, a clock and a percentage."},
  {"Symbols", "!? @#$% &*() [] {} +−×÷", "Punctuation, brackets and mathematical symbols."},
  {"Korean", "빛으로 채우는 글자\n안녕하세요, DALi!", "Korean text with an explicit line break."},
  {"Japanese", "文字から広がる世界\nこんにちは DALi", "Japanese text with an explicit line break."},
  {"RTL / BiDi", "مرحبا بالعالم  שלום  DALi 123", "Arabic, Hebrew and Latin in the same string."},
  {"Mixed languages", "Hello · 안녕 · こんにちは · مرحبا", "Several writing systems with the same settings."},
  {"Emoji", "Hello 😀 🌍 ✨ DALi", "Emoji appearance depends on the installed fonts."},
  {"Long word", "Supercalifragilisticexpialidocious", "Try Auto fit or change the overflow setting."},
  {"Spaces", "SPACE   BETWEEN   WORDS", "Repeated spaces are part of this string."},
  {"Empty string", "", "No glyphs in this page. All settings are still kept."},
};
constexpr unsigned CONTENT_COUNT = sizeof(CONTENTS) / sizeof(CONTENTS[0]);

// Keep action names distinct from platform macros such as OVERFLOW.
enum Action : unsigned
{
  CUTOUT_ACTION_PREVIOUS,
  CUTOUT_ACTION_NEXT,
  CUTOUT_ACTION_CUTOUT,
  CUTOUT_ACTION_ASYNC,
  CUTOUT_ACTION_TEXT_ALPHA,
  CUTOUT_ACTION_BG_ALPHA,
  CUTOUT_ACTION_BACKGROUND,
  CUTOUT_ACTION_SHADOW,
  CUTOUT_ACTION_OUTLINE,
  CUTOUT_ACTION_UNDERLINE,
  CUTOUT_ACTION_BG_COLOR,
  CUTOUT_ACTION_FONT,
  CUTOUT_ACTION_FIT,
  CUTOUT_ACTION_MULTILINE,
  CUTOUT_ACTION_MOTION,
  CUTOUT_ACTION_MORE,
  CUTOUT_ACTION_SIZE,
  CUTOUT_ACTION_HORIZONTAL,
  CUTOUT_ACTION_VERTICAL,
  CUTOUT_ACTION_PADDING,
  CUTOUT_ACTION_MAX_LINES,
  CUTOUT_ACTION_OVERFLOW,
  CUTOUT_ACTION_RESET,
  CUTOUT_ACTION_COUNT
};

Label MakeLabel(const char* text, float fontSize = 14.f)
{
  auto label = Label::New(text);
  label.SetUiScalePolicy(UiScalePolicy::DISABLED);
  label.SetFontFamily("SamsungOneUI_600");
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(0xF4F7FB));
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

void Place(View view, float x, float y, float width, float height)
{
  view.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(x, y, width, height)));
}
} // namespace

class TextCutoutController : public ConnectionTracker
{
public:
  explicit TextCutoutController(Application& application)
  : mApplication(application)
  {
    application.InitSignal().Connect(this, &TextCutoutController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    auto window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x0C1424));
    mRoot = AbsoluteLayout::New();
    mRoot.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mRoot.SetRequestedWidth(MATCH_PARENT);
    mRoot.SetRequestedHeight(MATCH_PARENT);
    window.Add(mRoot);
    mTitle = MakeLabel("Text cutout", 26.f);
    mGuide = MakeLabel("", 12.f);
    mGuide.SetTextColor(UiColor(0xA8BDD4));
    mPage = MakeLabel("");
    mPage.SetMultiLine(true);
    mPage.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mPage.SetTextFit(Text::Fit::Range(11.f, 16.f, 1.f));
    mStatus = MakeLabel("", 12.f);
    mStatus.SetMultiLine(true);
    mStatus.SetTextFit(Text::Fit::Range(10.f, 12.f, 1.f));
    mPreview = AbsoluteLayout::New();
    mPreview.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
    for(unsigned i = 0; i < STRIP_COUNT * 2u; ++i)
    {
      auto strip = View::New();
      mStrips.push_back(strip);
      mPreview.Add(strip);
    }
    for(unsigned i = 0; i < 5u; ++i)
    {
      auto orb = View::New();
      orb.SetBackgroundColor(UiColor(STRIP_COLORS[(i * 3u + 2u) % STRIP_COUNT]).WithAlpha(.72f));
      orb.SetCornerRadiusPolicyRelative();
      orb.SetCornerRadius(.5f);
      mOrbs.push_back(orb);
      mPreview.Add(orb);
    }
    mCard  = AbsoluteLayout::New();
    mLabel = Label::New();
    mLabel.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mLabel.SetFontFamily("SamsungOneUI_600");
    mLabel.SetCornerRadius(24.f);
    mCard.Add(mLabel);
    // This mask only rounds the preview panel; it is not the text cutout.
    mCardMask = View::New();
    mCardMask.SetBackgroundColor(UiColor(Color::WHITE));
    mCardMask.SetCornerRadius(24.f);
    mCard.Add(mCardMask);
    mCard.SetRenderEffect(MaskEffect::New(mCardMask));
    mPreview.Add(mCard);
    mControls = AbsoluteLayout::New();
    mScroll   = ScrollView::New();
    mScroll.SetBackgroundColor(UiColor(0x142036));
    mScroll.SetCornerRadius(16.f);
    mScroll.SetScrollDirection(ScrollDirection::Vertical);
    mScroll.SetContent(mControls);
    for(const char* title : {"RENDERING", "APPEARANCE", "TEXT & PREVIEW"})
    {
      auto heading = MakeLabel(title, 12.f);
      heading.SetTextColor(UiColor(0x91ABC8));
      mHeadings.push_back(heading);
      mControls.Add(heading);
    }
    for(unsigned i = 0; i < CUTOUT_ACTION_COUNT; ++i)
    {
      auto button = MakeLabel("");
      button.SetMultiLine(true);
      button.SetTextFit(Text::Fit::Range(11.f, 14.f, 1.f));
      button.SetPadding(Insets(10, 10, 2, 2));
      button.SetCornerRadius(10.f);
      button.SetBorderlineOffset(-1.f);
      button.SetStateEffect(OverlayEffect::Plain());
      // One activation path handles pointer and keyboard input. Selection is
      // derived from the setting, not from which button was clicked last.
      button.SetFocusable(false);
      button.AsSelectable().SetToggleByClickEnabled(false);
      button.AsInteractive().ClickedSignal().Connect(this, [this, i](View, InputEvent) -> bool
      {
        mSelection = static_cast<Action>(i);
        Activate(mSelection);
        return true;
      });
      mButtons.push_back(button);
      if(i == CUTOUT_ACTION_PREVIOUS || i == CUTOUT_ACTION_NEXT || i == CUTOUT_ACTION_RESET)
      {
        button.SetMultiLine(false);
        button.SetPadding(Insets(4, 4, 0, 0));
        button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
        mRoot.Add(button);
      }
      else
      {
        mControls.Add(button);
      }
    }
    mRoot.Add(mTitle);
    mRoot.Add(mGuide);
    mRoot.Add(mPage);
    mRoot.Add(mPreview);
    mRoot.Add(mStatus);
    mRoot.Add(mScroll);
    window.KeyEventSignal().Connect(this, &TextCutoutController::OnKeyEvent);
    window.ResizedSignal().Connect(this, &TextCutoutController::OnResize);
    const auto bounds = window.GetPositionSize();
    mWidth            = static_cast<float>(bounds.width);
    mHeight           = static_cast<float>(bounds.height);
    ResetSettings();
    RefreshControls();
  }

  void ResetSettings()
  {
    mAsync  = false;
    mCutout = true;
    mShadow = mOutline = mUnderline = false;
    mTextAlpha = mBgColor = 0;
    mMaxLines             = 0;
    mBgAlpha              = 2;
    mHasBackground = mAnimating = mEllipsis = true;
    mMultiline = mFit = false;
    mHorizontal = mVertical = mPadding = 1;
    mSize                              = 2;
    mFont                              = mWidth < 600.f ? 0u : 2u;
    mLabel.SetAsyncRendering(mAsync);
    mLabel.SetTextCutoutEnabled(mCutout);
    mLabel.SetFontSize(FONT_SIZES[mFont]);
    mLabel.SetMultiLine(mMultiline);
    mLabel.SetMaximumLines(mMaxLines);
    mLabel.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    mLabel.SetLineWrapMode(Text::LineWrapMode::WORD);
    mLabel.SetTextFit(Text::Fit::None());
    mLabel.SetPadding(Insets(12, 12, 12, 12));
    mLabel.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    mLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
    ApplyBackground();
    ApplyTextColor();
    ApplyStyle();
    ApplyContent();
    Layout();
  }

  void ApplyContent()
  {
    // Deliberately do not apply page-specific presets or reset any property.
    mLabel.SetText(CONTENTS[mContent].text);
  }

  void ApplyTextColor()
  {
    mLabel.SetTextColor(UiColor(Color::WHITE).WithAlpha(ALPHAS[mTextAlpha]));
  }

  void ApplyBackground()
  {
    if(mHasBackground)
    {
      mLabel.SetBackgroundColor(UiColor(COLORS[mBgColor]).WithAlpha(ALPHAS[mBgAlpha]));
    }
    else
    {
      mLabel.ClearBackground();
    }
  }

  void ApplyStyle()
  {
    Text::Shadow shadow;
    shadow.SetColor(UiColor(0xF79D32));
    shadow.SetOffset(Vector2(5.f, 5.f));
    shadow.SetBlurRadius(3.f);
    Text::Outline outline;
    outline.SetColor(UiColor(0x56E39F));
    outline.SetWidth(2.f);
    Text::Underline underline;
    underline.SetThickness(2.f);
    mLabel.SetTextShadow(mShadow ? shadow : Text::Shadow::None());
    mLabel.SetTextOutline(mOutline ? outline : Text::Outline::None());
    mLabel.SetTextUnderline(mUnderline ? underline : Text::Underline::None());
  }

  bool IsAvailable(Action action) const
  {
    return !(action == CUTOUT_ACTION_FONT && mFit) && !((action == CUTOUT_ACTION_BG_COLOR || action == CUTOUT_ACTION_BG_ALPHA) && !mHasBackground) &&
           !(action == CUTOUT_ACTION_MAX_LINES && !mMultiline);
  }

  void Activate(Action action)
  {
    if(!IsAvailable(action)) return;
    switch(action)
    {
      case CUTOUT_ACTION_PREVIOUS:
        mContent = (mContent + CONTENT_COUNT - 1) % CONTENT_COUNT;
        ApplyContent();
        break;
      case CUTOUT_ACTION_NEXT:
        mContent = (mContent + 1) % CONTENT_COUNT;
        ApplyContent();
        break;
      case CUTOUT_ACTION_CUTOUT:
        mCutout = !mCutout;
        mLabel.SetTextCutoutEnabled(mCutout);
        break;
      case CUTOUT_ACTION_ASYNC:
        mAsync = !mAsync;
        mLabel.SetAsyncRendering(mAsync);
        break;
      case CUTOUT_ACTION_TEXT_ALPHA:
        mTextAlpha = (mTextAlpha + 1) % 3;
        ApplyTextColor();
        break;
      case CUTOUT_ACTION_BG_ALPHA:
        mBgAlpha = (mBgAlpha + 1) % 3;
        ApplyBackground();
        break;
      case CUTOUT_ACTION_BACKGROUND:
        mHasBackground = !mHasBackground;
        ApplyBackground();
        break;
      case CUTOUT_ACTION_SHADOW:
        mShadow = !mShadow;
        ApplyStyle();
        break;
      case CUTOUT_ACTION_OUTLINE:
        mOutline = !mOutline;
        ApplyStyle();
        break;
      case CUTOUT_ACTION_UNDERLINE:
        mUnderline = !mUnderline;
        ApplyStyle();
        break;
      case CUTOUT_ACTION_BG_COLOR:
        mBgColor = (mBgColor + 1) % 4;
        ApplyBackground();
        break;
      case CUTOUT_ACTION_FONT:
        mFont = (mFont + 1) % 3;
        mLabel.SetFontSize(FONT_SIZES[mFont]);
        break;
      case CUTOUT_ACTION_FIT:
        mFit = !mFit;
        if(mFit)
        {
          mLabel.SetTextFit(Text::Fit::Range(16.f, 84.f, 2.f));
        }
        else
        {
          mLabel.SetTextFit(Text::Fit::None());
          // Return to the user's chosen font size when Auto fit is turned off.
          mLabel.SetFontSize(FONT_SIZES[mFont]);
        }
        break;
      case CUTOUT_ACTION_MULTILINE:
        mMultiline = !mMultiline;
        mLabel.SetMultiLine(mMultiline);
        break;
      case CUTOUT_ACTION_MOTION:
        mAnimating = !mAnimating;
        if(mAnimating)
          mMotion.Play();
        else
          mMotion.Pause();
        break;
      case CUTOUT_ACTION_MORE:
        mMore = !mMore;
        Layout(false);
        break;
      case CUTOUT_ACTION_SIZE:
        mSize = (mSize + 1) % 3;
        Layout();
        break;
      case CUTOUT_ACTION_HORIZONTAL:
        mHorizontal = (mHorizontal + 1) % 3;
        mLabel.SetHorizontalTextAlignment(static_cast<Text::Alignment>(mHorizontal));
        break;
      case CUTOUT_ACTION_VERTICAL:
        mVertical = (mVertical + 1) % 3;
        mLabel.SetVerticalTextAlignment(static_cast<Text::Alignment>(mVertical));
        break;
      case CUTOUT_ACTION_PADDING:
      {
        mPadding            = (mPadding + 1) % 3;
        const float padding = static_cast<float>(mPadding) * 12.f;
        mLabel.SetPadding(Insets(padding, padding, padding, padding));
        break;
      }
      case CUTOUT_ACTION_MAX_LINES:
        mMaxLines = (mMaxLines + 1) % 4;
        mLabel.SetMaximumLines(mMaxLines);
        break;
      case CUTOUT_ACTION_OVERFLOW:
        mEllipsis = !mEllipsis;
        mLabel.SetTextOverflowMode(mEllipsis ? Text::OverflowMode::ELLIPSIS : Text::OverflowMode::CLIP);
        break;
      case CUTOUT_ACTION_RESET:
        ResetSettings();
        break;
      default:
        break;
    }
    RefreshControls();
  }

  void LayoutPreview(float width, float height)
  {
    const float sizeStep    = static_cast<float>(mSize);
    const float labelWidth  = width * (.5f + sizeStep * .16f);
    const float labelHeight = height * (.46f + sizeStep * .14f);
    Place(mCard, (width - labelWidth) * .5f, (height - labelHeight) * .5f, labelWidth, labelHeight);
    Place(mLabel, 0, 0, labelWidth, labelHeight);
    Place(mCardMask, 0, 0, labelWidth, labelHeight);
    if(mMotion)
    {
      mMotion.Stop();
      mMotion.Clear();
    }
    mMotion                = Animation::New(18.f);
    const float stripWidth = width / STRIP_COUNT;
    for(unsigned i = 0; i < mStrips.size(); ++i)
    {
      const float stripX = static_cast<float>(i) * stripWidth;
      Place(mStrips[i], stripX, 0, stripWidth + 1.f, height);
      mStrips[i].SetBackgroundColor(UiColor(STRIP_COLORS[i % STRIP_COUNT]));
      auto travel = KeyFrames::New();
      travel.Add(0.f, stripX);
      travel.Add(1.f, stripX - width);
      mMotion.AnimateBetween(Property(mStrips[i], Actor::Property::POSITION_X), travel);
    }
    for(unsigned i = 0; i < mOrbs.size(); ++i)
    {
      const float orbIndex = static_cast<float>(i);
      const float diameter = height * (.32f + .06f * orbIndex);
      Place(mOrbs[i], 0, 0, diameter, diameter);
      auto x = KeyFrames::New();
      auto y = KeyFrames::New();
      for(unsigned step = 0; step <= 8u; ++step)
      {
        const float progress = static_cast<float>(step) / 8.f;
        const float phase    = progress * 6.283185304f + orbIndex * 1.256637061f;
        x.Add(progress, (width - diameter) * (.5f + .46f * std::sin(phase)));
        y.Add(progress, (height - diameter) * (.5f + .45f * std::cos(phase)));
      }
      mMotion.AnimateBetween(Property(mOrbs[i], Actor::Property::POSITION_X), x);
      mMotion.AnimateBetween(Property(mOrbs[i], Actor::Property::POSITION_Y), y);
    }
    mMotion.SetLoopCount(0);
    if(mAnimating) mMotion.Play();
  }

  void Layout(bool layoutPreview = true)
  {
    const float margin        = mWidth < 600.f ? 16.f : 24.f;
    const float width         = std::max(1.f, mWidth - 2.f * margin);
    const bool  wide          = mWidth >= 760.f && mHeight >= 360.f;
    const bool  shortWindow   = mHeight < 540.f;
    const float panelWidth    = wide ? std::min(500.f, width * .41f) : width;
    const float previewWidth  = wide ? width - panelWidth - 20.f : width;
    const float navY          = shortWindow ? 64.f : 82.f;
    const float previewY      = navY + BUTTON_HEIGHT + GAP;
    const float previewHeight = wide ? std::max(140.f, mHeight - previewY - 88.f)
                                     : std::clamp(mHeight * .27f, 88.f, 220.f);
    Place(mTitle, margin, 14, std::max(1.f, width - 116.f), 32);
    Place(mButtons[CUTOUT_ACTION_RESET], mWidth - margin - 108.f, 12, 108, 40);
    mGuide.SetText(mWidth < 600.f ? "Teal = ON  |  Arrows + Enter  |  [ / ] text"
                                  : "Teal = ON  |  Arrows + Enter  |  [ / ] text  |  C cutout  S async  A text opacity  G panel opacity  R reset");
    Place(mGuide, margin, 48, width, shortWindow ? 14.f : 24.f);
    const float navWidth = mWidth < 600.f ? 62.f : 88.f;
    Place(mButtons[CUTOUT_ACTION_PREVIOUS], margin, navY, navWidth, BUTTON_HEIGHT);
    Place(mPage, margin + navWidth + GAP, navY, std::max(1.f, previewWidth - 2.f * (navWidth + GAP)), BUTTON_HEIGHT);
    Place(mButtons[CUTOUT_ACTION_NEXT], margin + previewWidth - navWidth, navY, navWidth, BUTTON_HEIGHT);
    Place(mPreview, margin, previewY, previewWidth, previewHeight);
    Place(mStatus, margin, previewY + previewHeight + GAP, previewWidth, 56);
    const float panelY = wide ? navY : previewY + previewHeight + 72.f;
    Place(mScroll, wide ? margin + previewWidth + 20.f : margin, panelY, panelWidth, std::max(64.f, mHeight - panelY - margin));
    if(layoutPreview) LayoutPreview(previewWidth, previewHeight);

    mColumns              = panelWidth < 300.f ? 1u : 2u;
    const float columns   = static_cast<float>(mColumns);
    const float cellWidth = (panelWidth - 24.f - (columns - 1.f) * GAP) / columns;
    float       y         = 12.f;
    mFocusOrder           = {CUTOUT_ACTION_PREVIOUS, CUTOUT_ACTION_NEXT};
    const auto placeCell  = [&](Action action, unsigned cell)
    {
      const float column = static_cast<float>(cell % mColumns);
      const float row    = static_cast<float>(cell / mColumns);
      Place(mButtons[action], 12.f + column * (cellWidth + GAP), y + row * (BUTTON_HEIGHT + GAP), cellWidth, BUTTON_HEIGHT);
      mFocusOrder.push_back(action);
    };
    constexpr Action groupStarts[] = {CUTOUT_ACTION_CUTOUT, CUTOUT_ACTION_SHADOW, CUTOUT_ACTION_FONT, CUTOUT_ACTION_MORE};
    for(unsigned group = 0; group < mHeadings.size(); ++group)
    {
      Place(mHeadings[group], 12, y, panelWidth - 24.f, 22);
      y += 28.f;
      const unsigned groupSize = groupStarts[group + 1] - groupStarts[group];
      for(unsigned cell = 0; cell < groupSize; ++cell)
      {
        placeCell(static_cast<Action>(groupStarts[group] + cell), cell);
      }
      const float rows = static_cast<float>((groupSize + mColumns - 1u) / mColumns);
      y += rows * (BUTTON_HEIGHT + GAP) + GAP;
    }
    Place(mButtons[CUTOUT_ACTION_MORE], 12, y, panelWidth - 24.f, BUTTON_HEIGHT);
    mFocusOrder.push_back(CUTOUT_ACTION_MORE);
    y += BUTTON_HEIGHT + GAP;
    for(unsigned i = CUTOUT_ACTION_SIZE; i <= CUTOUT_ACTION_OVERFLOW; ++i)
    {
      mButtons[i].SetProperty(Actor::Property::VISIBLE, mMore);
      if(mMore)
      {
        placeCell(static_cast<Action>(i), i - CUTOUT_ACTION_SIZE);
      }
    }
    if(mMore)
    {
      const float rows = static_cast<float>((CUTOUT_ACTION_OVERFLOW - CUTOUT_ACTION_SIZE + mColumns) / mColumns);
      y += rows * (BUTTON_HEIGHT + GAP);
    }
    mFocusOrder.push_back(CUTOUT_ACTION_RESET);
    mControls.SetRequestedWidth(panelWidth);
    mControls.SetRequestedHeight(y + GAP);
    if(!mMore && mSelection >= CUTOUT_ACTION_SIZE && mSelection <= CUTOUT_ACTION_OVERFLOW) mSelection = CUTOUT_ACTION_MORE;
  }

  void SetButton(Action action, const std::string& text, bool selected = false)
  {
    auto button = mButtons[action];
    button.SetText(text.c_str());
    button.AsSelectable().SetSelected(selected);
    button.SetBackgroundColor(UiColor(selected ? 0x174D48 : 0x22334C));
    button.SetTextColor(UiColor(selected ? 0xB9FFDF : 0xE4ECF8));
    const bool available = IsAvailable(action);
    button.SetSensitive(available);
    button.SetProperty(Actor::Property::OPACITY, available ? 1.f : .45f);
  }

  void SetToggle(Action action, const char* name, bool enabled)
  {
    SetButton(action, std::string(name) + (enabled ? "\nON" : "\nOFF"), enabled);
  }

  void RefreshControls()
  {
    SetButton(CUTOUT_ACTION_PREVIOUS, "< Prev");
    SetButton(CUTOUT_ACTION_NEXT, "Next >");
    SetButton(CUTOUT_ACTION_RESET, "Reset settings");
    SetToggle(CUTOUT_ACTION_CUTOUT, "Cutout", mCutout);
    SetToggle(CUTOUT_ACTION_ASYNC, "Async rendering", mAsync);
    SetButton(CUTOUT_ACTION_TEXT_ALPHA, "Text opacity\n" + std::to_string(static_cast<int>(ALPHAS[mTextAlpha] * 100.f)) + "%");
    SetButton(CUTOUT_ACTION_BG_ALPHA, "Panel opacity\n" + std::to_string(static_cast<int>(ALPHAS[mBgAlpha] * 100.f)) + "%");
    SetToggle(CUTOUT_ACTION_BACKGROUND, "Panel background", mHasBackground);
    SetToggle(CUTOUT_ACTION_SHADOW, "Shadow", mShadow);
    SetToggle(CUTOUT_ACTION_OUTLINE, "Outline", mOutline);
    SetToggle(CUTOUT_ACTION_UNDERLINE, "Underline", mUnderline);
    SetButton(CUTOUT_ACTION_BG_COLOR, std::string("Panel color\n") + COLOR_NAMES[mBgColor]);
    SetButton(CUTOUT_ACTION_FONT, mFit ? "Font size\nAuto (16–84 px)" : "Font size\n" + std::to_string(static_cast<int>(FONT_SIZES[mFont])) + " px");
    SetToggle(CUTOUT_ACTION_FIT, "Auto fit", mFit);
    SetToggle(CUTOUT_ACTION_MULTILINE, "Multiline", mMultiline);
    SetToggle(CUTOUT_ACTION_MOTION, "Animate behind text", mAnimating);
    const int changed = (mSize != 2) + (mHorizontal != 1) + (mVertical != 1) + (mPadding != 1) + (mMaxLines != 0) + !mEllipsis;
    SetButton(CUTOUT_ACTION_MORE, std::string(mMore ? "− Less layout" : "+ More layout") + (changed ? "\n" + std::to_string(changed) + " changed" : "\nAlignment, size, padding, overflow"), mMore);
    constexpr const char* sizes[]      = {"Small", "Medium", "Large"};
    constexpr const char* horizontal[] = {"Start", "Center", "End"};
    constexpr const char* vertical[]   = {"Top", "Center", "Bottom"};
    SetButton(CUTOUT_ACTION_SIZE, std::string("Label size\n") + sizes[mSize]);
    SetButton(CUTOUT_ACTION_HORIZONTAL, std::string("Horizontal\n") + horizontal[mHorizontal]);
    SetButton(CUTOUT_ACTION_VERTICAL, std::string("Vertical\n") + vertical[mVertical]);
    SetButton(CUTOUT_ACTION_PADDING, "Padding\n" + std::to_string(mPadding * 12) + " px");
    SetButton(CUTOUT_ACTION_MAX_LINES, std::string("Line limit\n") + (mMaxLines ? std::to_string(mMaxLines) : "Unlimited") + (mMultiline ? "" : " (inactive)"));
    SetButton(CUTOUT_ACTION_OVERFLOW, mEllipsis ? "Overflow\nEllipsis (…)" : "Overflow\nClip");
    mPage.SetText((std::to_string(mContent + 1) + " / " + std::to_string(CONTENT_COUNT) + "\n" + CONTENTS[mContent].name).c_str());
    std::string status = std::string(mAsync ? "Async" : "Sync") + "  ·  Cutout " + (mCutout ? "ON" : "OFF") + "  ·  Text " + std::to_string(static_cast<int>(ALPHAS[mTextAlpha] * 100.f)) + "%";
    status += "  ·  Panel " + (mHasBackground ? std::to_string(static_cast<int>(ALPHAS[mBgAlpha] * 100.f)) + "%" : "OFF");
    if(mCutout && !mHasBackground)
      status += "\nPanel OFF: cutout needs a background.";
    else if(mCutout && mBgAlpha == 0)
      status += "\nPanel is transparent. Increase Panel opacity to see the cutout.";
    else if(!mCutout && mTextAlpha == 0)
      status += "\nText is transparent. Increase Text opacity to see it.";
    else
      status += std::string("\n") + CONTENTS[mContent].hint;
    mStatus.SetText(status.c_str());
    if(!IsAvailable(mSelection)) mSelection = mSelection == CUTOUT_ACTION_FONT ? CUTOUT_ACTION_FIT : (mSelection == CUTOUT_ACTION_BG_COLOR || mSelection == CUTOUT_ACTION_BG_ALPHA) ? CUTOUT_ACTION_BACKGROUND
                                                                                                                                                                                    : CUTOUT_ACTION_MULTILINE;
    HighlightSelection();
  }

  void HighlightSelection()
  {
    for(unsigned i = 0; i < mButtons.size(); ++i)
    {
      mButtons[i].SetBorderlineWidth(i == mSelection ? 2.f : 1.f);
      mButtons[i].SetBorderlineColor(UiColor(i == mSelection ? 0xF4F7FB : mButtons[i].AsSelectable().IsSelected() ? 0x368A79
                                                                                                                  : 0x334C69));
    }
  }

  void MoveSelection(int direction, unsigned step)
  {
    const auto        found  = std::find(mFocusOrder.begin(), mFocusOrder.end(), mSelection);
    std::size_t       index  = found == mFocusOrder.end() ? 0u : static_cast<std::size_t>(found - mFocusOrder.begin());
    const std::size_t count  = mFocusOrder.size();
    const std::size_t stride = step % count;
    index                    = direction < 0 ? (index + count - stride) % count : (index + stride) % count;
    while(!IsAvailable(mFocusOrder[index])) index = direction < 0 ? (index + count - 1u) % count : (index + 1u) % count;
    mSelection = mFocusOrder[index];
    if(mSelection >= CUTOUT_ACTION_CUTOUT && mSelection <= CUTOUT_ACTION_OVERFLOW) mScroll.ScrollTo(mButtons[mSelection]);
    HighlightSelection();
  }

  void OnResize(Window, Window::WindowSize size)
  {
    mWidth  = static_cast<float>(size.GetWidth());
    mHeight = static_cast<float>(size.GetHeight());
    Layout();
    RefreshControls();
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP) return;
    const auto key = event.GetKeyName();
    if(key == "Escape" || key == "BackSpace")
      mApplication.Quit();
    else if(key == "Left" || key == "Right" || key == "Up" || key == "Down" || key == "Tab")
    {
      MoveSelection(key == "Left" || key == "Up" || (key == "Tab" && event.IsShiftModifier()) ? -1 : 1,
                    key == "Up" || key == "Down" ? mColumns : 1u);
    }
    else if(key == "Return" || key == "KP_Enter" || key == "space")
      Activate(mSelection);
    else if(key == "s" || key == "S")
      Activate(CUTOUT_ACTION_ASYNC);
    else if(key == "c" || key == "C")
      Activate(CUTOUT_ACTION_CUTOUT);
    else if(key == "a" || key == "A")
      Activate(CUTOUT_ACTION_TEXT_ALPHA);
    else if(key == "g" || key == "G")
      Activate(CUTOUT_ACTION_BG_ALPHA);
    else if(key == "b" || key == "B")
      Activate(CUTOUT_ACTION_BACKGROUND);
    else if(key == "f" || key == "F")
      Activate(CUTOUT_ACTION_FIT);
    else if(key == "t" || key == "T" || key == "bracketright" || key == "Next")
      Activate(CUTOUT_ACTION_NEXT);
    else if(key == "bracketleft" || key == "Prior")
      Activate(CUTOUT_ACTION_PREVIOUS);
    else if(key == "r" || key == "R")
      Activate(CUTOUT_ACTION_RESET);
  }

  Application&        mApplication;
  AbsoluteLayout      mRoot, mPreview, mControls, mCard;
  ScrollView          mScroll;
  Label               mTitle, mLabel, mPage, mStatus, mGuide;
  View                mCardMask;
  std::vector<View>   mStrips, mOrbs;
  std::vector<Label>  mButtons, mHeadings;
  std::vector<Action> mFocusOrder;
  Animation           mMotion;
  float               mWidth{1280.f}, mHeight{720.f};
  Action              mSelection{CUTOUT_ACTION_CUTOUT};
  unsigned            mContent{0}, mTextAlpha{0}, mBgAlpha{2}, mBgColor{0}, mFont{2};
  unsigned            mHorizontal{1}, mVertical{1}, mPadding{1}, mSize{2}, mColumns{2};
  int                 mMaxLines{0};
  bool                mAsync{false}, mCutout{true}, mHasBackground{true}, mShadow{false}, mOutline{false}, mUnderline{false};
  bool                mMultiline{false}, mFit{false}, mAnimating{true}, mEllipsis{true}, mMore{false};
};

int main(int argc, char** argv)
{
  auto application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextCutoutController controller(application);
  application.MainLoop();
  return 0;
}
