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
#include <cstdio>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// Card data
const char* CARD_TEXTS[] = {
  "LIFE Picture Collection",
  "The Metropolitan Museum of Art",
  "National Gallery in Prague",
  "Tokyo National Museum",
  "Victoria and Albert Museum"
};

const char* CARD_DESCS[] = {
  "322 Pieces",
  "14 Pieces",
  "2 Pieces",
  "15 Pieces",
  "22 Pieces"
};

const Vector4 CARD_COLORS[] = {
  Vector4(0.0f, 0.5f, 1.0f, 1.0f),  // Blue
  Vector4(1.0f, 0.3f, 0.0f, 1.0f),  // Orange
  Vector4(0.9f, 0.5f, 0.8f, 1.0f),  // Pink
  Vector4(0.3f, 0.8f, 0.75f, 1.0f), // Teal
  Vector4(0.1f, 0.45f, 0.95f, 1.0f) // Blue-purple
};

// Layout constants
constexpr float CARD_WIDTH           = 350.0f;
constexpr float CARD_HEIGHT          = 500.0f;
constexpr float CARD_CORNER_RADIUS   = 25.0f;
constexpr float CARD_SPACING         = 30.0f;
constexpr float SECTION_SPACING      = 100.0f;
constexpr float PADDING              = 25.0f;
constexpr float TITLE_HEIGHT         = 260.0f;
constexpr float DESC_HEIGHT          = 40.0f;
constexpr float FONT_SIZE_TITLE      = 48.0f;
constexpr float FONT_SIZE_DESC       = 24.0f;
constexpr float FONT_SIZE_LABEL      = 16.0f;
constexpr float SCALE_CONTROL_HEIGHT = 40.0f;

// Colors
const UiColor CARD_BG_COLOR(0x1A1A1A);         // Dark gray
const UiColor CARD_BG_FOCUSED_COLOR(0xFFFFFF); // White
const UiColor TEXT_COLOR(0xFFFFFF);            // White
const UiColor TEXT_FOCUSED_COLOR(0x1A1A1A);    // Dark gray
const UiColor DESC_TEXT_COLOR(0x696969);       // Dim gray
const UiColor MAIN_BG_COLOR(0x000000);         // Black
const UiColor SECTION_LABEL_BG(0x333333);      // Dark gray

// Animation durations
constexpr float SCALE_ANIM_DURATION = 0.15f; // 150ms
constexpr float SNAP_ANIM_DURATION  = 0.20f; // 200ms

} // namespace

/**
 * @brief Example application for testing Label RenderScale.
 *
 * This sample shows the difference between:
 * - cards using Label RenderScale, and
 * - cards using only normal actor scaling.
 *
 * The scale can be adjusted with keys 1 and 2, or by dragging the scale bar.
 * The current scale value is shown in the UI.
 */
class TextRenderScaleController : public ConnectionTracker
{
private:
  // -------------------------------------------------------------------------
  // Internal Types (forward declaration at top)
  // -------------------------------------------------------------------------
  struct CardInfo;

public:
  explicit TextRenderScaleController(Application& application)
  : mApplication(application),
    mCurrentScale(1.1f)
  {
    mApplication.InitSignal().Connect(this, &TextRenderScaleController::OnInit);
  }

private:
  // -------------------------------------------------------------------------
  // Initialization
  // -------------------------------------------------------------------------
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(MAIN_BG_COLOR);
    window.SetSize(Dali::Window::WindowSize(1920, 1080));

