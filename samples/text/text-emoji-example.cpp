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

/**
 * Emoji Example Application
 *
 * This sample demonstrates emoji rendering with Dali UI Label.
 * It was ported from the NUI C# emoji & async performance test to Dali UI by Cline.
 *
 * Features:
 * - Display various emoji sequences (ZWJ, VS15, VS16, skin tone, family, flags)
 * - Tap emoji to add to work bench
 * - History log panel
 * - Scale up/down with keyboard
 * - Async/Sync rendering toggle
 * - Scale test timer
 */

#include <dali/integration-api/debug.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/devel-api/ui-foundation-pre-initialize.h>

#include <array>
#include <map>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// Tag for logging
constexpr const char* TAG = "EMOJI";

// Configuration constants
constexpr float    DEFAULT_SCALE             = 1.2f;
constexpr float    SCALE_STEP                = 0.2f;
constexpr int      DEFAULT_ROW               = 18;
constexpr int      DEFAULT_COL               = 19;
constexpr float    DEFAULT_PIXEL_SIZE        = 29.0f;
constexpr int      DEFAULT_CELL              = 1;
constexpr int      DEFAULT_WIDTH             = 30;
constexpr int      DEFAULT_HEIGHT            = 29;
constexpr int      DEFAULT_LOG_WIDTH         = 300;
constexpr int      NUM_OF_BUTTON             = 4;
constexpr uint32_t SCALE_TEST_INTERVAL_MS    = 500;

// Colors
constexpr uint32_t COLOR_WHITE      = 0xFFFFFF;
constexpr uint32_t COLOR_BLACK      = 0x000000;
constexpr uint32_t COLOR_LIGHT_GRAY = 0xEEEEEE;

// Font family
constexpr const char* FONT_FAMILY = "SamsungOneUI_400";

// Unicode constants for emoji sequences
constexpr const char* ZWJ  = "\u200d";   // Zero Width Joiner
constexpr const char* ZWNJ = "\u200c";   // Zero Width Non Joiner
constexpr const char* VS15 = "\ufe0e";   // Variation Selector 15 (text)
constexpr const char* VS16 = "\ufe0f";   // Variation Selector 16 (emoji)

} // namespace

/**
 * Tracked label info for in-place scale updates
 */
struct TrackedLabel
{
  Label label;
  bool  updateWidth;
  bool  updateHeight;
  bool  updateFontSize;
  float fontSizeScale;  // Multiplier for mPixelSize

  TrackedLabel()
  : label(),
    updateWidth(true),
    updateHeight(true),
    updateFontSize(true),
    fontSizeScale(1.0f)
  {}
};

/**
 * Tracked container info for in-place scale updates
 */
struct TrackedContainer
{
  View container;
  bool updateWidth;
  bool updateHeight;
  int  widthMultiplier;  // Multiplier for (mWidth + mCell)
  int  heightMultiplier; // Multiplier for (mHeight + mCell)

  TrackedContainer()
  : container(),
    updateWidth(false),
    updateHeight(false),
    widthMultiplier(0),
    heightMultiplier(0)
  {}
};

/**
 * Tracked log row for in-place scale updates
 */
struct TrackedLogRow
{
  View  row;
  Label emojiLabel;
  Label descLabel;

  TrackedLogRow()
  : row(),
    emojiLabel(),
    descLabel()
  {}
};

/**
 * Emoji Example Controller
 */
class EmojiController : public ConnectionTracker
{
public:
  explicit EmojiController(Application& application)
  : mApplication(application),
    mScale(DEFAULT_SCALE),
    mAsyncRendering(true),
    mPixelSize(0.0f),
    mCell(DEFAULT_CELL),
    mWidth(0),
    mHeight(0),
    mLogWidth(0),
    mLogHeight(0),
    mScaleTestTimer(),
    mScaleTestRunning(false),
    mScaleTestInterval(SCALE_TEST_INTERVAL_MS)
  {
    mApplication.InitSignal().Connect(this, &EmojiController::OnInit);
  }

private:
  static bool HasText(const char* text)
  {
    return text && text[0] != '\0';
  }

  int GetBenchLength() const
  {
    return (mWidth + mCell) * (DEFAULT_ROW - NUM_OF_BUTTON) - mCell;
  }

  int GetWindowWidth() const
  {
    return (mWidth + mCell) * DEFAULT_ROW + mLogWidth + 1;
  }

  int GetWindowHeight() const
  {
    return (mHeight + mCell) * DEFAULT_COL;
  }

  void RegisterDescription(const char* unicode, const char* description)
  {
    if(HasText(unicode) && HasText(description))
    {
      mEmojiDictionary[unicode] = description;
    }
  }

  void IncreaseScale()
  {
    mScale += SCALE_STEP;
    if(mScale > 2.6f)
    {
      mScale = 0.6f;
    }
  }

  void DecreaseScale()
  {
    mScale -= SCALE_STEP;
    if(mScale < 0.59f)
    {
      mScale = 2.4f;
    }
  }

  void SetAsyncRenderingMode(bool enabled)
  {
    mAsyncRendering = enabled;
    DALI_LOG_ERROR("[%s] %s rendering\n", TAG, enabled ? "Async" : "Sync");

    // Update all existing labels
    for(auto& info : mTrackedEmojiLabels)
    {
      if(info.label)
      {
        info.label.SetAsyncRendering(enabled);
      }
    }
    for(auto& info : mTrackedOverlayLabels)
    {
      if(info.label)
      {
        info.label.SetAsyncRendering(enabled);
      }
    }
    for(auto& info : mTrackedLogRows)
    {
      if(info.emojiLabel)
      {
        info.emojiLabel.SetAsyncRendering(enabled);
      }
      if(info.descLabel)
      {
        info.descLabel.SetAsyncRendering(enabled);
      }
    }
    if(mTrackedWorkbench.label)
    {
      mTrackedWorkbench.label.SetAsyncRendering(enabled);
    }
    if(mTrackedTitleLabel.label)
    {
      mTrackedTitleLabel.label.SetAsyncRendering(enabled);
    }
  }

