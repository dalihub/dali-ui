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

#include <cstdlib>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING       = 8.0f;
constexpr float STACK_PADDING       = 16.0f;
constexpr float BUTTON_HEIGHT       = 44.0f;
constexpr float TARGET_LABEL_HEIGHT = 80.0f;
constexpr float TARGET_INPUT_HEIGHT = 80.0f;

const char* FONT_SIZE_ENV       = "DALI_SYSTEM_FONT_SIZE";
const char* FONT_SIZE_WATCH_ENV = "DALI_SYSTEM_FONT_SIZE_WATCH";
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
  Label button = Label::New(text);
  button.SetFontSize(fontSize);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetBackgroundColor(UiColor(0x4A90D9));
  button.SetRequestedWidth(MATCH_PARENT);
  button.SetRequestedHeight(BUTTON_HEIGHT);
  button.SetPadding(Extents(10, 10, 10, 10));
  return button;
}

Label CreateHeaderLabel(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(14.0f);
  return label;
}

void SetSystemFontSizeEnv(const char* fontSize)
{
  setenv(FONT_SIZE_ENV, fontSize, 1);
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
    mTargetLabel = Label::New(TEST_TEXT);
    mTargetLabel.SetRequestedWidth(MATCH_PARENT);
    mTargetLabel.SetRequestedHeight(TARGET_LABEL_HEIGHT);
    mTargetLabel.SetMultiLine(true);
    mTargetLabel.SetFontSize(24.0f);
    mTargetLabel.SetBackgroundColor(UiColor(0xFFFFFF));
    mTargetLabel.SetPadding(Extents(16, 16, 16, 16));

    // Target InputField for visual testing
    mTargetInputField = InputField::New();
    mTargetInputField.SetRequestedWidth(MATCH_PARENT);
    mTargetInputField.SetRequestedHeight(TARGET_INPUT_HEIGHT);
    mTargetInputField.SetFontSize(24.0f);
    mTargetInputField.SetText("InputField test text");
    mTargetInputField.SetBackgroundColor(UiColor(0xFFFFFF));
    mTargetInputField.SetPadding(Extents(16, 16, 16, 16));

    mFitLabel = Label::New(TEST_TEXT);
    mFitLabel.SetRequestedWidth(MATCH_PARENT);
    mFitLabel.SetRequestedHeight(WRAP_CONTENT);
    mFitLabel.SetMaximumHeight(120);
    mFitLabel.SetMultiLine(true);
    mFitLabel.SetBackgroundColor(UiColor(0xFFFFFF));
    mFitLabel.SetTextFit(Text::FitRange(10, 20, 2));
    mFitLabel.SetLineHeight(40.0f);
    mFitLabel.SetLineHeightMode(Text::LineHeightMode::ABSOLUTE);
    mFitLabel.SetPadding(Extents(16, 16, 0, 0));

    mFitCandidateLabel = Label::New(TEST_TEXT);
    mFitCandidateLabel.SetRequestedWidth(MATCH_PARENT);
    mFitCandidateLabel.SetRequestedHeight(WRAP_CONTENT);
    mFitCandidateLabel.SetMaximumHeight(120);
    mFitCandidateLabel.SetMultiLine(true);
    mFitCandidateLabel.SetBackgroundColor(UiColor(0xFFFFFF));
    mFitCandidateLabel.SetTextFit(GetFitCandidates());
    mFitCandidateLabel.SetPadding(Extents(16, 16, 0, 0));

    mStatusLabel = Label::New();
    mStatusLabel.SetRequestedWidth(MATCH_PARENT);
    mStatusLabel.SetRequestedHeight(WRAP_CONTENT);
    mStatusLabel.SetFontSize(12.0f);
    mStatusLabel.SetMultiLine(true);
    mStatusLabel.SetBackgroundColor(UiColor(0xE8E8E8));
    mStatusLabel.SetPadding(Extents(16, 16, 16, 16));

    UpdateStatus();

    Label titleButton = CreateButton("System Font Size Scale Test", 16.0f);
    titleButton.SetBackgroundColor(UiColor(0x2C3E50));

    Label btn1 = CreateButton("1. System Font SMALL");
    btn1.SetBackgroundColor(UiColor(0x3498DB));

    Label btn2 = CreateButton("2. System Font NORMAL");
    btn2.SetBackgroundColor(UiColor(0x2ECC71));

    Label btn3 = CreateButton("3. System Font LARGE");
    btn3.SetBackgroundColor(UiColor(0xE74C3C));

    Label btn4 = CreateButton("4. System Font EXTRA_LARGE");
    btn4.SetBackgroundColor(UiColor(0xE67E22));

    Label btn5 = CreateButton("5. System Font GIANT");
    btn5.SetBackgroundColor(UiColor(0x9B59B6));

    Label btn6 = CreateButton("6. Enable System Scale");
    btn6.SetBackgroundColor(UiColor(0x1ABC9C));

    Label btn7 = CreateButton("7. Disable System Scale");
    btn7.SetBackgroundColor(UiColor(0x7F8C8D));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    root.Add(titleButton);
    root.Add(CreateHeaderLabel("Target Label:"));
    root.Add(mTargetLabel);
    root.Add(CreateHeaderLabel("Target InputField:"));
    root.Add(mTargetInputField);
    root.Add(CreateHeaderLabel("Fit & FitCandidate:"));
    root.Add(mFitLabel);
    root.Add(mFitCandidateLabel);
    root.Add(CreateHeaderLabel("Current Status:"));
    root.Add(mStatusLabel);
    root.Add(CreateHeaderLabel("Test Scenarios:"));
    root.Add(btn1);
    root.Add(btn2);
    root.Add(btn3);
    root.Add(btn4);
    root.Add(btn5);
    root.Add(btn6);
    root.Add(btn7);
    window.Add(root);

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
    float labelMinFontSizeScale = mTargetLabel.GetMinimumFontSizeScale();
    float labelMaxFontSizeScale = mTargetLabel.GetMaximumFontSizeScale();
    bool  labelSystemEnabled    = mTargetLabel.IsSystemFontSizeScaleEnabled();
    float labelAdjustedScale    = mTargetLabel.GetAdjustedFontSizeScale();

    // Get InputField values
    float inputMinFontSizeScale = mTargetInputField.GetMinimumFontSizeScale();
    float inputMaxFontSizeScale = mTargetInputField.GetMaximumFontSizeScale();
    bool  inputSystemEnabled    = mTargetInputField.IsSystemFontSizeScaleEnabled();
    float inputAdjustedScale    = mTargetInputField.GetAdjustedFontSizeScale();

    Dali::String status;

    const char* envFontSize = std::getenv(FONT_SIZE_ENV);

    status += "[System Font Env]\n";
    status += envFontSize ? envFontSize : "(null)";
    status += "\nScale updates after X backend watch tick.";
    status += "\n\n";

    status += "[Label]\n";
    status += "Min: ";
    status += std::to_string(labelMinFontSizeScale).substr(0, 5).c_str();
    status += ", Max: ";
    status += std::to_string(labelMaxFontSizeScale).substr(0, 5).c_str();
    status += "\nSystem: ";
    status += (labelSystemEnabled ? "ON" : "OFF");
    status += ", Adjusted: ";
    status += std::to_string(labelAdjustedScale).substr(0, 5).c_str();

    status += "\n\n[InputField]\n";
    status += "Min: ";
    status += std::to_string(inputMinFontSizeScale).substr(0, 5).c_str();
    status += ", Max: ";
    status += std::to_string(inputMaxFontSizeScale).substr(0, 5).c_str();
    status += "\nSystem: ";
    status += (inputSystemEnabled ? "ON" : "OFF");
    status += ", Adjusted: ";
    status += std::to_string(inputAdjustedScale).substr(0, 5).c_str();

    mStatusLabel.SetText(status);
  }

  void SetSystemFontSize(const char* fontSize)
  {
    SetSystemFontSizeEnv(fontSize);

    UpdateStatus();
  }

  void SetSystemFontSizeScaleEnabled(bool enabled)
  {
    mTargetLabel.SetSystemFontSizeScaleEnabled(enabled);
    mTargetInputField.SetSystemFontSizeScaleEnabled(enabled);
    mFitLabel.SetSystemFontSizeScaleEnabled(enabled);
    mFitCandidateLabel.SetSystemFontSizeScaleEnabled(enabled);

    UpdateStatus();
  }

  void ApplyMinMaxScale(float minScale, float maxScale)
  {
    mTargetLabel.SetMinimumFontSizeScale(minScale);
    mTargetLabel.SetMaximumFontSizeScale(maxScale);

    mTargetInputField.SetMinimumFontSizeScale(minScale);
    mTargetInputField.SetMaximumFontSizeScale(maxScale);

    mFitLabel.SetMinimumFontSizeScale(minScale);
    mFitLabel.SetMaximumFontSizeScale(maxScale);

    mFitCandidateLabel.SetMinimumFontSizeScale(minScale);
    mFitCandidateLabel.SetMaximumFontSizeScale(maxScale);

    UpdateStatus();
  }

  bool OnButton1Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      SetSystemFontSize("SMALL");
    }
    return true;
  }

  bool OnButton2Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      SetSystemFontSize("NORMAL");
    }
    return true;
  }

  bool OnButton3Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      SetSystemFontSize("LARGE");
    }
    return true;
  }

  bool OnButton4Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      SetSystemFontSize("EXTRA_LARGE");
    }
    return true;
  }

  bool OnButton5Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      SetSystemFontSize("GIANT");
    }
    return true;
  }

  bool OnButton6Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      SetSystemFontSizeScaleEnabled(true);
    }
    return true;
  }

  bool OnButton7Touched(Actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      SetSystemFontSizeScaleEnabled(false);
    }
    return true;
  }

  void OnKeyEvent(Window window, KeyEvent event)
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

    // system font size scale
    if(event.GetKeyName() == "1")
    {
      SetSystemFontSize("SMALL");
      ApplyMinMaxScale(0.5f, 2.0f);
    }
    else if(event.GetKeyName() == "2")
    {
      SetSystemFontSize("NORMAL");
      ApplyMinMaxScale(0.5f, 2.0f);
    }
    else if(event.GetKeyName() == "3")
    {
      SetSystemFontSize("LARGE");
      ApplyMinMaxScale(1.2f, 2.0f);
    }
    else if(event.GetKeyName() == "4")
    {
      SetSystemFontSize("EXTRA_LARGE");
      ApplyMinMaxScale(0.5f, 1.3f);
    }
    else if(event.GetKeyName() == "5")
    {
      SetSystemFontSize("GIANT");
      ApplyMinMaxScale(1.4f, 1.0f);
    }
    else if(event.GetKeyName() == "6")
    {
      SetSystemFontSizeScaleEnabled(true);
    }
    else if(event.GetKeyName() == "7")
    {
      SetSystemFontSizeScaleEnabled(false);
    }
    // ui scale
    else if(event.GetKeyName() == "q")
    {
      UiScaleManager::Get().SetScale(0.8f);
    }
    else if(event.GetKeyName() == "w")
    {
      UiScaleManager::Get().SetScale(1.0f);
    }
    else if(event.GetKeyName() == "e")
    {
      UiScaleManager::Get().SetScale(1.2f);
    }
    else if(event.GetKeyName() == "r")
    {
      UiScaleManager::Get().SetScale(1.5f);
    }
    else if(event.GetKeyName() == "t")
    {
      UiScaleManager::Get().SetScale(2.0f);
    }
    else if(event.GetKeyName() == "s")
    {
      mTargetLabel.SetAsyncRendering(false);
      mFitLabel.SetAsyncRendering(false);
      mFitCandidateLabel.SetAsyncRendering(false);
    }
    else if(event.GetKeyName() == "a")
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
  // Ubuntu/X backend-only test hook for runtime system font size changes.
  setenv(FONT_SIZE_WATCH_ENV, "1", 1);
  setenv(FONT_SIZE_ENV, "NORMAL", 1);

  Application application = Application::New(&argc, &argv);
  auto config = UiConfig::New();
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::SMALL, 0.8f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::NORMAL, 1.0f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::LARGE, 1.2f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::EXTRA_LARGE, 1.4f);
  config.SetScaleForSystemFontSize(UiConfig::SystemFontSize::GIANT, 1.5f);
  config.Apply();

  TextScaleController controller(application);
  application.MainLoop();

  return 0;
}
