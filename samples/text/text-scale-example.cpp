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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING       = 8.0f;
constexpr float STACK_PADDING       = 16.0f;
constexpr float BUTTON_HEIGHT       = 44.0f;
constexpr float TARGET_LABEL_HEIGHT = 80.0f;
constexpr float TARGET_INPUT_HEIGHT = 80.0f;

const char* TEST_TEXT = "The quick brown fox jumps over the lazy dog. 1234567890";

const Dali::Vector<Text::FitCandidate>& GetFitCandidates()
{
  static Dali::Vector<Text::FitCandidate> candidates = [] {
    Dali::Vector<Text::FitCandidate> values;
    values.PushBack(Text::FitCandidate(10.0f, 20.0f));
    values.PushBack(Text::FitCandidate(12.0f, 24.0f));
    values.PushBack(Text::FitCandidate(14.0f, 28.0f));
    values.PushBack(Text::FitCandidate(16.0f, 32.0f));
    values.PushBack(Text::FitCandidate(18.0f, 36.0f));
    values.PushBack(Text::FitCandidate(20.0f, 40.0f));
    return values;
  }();
  return candidates;
}

Label CreateButton(const char* text, float fontSize = 14.0f)
{
  return Label::New(text)
    .SetFontSize(fontSize)
    .SetHorizontalTextAlignment(Text::Alignment::CENTER)
    .SetVerticalTextAlignment(Text::Alignment::CENTER)
    .SetBackgroundColor(UiColor(0x4A90D9))
    .SetRequestedWidth(MATCH_PARENT)
    .SetRequestedHeight(BUTTON_HEIGHT)
    .SetPadding(Extents(10, 10, 10, 10));
}
} // namespace