  void OnInit(Application& application)
  {
    DALI_LOG_ERROR("[%s] Application OnInit\n", TAG);

    SetConfig();
    GenerateUI();
    GenerateEmoji();

    window = mApplication.GetWindow();
    window.KeyEventSignal().Connect(this, &EmojiController::OnKeyEvent);

    mScaleTestTimer = Timer::New(mScaleTestInterval);
    mScaleTestTimer.TickSignal().Connect(this, &EmojiController::OnScaleTestTick);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Configuration
  //////////////////////////////////////////////////////////////////////////////

  void SetConfig()
  {
    mPixelSize = DEFAULT_PIXEL_SIZE * mScale;
    mCell      = DEFAULT_CELL;
    mWidth     = static_cast<int>(DEFAULT_WIDTH * mScale);
    mHeight    = static_cast<int>(DEFAULT_HEIGHT * mScale);
    mLogWidth  = static_cast<int>(DEFAULT_LOG_WIDTH * mScale);

    // Make height odd
    if((mHeight % 2) == 0)
    {
      ++mHeight;
    }

    mLogHeight = (mHeight + mCell) * (DEFAULT_COL - 1) - 1;
  }

  //////////////////////////////////////////////////////////////////////////////
  // UI Tracking for In-Place Scale Updates
  //////////////////////////////////////////////////////////////////////////////

  void TrackEmojiLabel(Label label)
  {
    TrackedLabel info;
    info.label          = label;
    info.updateWidth    = true;
    info.updateHeight   = true;
    info.updateFontSize = true;
    info.fontSizeScale  = 1.0f;
    mTrackedEmojiLabels.push_back(info);
  }

  void TrackOverlayLabel(Label label)
  {
    TrackedLabel info;
    info.label          = label;
    info.updateWidth    = false;
    info.updateHeight   = false;
    info.updateFontSize = true;
    info.fontSizeScale  = 1.0f / 3.0f;
    mTrackedOverlayLabels.push_back(info);
  }

  void TrackWorkbench(Label label)
  {
    mTrackedWorkbench.label          = label;
    mTrackedWorkbench.updateWidth    = true;
    mTrackedWorkbench.updateHeight   = true;
    mTrackedWorkbench.updateFontSize = true;
    mTrackedWorkbench.fontSizeScale  = 1.0f;
  }

  void TrackTitleLabel(Label label)
  {
    mTrackedTitleLabel.label          = label;
    mTrackedTitleLabel.updateWidth    = false;
    mTrackedTitleLabel.updateHeight   = true;
    mTrackedTitleLabel.updateFontSize = true;
    mTrackedTitleLabel.fontSizeScale  = 1.0f / 3.0f;
  }

  void TrackMainContainer(View container)
  {
    mTrackedMainContainer.container       = container;
    mTrackedMainContainer.updateWidth     = true;
    mTrackedMainContainer.updateHeight    = true;
    mTrackedMainContainer.widthMultiplier  = DEFAULT_ROW;
    mTrackedMainContainer.heightMultiplier = DEFAULT_COL;
  }

  void TrackLogMainContainer(View container)
  {
    mTrackedLogMainContainer.container       = container;
    mTrackedLogMainContainer.updateWidth     = false;
    mTrackedLogMainContainer.updateHeight    = true;
    mTrackedLogMainContainer.heightMultiplier = DEFAULT_COL;
  }

  void TrackLogView(View container)
  {
    mTrackedLogView.container = container;
  }

  void TrackLogRow(View row, Label emojiLabel, Label descLabel)
  {
    TrackedLogRow info;
    info.row        = row;
    info.emojiLabel = emojiLabel;
    info.descLabel  = descLabel;
    mTrackedLogRows.push_back(info);
  }

  //////////////////////////////////////////////////////////////////////////////
  // In-Place Scale Update
  //////////////////////////////////////////////////////////////////////////////

  void UpdateScaleInPlace()
  {
    // Save current state
    const std::string workbenchText = mWorkBench.GetText().CStr();

    // Update configuration
    SetConfig();

    // Update window size
    window.SetSize(Dali::Window::WindowSize(GetWindowWidth(), GetWindowHeight()));

    // Update main container
    if(mTrackedMainContainer.container)
    {
      int newWidth  = (mWidth + mCell) * mTrackedMainContainer.widthMultiplier;
      int newHeight = (mHeight + mCell) * mTrackedMainContainer.heightMultiplier;
      mTrackedMainContainer.container.SetRequestedWidth(newWidth);
      mTrackedMainContainer.container.SetRequestedHeight(newHeight);
    }

    // Update log main container
    if(mTrackedLogMainContainer.container)
    {
      int newHeight = (mHeight + mCell) * mTrackedLogMainContainer.heightMultiplier;
      mTrackedLogMainContainer.container.SetRequestedHeight(newHeight);
    }

    // Update log view
    if(mTrackedLogView.container)
    {
      mTrackedLogView.container.SetRequestedHeight(mLogHeight);
    }

    // Update all emoji labels
    for(auto& info : mTrackedEmojiLabels)
    {
      if(info.label)
      {
        if(info.updateWidth)
        {
          info.label.SetRequestedWidth(mWidth);
        }
        if(info.updateHeight)
        {
          info.label.SetRequestedHeight(mHeight);
        }
        if(info.updateFontSize)
        {
          info.label.SetFontSize(mPixelSize * info.fontSizeScale);
        }
      }
    }

    // Update all overlay labels
    for(auto& info : mTrackedOverlayLabels)
    {
      if(info.label)
      {
        if(info.updateFontSize)
        {
          info.label.SetFontSize(mPixelSize / 3);
        }
      }
    }

    // Update workbench
    if(mTrackedWorkbench.label)
    {
      mTrackedWorkbench.label.SetRequestedWidth(GetBenchLength());
      mTrackedWorkbench.label.SetRequestedHeight(mHeight);
      mTrackedWorkbench.label.SetFontSize(mPixelSize);
      // Restore text
      mTrackedWorkbench.label.SetText(Dali::String(workbenchText.c_str()));
    }

    // Update title label
    if(mTrackedTitleLabel.label)
    {
      if(mTrackedTitleLabel.updateHeight)
      {
        mTrackedTitleLabel.label.SetRequestedHeight(mHeight);
      }
      if(mTrackedTitleLabel.updateFontSize)
      {
        mTrackedTitleLabel.label.SetFontSize(mPixelSize / 3);
      }
    }

    // Update all log rows
    for(auto& info : mTrackedLogRows)
    {
      if(info.emojiLabel)
      {
        info.emojiLabel.SetRequestedWidth(mWidth / 2);
        info.emojiLabel.SetRequestedHeight(mHeight / 2);
        info.emojiLabel.SetFontSize(mPixelSize / 2);
      }
      if(info.descLabel)
      {
        info.descLabel.SetRequestedHeight(mHeight / 2);
        info.descLabel.SetFontSize(mPixelSize / 3 - 1);
      }
    }

    DALI_LOG_ERROR("[%s] Scale: %f\n", TAG, mScale);
  }

  //////////////////////////////////////////////////////////////////////////////
  // UI Generation
  //////////////////////////////////////////////////////////////////////////////

  View CreateContainerView(int width, int height, bool horizontal)
  {
    return StackLayout::New(horizontal ? StackOrientation::HORIZONTAL : StackOrientation::VERTICAL)
      .Spacing(1)
      .SetRequestedWidth(width)
      .SetRequestedHeight(height)
      .SetBackgroundColor(UiColor(COLOR_BLACK));
  }

  View AddRow(View parent)
  {
    View row = CreateContainerView(MATCH_PARENT, WRAP_CONTENT, true);
    parent.Add(row);
    return row;
  }

  Label CreateBaseLabel()
  {
    return Label::New()
      .SetFontFamily(Dali::String(FONT_FAMILY))
      .SetOverflowMode(Text::OverflowMode::CLIP)
      .SetBackgroundColor(UiColor(COLOR_WHITE))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetAsyncRendering(mAsyncRendering);
  }

  Label CreateEmojiLabel(const std::string& text, float pixelSize, bool tapEnabled)
  {
    Label label = CreateBaseLabel()
      .SetText(Dali::String(text.c_str()))
      .SetFontSize(pixelSize)
      .SetRequestedWidth(mWidth)
      .SetRequestedHeight(mHeight);

    if(tapEnabled)
    {
      label.TouchedSignal().Connect(this, &EmojiController::OnEmojiTouched);
    }

    return label;
  }

  Label CreateOverlayLabel(const char* text)
  {
    return Label::New()
      .SetText(Dali::String(text ? text : ""))
      .SetFontSize(mPixelSize / 3)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetLayoutMode(LayoutMode::STANDALONE)
      .SetOverflowMode(Text::OverflowMode::CLIP)
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetAsyncRendering(mAsyncRendering);
  }

  void GenerateUI()
  {
    window = mApplication.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_WHITE));
    window.SetSize(Dali::Window::WindowSize(GetWindowWidth(), GetWindowHeight()));

    // Create main view structure:
    // VIEW (horizontal)
    //   - MAIN_VIEW (vertical)
    //     - MAIN_TOP_VIEW (horizontal) - work bench + buttons
    //     - EMOJI_VIEW_1..N (horizontal) - emoji rows
    //   - LOG_MAIN_VIEW (vertical)
    //     - LOG_TOP_VIEW (horizontal) - title + buttons
    //     - LOG_VIEW (vertical) - history

    mView = CreateContainerView(MATCH_PARENT, MATCH_PARENT, true);
    window.Add(mView);

    // Main view (left side - emoji area)
    mMainView = CreateContainerView((mWidth + mCell) * DEFAULT_ROW, (mHeight + mCell) * DEFAULT_COL, false);
    mView.Add(mMainView);
    TrackMainContainer(mMainView);

    // Log main view (right side)
    View logMainView = CreateContainerView(WRAP_CONTENT, (mHeight + mCell) * DEFAULT_COL, false);
    logMainView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mView.Add(logMainView);
    TrackLogMainContainer(logMainView);

    // Log top view with title and buttons
    View logTopView = CreateContainerView(MATCH_PARENT, WRAP_CONTENT, true);
    logMainView.Add(logTopView);

    Label titleLabel = CreateEmojiLabel("History", mPixelSize / 3, false);
    titleLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
      .SetRequestedHeight(mHeight)
      .SetBackgroundColor(UiColor(COLOR_LIGHT_GRAY))
      .SetHorizontalTextAlignment(Text::Alignment::CENTER)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
    logTopView.Add(titleLabel);
    TrackTitleLabel(titleLabel);

    // Clear Log button
    Label clearLogBtn = AddEmojiButton(logTopView, "", "Clear", "", false);
    clearLogBtn.TouchedSignal().Connect(this, &EmojiController::OnClearLogTouched);

    // Scale button
    Label scaleBtn = AddEmojiButton(logTopView, "", "Scale", "", false);
    scaleBtn.TouchedSignal().Connect(this, &EmojiController::OnScaleTouched);

    // Help button
    Label helpBtn = AddEmojiButton(logTopView, "", "Help", "", false);
    helpBtn.TouchedSignal().Connect(this, &EmojiController::OnHelpTouched);

    // Log view
    mLogView = CreateContainerView(MATCH_PARENT, WRAP_CONTENT, false);
    mLogView.SetRequestedHeight(mLogHeight);
    mLogView.SetBackgroundColor(UiColor(COLOR_LIGHT_GRAY));
    logMainView.Add(mLogView);
    TrackLogView(mLogView);

    // Main top view with work bench
    View mainTopView = AddRow(mMainView);

    mWorkBench = CreateEmojiLabel("", mPixelSize, true);
    mWorkBench.SetRequestedWidth(GetBenchLength())
      .SetRequestedHeight(mHeight)
      .SetBackgroundColor(UiColor(COLOR_LIGHT_GRAY))
      .SetHorizontalTextAlignment(Text::Alignment::START)
      .SetVerticalTextAlignment(Text::Alignment::CENTER);
    mainTopView.Add(mWorkBench);
    TrackWorkbench(mWorkBench);

    // Add special character buttons
    AddEmojiButton(mainTopView, ZWJ, "ZWJ", "Zero Width Joiner");
    AddEmojiButton(mainTopView, VS15, "VS15", "Variation Selector 15");
    AddEmojiButton(mainTopView, VS16, "VS16", "Variation Selector 16");

    // Clear button
    Label clearBtn = AddEmojiButton(mainTopView, "", "Clear", "", false);
    clearBtn.TouchedSignal().Connect(this, &EmojiController::OnClearTouched);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Emoji Data Generation
  //////////////////////////////////////////////////////////////////////////////

