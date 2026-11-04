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
#include <dali-ui-foundation/integration-api/text/reveal-integ.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include "sample-slider.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;
namespace RevealBlur = Dali::Ui::Integration::Text::Reveal;

namespace
{
constexpr float       CONTROLS_PANEL_HEIGHT   = 612.0f;
constexpr float       CONTROL_HEIGHT          = 32.0f;
constexpr float       CONTROL_SPACING         = 5.0f;
constexpr float       MENU_TITLE_WIDTH        = 76.0f;
constexpr float       PREVIEW_GUTTER          = 12.0f;
constexpr std::size_t ALPHA_CASE_COUNT        = 7u;
constexpr std::size_t TEXT_CASE_COUNT         = 9u;
constexpr std::size_t SEQUENCE_CASE_COUNT     = 2u;
constexpr std::size_t DEFAULT_TEXT_CASE_INDEX = 1u;
constexpr std::size_t LOCAL_IMAGE_CASE_INDEX  = 5u;
constexpr std::size_t REMOTE_IMAGE_CASE_INDEX = 6u;
constexpr std::size_t RTL_TEXT_CASE_INDEX     = 8u;
constexpr uint32_t    PANEL_COLOR             = 0x111827;
constexpr uint32_t    BUTTON_COLOR            = 0x1E293B;
constexpr uint32_t    BUTTON_BORDER_COLOR     = 0x475569;
constexpr uint32_t    SELECTED_BUTTON_COLOR   = 0x1D4ED8;
constexpr uint32_t    SELECTED_BORDER_COLOR   = 0x93C5FD;
constexpr const char* REMOTE_IMAGE_URL =
  "https://raw.githubusercontent.com/dalihub/dali-ui/devel/samples/text/res/flag_us_alt.png";

enum class FillMode : uint8_t
{
  SOLID,
  WHITE_ON_BLACK,
  TEXT_GRADIENT,
  GRADIENT_SPAN
};

enum class SpanGradientKind : uint8_t
{
  LINEAR,
  RADIAL,
  CONIC
};

enum class UxPlaybackPhase : uint8_t
{
  NONE,
  ENTERING,
  WAITING,
  EXITING
};

enum class SliderId : std::size_t
{
  PROGRESS,
  FADE,
  STAGGER,
  DURATION,
  RADIUS,
  BLUR_TIME,
  WIDTH,
  HEIGHT,
  COUNT
};

const AlphaFunction::BuiltinFunction ALPHA_FUNCTIONS[ALPHA_CASE_COUNT] = {
  AlphaFunction::LINEAR,
  AlphaFunction::EASE_IN_SQUARE,
  AlphaFunction::EASE_OUT_SQUARE,
  AlphaFunction::EASE_IN,
  AlphaFunction::EASE_OUT,
  AlphaFunction::EASE_IN_OUT,
  AlphaFunction::EASE_IN_OUT_SINE};

const char* const ALPHA_FUNCTION_BUTTON_LABELS[ALPHA_CASE_COUNT] = {
  "Linear",
  "In Square",
  "Out Square",
  "In Cubic",
  "Out Cubic",
  "In-Out",
  "In-Out Sine"};

const char* const TEXT_BUTTON_LABELS[TEXT_CASE_COUNT] = {
  "English",
  "Korean",
  "Bidi",
  "Emoji",
  "Long",
  "Image Local",
  "Image Remote",
  "Short Tail",
  "Arabic (RTL)"};

const Text::Reveal::Sequence SEQUENCES[SEQUENCE_CASE_COUNT] = {
  Text::Reveal::Sequence::WHOLE_TEXT,
  Text::Reveal::Sequence::PER_LINE};

const char* const SEQUENCE_BUTTON_LABELS[SEQUENCE_CASE_COUNT] = {
  "Whole Text",
  "Per Line"};

const char* const SEQUENCE_STATUS_LABELS[SEQUENCE_CASE_COUNT] = {
  "WHOLE_TEXT",
  "PER_LINE"};

const char* const TEXT_CASES[TEXT_CASE_COUNT] = {
  "Good morning. Start the day with cafe\u0301 by the window, "
  "a design review at 11:00, and a quiet walk by the river before sunset.",
  "오늘의 일정이 준비되었습니다.\n"
  "오전에는 천천히 커피를 즐기고, 오후 3시에는 Atlas 룸에서 "
  "디자인 리뷰를 진행한 뒤 해 질 무렵 산책을 떠나보세요.",
  "Flight SK204 to دبي departs at 18:30.\n"
  "Dinner in תל אביב begins at 20:10.",
  "Build complete 👩‍💻✨ — time to close the laptop.\n"
  "Dinner 🍜, a high-five 👍🏽, then a family walk 👨‍👩‍👧‍👦 under the city lights 🌙.",
  "Saturday is yours to explore. Start with brunch at 11:30, wander through "
  "the museum at 14:00, and reach the riverside just before sunset. Keep the "
  "evening open for a small jazz bar tucked behind the old market.\n"
  "토요일은 천천히 즐겨보세요. 오전 11시 30분 브런치로 시작해 오후 2시 "
  "전시를 보고, 해 질 무렵에는 강변을 걸어보세요.",
  "Tonight in Seoul: warm lights, late cafés, and a quiet walk along the Han River.",
  "Next stop, San Francisco: morning coffee, cool fog, and a sunset walk along the waterfront.",
  "A quiet morning brings warm coffee and a little time to breathe.\n"
  "The afternoon is yours to explore, one small discovery at a time.\n"
  "Go.",
  "صباح الخير! ابدأ يومك بفنجان قهوة قرب النافذة، ثم انضم إلى اجتماع فريق Atlas في الساعة 10:30.\n"
  "بعد الغداء، خذ استراحة قصيرة واستمتع بنزهة هادئة على ضفاف النهر قبل العودة إلى المنزل."};

// Each GradientSpan fill keeps the ordinary foreground color outside these
// words, and applies Linear, Radial and Conic SPAN_BOUND gradients in order.
const char* const GRADIENT_SPAN_WORDS[TEXT_CASE_COUNT][3u] = {
  {"morning", "design", "river"},
  {"오전", "Atlas", "산책"},
  {"SK204", "دبي", "תל אביב"},
  {"Build", "Dinner", "city"},
  {"Saturday", "museum", "강변"},
  {"Seoul", "cafés", "Han River"},
  {"San Francisco", "coffee", "waterfront"},
  {"morning", "afternoon", "Go"},
  {"قهوة", "Atlas", "النهر"}};

Gradient::Base CreateGradientSpanGradient(SpanGradientKind kind)
{
  Gradient::Base gradient;
  switch(kind)
  {
    case SpanGradientKind::RADIAL:
    {
      gradient = Gradient::Radial(Vector2::ZERO, 0.55f);
      gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(0xFACC15)),
                             Gradient::StopNode(0.45f, UiColor(0x16A34A)),
                             Gradient::StopNode(1.0f, UiColor(0x0891B2))});
      break;
    }
    case SpanGradientKind::CONIC:
    {
      gradient = Gradient::Conic(Vector2::ZERO, Radian(0.0f));
      gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(0xDB2777)),
                             Gradient::StopNode(0.34f, UiColor(0x7C3AED)),
                             Gradient::StopNode(0.68f, UiColor(0x2563EB)),
                             Gradient::StopNode(1.0f, UiColor(0xDB2777))});
      break;
    }
    case SpanGradientKind::LINEAR:
    default:
    {
      gradient = Gradient::Linear(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
      gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(0xDC2626)),
                             Gradient::StopNode(0.5f, UiColor(0xF97316)),
                             Gradient::StopNode(1.0f, UiColor(0xEAB308))});
      break;
    }
  }
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  return gradient;
}