class TextScaleController : public ConnectionTracker
{
public:
  explicit TextScaleController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextScaleController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));
    window.KeyEventSignal().Connect(this, &TextScaleController::OnKeyEvent);

    // Target Label for visual testing
    mTargetLabel = Label::New(TEST_TEXT)
                     .SetRequestedWidth(MATCH_PARENT)
                     .SetRequestedHeight(TARGET_LABEL_HEIGHT)
                     .SetMultiLine(true)
                     .SetFontSize(24.0f)
                     .SetBackgroundColor(UiColor(0xFFFFFF))
                     .SetPadding(Extents(16, 16, 16, 16));

    // Target InputField for visual testing
    mTargetInputField = InputField::New()
                          .SetRequestedWidth(MATCH_PARENT)
                          .SetRequestedHeight(TARGET_INPUT_HEIGHT)
                          .SetFontSize(24.0f)
                          .SetText("InputField test text")
                          .SetBackgroundColor(UiColor(0xFFFFFF))
                          .SetPadding(Extents(16, 16, 16, 16));

    mFitLabel = Label::New(TEST_TEXT)
                     .SetRequestedWidth(MATCH_PARENT)
                     .SetRequestedHeight(WRAP_CONTENT)
                     .SetMaximumHeight(120)
                     .SetMultiLine(true)
                     .SetBackgroundColor(UiColor(0xFFFFFF))
                     .SetTextFit(Text::FitRange(10, 20, 2))
                     .SetLineHeight(40.0f)
                     .SetLineHeightMode(Text::LineHeightMode::ABSOLUTE)
                     .SetPadding(Extents(16, 16, 0, 0));

   mFitCandidateLabel = Label::New(TEST_TEXT)
                     .SetRequestedWidth(MATCH_PARENT)
                     .SetRequestedHeight(WRAP_CONTENT)
                     .SetMaximumHeight(120)
                     .SetMultiLine(true)
                     .SetBackgroundColor(UiColor(0xFFFFFF))
                     .SetTextFit(GetFitCandidates())
                     .SetPadding(Extents(16, 16, 0, 0));

   mStatusLabel = Label::New()
                     .SetRequestedWidth(MATCH_PARENT)
                     .SetRequestedHeight(WRAP_CONTENT)
                     .SetFontSize(12.0f)
                     .SetMultiLine(true)
                     .SetBackgroundColor(UiColor(0xE8E8E8))
                     .SetPadding(Extents(16, 16, 16, 16));

    UpdateStatus();

    Label titleButton = CreateButton("Font Size Scale Test", 16.0f)
                          .SetBackgroundColor(UiColor(0x2C3E50));

    Label btn1 = CreateButton("1. Normal Scale (1.0, min=0.5, max=2.0)")
                   .SetBackgroundColor(UiColor(0x3498DB));

    Label btn2 = CreateButton("2. Increase Scale (1.5)")
                   .SetBackgroundColor(UiColor(0x2ECC71));

    Label btn3 = CreateButton("3. Clamp to Min (scale=0.8, min=1.2)")
                   .SetBackgroundColor(UiColor(0xE74C3C));

    Label btn4 = CreateButton("4. Clamp to Max (scale=1.8, max=1.3)")
                   .SetBackgroundColor(UiColor(0xE67E22));

    Label btn5 = CreateButton("5. Inverted Range (min=1.4 > max=1.0)")
                   .SetBackgroundColor(UiColor(0x9B59B6));

    Label btn6 = CreateButton("6. Enable System Scale")
                   .SetBackgroundColor(UiColor(0x1ABC9C));

    Label btn7 = CreateButton("7. Disable System Scale (scale=1.6)")
                   .SetBackgroundColor(UiColor(0x7F8C8D));

    window.Add(
      StackLayout::New(StackOrientation::VERTICAL)
        .SetSpacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .SetRequestedHeight(MATCH_PARENT)
        .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
        .Children({
          titleButton,
          Label::New("Target Label:").SetFontSize(14.0f),
          mTargetLabel,
          Label::New("Target InputField:").SetFontSize(14.0f),
          mTargetInputField,
          Label::New("Fit & FitCandidate:").SetFontSize(14.0f),
          mFitLabel,
          mFitCandidateLabel,
          Label::New("Current Status:").SetFontSize(14.0f),
          mStatusLabel,
          Label::New("Test Scenarios:").SetFontSize(14.0f),
          btn1, btn2, btn3, btn4, btn5, btn6, btn7,
        }));

    btn1.TouchedSignal().Connect(this, &TextScaleController::OnButton1Touched);
    btn2.TouchedSignal().Connect(this, &TextScaleController::OnButton2Touched);
    btn3.TouchedSignal().Connect(this, &TextScaleController::OnButton3Touched);
    btn4.TouchedSignal().Connect(this, &TextScaleController::OnButton4Touched);
    btn5.TouchedSignal().Connect(this, &TextScaleController::OnButton5Touched);
    btn6.TouchedSignal().Connect(this, &TextScaleController::OnButton6Touched);
    btn7.TouchedSignal().Connect(this, &TextScaleController::OnButton7Touched);
  }

  void UpdateStatus()
  {
    // Get Label values
    float labelFontSizeScale    = mTargetLabel.GetFontSizeScale();
    float labelMinFontSizeScale = mTargetLabel.GetMinimumFontSizeScale();
    float labelMaxFontSizeScale = mTargetLabel.GetMaximumFontSizeScale();
    bool  labelSystemEnabled    = mTargetLabel.IsSystemFontSizeScaleEnabled();
    float labelAdjustedScale    = mTargetLabel.GetAdjustedFontSizeScale();

    // Get InputField values
    float inputFontSizeScale    = mTargetInputField.GetFontSizeScale();
    float inputMinFontSizeScale = mTargetInputField.GetMinimumFontSizeScale();
    float inputMaxFontSizeScale = mTargetInputField.GetMaximumFontSizeScale();
    bool  inputSystemEnabled    = mTargetInputField.IsSystemFontSizeScaleEnabled();
    float inputAdjustedScale    = mTargetInputField.GetAdjustedFontSizeScale();

    Dali::String status;

    status += "[Label]\n";
    status += "Scale: ";
    status += std::to_string(labelFontSizeScale).substr(0, 5).c_str();
    status += ", Min: ";
    status += std::to_string(labelMinFontSizeScale).substr(0, 5).c_str();
    status += ", Max: ";
    status += std::to_string(labelMaxFontSizeScale).substr(0, 5).c_str();
    status += "\nSystem: ";
    status += (labelSystemEnabled ? "ON" : "OFF");
    status += ", Adjusted: ";
    status += std::to_string(labelAdjustedScale).substr(0, 5).c_str();

    status += "\n\n[InputField]\n";
    status += "Scale: ";
    status += std::to_string(inputFontSizeScale).substr(0, 5).c_str();
    status += ", Min: ";
    status += std::to_string(inputMinFontSizeScale).substr(0, 5).c_str();
    status += ", Max: ";
    status += std::to_string(inputMaxFontSizeScale).substr(0, 5).c_str();
    status += "\nSystem: ";
    status += (inputSystemEnabled ? "ON" : "OFF");
    status += ", Adjusted: ";
    status += std::to_string(inputAdjustedScale).substr(0, 5).c_str();

    mStatusLabel.SetText(status);
  }

  void ApplyScaleToBoth(float scale, float minScale, float maxScale, bool systemEnabled)
  {
    mTargetLabel.SetFontSizeScale(scale)
                .SetMinimumFontSizeScale(minScale)
                .SetMaximumFontSizeScale(maxScale)
                .SetSystemFontSizeScaleEnabled(systemEnabled);

    mTargetInputField.SetFontSizeScale(scale)
                     .SetMinimumFontSizeScale(minScale)
                     .SetMaximumFontSizeScale(maxScale)
                     .SetSystemFontSizeScaleEnabled(systemEnabled);

    mFitLabel.SetFontSizeScale(scale)
                .SetMinimumFontSizeScale(minScale)
                .SetMaximumFontSizeScale(maxScale)
                .SetSystemFontSizeScaleEnabled(systemEnabled);

    mFitCandidateLabel.SetFontSizeScale(scale)
                .SetMinimumFontSizeScale(minScale)
                .SetMaximumFontSizeScale(maxScale)
                .SetSystemFontSizeScaleEnabled(systemEnabled);

    UpdateStatus();
  }

  bool OnButton1Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ApplyScaleToBoth(1.0f, 0.5f, 2.0f, false);
    }
    return true;
  }

  bool OnButton2Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ApplyScaleToBoth(1.5f, 0.5f, 2.0f, false);
    }
    return true;
  }

  bool OnButton3Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ApplyScaleToBoth(0.8f, 1.2f, 2.0f, false);
    }
    return true;
  }

  bool OnButton4Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ApplyScaleToBoth(1.8f, 0.5f, 1.3f, false);
    }
    return true;
  }

  bool OnButton5Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ApplyScaleToBoth(0.7f, 1.4f, 1.0f, false);
    }
    return true;
  }

  bool OnButton6Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ApplyScaleToBoth(1.8f, 0.8f, 1.3f, true);
    }
    return true;
  }

  bool OnButton7Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      ApplyScaleToBoth(1.6f, 0.5f, 2.0f, false);
    }
    return true;
  }

  void OnKeyEvent(const KeyEvent& event)
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

    if(event.GetKeyName() == "1")
    {
      ApplyScaleToBoth(1.0f, 0.5f, 2.0f, false);
    }
    else if(event.GetKeyName() == "2")
    {
      ApplyScaleToBoth(1.5f, 0.5f, 2.0f, false);
    }
    else if(event.GetKeyName() == "3")
    {
      ApplyScaleToBoth(0.8f, 1.2f, 2.0f, false);
    }
    else if(event.GetKeyName() == "4")
    {
      ApplyScaleToBoth(1.8f, 0.5f, 1.3f, false);
    }
    else if(event.GetKeyName() == "5")
    {
      ApplyScaleToBoth(0.7f, 1.4f, 1.0f, false);
    }
    else if(event.GetKeyName() == "6")
    {
      ApplyScaleToBoth(1.8f, 0.8f, 1.3f, true);
    }
    else if(event.GetKeyName() == "7")
    {
      ApplyScaleToBoth(1.6f, 0.5f, 2.0f, false);
    }
    else if(event.GetKeyName() == "q")
    {
      mTargetLabel.SetAsyncRendering(false);
      mFitLabel.SetAsyncRendering(false);
      mFitCandidateLabel.SetAsyncRendering(false);
    }
    else if(event.GetKeyName() == "w")
    {
      mTargetLabel.SetAsyncRendering(true);
      mFitLabel.SetAsyncRendering(true);
      mFitCandidateLabel.SetAsyncRendering(true);
    }
  }

private:
  Application& mApplication;
  Label        mTargetLabel;
  InputField   mTargetInputField;
  Label        mFitLabel;
  Label        mFitCandidateLabel;
  Label        mStatusLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextScaleController controller(application);
  application.MainLoop();

  return 0;
}