  void AddEmojiItem(View parent, const char* unicode, const char* description = "", bool tapEnabled = true)
  {
    RegisterDescription(unicode, description);
    Label label = CreateEmojiLabel(unicode ? unicode : "", mPixelSize, tapEnabled);
    parent.Add(label);
    TrackEmojiLabel(label);
  }

  void AddEmoji(View parent, const char* unicode, const char* description = "")
  {
    AddEmojiItem(parent, unicode, description, true);
  }

  template<size_t N>
  void AddEmoji(View parent, const std::array<const char*, N>& unicodeArray)
  {
    for(const auto* unicode : unicodeArray)
    {
      AddEmojiItem(parent, unicode, "", true);
    }
  }

  template<size_t N>
  void AddEmoji(View parent, const std::array<const char*, N>& unicodeArray, const std::array<const char*, N>& descArray)
  {
    for(size_t i = 0; i < N; ++i)
    {
      AddEmojiItem(parent, unicodeArray[i], descArray[i], true);
    }
  }

  Label AddEmojiButton(View parent, const char* unicode, const char* labelText, const char* description, bool tapEnabled = true)
  {
    RegisterDescription(unicode, description);

    Label emoji = CreateEmojiLabel(unicode ? unicode : "", mPixelSize, tapEnabled);
    Label label = CreateOverlayLabel(labelText);

    emoji.Add(label);
    parent.Add(emoji);

    TrackEmojiLabel(emoji);
    TrackOverlayLabel(label);

    return emoji;
  }