Label NewLabel(const char* text, float size, uint32_t color)
{
  Label label = Label::New(text);
  label.SetFontSize(size);
  label.SetFontFamily("SamsungOneUI_500");
  label.SetTextColor(UiColor(color));
  label.SetMultiLine(true);
  label.SetRequestedWidth(MATCH_PARENT);
  return label;
}

Label NewButton(const char* text)
{
  Label button = NewLabel(text, 12.0f, 0xCBD5E1);
  button.SetBackgroundColor(UiColor(BUTTON_COLOR));
  button.SetBorderlineWidth(1.0f);
  button.SetBorderlineOffset(-1.0f);
  button.SetBorderlineColor(UiColor(BUTTON_BORDER_COLOR));
  button.SetPadding(Insets(4.0f, 4.0f, 0.0f, 0.0f));
  button.SetMultiLine(false);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetRequestedHeight(CONTROL_HEIGHT);
  button.SetCornerRadius(7.0f);
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  // Keep toggle colors independent of the built-in press/release feedback.
  button.SetStateEffect(OverlayEffect::Plain());
  return button;
}

Label NewMenuTitle(const char* text)
{
  Label title = NewLabel(text, 12.0f, 0xF8FAFC);
  title.SetRequestedWidth(MENU_TITLE_WIDTH);
  title.SetRequestedHeight(CONTROL_HEIGHT);
  title.SetTextColor(UiColor(0x94A3B8));
  title.SetPadding(Insets(2.0f, 2.0f, 0.0f, 0.0f));
  title.SetMultiLine(false);
  title.SetHorizontalTextAlignment(Text::Alignment::START);
  title.SetVerticalTextAlignment(Text::Alignment::CENTER);
  title.SetCornerRadius(7.0f);
  return title;
}

Label NewCompactButton(const char* text, float width = 60.0f)
{
  Label button = NewButton(text);
  button.SetRequestedWidth(width);
  button.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));
  return button;
}

StackLayout NewMenuRow(const char* title)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(CONTROL_HEIGHT);
  row.SetSpacing(CONTROL_SPACING);
  row.Add(NewMenuTitle(title));
  return row;
}

void SetButtonSelected(Label button, bool selected)
{
  button.SetTextColor(UiColor(selected ? 0xF8FAFC : 0xCBD5E1));
  button.SetBackgroundColor(UiColor(selected ? SELECTED_BUTTON_COLOR : BUTTON_COLOR));
  button.SetBorderlineColor(UiColor(selected ? SELECTED_BORDER_COLOR : BUTTON_BORDER_COLOR));
}
} // unnamed namespace