    // Create main container with vertical layout
    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetSpacing(SECTION_SPACING)
        .Children({
          CreateScaleControlSection(),
          CreateCardSection("RenderScale", true),
          CreateCardSection("Original", false)
    }));

    window.KeyEventSignal().Connect(this, &TextRenderScaleController::OnKeyEvent);
  }

  // -------------------------------------------------------------------------
  // Scale Control Section
  // -------------------------------------------------------------------------
  View CreateScaleControlSection()
  {
    StackLayout layout = StackLayout::New(StackOrientation::HORIZONTAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .Children({
        Label::New("SCALE")
          .SetFontSize(FONT_SIZE_LABEL)
          .SetTextColor(TEXT_COLOR)
          .SetBackgroundColor(SECTION_LABEL_BG)
          .SetRequestedWidth(200.0f)
          .SetRequestedHeight(40.0f)
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),

        // Scale control bar (visual indicator)
        StackLayout::New()
          .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
          .SetRequestedHeight(SCALE_CONTROL_HEIGHT)
          .SetBackgroundColor(CARD_BG_COLOR)
          .SetMargin(Extents(10, 0, 0, 0))
          .Children({
            // Overlay for description
            Label::New("Press 1/2 or drag to adjust scale")
              .SetTextColor(TEXT_COLOR)
              .SetRequestedWidth(MATCH_PARENT)
              .SetRequestedHeight(MATCH_PARENT)
              .SetHorizontalTextAlignment(Text::Alignment::CENTER)
              .SetVerticalTextAlignment(Text::Alignment::CENTER)
              .SetLayoutMode(LayoutMode::STANDALONE)
              .SetBackgroundColor(UiColor(0xFFFFFF).WithAlpha(0.1f)),

            // Indicator line for visual feedback
            View::New()
              .SetRequestedWidth(2.0f)
              .SetRequestedHeight(MATCH_PARENT)
              .SetLayoutMode(LayoutMode::STANDALONE)
              .SetBackgroundColor(UiColor(0xFF0000))
              .As(mScaleIndicator)
        }).As(mScaleBar),

        // Scale value display
        Label::New()
          .SetFontSize(FONT_SIZE_LABEL)
          .SetTextColor(TEXT_COLOR)
          .SetRequestedWidth(WRAP_CONTENT)
          .SetRequestedHeight(MATCH_PARENT)
          .SetVerticalTextAlignment(Text::Alignment::CENTER)
          .SetMargin(Extents(10, 0, 0, 0))
          .As(mScaleValueLabel)
    });

    // Update scale display
    UpdateScaleDisplay();

    mScaleBar.TouchedSignal().Connect(this, &TextRenderScaleController::OnScaleTouched);

    return layout;
  }

  void UpdateScaleDisplay()
  {
    // Update label text
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.3f", mCurrentScale);
    mScaleValueLabel.SetText(buffer);

    // Update indicator position (scale ranges from 1.0 to 2.5)
    float indicatorX = ((mCurrentScale - 1.0f) / 1.5f) * mScaleBar.GetSize().GetWidth();
    mScaleIndicator.SetRequestedPositionX(indicatorX);
  }

  // -------------------------------------------------------------------------
  // Card Section Creation
  // -------------------------------------------------------------------------
  View CreateCardSection(const char* title, bool useRenderScale)
  {
    StackLayout cardLayout;

    StackLayout section = StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .Children({
        Label::New(title)
          .SetFontSize(FONT_SIZE_LABEL)
          .SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER))
          .SetTextColor(TEXT_COLOR),

        StackLayout::New(StackOrientation::HORIZONTAL)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(WRAP_CONTENT)
          .SetMargin(Extents(0, 0, 10, 0))
          .As(cardLayout)
      });

    // Create 5 cards
    for(int i = 0; i < 5; ++i)
    {
      View card = CreateCard(i, useRenderScale);
      cardLayout.Add(card);
    }

    return section;
  }

  View CreateCard(int index, bool useRenderScale)
  {
    StackLayout card;
    Label titleLabel;
    Label descLabel;
    View colorBlock;

    StackLayout::New(StackOrientation::VERTICAL)
      .SetRequestedWidth(CARD_WIDTH)
      .SetRequestedHeight(CARD_HEIGHT)
      .SetBackgroundColor(CARD_BG_COLOR)
      .SetCornerRadius(CARD_CORNER_RADIUS)
      .SetMargin(Extents(CARD_SPACING / 2, CARD_SPACING / 2, 0, 0))
      .SetFocusable(true)
      .SetTouchFocusable(true)
      .Children({
        // Title label
        Label::New(CARD_TEXTS[index])
          .SetFontSize(FONT_SIZE_TITLE)
          .SetFontFamily("SamsungOneUI_700")
          .SetTextColor(TEXT_COLOR)
          .SetMultiLine(true)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(TITLE_HEIGHT)
          .SetPadding(Extents(PADDING, PADDING, PADDING, PADDING))
          .SetVerticalTextAlignment(Text::Alignment::CENTER)
          .SetAsyncRendering(true)
          .SetMarqueeSpeed(100)
          .SetMarqueeLoopCount(2)
          .SetMarqueeLoopDelay(1.0f)
          .SetMarqueeGap(50)
          .SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE)
          .SetMarqueeOrientation(Text::MarqueeOrientation::VERTICAL)
          .As(titleLabel),

        // Description label
        Label::New(CARD_DESCS[index])
          .SetFontSize(FONT_SIZE_DESC)
          .SetFontFamily("SamsungOneUI_600")
          .SetTextColor(DESC_TEXT_COLOR)
          .SetRequestedWidth(MATCH_PARENT)
          .SetRequestedHeight(DESC_HEIGHT)
          .SetPadding(Extents(PADDING, PADDING, 0, 0))
          .SetHorizontalTextAlignment(Text::Alignment::START)
          .SetVerticalTextAlignment(Text::Alignment::CENTER)
          .SetAsyncRendering(true)
          .As(descLabel),

        // Color block at bottom
        View::New()
          .SetRequestedWidth(MATCH_PARENT)
          .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
          .SetBackgroundColor(UiColor(CARD_COLORS[index]))
          .As(colorBlock)

      }).As(card);

    card.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
    card.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);

    // Store card info for animation
    CardInfo info;
    info.card = card;
    info.titleLabel = titleLabel;
    info.descLabel = descLabel;
    info.useRenderScale = useRenderScale;
    info.colorBlock = colorBlock;

    if(useRenderScale)
    {
      mRenderScaleCards.push_back(info);
    }
    else
    {
      mOriginalCards.push_back(info);
    }

    // Connect focus signals
    card.FocusChangedSignal().Connect(this, &TextRenderScaleController::OnFocusChanged);

    return card;
  }

  // -------------------------------------------------------------------------
  // Focus Handling
  // -------------------------------------------------------------------------
  void OnFocusChanged(View view, bool focused)
  {
    // Find the card info
    CardInfo* info = FindCardInfo(view);
    if(!info)
    {
      return;
    }

    if(focused)
    {
      HandleFocusGained(*info);
    }
    else
    {
      HandleFocusLost(*info);
    }
  }

  CardInfo* FindCardInfo(View view)
  {
    // Search in RenderScale cards
    for(auto& info : mRenderScaleCards)
    {
      if(info.card == view)
      {
        return &info;
      }
    }

    // Search in Original cards
    for(auto& info : mOriginalCards)
    {
      if(info.card == view)
      {
        return &info;
      }
    }

    return nullptr;
  }

  void HandleFocusGained(CardInfo& info)
  {
    // Change card background to white
    info.card.SetBackgroundColor(CARD_BG_FOCUSED_COLOR);
    info.card.RaiseToTop(LayoutOrderPolicy::PRESERVE);

    // Change text color to dark
    info.titleLabel.SetTextColor(TEXT_FOCUSED_COLOR);

    // Get current scale
    float scale = mCurrentScale;

    // Create scale animation for card
    Animation scaleAnim = Animation::New(SCALE_ANIM_DURATION);
    scaleAnim.AnimateTo(Property(info.card, Actor::Property::SCALE), Vector3(scale, scale, 1.0f));
    scaleAnim.Play();

    if(info.useRenderScale)
    {
      // Apply RenderScale to labels
      info.titleLabel.SetRenderScale(scale);
      info.descLabel.SetRenderScale(scale);

      // Set initial PixelSnapFactor to 0
      info.titleLabel.SetProperty(Label::Property::PIXEL_SNAP_FACTOR, 0.0f);
      info.descLabel.SetProperty(Label::Property::PIXEL_SNAP_FACTOR, 0.0f);

      // Animate PixelSnapFactor to 1.0
      Animation snapAnim = Animation::New(SNAP_ANIM_DURATION);
      snapAnim.AnimateTo(Property(info.titleLabel, Label::Property::PIXEL_SNAP_FACTOR), 1.0f);
      snapAnim.AnimateTo(Property(info.descLabel, Label::Property::PIXEL_SNAP_FACTOR), 1.0f);
      snapAnim.Play();

      // Store animation
      if(info.currentAnimation)
      {
        info.currentAnimation.Stop();
      }
      info.currentAnimation = snapAnim;
    }

    if(info.titleLabel.GetHeightForWidth(info.titleLabel.GetSize().GetWidth()) > TITLE_HEIGHT)
    {
      info.titleLabel.StartMarquee();
    }
  }

  void HandleFocusLost(CardInfo& info)
  {
    // Restore card background
    info.card.SetBackgroundColor(CARD_BG_COLOR);

    // Restore text color
    info.titleLabel.SetTextColor(TEXT_COLOR);

    // Stop marquee
    info.titleLabel.StopMarquee();

    if(info.useRenderScale)
    {
      // Reset RenderScale
      info.titleLabel.SetRenderScale(1.0f);
      info.descLabel.SetRenderScale(1.0f);

      // Animate PixelSnapFactor back to 0
      Animation snapAnim = Animation::New(SNAP_ANIM_DURATION);
      snapAnim.AnimateTo(Property(info.titleLabel, Label::Property::PIXEL_SNAP_FACTOR), 0.0f);
      snapAnim.AnimateTo(Property(info.descLabel, Label::Property::PIXEL_SNAP_FACTOR), 0.0f);
      snapAnim.Play();

      if(info.currentAnimation)
      {
        info.currentAnimation.Stop();
      }
      info.currentAnimation = snapAnim;
    }

    // Create scale animation to restore card scale
    Animation scaleAnim = Animation::New(SCALE_ANIM_DURATION);
    scaleAnim.AnimateTo(Property(info.card, Actor::Property::SCALE), Vector3(1.0f, 1.0f, 1.0f));
    scaleAnim.Play();
  }

  // -------------------------------------------------------------------------
  // Key Event Handling
  // -------------------------------------------------------------------------
  void OnKeyEvent(KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    // ESC or BACK to quit
    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    // 1/2 keys to adjust scale
    if(event.GetKeyName() == "1")
    {
      mCurrentScale = std::max(1.0f, mCurrentScale - 0.05f);
      UpdateScaleDisplay();
    }
    else if(event.GetKeyName() == "2")
    {
      mCurrentScale = std::min(2.5f, mCurrentScale + 0.05f);
      UpdateScaleDisplay();
    }
  }

  bool OnScaleTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::MOTION)
    {
      Vector2 position = touch.GetLocalPosition(0);
      mCurrentScale = std::clamp(1.0f + (position.x / mScaleBar.GetSize().GetWidth()) * 1.5f, 1.0f, 2.5f);
      UpdateScaleDisplay();
    }
    return true;
  }

private:
  // -------------------------------------------------------------------------
  // Internal Types
  // -------------------------------------------------------------------------
  struct CardInfo
  {
    View        card;
    Label       titleLabel;
    Label       descLabel;
    View        colorBlock;
    bool        useRenderScale;
    Animation   currentAnimation;
  };

  // -------------------------------------------------------------------------
  // Members
  // -------------------------------------------------------------------------
  Application& mApplication;

  float        mCurrentScale;
  View         mScaleBar;
  View         mScaleIndicator;
  Label        mScaleValueLabel;

  std::vector<CardInfo> mRenderScaleCards;
  std::vector<CardInfo> mOriginalCards;
};

// -------------------------------------------------------------------------
// Entry Point
// -------------------------------------------------------------------------
int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextRenderScaleController controller(application);
  application.MainLoop();

  return 0;
}