  void GenerateEmoji()
  {
    // * 1 * Face emojis
    View emojiView1 = AddRow(mMainView);
    const std::array<const char*, 18> faceAry{{
      "\U0001F600", "\U0001F970", "\U0001F60B", "\U0001F917",
      "\U0001F910", "\U0001F60C", "\U0001F637", "\U0001F631",
      "\U0001F97A", "\U0001F615", "\U0001F923", "\U0001FAE0",
      "\U0001F607", "\U0001F970", "\U0001F911", "\U0001F914",
      "\U0001F976", "\U0001F92F"
    }};
    AddEmoji(emojiView1, faceAry);

    // * 2 * Face combinations
    std::string faceWithoutMouth = "\U0001F636";
    std::string cloud            = "\U0001F32B";
    std::string faceInCloud      = faceWithoutMouth + ZWJ + cloud + VS16;

    std::string faceSmile    = "\U0001F62E";
    std::string dash         = "\U0001F4A8";
    std::string faceExhaling = faceSmile + ZWJ + dash;

    std::string faceXEye   = "\U0001F635";
    std::string dizzy      = "\U0001F4AB";
    std::string faceSpiral = faceXEye + ZWJ + dizzy;

    View emojiView2 = AddRow(mMainView);

    const std::array<const char*, 9> faceCombAry{{
      faceWithoutMouth.c_str(), cloud.c_str(), faceInCloud.c_str(),
      faceSmile.c_str(), dash.c_str(), faceExhaling.c_str(),
      faceXEye.c_str(), dizzy.c_str(), faceSpiral.c_str()
    }};
    const std::array<const char*, 9> faceCombDescAry{{
      "FACE_WITHOUT_MOUTH", "CLOUD", "FACE_IN_CLOUD = FACE_WITHOUT_MOUTH + ZWJ + CLOUD + VS16",
      "FACE_SMILE", "DASH", "FACE_EXHALING = FACE_SMILE + ZWJ + DASH",
      "FACE_X_EYE", "DIZZY", "FACE_SPIRAL = FACE_X_EYE + ZWJ + DIZZY"
    }};
    AddEmoji(emojiView2, faceCombAry, faceCombDescAry);

    const std::array<const char*, 9> faceMoreAry{{
      "\U0001F920", "\U0001F973", "\U0001F978",
      "\U0001F60E", "\U0001F913", "\U0001F9D0",
      "\U0001F92C", "\U0001F608", "\U0001F480"
    }};
    const std::array<const char*, 9> faceMoreDescAry{{
      "COWBOY", "PARTY", "DISGUISED", "SUNGLASS", "NERD", "MONOCLE",
      "MOUTH", "DEVIL", "SKULL"
    }};
    AddEmoji(emojiView2, faceMoreAry, faceMoreDescAry);

    // * 3 * Emotion emojis
    std::string redHeart          = "\u2764";
    std::string fire              = "\U0001F525";
    std::string heartOnFire       = redHeart + VS16 + ZWJ + fire;

    std::string eye               = "\U0001F441";
    std::string speechBubble      = "\U0001F5E8";
    std::string eyeInSpeechBubble = eye + VS16 + ZWJ + speechBubble + VS16;

    View emojiView3 = AddRow(mMainView);

    const std::array<const char*, 9> emotionAry{{
      "\U0001F48B", "\U0001F4AF", "\U0001F4A2", "\U0001F4A5",
      "\U0001F4A6", "\U0001F573", "\U0001F4AC", "\U0001F4A4", "\U0001F4AD"
    }};
    AddEmoji(emojiView3, emotionAry);

    const std::array<const char*, 9> emotionCombAry{{
      redHeart.c_str(), fire.c_str(), heartOnFire.c_str(),
      eye.c_str(), speechBubble.c_str(), eyeInSpeechBubble.c_str(),
      "\U0001F977", "\U0001F934", "\U0001F47C"
    }};
    const std::array<const char*, 9> emotionCombDescAry{{
      "RED_HEART", "FIRE", "HEART_ON_FIRE = RED_HEART + VS16 + ZWJ + FIRE",
      "EYE", "SPEECH_BUBBLE", "EYE_IN_SPEECH_BUBBLE = EYE + VS16 + ZWJ + SPEECH_BUBBLE + VS16",
      "NINJA", "PRINCE", "BABY"
    }};
    AddEmoji(emojiView3, emotionCombAry, emotionCombDescAry);

    // * 4 * Person emojis
    std::string maleSign        = "\u2642";
    std::string femaleSign      = "\u2640";
    std::string transgenderSign = "\u26A7";

    std::string beard      = "\U0001F9D4";
    std::string beardMan   = beard + ZWJ + maleSign + VS16;
    std::string beardWoman = beard + ZWJ + femaleSign + VS16;

    std::string blond      = "\U0001F471";
    std::string blondMan   = blond + ZWJ + maleSign + VS16;
    std::string blondWoman = blond + ZWJ + femaleSign + VS16;

    std::string deaf      = "\U0001F9CF";
    std::string deafMan   = deaf + ZWJ + maleSign + VS16;
    std::string deafWoman = deaf + ZWJ + femaleSign + VS16;

    std::string olderPerson = "\U0001F9D3";
    std::string olderMan    = "\U0001F474";
    std::string olderWoman  = "\U0001F475";

    std::string person = "\U0001F9D1";
    std::string man    = "\U0001F468";
    std::string woman  = "\U0001F469";

    View emojiView4 = AddRow(mMainView);

    const std::array<const char*, 18> personAry{{
      maleSign.c_str(), femaleSign.c_str(), transgenderSign.c_str(),
      beard.c_str(), beardMan.c_str(), beardWoman.c_str(),
      blond.c_str(), blondMan.c_str(), blondWoman.c_str(),
      deaf.c_str(), deafMan.c_str(), deafWoman.c_str(),
      olderPerson.c_str(), olderMan.c_str(), olderWoman.c_str(),
      person.c_str(), man.c_str(), woman.c_str()
    }};
    const std::array<const char*, 18> personDescAry{{
      "MALE_SIGN", "FEMALE_SIGN", "TRANSGENDER_SIGN",
      "BEARD", "BEARD_MAN = BEARD + ZWJ + MALE_SIGN + VS16", "BEARD_WOMAN = BEARD + ZWJ + FEMALE_SIGN + VS16",
      "BLOND", "BLOND_MAN = BLOND + ZWJ + MALE_SIGN + VS16", "BLOND_WOMAN = BLOND + ZWJ + FEMALE_SIGN + VS16",
      "DEAF", "DEAF_MAN = DEAF + ZWJ + MALE_SIGN + VS16", "DEAF_WOMAN = DEAF + ZWJ + FEMALE_SIGN + VS16",
      "OLDER_PERSON", "OLDER_MAN", "OLDER_WOMAN",
      "PERSON", "MAN", "WOMAN"
    }};
    AddEmoji(emojiView4, personAry, personDescAry);

    // * 5 * Hair variations
    std::string redHair   = "\U0001F9B0";
    std::string curlyHair = "\U0001F9B1";
    std::string whiteHair = "\U0001F9B3";
    std::string baldHair  = "\U0001F9B2";

    std::string personHair1 = person + ZWJ + redHair;
    std::string personHair2 = person + ZWJ + curlyHair;
    std::string personHair3 = person + ZWJ + whiteHair;
    std::string personHair4 = person + ZWJ + baldHair;

    std::string manHair1 = man + ZWJ + redHair;
    std::string manHair2 = man + ZWJ + curlyHair;
    std::string manHair3 = man + ZWJ + whiteHair;
    std::string manHair4 = man + ZWJ + baldHair;

    std::string womanHair1 = woman + ZWJ + redHair;
    std::string womanHair2 = woman + ZWJ + curlyHair;
    std::string womanHair3 = woman + ZWJ + whiteHair;
    std::string womanHair4 = woman + ZWJ + baldHair;

    std::string boy  = "\U0001F466";
    std::string girl = "\U0001F467";

    View emojiView5 = AddRow(mMainView);

    const std::array<const char*, 18> hairAry{{
      redHair.c_str(), curlyHair.c_str(), whiteHair.c_str(), baldHair.c_str(),
      personHair1.c_str(), personHair2.c_str(), personHair3.c_str(), personHair4.c_str(),
      manHair1.c_str(), manHair2.c_str(), manHair3.c_str(), manHair4.c_str(),
      womanHair1.c_str(), womanHair2.c_str(), womanHair3.c_str(), womanHair4.c_str(),
      boy.c_str(), girl.c_str()
    }};
    const std::array<const char*, 18> hairDescAry{{
      "RED_HAIR", "CURLY_HAIR", "WHITE_HAIR", "BALD_HAIR",
      "PERSON_HAIR1 = PERSON + ZWJ + RED_HAIR",
      "PERSON_HAIR2 = PERSON + ZWJ + CURLY_HAIR",
      "PERSON_HAIR3 = PERSON + ZWJ + WHITE_HAIR",
      "PERSON_HAIR4 = PERSON + ZWJ + BALD_HAIR",
      "MAN_HAIR1 = MAN + ZWJ + RED_HAIR",
      "MAN_HAIR2 = MAN + ZWJ + CURLY_HAIR",
      "MAN_HAIR3 = MAN + ZWJ + WHITE_HAIR",
      "MAN_HAIR4 = MAN + ZWJ + BALD_HAIR",
      "WOMAN_HAIR1 = WOMAN + ZWJ + RED_HAIR",
      "WOMAN_HAIR2 = WOMAN + ZWJ + CURLY_HAIR",
      "WOMAN_HAIR3 = WOMAN + ZWJ + WHITE_HAIR",
      "WOMAN_HAIR4 = WOMAN + ZWJ + BALD_HAIR",
      "BOY", "GIRL"
    }};
    AddEmoji(emojiView5, hairAry, hairDescAry);

    // * 6 * Work/Profession emojis
    std::string medicalSymbol = "\u2695";
    std::string balanceScale  = "\u2696";
    std::string airPlane      = "\u2708";
    std::string graduationCap = "\U0001F393";
    std::string school        = "\U0001F3EB";
    std::string rice          = "\U0001F33E";
    std::string cooking       = "\U0001F373";
    std::string wrench        = "\U0001F527";
    std::string microScope    = "\U0001F52C";
    std::string laptop        = "\U0001F4BB";
    std::string rocket        = "\U0001F680";
    std::string palette       = "\U0001F3A8";

    View emojiView6 = AddRow(mMainView);

    const std::array<const char*, 12> workAry{{
      medicalSymbol.c_str(), balanceScale.c_str(), airPlane.c_str(), graduationCap.c_str(),
      school.c_str(), rice.c_str(), cooking.c_str(), wrench.c_str(),
      microScope.c_str(), laptop.c_str(), rocket.c_str(), palette.c_str()
    }};
    const std::array<const char*, 12> workDescAry{{
      "MEDICAL_SYMBOL", "BALANCE_SCALE", "AIR_PLANE", "GRADUATION_CAP",
      "SCHOOL", "RICE", "COOKING", "WRENCH",
      "MICRO_SCOPE", "LAPTOP", "ROCKET", "PALETTE"
    }};
    AddEmoji(emojiView6, workAry, workDescAry);

    std::string detective      = "\U0001F575";
    std::string detectiveMan   = detective + VS16 + ZWJ + maleSign + VS16;
    std::string detectiveWoman = detective + VS16 + ZWJ + femaleSign + VS16;

    std::string guard      = "\U0001F482";
    std::string guardMan   = guard + ZWJ + maleSign + VS16;
    std::string guardWoman = guard + ZWJ + femaleSign + VS16;

    const std::array<const char*, 6> workCombAry{{
      detective.c_str(), detectiveMan.c_str(), detectiveWoman.c_str(),
      guard.c_str(), guardMan.c_str(), guardWoman.c_str()
    }};
    const std::array<const char*, 6> workCombDescAry{{
      "DETECTIVE",
      "DETECTIVE_MAN = DETECTIVE + VS16 + ZWJ + MALE_SIGN + VS16",
      "DETECTIVE_WOMAN = DETECTIVE + VS16 + ZWJ + FEMALE_SIGN + VS16",
      "GUARD",
      "GUARD_MAN = GUARD + ZWJ + MALE_SIGN + VS16",
      "GUARD_WOMAN = GUARD + ZWJ + FEMALE_SIGN + VS16"
    }};
    AddEmoji(emojiView6, workCombAry, workCombDescAry);

    // * 7 * Work combinations 2
    std::string personDoctor = person + ZWJ + medicalSymbol + VS16;
    std::string manDoctor    = man + ZWJ + medicalSymbol + VS16;
    std::string womanDoctor  = woman + ZWJ + medicalSymbol + VS16;

    std::string personJudge = person + ZWJ + balanceScale + VS16;
    std::string manJudge    = man + ZWJ + balanceScale + VS16;
    std::string womanJudge  = woman + ZWJ + balanceScale + VS16;

    std::string personPilot = person + ZWJ + airPlane + VS16;
    std::string manPilot    = man + ZWJ + airPlane + VS16;
    std::string womanPilot  = woman + ZWJ + airPlane + VS16;

    std::string personStudent = person + ZWJ + graduationCap;
    std::string manStudent    = man + ZWJ + graduationCap;
    std::string womanStudent  = woman + ZWJ + graduationCap;

    std::string personTeacher = person + ZWJ + school;
    std::string manTeacher    = man + ZWJ + school;
    std::string womanTeacher  = woman + ZWJ + school;

    std::string personFarmer = person + ZWJ + rice;
    std::string manFarmer    = man + ZWJ + rice;
    std::string womanFarmer  = woman + ZWJ + rice;

    View emojiView7 = AddRow(mMainView);

    const std::array<const char*, 18> workComb2Ary{{
      personDoctor.c_str(), manDoctor.c_str(), womanDoctor.c_str(),
      personJudge.c_str(), manJudge.c_str(), womanJudge.c_str(),
      personPilot.c_str(), manPilot.c_str(), womanPilot.c_str(),
      personStudent.c_str(), manStudent.c_str(), womanStudent.c_str(),
      personTeacher.c_str(), manTeacher.c_str(), womanTeacher.c_str(),
      personFarmer.c_str(), manFarmer.c_str(), womanFarmer.c_str()
    }};
    const std::array<const char*, 18> workComb2DescAry{{
      "PERSON_DOCTOR = PERSON + ZWJ + MEDICAL_SYMBOL + VS16",
      "MAN_DOCTOR = MAN + ZWJ + MEDICAL_SYMBOL + VS16",
      "WOMAN_DOCTOR = WOMAN + ZWJ + MEDICAL_SYMBOL + VS16",
      "PERSON_JUDGE = PERSON + ZWJ + BALANCE_SCALE + VS16",
      "MAN_JUDGE = MAN + ZWJ + BALANCE_SCALE + VS16",
      "WOMAN_JUDGE = WOMAN + ZWJ + BALANCE_SCALE + VS16",
      "PERSON_PILOT = PERSON + ZWJ + AIR_PLANE + VS16",
      "MAN_PILOT = MAN + ZWJ + AIR_PLANE + VS16",
      "WOMAN_PILOT = WOMAN + ZWJ + AIR_PLANE + VS16",
      "PERSON_STUDENT = PERSON + ZWJ + GRADUATION_CAP",
      "MAN_STUDENT = MAN + ZWJ + GRADUATION_CAP",
      "WOMAN_STUDENT = WOMAN + ZWJ + GRADUATION_CAP",
      "PERSON_TEACHER = PERSON + ZWJ + SCHOOL",
      "MAN_TEACHER = MAN + ZWJ + SCHOOL",
      "WOMAN_TEACHER = WOMAN + ZWJ + SCHOOL",
      "PERSON_FARMER = PERSON + ZWJ + RICE",
      "MAN_FARMER = MAN + ZWJ + RICE",
      "WOMAN_FARMER = WOMAN + ZWJ + RICE"
    }};
    AddEmoji(emojiView7, workComb2Ary, workComb2DescAry);

    // * 8 * Work combinations 3
    std::string personCook = person + ZWJ + cooking;
    std::string manCook    = man + ZWJ + cooking;
    std::string womanCook  = woman + ZWJ + cooking;

    std::string personMechanic = person + ZWJ + wrench;
    std::string manMechanic    = man + ZWJ + wrench;
    std::string womanMechanic  = woman + ZWJ + wrench;

    std::string personScientist = person + ZWJ + microScope;
    std::string manScientist    = man + ZWJ + microScope;
    std::string womanScientist  = woman + ZWJ + microScope;

    std::string personTechnologist = person + ZWJ + laptop;
    std::string manTechnologist    = man + ZWJ + laptop;
    std::string womanTechnologist  = woman + ZWJ + laptop;

    std::string personAstronaut = person + ZWJ + rocket;
    std::string manAstronaut    = man + ZWJ + rocket;
    std::string womanAstronaut  = woman + ZWJ + rocket;

    std::string personArtist = person + ZWJ + palette;
    std::string manArtist    = man + ZWJ + palette;
    std::string womanArtist  = woman + ZWJ + palette;

    View emojiView8 = AddRow(mMainView);

    const std::array<const char*, 18> workComb3Ary{{
      personCook.c_str(), manCook.c_str(), womanCook.c_str(),
      personMechanic.c_str(), manMechanic.c_str(), womanMechanic.c_str(),
      personScientist.c_str(), manScientist.c_str(), womanScientist.c_str(),
      personTechnologist.c_str(), manTechnologist.c_str(), womanTechnologist.c_str(),
      personAstronaut.c_str(), manAstronaut.c_str(), womanAstronaut.c_str(),
      personArtist.c_str(), manArtist.c_str(), womanArtist.c_str()
    }};
    const std::array<const char*, 18> workComb3DescAry{{
      "PERSON_COOK = PERSON + ZWJ + COOKING",
      "MAN_COOK = MAN + ZWJ + COOKING",
      "WOMAN_COOK = WOMAN + ZWJ + COOKING",
      "PERSON_MECHANIC = PERSON + ZWJ + WRENCH",
      "MAN_MECHANIC = MAN + ZWJ + WRENCH",
      "WOMAN_MECHANIC = WOMAN + ZWJ + WRENCH",
      "PERSON_SCIENTIST = PERSON + ZWJ + MICRO_SCOPE",
      "MAN_SCIENTIST = MAN + ZWJ + MICRO_SCOPE",
      "WOMAN_SCIENTIST = WOMAN + ZWJ + MICRO_SCOPE",
      "PERSON_TECHNOLOGIST = PERSON + ZWJ + LAPTOP",
      "MAN_TECHNOLOGIST = MAN + ZWJ + LAPTOP",
      "WOMAN_TECHNOLOGIST = WOMAN + ZWJ + LAPTOP",
      "PERSON_ASTRONAUT = PERSON + ZWJ + ROCKET",
      "MAN_ASTRONAUT = MAN + ZWJ + ROCKET",
      "WOMAN_ASTRONAUT = WOMAN + ZWJ + ROCKET",
      "PERSON_ARTIST = PERSON + ZWJ + PALETTE",
      "MAN_ARTIST = MAN + ZWJ + PALETTE",
      "WOMAN_ARTIST = WOMAN + ZWJ + PALETTE"
    }};
    AddEmoji(emojiView8, workComb3Ary, workComb3DescAry);

    // * 9 * Fantasy characters
    std::string christmasTree = "\U0001F384";
    std::string santa         = "\U0001F385";
    std::string santaWoman    = "\U0001F936";
    std::string santaPerson   = person + ZWJ + christmasTree;

    std::string mage      = "\U0001F9D9";
    std::string mageMan   = mage + ZWJ + maleSign + VS16;
    std::string mageWoman = mage + ZWJ + femaleSign + VS16;

    std::string vampire      = "\U0001F9DB";
    std::string vampireMan   = vampire + ZWJ + maleSign + VS16;
    std::string vampireWoman = vampire + ZWJ + femaleSign + VS16;

    std::string elf      = "\U0001F9DD";
    std::string elfMan   = elf + ZWJ + maleSign + VS16;
    std::string elfWoman = elf + ZWJ + femaleSign + VS16;

    std::string zombie      = "\U0001F9DF";
    std::string zombieMan   = zombie + ZWJ + maleSign + VS16;
    std::string zombieWoman = zombie + ZWJ + femaleSign + VS16;

    std::string merperson      = "\U0001F9DC";
    std::string merpersonWoman = merperson + ZWJ + femaleSign + VS16;

    std::string genie    = "\U0001F9DE";
    std::string genieMan = genie + ZWJ + maleSign + VS16;

    View emojiView9 = AddRow(mMainView);

    const std::array<const char*, 18> fantasyAry{{
      christmasTree.c_str(), santa.c_str(), santaWoman.c_str(), santaPerson.c_str(),
      mage.c_str(), mageMan.c_str(), mageWoman.c_str(),
      vampire.c_str(), vampireMan.c_str(), vampireWoman.c_str(),
      elf.c_str(), elfMan.c_str(), elfWoman.c_str(),
      zombie.c_str(), zombieMan.c_str(), zombieWoman.c_str(),
      merpersonWoman.c_str(), genieMan.c_str()
    }};
    const std::array<const char*, 18> fantasyDescAry{{
      "CHRISTMAS_TREE", "SANTA", "SANTA_WOMAN", "SANTA_PERSON = PERSON + ZWJ + CHRISTMAS_TREE",
      "MAGE",
      "MAGE_MAN = MAGE + ZWJ + MALE_SIGN + VS16",
      "MAGE_WOMAN = MAGE + ZWJ + FEMALE_SIGN + VS16",
      "VAMPIRE",
      "VAMPIRE_MAN = VAMPIRE + ZWJ + MALE_SIGN + VS16",
      "VAMPIRE_WOMAN = VAMPIRE + ZWJ + FEMALE_SIGN + VS16",
      "ELF",
      "ELF_MAN = ELF + ZWJ + MALE_SIGN + VS16",
      "ELF_WOMAN = ELF + ZWJ + FEMALE_SIGN + VS16",
      "ZOMBIE",
      "ZOMBIE_MAN = ZOMBIE + ZWJ + MALE_SIGN + VS16",
      "ZOMBIE_WOMAN = ZOMBIE + ZWJ + FEMALE_SIGN + VS16",
      "MERPERSON_WOMAN = MERPERSON + ZWJ + FEMALE_SIGN + VS16",
      "GENIE_MAN = GENIE + ZWJ + MALE_SIGN + VS16"
    }};
    AddEmoji(emojiView9, fantasyAry, fantasyDescAry);

    // * 10 ~ 11 * Family sequences
    GenerateFamilySequences();

    // * 12 * Activity emojis
    std::string juggling      = "\U0001F939";
    std::string jugglingMan   = juggling + ZWJ + maleSign + VS16;
    std::string jugglingWoman = juggling + ZWJ + femaleSign + VS16;

    std::string surfer      = "\U0001F3C4";
    std::string surferMan   = surfer + ZWJ + maleSign + VS16;
    std::string surferWoman = surfer + ZWJ + femaleSign + VS16;

    std::string whiteCane       = "\U0001F9AF";
    std::string whiteCanePerson = person + ZWJ + whiteCane;
    std::string whiteCaneMan    = man + ZWJ + whiteCane;
    std::string whiteCaneWoman  = woman + ZWJ + whiteCane;

    std::string manualWheelchair       = "\U0001F9BD";
    std::string manualWheelchairPerson = person + ZWJ + manualWheelchair;
    std::string manualWheelchairMan    = man + ZWJ + manualWheelchair;
    std::string manualWheelchairWoman  = woman + ZWJ + manualWheelchair;

    std::string motorizedWheelchair       = "\U0001F9BC";
    std::string motorizedWheelchairPerson = person + ZWJ + motorizedWheelchair;
    std::string motorizedWheelchairMan    = man + ZWJ + motorizedWheelchair;
    std::string motorizedWheelchairWoman  = woman + ZWJ + motorizedWheelchair;

    View emojiView12 = AddRow(mMainView);

    const std::array<const char*, 18> activityAry{{
      juggling.c_str(), jugglingMan.c_str(), jugglingWoman.c_str(),
      surfer.c_str(), surferMan.c_str(), surferWoman.c_str(),
      whiteCane.c_str(), whiteCanePerson.c_str(), whiteCaneMan.c_str(), whiteCaneWoman.c_str(),
      manualWheelchair.c_str(), manualWheelchairPerson.c_str(), manualWheelchairMan.c_str(), manualWheelchairWoman.c_str(),
      motorizedWheelchair.c_str(), motorizedWheelchairPerson.c_str(), motorizedWheelchairMan.c_str(), motorizedWheelchairWoman.c_str()
    }};
    const std::array<const char*, 18> activityDescAry{{
      "JUGGLING",
      "JUGGLING_MAN = JUGGLING + ZWJ + MALE_SIGN + VS16",
      "JUGGLING_WOMAN = JUGGLING + ZWJ + FEMALE_SIGN + VS16",
      "SURFER",
      "SURFER_MAN = SURFER + ZWJ + MALE_SIGN + VS16",
      "SURFER_WOMAN = SURFER + ZWJ + FEMALE_SIGN + VS16",
      "WHITE_CANE",
      "WHITE_CANE_PERSON = PERSON + ZWJ + WHITE_CANE",
      "WHITE_CANE_MAN = MAN + ZWJ + WHITE_CANE",
      "WHITE_CANE_WOMAN = WOMAN + ZWJ + WHITE_CANE",
      "MANUAL_WHEELCHAIR",
      "MANUAL_WHEELCHAIR_PERSON = PERSON + ZWJ + MANUAL_WHEELCHAIR",
      "MANUAL_WHEELCHAIR_MAN = MAN + ZWJ + MANUAL_WHEELCHAIR",
      "MANUAL_WHEELCHAIR_WOMAN = WOMAN + ZWJ + MANUAL_WHEELCHAIR",
      "MOTORIZED_WHEELCHAIR",
      "MOTORIZED_WHEELCHAIR_PERSON = PERSON + ZWJ + MOTORIZED_WHEELCHAIR",
      "MOTORIZED_WHEELCHAIR_MAN = MAN + ZWJ + MOTORIZED_WHEELCHAIR",
      "MOTORIZED_WHEELCHAIR_WOMAN = WOMAN + ZWJ + MOTORIZED_WHEELCHAIR"
    }};
    AddEmoji(emojiView12, activityAry, activityDescAry);

    // * 13 * Animal emojis
    std::string dragon  = "\U0001F409";
    std::string trex    = "\U0001F996";
    std::string mammoth = "\U0001F9A3";
    std::string beaver  = "\U0001F9AB";
    std::string unicorn = "\U0001F984";
    std::string tiger   = "\U0001F405";
    std::string buffalo = "\U0001F403";
    std::string whale   = "\U0001F40B";

    View emojiView13 = AddRow(mMainView);

    const std::array<const char*, 8> animalAry{{
      dragon.c_str(), trex.c_str(), mammoth.c_str(), beaver.c_str(),
      unicorn.c_str(), tiger.c_str(), buffalo.c_str(), whale.c_str()
    }};
    const std::array<const char*, 8> animalDescAry{{
      "DRAGON", "TREX", "MAMMOTH", "BEAVER", "UNICORN", "TIGER", "BUFFALO", "WHALE"
    }};
    AddEmoji(emojiView13, animalAry, animalDescAry);

    std::string guideDog          = "\U0001F9AE";
    std::string dog               = "\U0001F415";
    std::string safetyVest        = "\U0001F9BA";
    std::string serviceDog        = dog + ZWJ + safetyVest;

    std::string cat               = "\U0001F408";
    std::string blackLargeSquare  = "\u2B1B";
    std::string blackCat          = cat + ZWJ + blackLargeSquare;

    std::string bear              = "\U0001F43B";
    std::string snowFlake         = "\u2744";
    std::string polarBear         = bear + ZWJ + snowFlake + VS16;

    const std::array<const char*, 10> animalCombAry{{
      guideDog.c_str(), dog.c_str(), safetyVest.c_str(), serviceDog.c_str(),
      cat.c_str(), blackLargeSquare.c_str(), blackCat.c_str(),
      bear.c_str(), snowFlake.c_str(), polarBear.c_str()
    }};
    const std::array<const char*, 10> animalCombDescAry{{
      "GUIDE_DOG", "DOG", "SAFETY_VEST", "SERVICE_DOG = DOG + ZWJ + SAFETY_VEST",
      "CAT", "BLACK_LARGE_SQUARE", "BLACK_CAT = CAT + ZWJ + BLACK_LARGE_SQUARE",
      "BEAR", "SNOW_FLAKE", "POLAR_BEAR = BEAR + ZWJ + SNOW_FLAKE + VS16"
    }};
    AddEmoji(emojiView13, animalCombAry, animalCombDescAry);

    // * 14 * Keycap emojis
    std::string keycap         = "\u20E3";
    std::string keycapSharp    = "#" + std::string(VS16) + keycap;
    std::string keycapAsterisk = "*" + std::string(VS16) + keycap;
    std::string keycap10       = "\U0001F51F";

    View emojiView14 = AddRow(mMainView);

    AddEmoji(emojiView14, keycap.c_str(), "KEYCAP");
    AddEmoji(emojiView14, keycapSharp.c_str(), "KEYCAP_SHARP = # + VS16 + KEYCAP");
    AddEmoji(emojiView14, keycapAsterisk.c_str(), "KEYCAP_ASTERISK = * + VS16 + KEYCAP");

    for(int key = 0; key < 10; ++key)
    {
      std::string keycapNum = std::to_string(key) + VS16 + keycap;
      std::string desc      = "KEYCAP_" + std::to_string(key) + " = " + std::to_string(key) + " + VS16 + KEYCAP";
      AddEmoji(emojiView14, keycapNum.c_str(), desc.c_str());
    }
    AddEmoji(emojiView14, keycap10.c_str(), "KEYCAP_10");

    std::string abButton       = "\U0001F18E";
    std::string vsButton       = "\U0001F19A";
    std::string upButton       = "\U0001F199";
    std::string reservedButton = "\U0001F22F";

    const std::array<const char*, 4> keycapMoreAry{{
      abButton.c_str(), vsButton.c_str(), upButton.c_str(), reservedButton.c_str()
    }};
    const std::array<const char*, 4> keycapMoreDescAry{{
      "AB_BUTTON_BLOOD_TYPE", "VS_BUTTON", "UP_BUTTON", "JAPANESE_RESERVED_BUTTON"
    }};
    AddEmoji(emojiView14, keycapMoreAry, keycapMoreDescAry);

    // * 15 * Flag emojis
    std::string whiteFlag = "\U0001F3F3";
    std::string rainbow   = "\U0001F308";

    std::string rainbowFlag      = whiteFlag + VS16 + ZWJ + rainbow;
    std::string transgenderFlag  = whiteFlag + VS16 + ZWJ + transgenderSign + VS16;

    std::string blackFlag          = "\U0001F3F4";
    std::string skullAndCrossbones = "\u2620";
    std::string pirateFlag         = blackFlag + ZWJ + skullAndCrossbones + VS16;

    View emojiView15 = AddRow(mMainView);

    const std::array<const char*, 9> flagAry{{
      whiteFlag.c_str(), rainbow.c_str(), rainbowFlag.c_str(),
      whiteFlag.c_str(), transgenderSign.c_str(), transgenderFlag.c_str(),
      blackFlag.c_str(), skullAndCrossbones.c_str(), pirateFlag.c_str()
    }};
    const std::array<const char*, 9> flagDescAry{{
      "WHITE_FLAG", "RAINBOW", "RAINBOW_FLAG = WHITE_FLAG + VS16 + ZWJ + RAINBOW",
      "WHITE_FLAG", "TRANSGENDER_SIGN", "TRANSGENDER_FLAG = WHITE_FLAG + VS16 + ZWJ + TRANSGENDER_SIGN + VS16",
      "BLACK_FLAG", "SKULL_AND_CROSSBONES", "PIRATE_FLAG = BLACK_FLAG + ZWJ + SKULL_AND_CROSSBONES + VS16"
    }};
    AddEmoji(emojiView15, flagAry, flagDescAry);

    // Flag tag letters
    std::string letter_b  = "\U000E0062";
    std::string letter_c  = "\U000E0063";
    std::string letter_e  = "\U000E0065";
    std::string letter_g  = "\U000E0067";
    std::string letter_l  = "\U000E006C";
    std::string letter_n  = "\U000E006E";
    std::string letter_s  = "\U000E0073";
    std::string letter_t  = "\U000E0074";
    std::string letter_w  = "\U000E0077";
    std::string cancelTag = "\U000E007F";

    std::string england  = blackFlag + letter_g + letter_b + letter_e + letter_n + letter_g + cancelTag;
    std::string scotland = blackFlag + letter_g + letter_b + letter_s + letter_c + letter_t + cancelTag;
    std::string wales    = blackFlag + letter_g + letter_b + letter_w + letter_l + letter_s + cancelTag;

    std::string uniB = "\U0001F1E7";
    std::string uniD = "\U0001F1E9";
    std::string uniE = "\U0001F1EA";
    std::string uniG = "\U0001F1EC";
    std::string uniK = "\U0001F1F0";
    std::string uniN = "\U0001F1F3";
    std::string uniR = "\U0001F1F7";
    std::string uniS = "\U0001F1F8";
    std::string uniU = "\U0001F1FA";

    std::string GB = uniG + uniB;
    std::string DE = uniD + uniE;
    std::string EU = uniE + uniU;
    std::string UN = uniU + uniN;
    std::string US = uniU + uniS;
    std::string KR = uniK + uniR;

    const std::array<const char*, 9> flagNationAry{{
      england.c_str(), scotland.c_str(), wales.c_str(),
      GB.c_str(), DE.c_str(), EU.c_str(), UN.c_str(), US.c_str(), KR.c_str()
    }};
    const std::array<const char*, 9> flagNationDescAry{{
      "ENGLAND = BLACK_FLAG + g + b + e + n + g + CANCEL_TAG",
      "SCOTLAND = BLACK_FLAG + g + b + s + c + t + CANCEL_TAG",
      "WALES = BLACK_FLAG + g + b + w + l + s + CANCEL_TAG",
      "GB = G + B", "DE = D + E", "EU = E + U", "UN = U + N", "US = U + S", "KR = K + R"
    }};
    AddEmoji(emojiView15, flagNationAry, flagNationDescAry);

    // * 16 * Flag letters A-R
    View emojiView16 = AddRow(mMainView);
    const std::array<const char*, 18> flagLetterAry{{
      "\U0001F1E6", "\U0001F1E7", "\U0001F1E8", "\U0001F1E9", "\U0001F1EA", "\U0001F1EB",
      "\U0001F1EC", "\U0001F1ED", "\U0001F1EE", "\U0001F1EF", "\U0001F1F0", "\U0001F1F1",
      "\U0001F1F2", "\U0001F1F3", "\U0001F1F4", "\U0001F1F5", "\U0001F1F6", "\U0001F1F7"
    }};
    const std::array<const char*, 18> flagLetterDescAry{{
      "Unicode Character A", "Unicode Character B", "Unicode Character C",
      "Unicode Character D", "Unicode Character E", "Unicode Character F",
      "Unicode Character G", "Unicode Character H", "Unicode Character I",
      "Unicode Character J", "Unicode Character K", "Unicode Character L",
      "Unicode Character M", "Unicode Character N", "Unicode Character O",
      "Unicode Character P", "Unicode Character Q", "Unicode Character R"
    }};
    AddEmoji(emojiView16, flagLetterAry, flagLetterDescAry);

    // * 17 * Flag letters S-Z and tag letters
    View emojiView17 = AddRow(mMainView);
    const std::array<const char*, 8> flagLetter2Ary{{
      "\U0001F1F8", "\U0001F1F9", "\U0001F1FA", "\U0001F1FB",
      "\U0001F1FC", "\U0001F1FD", "\U0001F1FE", "\U0001F1FF"
    }};
    const std::array<const char*, 8> flagLetter2DescAry{{
      "Unicode Character S", "Unicode Character T", "Unicode Character U",
      "Unicode Character V", "Unicode Character W", "Unicode Character X",
      "Unicode Character Y", "Unicode Character Z"
    }};
    AddEmoji(emojiView17, flagLetter2Ary, flagLetter2DescAry);

    AddEmojiButton(emojiView17, letter_b.c_str(), "b", "Latin Small Letter B");
    AddEmojiButton(emojiView17, letter_c.c_str(), "c", "Latin Small Letter C");
    AddEmojiButton(emojiView17, letter_e.c_str(), "e", "Latin Small Letter E");
    AddEmojiButton(emojiView17, letter_g.c_str(), "g", "Latin Small Letter G");
    AddEmojiButton(emojiView17, letter_l.c_str(), "l", "Latin Small Letter L");
    AddEmojiButton(emojiView17, letter_n.c_str(), "n", "Latin Small Letter N");
    AddEmojiButton(emojiView17, letter_s.c_str(), "s", "Latin Small Letter S");
    AddEmojiButton(emojiView17, letter_t.c_str(), "t", "Latin Small Letter T");
    AddEmojiButton(emojiView17, letter_w.c_str(), "w", "Latin Small Letter W");
    AddEmojiButton(emojiView17, cancelTag.c_str(), "Cancel", "CANCEL_TAG");

    // * 18 * Digits and skin tones
    View emojiView18 = AddRow(mMainView);

    const std::array<const char*, 12> flagDigitAry{{
      "#", "*", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
    }};
    const std::array<const char*, 12> flagDigitDescAry{{
      "HASH", "ASTERISK", "DIGIT 0", "DIGIT 1", "DIGIT 2", "DIGIT 3",
      "DIGIT 4", "DIGIT 5", "DIGIT 6", "DIGIT 7", "DIGIT 8", "DIGIT 9"
    }};
    AddEmoji(emojiView18, flagDigitAry, flagDigitDescAry);

    std::string lightSkinTone       = "\U0001F3FB";
    std::string mediumLightSkinTone = "\U0001F3FC";
    std::string mediumSkinTone      = "\U0001F3FD";
    std::string mediumDarkSkinTone  = "\U0001F3FE";
    std::string darkSkinTone        = "\U0001F3FF";

    const std::array<const char*, 5> skinToneAry{{
      lightSkinTone.c_str(), mediumLightSkinTone.c_str(), mediumSkinTone.c_str(),
      mediumDarkSkinTone.c_str(), darkSkinTone.c_str()
    }};
    const std::array<const char*, 5> skinToneDescAry{{
      "LIGHT_SKIN_TONE", "MEDIUM_LIGHT_SKIN_TONE", "MEDIUM_SKIN_TONE",
      "MEDIUM_DARK_SKIN_TONE", "DARK_SKIN_TONE"
    }};
    AddEmoji(emojiView18, skinToneAry, skinToneDescAry);

    AddEmojiButton(emojiView18, ZWNJ, "ZWNJ", "Zero Width Non Joiner");
  }