class TextRevealController : public ConnectionTracker
{
public:
  explicit TextRevealController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextRevealController::OnInit);
  }

  ~TextRevealController()
  {
    CancelSliderEdit();
    StopAnimation();
    if(mPreviewLayoutTimer)
    {
      mPreviewLayoutTimer.Stop();
    }
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF8FAFC));
    window.KeyEventSignal().Connect(this, &TextRevealController::OnKeyEvent);
    window.ResizedSignal().Connect(this, [this](Window, Window::WindowSize)
    {
      CancelSliderEdit();
      UpdateControlsViewport();
    });
    window.FocusChangedSignal().Connect(this, [this](Window, bool focused)
    {
      if(!focused)
      {
        CancelSliderEdit();
      }
    });
    UiScaleManager::Get().SetScale(mUiScale);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(0.0f);
    root.SetBackgroundColor(UiColor(0xF8FAFC));

    mPreviewViewport = ScrollView::New();
    mPreviewViewport.SetRequestedWidth(MATCH_PARENT);
    mPreviewViewport.SetRequestedHeight(MATCH_PARENT);
    mPreviewViewport.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mPreviewViewport.SetScrollDirection(ScrollDirection::Both);
    // Keep the workspace chrome-free; its content still pans in both axes.
    mPreviewViewport.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
    mPreviewViewport.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
    mPreviewCanvas = View::New();
    mPreviewViewport.SetContent(mPreviewCanvas);
    // A layout-finished slot cannot invalidate layout. Coalesce only actual
    // viewport size changes into one event-time update, never a polling loop.
    mPreviewLayoutTimer = Timer::New(16u);
    mPreviewLayoutTimer.TickSignal().Connect(this, [this]()
    {
      UpdatePreviewLayout();
      return false;
    });
    mPreviewViewport.LayoutFinishedSignal().Connect(this, [this](View, LayoutRect bounds)
    {
      const Vector2 size(bounds.width, bounds.height);
      if(size != mPreviewViewportSize)
      {
        mPreviewViewportSize = size;
        mPreviewLayoutTimer.Start();
      }
    });

    mPreview = NewLabel(TEXT_CASES[mTextCaseIndex], 28.0f, 0x0F172A);
    mPreview.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
    mPreview.SetLineHeight(1.25f);
    mPreview.SetPadding(Insets(18.0f, 18.0f, 16.0f, 16.0f));
    mPreview.SetBackgroundColor(UiColor(0xFFFFFF));
    mPreview.SetBorderlineWidth(1.0f);
    mPreview.SetBorderlineOffset(-1.0f);
    mPreview.SetBorderlineColor(UiColor(0xCBD5E1));
    mPreview.SetCornerRadius(8.0f);
    mPreview.SetTextOverflowMode(Text::OverflowMode::ELLIPSIS);
    mPreview.SetLayoutMode(LayoutMode::STANDALONE);
    mPreview.SetRequestedX(PREVIEW_GUTTER);
    mPreview.SetRequestedY(PREVIEW_GUTTER);
    mPreview.SetRequestedWidth(mPreviewWidth);
    mPreview.SetRequestedHeight(mPreviewHeight);
    mPreviewCanvas.Add(mPreview);

    StackLayout configurationControls = NewMenuRow("CONFIG");
    mUnitButton                       = NewButton("");
    mAsyncButton                      = NewButton("");
    mFillButton                       = NewButton("");
    mRevealButton                     = NewButton("");
    configurationControls.Add(mUnitButton);
    configurationControls.Add(mAsyncButton);
    configurationControls.Add(mFillButton);
    configurationControls.Add(mRevealButton);

    StackLayout uxControls = NewMenuRow("PRESET");
    Label       firstUx    = NewButton("Preset 1");
    Label       secondUx   = NewButton("Preset 2");
    uxControls.Add(firstUx);
    uxControls.Add(secondUx);
    firstUx.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      PlayUxTest(false);
    });
    secondUx.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      PlayUxTest(true);
    });

    StackLayout textControls = NewCycleRow("TEXT", mTextButton, [this](int direction)
    {
      SetTextCase((mTextCaseIndex + (direction > 0 ? 1u : TEXT_CASE_COUNT - 1u)) % TEXT_CASE_COUNT);
    });

    StackLayout sequenceControls = NewMenuRow("SEQUENCE");
    for(std::size_t sequenceIndex = 0u; sequenceIndex < SEQUENCE_CASE_COUNT; ++sequenceIndex)
    {
      mSequenceButtons[sequenceIndex] = NewButton(SEQUENCE_BUTTON_LABELS[sequenceIndex]);
      sequenceControls.Add(mSequenceButtons[sequenceIndex]);
      mSequenceButtons[sequenceIndex].AsInteractive().ClickedSignal().Connect(this, [this, sequenceIndex](View, InputEvent)
      {
        mSequenceIndex = sequenceIndex;
        ConfigureAndReplay();
      });
    }

    StackLayout fadeControls = NewMenuRow("FADE");
    mAutoFadeButton          = NewCompactButton("Auto");
    fadeControls.Add(mAutoFadeButton);
    fadeControls.Add(NewSlider(SliderId::FADE, "Fade", 0.0f, 1.0f, 0.01f, 2, mManualFade, [this](float value)
    {
      mManualFade = value;
      mAutoFade   = false;
      UpdateFadeButtons();
      Replay();
    }));
    mAutoFadeButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      mAutoFade = !mAutoFade;
      UpdateFadeButtons();
      Replay();
    });

    StackLayout staggerControls = NewMenuRow("STAGGER");
    staggerControls.Add(NewSlider(SliderId::STAGGER, "Stagger", 0.0f, 1.0f, 0.01f, 2, mStaggerRatio, [this](float value)
    {
      mStaggerRatio = value;
      Replay();
    }));

    StackLayout durationControls = NewMenuRow("DURATION");
    durationControls.Add(NewSlider(SliderId::DURATION, "Duration seconds", 0.5f, 8.0f, 0.5f, 1, mDurationSeconds, [this](float value)
    {
      mDurationSeconds = value;
      Replay();
    }));

    StackLayout alphaFunctionControls = NewCycleRow("ALPHA", mAlphaButton, [this](int direction)
    {
      mAlphaFunctionIndex = (mAlphaFunctionIndex + (direction > 0 ? 1u : ALPHA_CASE_COUNT - 1u)) % ALPHA_CASE_COUNT;
      UpdateAlphaFunctionButtons();
      Replay();
    });

    StackLayout blurRadiusControls = NewMenuRow("BLUR px");
    mBlurButton                    = NewCompactButton("Off");
    blurRadiusControls.Add(mBlurButton);
    mBlurButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      mBlurEnabled = !mBlurEnabled;
      UpdateBlurButtons();
      Replay();
    });
    blurRadiusControls.Add(NewSlider(SliderId::RADIUS, "Blur radius", 4.0f, 64.0f, 1.0f, 0, mBlurRadius, [this](float value)
    {
      mBlurRadius  = value;
      mBlurEnabled = true;
      UpdateBlurButtons();
      Replay();
    }));

    StackLayout blurModeControls    = NewMenuRow("BLUR MODE");
    mQualityBlurButton              = NewButton("Quality");
    mBlurPerformanceButton          = NewButton("Performance");
    blurModeControls.Add(mQualityBlurButton);
    blurModeControls.Add(mBlurPerformanceButton);
    mQualityBlurButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetBlurMode(RevealBlur::BlurMode::QUALITY);
    });
    mBlurPerformanceButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      SetBlurMode(RevealBlur::BlurMode::PERFORMANCE);
    });

    StackLayout blurEndControls = NewMenuRow("BLUR TIME");
    blurEndControls.Add(NewSlider(SliderId::BLUR_TIME, "Blur time", 0.0f, 1.0f, 0.01f, 2, mBlurDurationRatio, [this](float value)
    {
      mBlurDurationRatio = value;
      Replay();
    }));

    StackLayout playbackControls = NewMenuRow("PLAYBACK");
    Label       replayButton     = NewButton("Replay");
    mStopPlayButton              = NewButton("Stop");
    Label reverseButton          = NewButton("Reverse");
    playbackControls.Add(replayButton);
    playbackControls.Add(mStopPlayButton);
    playbackControls.Add(reverseButton);
    StackLayout progressControls = NewMenuRow("PROGRESS");
    progressControls.Add(NewSlider(SliderId::PROGRESS, "Progress", 0.0f, 1.0f, 0.01f, 2, 0.0f, [this](float value)
    {
      mPreview.SetTextRevealProgress(value);
      RestoreUxPreview();
      mPlaybackStatus = "Manual progress. Play resumes; Reverse retraces from here.";
      UpdateStatus();
    }));

    StackLayout previewControls = NewMenuRow("PREVIEW");
    mFitButton                  = NewButton("Fit: On");
    Label referenceSize         = NewButton("550 x 300");
    Label details               = NewButton("Details");
    previewControls.Add(mFitButton);
    previewControls.Add(referenceSize);
    previewControls.Add(details);
    mFitButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      FreezePlayback();
      mFitPreview = !mFitPreview;
      UpdatePreviewLayout();
    });
    referenceSize.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      FreezePlayback();
      mFitPreview    = false;
      mPreviewWidth  = 550.0f;
      mPreviewHeight = 300.0f;
      UpdatePreviewLayout();
    });
    details.AsInteractive().ClickedSignal().Connect(this, [this, details](View, InputEvent) mutable
    {
      mShowDetails = !mShowDetails;
      SetButtonSelected(details, mShowDetails);
      UpdateStatus();
    });
    StackLayout widthControls = NewMenuRow("WIDTH");
    widthControls.Add(NewSlider(SliderId::WIDTH, "Preview width", 80.0f, 1600.0f, 1.0f, 0, mPreviewWidth, [this](float value)
    {
      mFitPreview   = false;
      mPreviewWidth = value;
      UpdatePreviewLayout();
    }));
    StackLayout heightControls = NewMenuRow("HEIGHT");
    heightControls.Add(NewSlider(SliderId::HEIGHT, "Preview height", 48.0f, 1200.0f, 1.0f, 0, mPreviewHeight, [this](float value)
    {
      mFitPreview    = false;
      mPreviewHeight = value;
      UpdatePreviewLayout();
    }));

    mStatus = NewLabel("", 12.0f, 0xCBD5E1);
    mStatus.SetBackgroundColor(UiColor(0x1E293B));
    mStatus.SetBorderlineWidth(1.0f);
    mStatus.SetBorderlineOffset(-1.0f);
    mStatus.SetBorderlineColor(UiColor(BUTTON_BORDER_COLOR));
    mStatus.SetPadding(Insets(10.0f, 10.0f, 6.0f, 6.0f));
    mStatus.SetRequestedHeight(WRAP_CONTENT);
    mStatus.SetCornerRadius(7.0f);

    StackLayout controlsPanel = StackLayout::New(StackOrientation::VERTICAL);
    controlsPanel.SetRequestedWidth(MATCH_PARENT);
    controlsPanel.SetRequestedHeight(WRAP_CONTENT);
    controlsPanel.SetBackgroundColor(UiColor(PANEL_COLOR));
    controlsPanel.SetPadding(Insets(12.0f, 12.0f, 12.0f, 12.0f));
    controlsPanel.SetSpacing(CONTROL_SPACING);
    controlsPanel.Add(configurationControls);
    controlsPanel.Add(uxControls);
    controlsPanel.Add(playbackControls);
    controlsPanel.Add(progressControls);
    controlsPanel.Add(textControls);
    controlsPanel.Add(sequenceControls);
    controlsPanel.Add(fadeControls);
    controlsPanel.Add(staggerControls);
    controlsPanel.Add(durationControls);
    controlsPanel.Add(alphaFunctionControls);
    controlsPanel.Add(blurRadiusControls);
    controlsPanel.Add(blurModeControls);
    controlsPanel.Add(blurEndControls);
    controlsPanel.Add(previewControls);
    controlsPanel.Add(widthControls);
    controlsPanel.Add(heightControls);
    controlsPanel.Add(mStatus);

    mControlsViewport = ScrollView::New();
    mControlsViewport.SetScrollDirection(ScrollDirection::Vertical);
    mControlsViewport.SetVerticalScrollBarVisibility(ScrollBarVisibility::Never);
    mControlsViewport.SetHorizontalScrollBarVisibility(ScrollBarVisibility::Never);
    mControlsViewport.SetRequestedWidth(MATCH_PARENT);
    mControlsViewport.SetBackgroundColor(UiColor(PANEL_COLOR));
    mControlsViewport.SetContent(controlsPanel);
    UpdateControlsViewport();

    root.Add(mPreviewViewport);
    root.Add(mControlsViewport);
    window.Add(root);

    mUnitButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      if(mUnit == Text::Reveal::Unit::CHARACTER)
      {
        mUnit = Text::Reveal::Unit::WORD;
      }
      else if(mUnit == Text::Reveal::Unit::WORD)
      {
        mUnit = Text::Reveal::Unit::LINE;
      }
      else if(mUnit == Text::Reveal::Unit::LINE)
      {
        mUnit = Text::Reveal::Unit::PIXEL;
      }
      else
      {
        mUnit = Text::Reveal::Unit::CHARACTER;
      }
      ConfigureAndReplay();
    });
    mAsyncButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      mAsync = !mAsync;
      ConfigureAndReplay();
    });
    mFillButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      switch(mFillMode)
      {
        case FillMode::TEXT_GRADIENT:
          mFillMode = FillMode::GRADIENT_SPAN;
          break;
        case FillMode::GRADIENT_SPAN:
          mFillMode = FillMode::SOLID;
          break;
        case FillMode::SOLID:
          mFillMode = FillMode::WHITE_ON_BLACK;
          break;
        case FillMode::WHITE_ON_BLACK:
          mFillMode = FillMode::TEXT_GRADIENT;
          break;
      }
      ConfigureAndReplay();
    });
    mRevealButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      if(mUxPhase != UxPlaybackPhase::NONE)
      {
        StopAnimation();
        mStopPlayButton.SetText("Play");
        mPlaybackStatus = "UX playback cancelled by Reveal configuration.";
      }
      mRevealEnabled = !mRevealEnabled;
      ApplyRevealConfiguration();
      if(mUxPreviewHidden)
      {
        mUxPreviewHidden = false;
        mPreview.SetProperty(Actor::Property::VISIBLE, true);
      }
      mRevealButton.SetText(mRevealEnabled ? "Reveal: On" : "Reveal: Off");
      SetButtonSelected(mRevealButton, mRevealEnabled);
      UpdateStatus();
    });
    replayButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      Replay();
    });
    mStopPlayButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      StopOrPlay();
    });
    reverseButton.AsInteractive().ClickedSignal().Connect(this, [this](View, InputEvent)
    {
      Reverse();
    });

    ConfigureAndReplay();
  }

  void ConfigureAndReplay()
  {
    ApplyRevealConfiguration();
    mPreview.SetAsyncRendering(mAsync);
    ApplyFill();

    mUnitButton.SetText(GetUnitButtonLabel());
    mAsyncButton.SetText(mAsync ? "Path: Async" : "Path: Sync");
    mFillButton.SetText(GetFillButtonLabel());
    mRevealButton.SetText(mRevealEnabled ? "Reveal: On" : "Reveal: Off");
    SetButtonSelected(mAsyncButton, mAsync);
    SetButtonSelected(mFillButton, mFillMode != FillMode::SOLID);
    SetButtonSelected(mRevealButton, mRevealEnabled);
    UpdateTextButtons();
    UpdateSequenceButtons();
    UpdateStaggerButtons();
    UpdateFadeButtons();
    UpdateDurationButtons();
    UpdateAlphaFunctionButtons();
    UpdateBlurButtons();
    Replay();
  }

  StackLayout NewCycleRow(const char* title, Label& selected, std::function<void(int)> cycle)
  {
    StackLayout row      = NewMenuRow(title);
    Label       previous = NewCompactButton("<", 32.0f);
    selected             = NewButton("");
    Label next           = NewCompactButton(">", 32.0f);
    row.Add(previous);
    row.Add(selected);
    row.Add(next);
    previous.AsInteractive().ClickedSignal().Connect(this, [cycle](View, InputEvent)
    {
      cycle(-1);
    });
    next.AsInteractive().ClickedSignal().Connect(this, [cycle](View, InputEvent)
    {
      cycle(1);
    });
    selected.AsInteractive().ClickedSignal().Connect(this, [cycle](View, InputEvent)
    {
      cycle(1);
    });
    return row;
  }

  TextSample::Slider& GetSlider(SliderId id)
  {
    return *mSliders[static_cast<std::size_t>(id)];
  }

  View NewSlider(SliderId id, const char* name, float minimum, float maximum, float step, int decimals, float value,
                 std::function<void(float)> commit)
  {
    auto& entry  = mSliders[static_cast<std::size_t>(id)];
    entry        = std::make_unique<TextSample::Slider>(name, minimum, maximum, step, decimals, value);
    auto* slider = entry.get();
    slider->Started.Connect(this, [this, slider]()
    {
      CancelSliderEdit();
      FreezePlayback();
      mActiveSlider = slider;
      mControlsViewport.SetPanScrollEnabled(false);
    });
    slider->Committed.Connect(this, [this, commit](float current)
    {
      mActiveSlider = nullptr;
      mControlsViewport.SetPanScrollEnabled(true);
      commit(current);
    });
    slider->Cancelled.Connect(this, [this, id](float original)
    {
      mActiveSlider = nullptr;
      mControlsViewport.SetPanScrollEnabled(true);
      if(id == SliderId::PROGRESS)
      {
        mPreview.SetTextRevealProgress(original);
      }
    });
    if(id == SliderId::PROGRESS)
    {
      slider->Changed.Connect(this, [this](float current)
      {
        mPreview.SetTextRevealProgress(current);
        RestoreUxPreview();
      });
    }
    return slider->GetView();
  }

  void CancelSliderEdit()
  {
    if(mActiveSlider)
    {
      auto* slider  = mActiveSlider;
      mActiveSlider = nullptr;
      slider->Cancel();
    }
  }

  void FreezePlayback()
  {
    const float progress = mPreview.GetTextRevealProgress();
    StopAnimation();
    mPreview.SetTextRevealProgress(progress);
    mStopPlayButton.SetText("Play");
    mPlaybackStatus = "Paused for editing. Settings apply on release; progress seeks live.";
    UpdateStatus();
  }

  void UpdatePreviewLayout()
  {
    if(mPreviewViewportSize.x <= 0.0f || mPreviewViewportSize.y <= 0.0f)
    {
      return;
    }
    const Vector2 available = mPreviewViewportSize / mUiScale;
    if(mFitPreview)
    {
      mPreviewWidth  = std::clamp(std::floor(available.x - 2.0f * PREVIEW_GUTTER), 80.0f, 1600.0f);
      mPreviewHeight = std::clamp(std::floor(available.y - 2.0f * PREVIEW_GUTTER), 48.0f, 1200.0f);
    }
    // Resize the layout, not the Actor scale. Manual size survives a window
    // resize; the scrollable workspace includes margin for the blur halo.
    mPreview.SetRequestedWidth(mPreviewWidth);
    mPreview.SetRequestedHeight(mPreviewHeight);
    mPreview.SetRequestedX(std::max(PREVIEW_GUTTER, (available.x - mPreviewWidth) * 0.5f));
    mPreviewCanvas.SetRequestedWidth(std::max(available.x, mPreviewWidth + 2.0f * PREVIEW_GUTTER));
    mPreviewCanvas.SetRequestedHeight(std::max(available.y, mPreviewHeight + 2.0f * PREVIEW_GUTTER));
    GetSlider(SliderId::WIDTH).SetValue(mPreviewWidth);
    GetSlider(SliderId::HEIGHT).SetValue(mPreviewHeight);
    mFitButton.SetText(mFitPreview ? "Fit: On" : "Fit: Off");
    SetButtonSelected(mFitButton, mFitPreview);
    UpdateStatus();
  }

  void SetTextCase(std::size_t textIndex)
  {
    mTextCaseIndex = textIndex % TEXT_CASE_COUNT;
    ApplyFill();
    UpdateTextButtons();
    Replay();
  }

  const char* GetUnitButtonLabel() const
  {
    switch(mUnit)
    {
      case Text::Reveal::Unit::WORD:
        return "Unit: Word";
      case Text::Reveal::Unit::LINE:
        return "Unit: Line";
      case Text::Reveal::Unit::PIXEL:
        return "Unit: Pixel";
      case Text::Reveal::Unit::CHARACTER:
      default:
        return "Unit: Character";
    }
  }

  const char* GetUnitStatusLabel() const
  {
    switch(mUnit)
    {
      case Text::Reveal::Unit::WORD:
        return "WORD";
      case Text::Reveal::Unit::LINE:
        return "LINE";
      case Text::Reveal::Unit::PIXEL:
        return "PIXEL";
      case Text::Reveal::Unit::CHARACTER:
      default:
        return "CHARACTER";
    }
  }

  const char* GetFillButtonLabel() const
  {
    switch(mFillMode)
    {
      case FillMode::SOLID:
        return "Fill: Solid";
      case FillMode::WHITE_ON_BLACK:
        return "Fill: White/Black";
      case FillMode::GRADIENT_SPAN:
        return "Fill: Span";
      case FillMode::TEXT_GRADIENT:
      default:
        return "Fill: Gradient";
    }
  }

  const char* GetFillStatusLabel() const
  {
    switch(mFillMode)
    {
      case FillMode::SOLID:
        return "Solid";
      case FillMode::WHITE_ON_BLACK:
        return "White on black";
      case FillMode::GRADIENT_SPAN:
        return "GradientSpan (Linear/Radial/Conic, SPAN_BOUND)";
      case FillMode::TEXT_GRADIENT:
      default:
        return "TextGradient";
    }
  }

  void ApplyRevealConfiguration()
  {
    if(!mRevealEnabled)
    {
      // Reveal::None() disables the effect while preserving authored progress.
      mPreview.SetTextReveal(Text::Reveal::None());
      return;
    }

    Text::Reveal reveal;
    reveal.SetUnit(mUnit);
    reveal.SetSequence(SEQUENCES[mSequenceIndex]);
    reveal.SetSequenceStaggerRatio(mStaggerRatio);
    reveal.SetFadeDurationRatio(mAutoFade ? Text::Reveal::AUTO_FADE_DURATION_RATIO : mManualFade);
    RevealBlur::SetBlurRadius(reveal, mBlurEnabled ? mBlurRadius : 0.0f);
    RevealBlur::SetBlurDurationRatio(reveal, mBlurDurationRatio);
    RevealBlur::SetBlurMode(reveal, mBlurMode);
    mPreview.SetTextReveal(reveal);
  }

  void UpdateFadeButtons()
  {
    SetButtonSelected(mAutoFadeButton, mAutoFade);
    GetSlider(SliderId::FADE).SetValue(mManualFade);
    GetSlider(SliderId::FADE).SetEnabled(!mAutoFade);
  }

  const char* GetBlurBypassReason() const
  {
    if(mBlurDurationRatio == 0.0f)
    {
      return "blur duration is zero";
    }
    if(!mRevealEnabled)
    {
      return "Reveal is Off";
    }
    return nullptr;
  }

  void SetBlurMode(RevealBlur::BlurMode mode)
  {
    if(mBlurMode != mode)
    {
      mBlurMode = mode;
      UpdateBlurButtons();
      Replay();
    }
  }

  void UpdateBlurButtons()
  {
    mBlurButton.SetText(mBlurEnabled ? "On" : "Off");
    SetButtonSelected(mBlurButton, mBlurEnabled);
    SetButtonSelected(mQualityBlurButton, mBlurMode == RevealBlur::BlurMode::QUALITY);
    SetButtonSelected(mBlurPerformanceButton, mBlurMode == RevealBlur::BlurMode::PERFORMANCE);
    GetSlider(SliderId::RADIUS).SetValue(mBlurRadius);
    GetSlider(SliderId::BLUR_TIME).SetValue(mBlurDurationRatio);
  }

  void UpdateSequenceButtons()
  {
    for(std::size_t sequenceIndex = 0u; sequenceIndex < SEQUENCE_CASE_COUNT; ++sequenceIndex)
    {
      SetButtonSelected(mSequenceButtons[sequenceIndex], sequenceIndex == mSequenceIndex);
    }
  }

  void UpdateStaggerButtons()
  {
    GetSlider(SliderId::STAGGER).SetValue(mStaggerRatio);
  }

  void UpdateDurationButtons()
  {
    GetSlider(SliderId::DURATION).SetValue(mDurationSeconds);
  }

  void UpdateAlphaFunctionButtons()
  {
    mAlphaButton.SetText(ALPHA_FUNCTION_BUTTON_LABELS[mAlphaFunctionIndex]);
  }

  void UpdateTextButtons()
  {
    mTextButton.SetText(TEXT_BUTTON_LABELS[mTextCaseIndex]);
  }

  void ApplyFill()
  {
    // Set the direction for every corpus so leaving the RTL case restores LTR.
    // Only the preview changes direction; the sample controls keep their layout.
    mPreview.SetLayoutDirection(mTextCaseIndex == RTL_TEXT_CASE_INDEX ? LayoutDirection::RIGHT_TO_LEFT : LayoutDirection::LEFT_TO_RIGHT);
    const bool whiteOnBlack = mFillMode == FillMode::WHITE_ON_BLACK;
    mPreview.SetBackgroundColor(UiColor(whiteOnBlack ? 0x000000 : 0xFFFFFF));
    mPreview.SetTextColor(UiColor(whiteOnBlack ? 0xFFFFFF : 0x0F172A));
    mPreview.SetTextGradient(Gradient::Base::None());

    const bool isImageText = mTextCaseIndex == LOCAL_IMAGE_CASE_INDEX ||
                             mTextCaseIndex == REMOTE_IMAGE_CASE_INDEX;
    if(mFillMode == FillMode::GRADIENT_SPAN || isImageText)
    {
      mPreview.SetStyledText(BuildCurrentStyledText(mFillMode == FillMode::GRADIENT_SPAN));
    }
    else
    {
      mPreview.SetText(TEXT_CASES[mTextCaseIndex]);
    }

    if(mFillMode != FillMode::TEXT_GRADIENT)
    {
      return;
    }

    Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
    gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
    gradient.SetStopNodes({Gradient::StopNode(0.0f, UiColor(0x2563EB)),
                           Gradient::StopNode(0.5f, UiColor(0x7C3AED)),
                           Gradient::StopNode(1.0f, UiColor(0xEA580C))});
    mPreview.SetTextGradient(gradient);
  }

  Text::StyledText BuildCurrentStyledText(bool applyGradientSpans) const
  {
    const std::string       source(TEXT_CASES[mTextCaseIndex]);
    Text::StyledTextBuilder builder = Text::StyledTextBuilder::New(source.c_str());

    if(mTextCaseIndex == LOCAL_IMAGE_CASE_INDEX || mTextCaseIndex == REMOTE_IMAGE_CASE_INDEX)
    {
      builder.AppendText(" ");
      const uint32_t imageIndex = builder.GetUtf32Length();
      builder.AppendText(Text::ReplacementSpan::OBJECT_REPLACEMENT_CHARACTER);

      const bool            remoteImage = mTextCaseIndex == REMOTE_IMAGE_CASE_INDEX;
      Text::ImageAttributes imageAttributes(remoteImage ? REMOTE_IMAGE_URL : RESOURCES_DIR "flag_kr.png",
                                            remoteImage ? Vector2(96.0f, 69.0f) : Vector2(48.0f, 32.0f));
      imageAttributes.SetAlignment(Text::ImageAttributes::InlineAlignment::TEXT_CENTER);
      DALI_ASSERT_ALWAYS(builder.SetSpan(Text::ImageSpan::New(imageAttributes),
                                         imageIndex,
                                         imageIndex + 1u));
    }

    if(applyGradientSpans)
    {
      const SpanGradientKind kinds[3u] = {
        SpanGradientKind::LINEAR,
        SpanGradientKind::RADIAL,
        SpanGradientKind::CONIC};
      for(std::size_t index = 0u; index < 3u; ++index)
      {
        const char*       word      = GRADIENT_SPAN_WORDS[mTextCaseIndex][index];
        const std::size_t byteStart = source.find(word);
        DALI_ASSERT_ALWAYS(byteStart != std::string::npos);

        uint32_t utf32Start = 0u;
        uint32_t utf32End   = 0u;
        DALI_ASSERT_ALWAYS(Text::Utf8ToUtf32Range(Dali::StringView(source.data(), static_cast<uint32_t>(source.size())),
                                                  static_cast<uint32_t>(byteStart),
                                                  static_cast<uint32_t>(byteStart + std::string(word).size()),
                                                  utf32Start,
                                                  utf32End));
        DALI_ASSERT_ALWAYS(builder.SetSpan(Text::GradientSpan::New(CreateGradientSpanGradient(kinds[index]),
                                                                   Text::GradientSpan::BoundsMode::SPAN_BOUND),
                                           utf32Start,
                                           utf32End));
      }
    }

    return builder.Build();
  }

  float GetAnimationDuration() const
  {
    return mDurationSeconds;
  }

  void ApplyPreset(bool exiting, bool secondEntrance)
  {
    // Preserve the selected text, fill, Unit and render path.
    mRevealEnabled      = true;
    mBlurEnabled        = true;
    mSequenceIndex      = exiting ? 0u : 1u; // WHOLE_TEXT / PER_LINE
    mAutoFade           = false;
    mManualFade         = exiting ? 1.0f : 0.0f;
    mStaggerRatio       = exiting || secondEntrance ? 0.0f : 0.25f;
    mDurationSeconds    = exiting || secondEntrance ? 1.0f : 2.0f;
    mAlphaFunctionIndex = exiting ? 0u : 2u; // LINEAR / EASE_OUT_SQUARE
    mBlurRadius         = exiting ? 48.0f : 24.0f;
    mBlurDurationRatio  = exiting || secondEntrance ? 1.0f : 0.5f;
    ApplyRevealConfiguration();
    mRevealButton.SetText("Reveal: On");
    SetButtonSelected(mRevealButton, true);
    UpdateSequenceButtons();
    UpdateFadeButtons();
    UpdateStaggerButtons();
    UpdateDurationButtons();
    UpdateAlphaFunctionButtons();
    UpdateBlurButtons();
  }

  void PlayUxTest(bool secondEntrance)
  {
    StopAnimation();
    mUxSecondEntrance = secondEntrance;
    ApplyPreset(false, secondEntrance);
    mPreview.SetTextRevealProgress(0.0f);
    RestoreUxPreview();
    mUxPhase        = UxPlaybackPhase::ENTERING;
    mPlaybackStatus = secondEntrance ? "Preset 2: entering for 1 s (Out Square), then wait 1 s."
                                     : "Preset 1: entering for 2 s (Out Square), then wait 1 s.";
    StartAnimation(1.0f, GetAnimationDuration());
  }

  void RestoreUxPreview()
  {
    if(mUxPreviewHidden)
    {
      mUxPreviewHidden = false;
      mRevealEnabled   = true;
      ApplyRevealConfiguration();
      mRevealButton.SetText("Reveal: On");
      SetButtonSelected(mRevealButton, true);
      mPreview.SetProperty(Actor::Property::VISIBLE, true);
    }
  }

  void WaitForUxExit()
  {
    // Start the hold after actual animation completion, not an estimated
    // creation timestamp. A cancelled run must never start a delayed exit.
    mUxPhase        = UxPlaybackPhase::WAITING;
    mPlaybackStatus = "Preset: fully visible; waiting 1 s before changing to the exit settings.";
    mStopPlayButton.SetText("Stop");
    const uint32_t run = mUxRun;
    mUxWaitTimer       = Timer::New(1000u);
    mUxWaitTimer.TickSignal().Connect(this, [this, run]() -> bool
    {
      if(run != mUxRun || mUxPhase != UxPlaybackPhase::WAITING)
      {
        return false;
      }
      mUxPhase = UxPlaybackPhase::EXITING;
      ApplyPreset(true, mUxSecondEntrance);
      mPlaybackStatus = "Preset: exiting for 1 s (Linear), Whole Text, fade 1.0, blur 48 px / time 1.0.";
      StartAnimation(0.0f, 1.0f);
      return false;
    });
    mUxWaitTimer.Start();
    UpdateStatus();
  }

  void FinishUxTest()
  {
    mUxPhase = UxPlaybackPhase::NONE;
    mUxWaitTimer.Stop();
    mUxWaitTimer.Reset();
    // None restores ordinary text. Hide the preview first so cleanup does not
    // make the text reappear; Replay or a manual progress change restores it.
    mPreview.SetProperty(Actor::Property::VISIBLE, false);
    mUxPreviewHidden = true;
    mRevealEnabled   = false;
    ApplyRevealConfiguration();
    mRevealButton.SetText("Reveal: Off");
    SetButtonSelected(mRevealButton, false);
    mPlaybackStatus = "Preset finished: preview hidden and Reveal::None() applied. Replay or PRESET restores it.";
    UpdateStatus();
  }

  void Replay()
  {
    StopAnimation();
    ApplyRevealConfiguration();
    mPreview.SetTextRevealProgress(0.0f);
    RestoreUxPreview();
    const float duration = GetAnimationDuration();

    std::ostringstream playback;
    playback << "Replay: progress 0.00 -> 1.00 over " << std::fixed << std::setprecision(1)
             << duration << " s.";
    mPlaybackStatus = playback.str();
    StartAnimation(1.0f, duration);
  }

  void StopOrPlay()
  {
    if(mAnimationRunning || mUxPhase == UxPlaybackPhase::WAITING)
    {
      const float progress = mPreview.GetTextRevealProgress();
      StopAnimation();
      mPreview.SetTextRevealProgress(progress);
      mStopPlayButton.SetText("Play");

      std::ostringstream playback;
      playback << "Stopped at progress " << std::fixed << std::setprecision(2) << progress << ".";
      mPlaybackStatus = playback.str();
      UpdateStatus();
      return;
    }

    const float progress = mPreview.GetTextRevealProgress();
    if(progress >= 1.0f)
    {
      Replay();
      return;
    }

    // Scale the resume duration by the remaining progress.
    RestoreUxPreview();
    const float        remainingDuration = GetAnimationDuration() * (1.0f - progress);
    std::ostringstream playback;
    playback << "Play: progress " << std::fixed << std::setprecision(2) << progress
             << " -> 1.00 over " << std::setprecision(1) << remainingDuration << " s.";
    mPlaybackStatus = playback.str();
    StartAnimation(1.0f, remainingDuration);
  }

  void Reverse()
  {
    const float progress = mPreview.GetTextRevealProgress();
    StopAnimation();
    mPreview.SetTextRevealProgress(progress);
    RestoreUxPreview();

    if(progress <= 0.0f)
    {
      mStopPlayButton.SetText("Play");
      mPlaybackStatus = "Already hidden at progress 0.00.";
      UpdateStatus();
      return;
    }

    const float        reverseDuration = GetAnimationDuration() * progress;
    std::ostringstream playback;
    playback << "Reverse: progress " << std::fixed << std::setprecision(2) << progress
             << " -> 0.00 over " << std::setprecision(1) << reverseDuration << " s.";
    mPlaybackStatus = playback.str();
    StartAnimation(0.0f, reverseDuration);
  }

  void StartAnimation(float target, float duration)
  {
    mAnimationTarget  = target;
    mAnimationRunning = true;
    mStopPlayButton.SetText("Stop");
    mAnimation = Animation::New(duration);
    mPreview.Animate(mAnimation).TextRevealProgress(target, Duration(duration), AlphaFunction(ALPHA_FUNCTIONS[mAlphaFunctionIndex]));
    mAnimation.FinishedSignal().Connect(this, [this](Animation animation)
    {
      if(animation != mAnimation)
      {
        return;
      }

      mAnimationRunning = false;
      mProgressTimer.Stop();
      GetSlider(SliderId::PROGRESS).SetValue(mAnimationTarget);
      mAnimation.Reset();
      mStopPlayButton.SetText("Play");
      if(mUxPhase == UxPlaybackPhase::ENTERING)
      {
        WaitForUxExit();
        return;
      }
      if(mUxPhase == UxPlaybackPhase::EXITING)
      {
        FinishUxTest();
        return;
      }
      mPlaybackStatus = mAnimationTarget > 0.5f ? "Finished at progress 1.00."
                                                : "Reverse finished at progress 0.00.";
      UpdateStatus();
    });
    mAnimation.Play();
    if(!mProgressTimer)
    {
      mProgressTimer = Timer::New(33u);
      mProgressTimer.TickSignal().Connect(this, [this]()
      {
        if(!mAnimationRunning)
        {
          return false;
        }
        // Update only the small readout/track, not the multiline status Label.
        GetSlider(SliderId::PROGRESS).SetValue(mPreview.GetTextRevealProgress());
        return true;
      });
    }
    mProgressTimer.Start();
    UpdateStatus();
  }

  void StopAnimation()
  {
    CancelSliderEdit();
    if(mProgressTimer)
    {
      mProgressTimer.Stop();
    }
    ++mUxRun;
    mUxPhase = UxPlaybackPhase::NONE;
    if(mUxWaitTimer)
    {
      mUxWaitTimer.Stop();
      mUxWaitTimer.Reset();
    }
    if(mAnimation)
    {
      mAnimation.Stop();
      mAnimation.Reset();
    }
    mAnimationRunning = false;
  }

  const char* GetFadeDescription() const
  {
    if(mAutoFade)
    {
      return mUnit == Text::Reveal::Unit::PIXEL
               ? "Auto: fade adapts to the visible spatial range and text scale."
               : "Auto: fade adapts to the final visible unit count.";
    }
    if(mManualFade == 0.0f)
    {
      return mUnit == Text::Reveal::Unit::PIXEL
               ? "Fade 0: a continuous hard reveal front."
               : "Fade 0: each unit appears at its scheduled progress.";
    }
    return mManualFade == 1.0f
             ? "Fade 1: units within each sequence share the same fade interval."
             : "Fade controls each unit's transition interval; larger values increase overlap.";
  }

  void UpdateStatus()
  {
    const char*        revealDescription = mRevealEnabled
                                             ? GetFadeDescription()
                                             : "Reveal is unset with Text::Reveal::None(); progress is preserved.";
    std::ostringstream status;
    status << std::fixed << std::setprecision(0)
           << "Label " << mPreviewWidth << " x " << mPreviewHeight
           << " | Scale " << std::setprecision(1) << mUiScale
           << " | " << (mAsync ? "Async" : "Sync")
           << " | " << (mRevealEnabled ? "Reveal On" : "Reveal Off")
           << "\n"
           << mPlaybackStatus;
    if(mShowDetails)
    {
      status << "\n"
             << GetUnitStatusLabel() << " / " << SEQUENCE_STATUS_LABELS[mSequenceIndex]
             << " / " << GetFillStatusLabel() << "\n"
             << revealDescription
             << "\nDuration includes Reveal + blur; there is no extra tail. Blur Time is relative to one common sequence interval. Alpha affects both effects."
             << "\np=1 is sharp. Reverse retraces progress. Reflow remaps the current clock. ImageSpan shares its sequence timing.";
      if(mBlurEnabled && GetBlurBypassReason())
      {
        status << "\nBlur bypassed: " << GetBlurBypassReason();
      }
      status << "\nClick/drag sliders; Left/Right fine-tune, Home/End reach endpoints. Settings apply on release. Width/height are unscaled layout units; Fit follows the viewport."
             << "\nQ/W/E/R/T: UI scale 0.8/1.0/1.2/1.4/2.0 | Scroll preview/controls | ESC: Exit";
    }
    mStatus.SetText(status.str().c_str());
  }

  void UpdateControlsViewport()
  {
    if(mControlsViewport)
    {
      // Keep a visible preview on small windows and at larger UI scales.
      const float height = static_cast<float>(mApplication.GetWindow().GetPositionSize().height);
      mControlsViewport.SetRequestedHeight(std::min(CONTROLS_PANEL_HEIGHT, height * 0.60f / mUiScale));
    }
  }

  void SetUiScale(float scale)
  {
    CancelSliderEdit();
    mUiScale = scale;
    UiScaleManager::Get().SetScale(scale);
    UpdateControlsViewport();
    mPreviewLayoutTimer.Start();
    if(mBlurEnabled)
    {
      Replay();
    }
    else
    {
      UpdateStatus();
    }
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      StopAnimation();
      mApplication.Quit();
      return;
    }

    const Dali::String& keyName = event.GetKeyName();
    if(keyName == "q" || keyName == "Q")
    {
      SetUiScale(0.8f);
    }
    else if(keyName == "w" || keyName == "W")
    {
      SetUiScale(1.0f);
    }
    else if(keyName == "e" || keyName == "E")
    {
      SetUiScale(1.2f);
    }
    else if(keyName == "r" || keyName == "R")
    {
      SetUiScale(1.4f);
    }
    else if(keyName == "t" || keyName == "T")
    {
      SetUiScale(2.0f);
    }
  }