  void GenerateFamilySequences()
  {
    std::string person = "\U0001F9D1";
    std::string man    = "\U0001F468";
    std::string woman  = "\U0001F469";
    std::string boy    = "\U0001F466";
    std::string girl   = "\U0001F467";

    const std::array<const char*, 3> parent{{man.c_str(), woman.c_str(), ""}};
    const std::array<const char*, 3> child{{boy.c_str(), girl.c_str(), ""}};
    const std::array<const char*, 3> parentDesc{{"MAN", "WOMAN", "EMPTY"}};
    const std::array<const char*, 3> childDesc{{"BOY", "GIRL", "EMPTY"}};

    int  wrapCount = 0;
    View emojiView = CreateContainerView(MATCH_PARENT, WRAP_CONTENT, true);

    for(int p1 = 0; p1 < 3; ++p1)
    {
      for(int p2 = 0; p2 < 3; ++p2)
      {
        for(int c1 = 0; c1 < 3; ++c1)
        {
          for(int c2 = 0; c2 < 3; ++c2)
          {
            if(p1 == 1 && p2 == 0)
            {
              continue; // woman + man
            }
            if(p1 == 2)
            {
              continue; // empty + man, woman already performed
            }
            if(c1 == 0 && c2 == 1)
            {
              continue; // boy + girl
            }
            if(c1 == 2)
            {
              continue; // empty + girl, boy already performed
            }

            if((wrapCount % DEFAULT_ROW) == 0)
            {
              emojiView = AddRow(mMainView);
            }
            ++wrapCount;

            std::string unicode = std::string(parent[p1]) + ZWJ + parent[p2] + ZWJ + child[c1] + ZWJ + child[c2];
            std::string desc    = "FAMILY = " + std::string(parentDesc[p1]) + " + ZWJ + " +
                               parentDesc[p2] + " + ZWJ + " + childDesc[c1] + " + ZWJ + " + childDesc[c2];

            AddEmoji(emojiView, unicode.c_str(), desc.c_str());
          }
        }
      }
    }

    std::string family               = "\U0001F46A";
    std::string couple               = "\U0001F491";
    std::string redHeart             = "\u2764";
    std::string coupleWomanMan       = woman + ZWJ + redHeart + ZWJ + man;
    std::string coupleManMan         = man + ZWJ + redHeart + ZWJ + man;
    std::string coupleWomanWoman     = woman + ZWJ + redHeart + ZWJ + woman;

    std::string kissMark             = "\U0001F48B";
    std::string kiss                 = "\U0001F48F";
    std::string kissWomanMan         = woman + ZWJ + redHeart + VS16 + ZWJ + kissMark + ZWJ + man;
    std::string kissManMan           = man + ZWJ + redHeart + VS16 + ZWJ + kissMark + ZWJ + man;
    std::string kissWomanWoman       = woman + ZWJ + redHeart + VS16 + ZWJ + kissMark + ZWJ + woman;

    std::string handshake            = "\U0001F91D";
    std::string peopleHoldingHands   = person + ZWJ + handshake + ZWJ + person;
    std::string womanManHoldingHands = "\U0001F46B";

    const std::array<const char*, 11> familyAry{{
      family.c_str(),
      couple.c_str(), coupleWomanMan.c_str(), coupleManMan.c_str(), coupleWomanWoman.c_str(),
      kiss.c_str(), kissWomanMan.c_str(), kissManMan.c_str(), kissWomanWoman.c_str(),
      peopleHoldingHands.c_str(), womanManHoldingHands.c_str()
    }};
    const std::array<const char*, 11> familyDescAry{{
      "FAMILY",
      "COUPLE",
      "COUPLE_WOMAN_MAN = WOMAN + ZWJ + RED_HEART + ZWJ + MAN",
      "COUPLE_MAN_MAN = MAN + ZWJ + RED_HEART + ZWJ + MAN",
      "COUPLE_WOMAN_WOMAN = WOMAN + ZWJ + RED_HEART + ZWJ + WOMAN",
      "KISS",
      "KISS_WOMAN_MAN = WOMAN + ZWJ + RED_HEART + VS16 + ZWJ + KISS_MARK + ZWJ + MAN",
      "KISS_MAN_MAN = MAN + ZWJ + RED_HEART + VS16 + ZWJ + KISS_MARK + ZWJ + MAN",
      "KISS_WOMAN_WOMAN = WOMAN + ZWJ + RED_HEART + VS16 + ZWJ + KISS_MARK + ZWJ + WOMAN",
      "PEOPLE_HOLDING_HANDS = PERSON + ZWJ + HANDSHAKE + ZWJ + PERSON",
      "WOMAN_MAN_HOLDING_HANDS"
    }};
    AddEmoji(emojiView, familyAry, familyDescAry);
  }

  //////////////////////////////////////////////////////////////////////////////
  // Event Handlers
  //////////////////////////////////////////////////////////////////////////////

  bool OnEmojiTouched(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }

    Label label = Label::DownCast(actor);
    if(!label)
    {
      return false;
    }

    const std::string emoji = label.GetText().CStr();
    std::string       description = "[" + emoji + "] : No description";

    const auto it = mEmojiDictionary.find(emoji);
    if(it != mEmojiDictionary.end())
    {
      description = "[" + emoji + "] : " + it->second;
    }

    DALI_LOG_ERROR("[%s] %s\n", TAG, description.c_str());

    AddLog(emoji);

    std::string currentText = mWorkBench.GetText().CStr();
    currentText += emoji;

    const float textWidth = mWorkBench.GetNaturalSize().x;
    if(textWidth > GetBenchLength())
    {
      mWorkBench.SetText(Dali::String(emoji.c_str()));
    }
    else
    {
      mWorkBench.SetText(Dali::String(currentText.c_str()));
    }

    return true;
  }

  bool OnClearTouched(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }

    DALI_LOG_ERROR("[%s] Clear\n", TAG);
    mWorkBench.SetText(Dali::String(""));
    return true;
  }

  bool OnClearLogTouched(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }

    DALI_LOG_ERROR("[%s] Clear Log\n", TAG);
    ClearLog(mLogView.GetChildCount() / 2);
    return true;
  }

  bool OnScaleTouched(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }

    IncreaseScale();
    UpdateScaleInPlace();
    return true;
  }

  bool OnHelpTouched(Actor actor, const TouchEvent& touch)
  {
    if(touch.GetState(0) != PointState::STARTED)
    {
      return false;
    }

    PrintShortcut();
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    const Dali::String& keyName = event.GetKeyName();

    if(keyName == "F1")
    {
      PrintShortcut();
    }
    else if(keyName == "F2")
    {
      SetAsyncRenderingMode(false);
    }
    else if(keyName == "F3")
    {
      SetAsyncRenderingMode(true);
    }
    else if(keyName == "F5")
    {
      ClearAll();
      ScaleUpdate();
      DALI_LOG_ERROR("[%s] Refresh\n", TAG);
    }
    else if(keyName == "Escape")
    {
      if(mScaleTestRunning)
      {
        mScaleTestTimer.Stop();
        mScaleTestRunning = false;
        DALI_LOG_ERROR("[%s] Scale Test Stop\n", TAG);
      }
      else
      {
        mScaleTestTimer.Start();
        mScaleTestRunning = true;
        DALI_LOG_ERROR("[%s] Scale Test Start\n", TAG);
      }
    }
    else if(keyName == "1")
    {
      mScale = 1.0f;
      UpdateScaleInPlace();
      DALI_LOG_ERROR("[%s] Scale set: 1.0\n", TAG);
    }
    else if(keyName == "2")
    {
      mScale = 2.0f;
      UpdateScaleInPlace();
      DALI_LOG_ERROR("[%s] Scale set: 2.0\n", TAG);
    }
    else if(keyName == "3")
    {
      DecreaseScale();
      UpdateScaleInPlace();
      DALI_LOG_ERROR("[%s] Scale down: %f\n", TAG, mScale);
    }
    else if(keyName == "4")
    {
      IncreaseScale();
      UpdateScaleInPlace();
      DALI_LOG_ERROR("[%s] Scale up: %f\n", TAG, mScale);
    }
    else if(keyName == "9")
    {
      mScaleTestInterval -= 100;
      if(mScaleTestInterval < 100)
      {
        mScaleTestInterval = 100;
      }
      mScaleTestTimer.SetInterval(mScaleTestInterval);
      DALI_LOG_ERROR("[%s] Scale Test Interval Down: %d\n", TAG, mScaleTestInterval);
    }
    else if(keyName == "0")
    {
      mScaleTestInterval += 100;
      mScaleTestTimer.SetInterval(mScaleTestInterval);
      DALI_LOG_ERROR("[%s] Scale Test Interval Up: %d\n", TAG, mScaleTestInterval);
    }
    else if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
  }

  bool OnScaleTestTick()
  {
    IncreaseScale();
    UpdateScaleInPlace();
    return true;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Utility Functions
  //////////////////////////////////////////////////////////////////////////////

  void AddLog(const std::string& unicode)
  {
    const int calcedHeight = static_cast<int>(mLogView.GetChildCount() + 1) * (mHeight / 2 + 1);
    if(calcedHeight > mLogHeight)
    {
      ClearLog(mLogView.GetChildCount() / 2);
    }

    View row = CreateContainerView(MATCH_PARENT, WRAP_CONTENT, true);
    mLogView.Add(row);

    Label emoji = Label::New()
      .SetText(Dali::String(unicode.c_str()))
      .SetFontFamily(Dali::String(FONT_FAMILY))
      .SetRequestedWidth(mWidth / 2)
      .SetRequestedHeight(mHeight / 2)
      .SetFontSize(mPixelSize / 2)
      .SetOverflowMode(Text::OverflowMode::CLIP)
      .SetBackgroundColor(UiColor(COLOR_WHITE))
      .SetAsyncRendering(mAsyncRendering);
    row.Add(emoji);

    mHistory.push_back(unicode);

    std::string desc = " No description";
    const auto  it   = mEmojiDictionary.find(unicode);
    if(it != mEmojiDictionary.end())
    {
      desc = " " + it->second;
    }

    Label description = Label::New()
      .SetText(Dali::String(desc.c_str()))
      .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL))
      .SetRequestedHeight(mHeight / 2)
      .SetFontSize(mPixelSize / 3 - 1)
      .SetOverflowMode(Text::OverflowMode::ELLIPSIS)
      .SetHorizontalTextAlignment(Text::Alignment::START)
      .SetVerticalTextAlignment(Text::Alignment::CENTER)
      .SetBackgroundColor(UiColor(COLOR_WHITE))
      .SetAsyncRendering(mAsyncRendering);
    row.Add(description);

    // Track log row for in-place scale updates
    TrackLogRow(row, emoji, description);
  }

  void ClearLog(int count)
  {
    for(int i = 0; i < count && mLogView.GetChildCount() > 0; ++i)
    {
      Actor child = mLogView.GetChildAt(0);
      mLogView.Remove(child);

      if(!mHistory.empty())
      {
        mHistory.erase(mHistory.begin());
      }
    }
  }

  void ClearAll()
  {
    const int childCount = mView.GetChildCount();
    for(int i = 0; i < childCount; ++i)
    {
      mView.Remove(mView.GetChildAt(0));
    }
    window.Remove(mView);
  }

  void ScaleUpdate()
  {
    const std::string              text    = mWorkBench.GetText().CStr();
    const std::vector<std::string> history = mHistory;
    mHistory.clear();

    SetConfig();
    GenerateUI();
    GenerateEmoji();

    mWorkBench.SetText(Dali::String(text.c_str()));
    for(const auto& item : history)
    {
      AddLog(item);
    }

    DALI_LOG_ERROR("[%s] Scale: %f\n", TAG, mScale);
  }

  void PrintShortcut()
  {
    DALI_LOG_ERROR("[%s] EMOJI TEST HELP                  \n", TAG);
    DALI_LOG_ERROR("[%s] F1    : Help                     \n", TAG);
    DALI_LOG_ERROR("[%s] F2    : Sync  rendering          \n", TAG);
    DALI_LOG_ERROR("[%s] F3    : Async rendering          \n", TAG);
    DALI_LOG_ERROR("[%s] F5    : Refresh                  \n", TAG);
    DALI_LOG_ERROR("[%s] ESC   : Scale Test Start/Stop    \n", TAG);
    DALI_LOG_ERROR("[%s] Num 1 : Scale 1.0                \n", TAG);
    DALI_LOG_ERROR("[%s] Num 2 : Scale 2.0                \n", TAG);
    DALI_LOG_ERROR("[%s] Num 3 : Scale Down               \n", TAG);
    DALI_LOG_ERROR("[%s] Num 4 : Scale Up                 \n", TAG);
    DALI_LOG_ERROR("[%s] Num 9 : Scale Test Interval Down \n", TAG);
    DALI_LOG_ERROR("[%s] Num 0 : Scale Test Interval Up   \n", TAG);
  }