private:
  using SliderList = std::array<std::unique_ptr<TextSample::Slider>, static_cast<std::size_t>(SliderId::COUNT)>;

  Application&                           mApplication;
  Label                                  mPreview;
  Label                                  mUnitButton;
  std::array<Label, SEQUENCE_CASE_COUNT> mSequenceButtons;
  SliderList                             mSliders;
  TextSample::Slider*                    mActiveSlider{nullptr};
  Label                                  mTextButton;
  Label                                  mAlphaButton;
  Label                                  mAutoFadeButton;
  Label                                  mFitButton;
  ScrollView                             mPreviewViewport;
  View                                   mPreviewCanvas;
  Vector2                                mPreviewViewportSize;
  Timer                                  mPreviewLayoutTimer;
  Timer                                  mProgressTimer;
  Label                                  mBlurButton;
  Label                                  mQualityBlurButton;
  Label                                  mBlurPerformanceButton;
  ScrollView                             mControlsViewport;
  Label                                  mAsyncButton;
  Label                                  mFillButton;
  Label                                  mRevealButton;
  Label                                  mStopPlayButton;
  Label                                  mStatus;
  Animation                              mAnimation;
  Timer                                  mUxWaitTimer;
  UxPlaybackPhase                        mUxPhase{UxPlaybackPhase::NONE};
  uint32_t                               mUxRun{0u};
  bool                                   mUxSecondEntrance{false};
  bool                                   mUxPreviewHidden{false};
  std::string                            mPlaybackStatus;
  Text::Reveal::Unit                     mUnit{Text::Reveal::Unit::CHARACTER};
  std::size_t                            mSequenceIndex{0u};
  std::size_t                            mTextCaseIndex{DEFAULT_TEXT_CASE_INDEX};
  std::size_t                            mAlphaFunctionIndex{0u};
  float                                  mManualFade{0.25f};
  float                                  mStaggerRatio{0.0f};
  float                                  mDurationSeconds{4.0f};
  float                                  mBlurRadius{32.0f};
  float                                  mBlurDurationRatio{1.0f};
  float                                  mPreviewWidth{550.0f};
  float                                  mPreviewHeight{300.0f};
  bool                                   mAutoFade{true};
  bool                                   mFitPreview{true};
  bool                                   mShowDetails{false};
  bool                                   mBlurEnabled{false};
  bool                                   mAsync{false};
  FillMode                               mFillMode{FillMode::TEXT_GRADIENT};
  bool                                   mRevealEnabled{true};
  bool                                   mAnimationRunning{false};
  float                                  mAnimationTarget{1.0f};
  float                                  mUiScale{1.0f};
  RevealBlur::BlurMode                    mBlurMode{RevealBlur::BlurMode::PERFORMANCE};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TextRevealController controller(application);
  application.MainLoop();
  return 0;
}