private:
  Application& mApplication;
  Window       window;

  View  mView;
  View  mMainView;
  View  mLogView;
  Label mWorkBench;

  std::map<std::string, std::string> mEmojiDictionary;
  std::vector<std::string>           mHistory;

  float    mScale;
  bool     mAsyncRendering;
  float    mPixelSize;
  int      mCell;
  int      mWidth;
  int      mHeight;
  int      mLogWidth;
  int      mLogHeight;

  Timer    mScaleTestTimer;
  bool     mScaleTestRunning;
  uint32_t mScaleTestInterval;

  //////////////////////////////////////////////////////////////////////////////
  // UI Tracking for In-Place Scale Updates
  //////////////////////////////////////////////////////////////////////////////

  std::vector<TrackedLabel>     mTrackedEmojiLabels;
  std::vector<TrackedLabel>     mTrackedOverlayLabels;
  std::vector<TrackedLogRow>    mTrackedLogRows;
  TrackedLabel                  mTrackedWorkbench;
  TrackedLabel                  mTrackedTitleLabel;
  TrackedContainer              mTrackedMainContainer;
  TrackedContainer              mTrackedLogMainContainer;
  TrackedContainer              mTrackedLogView;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize START\n");
  DaliUiFoundationPreInitialize(nullptr, nullptr, nullptr);
  DALI_LOG_ERROR("DaliUiFoundationPreInitialize END\n");

  Application application = Application::New(&argc, &argv);
  UiConfig::New().SetLabelAsyncRendering(true).Apply();

  EmojiController controller(application);
  application.MainLoop();

  return 0;
}
