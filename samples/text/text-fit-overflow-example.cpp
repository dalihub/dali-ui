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
#include <array>
#include <cstddef>
#include <memory>
#include <string>

// INTERNAL INCLUDES
#include <dali-ui-components/public-api/text-button.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include "sample-slider.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float MIN_WIDTH      = 48.f;
constexpr float MAX_WIDTH      = 240.f;
constexpr float MIN_HEIGHT     = 18.f;
constexpr float MAX_HEIGHT     = 200.f;
constexpr float INITIAL_WIDTH  = 240.f;
constexpr float INITIAL_HEIGHT = 160.f;
constexpr float CARD_WIDTH     = 260.f;

enum class TextScenario
{
  BOUNDARY,
  LONG,
  SHORT
};

Label Caption(const char* text)
{
  auto label = Label::New(text);
  label.SetFontSize(14.f);
  label.SetTextColor(UiColor(0xE2E8F0));
  label.SetRequestedHeight(24.f);
  return label;
}

FlexLayout WrappingRow()
{
  auto row = FlexLayout::New();
  row.SetDirection(FlexDirection::ROW);
  row.SetWrap(FlexWrap::WRAP);
  row.SetAlignContent(FlexAlign::FLEX_START);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(WRAP_CONTENT);
  return row;
}
} // namespace

class FitOverflowExample : public ConnectionTracker
{
public:
  explicit FitOverflowExample(Application& application)
  : mApplication(application)
  {
    application.InitSignal().Connect(this, &FitOverflowExample::OnInit);
  }

private:
  const char* CurrentText() const
  {
    if(mSource == TextScenario::SHORT)
    {
      return "OK";
    }
    if(mSource == TextScenario::LONG)
    {
      return mVertical ? "One\nTwo\nThree\nFour\nFive\nSix"
                       : "The entire source scrolls at the minimum font size, without ellipsis.";
    }
    return mVertical ? "One\nTwo\nThree\nFour" : "Fit then scroll";
  }

  TextButton Button(FlexLayout row, const char* text, void (FitOverflowExample::*action)())
  {
    auto button = TextButton::New(text);
    button.SetFontSize(14.f);
    button.SetRequestedWidth(202.f);
    button.SetRequestedHeight(40.f);
    button.SetMargin(Insets(0.f, 6.f, 0.f, 6.f));
    button.ClickedSignal().Connect(this, [this, action](View, InputEvent)
    {
      (this->*action)();
    });
    row.Add(button);
    return button;
  }

  void OnInit(Application application)
  {
    auto window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0x0F172A));
    auto root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetPadding(Insets(12.f, 12.f, 12.f, 12.f));
    root.SetSpacing(8.f);
    root.Add(Caption("TextFit + ON_OVERFLOW"));
    auto help = Caption("Drag Width for horizontal text, Height for vertical text. Shrink until it scrolls, then grow to fit again.");
    help.SetMultiLine(true);
    help.SetRequestedHeight(44.f);
    root.Add(help);

    mWidth  = std::make_unique<TextSample::Slider>("Width", MIN_WIDTH, MAX_WIDTH, 1.f, 0, INITIAL_WIDTH);
    mHeight = std::make_unique<TextSample::Slider>("Height", MIN_HEIGHT, MAX_HEIGHT, 1.f, 0, INITIAL_HEIGHT);
    AddSlider(root, "Width", *mWidth, true);
    AddSlider(root, "Height", *mHeight, false);

    auto choices = WrappingRow();
    mOrientation = Button(choices, "", &FitOverflowExample::ToggleOrientation);
    mText        = Button(choices, "", &FitOverflowExample::CycleText);
    mFit         = Button(choices, "", &FitOverflowExample::ToggleFit);
    mLimit       = Button(choices, "", &FitOverflowExample::ToggleLimit);
    mLoops       = Button(choices, "", &FitOverflowExample::CycleLoops);
    root.Add(choices);
    auto actions = WrappingRow();
    Button(actions, "Restart (R)", &FitOverflowExample::Restart);
    Button(actions, "Stop (S)", &FitOverflowExample::Stop);
    mStopMode = Button(actions, "", &FitOverflowExample::ToggleStopMode);
    Button(actions, "Reset", &FitOverflowExample::Reset);
    Button(actions, "Check state", &FitOverflowExample::CheckState);
    root.Add(actions);

    auto                              comparisons = WrappingRow();
    const std::array<const char*, 4u> titles{{"Sync Range", "Async Range", "Sync Candidates", "Async Candidates"}};
    for(std::size_t index = 0u; index < mLabels.size(); ++index)
    {
      auto card = StackLayout::New(StackOrientation::VERTICAL);
      card.SetRequestedWidth(CARD_WIDTH);
      card.SetPadding(Insets(8.f, 8.f, 8.f, 8.f));
      card.SetMargin(Insets(0.f, 4.f, 0.f, 4.f));
      card.SetSpacing(6.f);
      card.SetBackgroundColor(UiColor(0x1E293B));
      card.Add(Caption(titles[index]));
      // Reserve the largest viewport so slider changes cannot move the controls.
      auto area = StackLayout::New(StackOrientation::VERTICAL);
      area.SetRequestedWidth(MAX_WIDTH);
      area.SetRequestedHeight(MAX_HEIGHT);
      auto label = Label::New(CurrentText());
      label.SetRequestedWidth(INITIAL_WIDTH);
      label.SetRequestedHeight(INITIAL_HEIGHT);
      label.SetBackgroundColor(UiColor(0xE3F2E5));
      label.SetTextColor(UiColor(0x14251B));
      label.SetBorderlineWidth(1.f);
      label.SetBorderlineOffset(-1.f);
      label.SetBorderlineColor(UiColor(0x45DFD4));
      label.SetFontSize(32.f);
      label.SetMarqueeTriggerPolicy(Text::MarqueeTriggerPolicy::ON_OVERFLOW);
      label.SetMarqueeLoopCount(mLoopCount);
      label.SetMarqueeSpeed(100);
      label.SetMarqueeGap(20);
      label.SetMarqueeLoopDelay(0.f);
      label.SetMarqueeStopMode(Text::MarqueeStopMode::FINISH_LOOP);
      label.SetAsyncRendering(index % 2u != 0u);
      mLabels[index] = label;
      ApplyFit(index);
      area.Add(label);
      card.Add(area);
      mStates[index] = Caption("State: press Check state");
      card.Add(mStates[index]);
      comparisons.Add(card);
    }
    root.Add(comparisons);
    auto note = Caption("Loop 0 repeats; 1 / 2 / 3 stop after that many loops. State is a snapshot taken by Check state. Esc exits.");
    note.SetMultiLine(true);
    note.SetRequestedHeight(44.f);
    root.Add(note);
    mScroll = ScrollView::New();
    mScroll.SetScrollDirection(ScrollDirection::Vertical);
    mScroll.SetRequestedWidth(MATCH_PARENT);
    mScroll.SetRequestedHeight(MATCH_PARENT);
    mScroll.SetContent(root);
    window.Add(mScroll);
    window.KeyEventSignal().Connect(this, &FitOverflowExample::OnKey);
    window.FocusChangedSignal().Connect(this, [this](Window, bool focused)
    {
      if(!focused)
      {
        mWidth->Cancel();
        mHeight->Cancel();
      }
    });
    UpdateControls();
  }

  void AddSlider(StackLayout root, const char* name, TextSample::Slider& slider, bool width)
  {
    auto row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    auto caption = Caption(name);
    caption.SetRequestedWidth(60.f);
    row.Add(caption);
    row.Add(slider.GetView());
    root.Add(row);
    slider.Changed.Connect(this, [this, width](float value)
    {
      ApplySize(width, value);
    });
    slider.Cancelled.Connect(this, [this, width](float value)
    {
      ApplySize(width, value);
      mScroll.SetPanScrollEnabled(true);
    });
    slider.Started.Connect(this, [this]()
    {
      mScroll.SetPanScrollEnabled(false);
    });
    slider.Committed.Connect(this, [this](float)
    {
      mScroll.SetPanScrollEnabled(true);
    });
  }

  void ApplySize(bool width, float value)
  {
    for(auto label : mLabels)
    {
      if(width)
      {
        label.SetRequestedWidth(value);
      }
      else
      {
        label.SetRequestedHeight(value);
      }
    }
  }

  void ApplyFit(std::size_t index)
  {
    if(index < 2u)
    {
      mLabels[index].SetTextFit(Text::Fit::Range(mLargeMinimum ? 24.f : 12.f, 32.f, 2.f));
    }
    else
    {
      Dali::Vector<Text::Fit::Candidate> choices;
      if(!mLargeMinimum)
      {
        choices.PushBack(Text::Fit::Candidate(12.f, 16.f));
      }
      choices.PushBack(Text::Fit::Candidate(mLargeMinimum ? 24.f : 22.f, mLargeMinimum ? 32.f : 28.f));
      choices.PushBack(Text::Fit::Candidate(32.f, 40.f));
      mLabels[index].SetTextFit(choices);
    }
  }

  void UpdateControls()
  {
    mOrientation.SetText(mVertical ? "Vertical (V)" : "Horizontal (V)");
    mText.SetText(mSource == TextScenario::BOUNDARY ? "Text: boundary (T)" : mSource == TextScenario::LONG ? "Text: long (T)"
                                                                                                           : "Text: short (T)");
    mFit.SetText(mLargeMinimum ? "Fit: 24 to 32 (C)" : "Fit: 12 to 32 (C)");
    mLimit.SetText(mLimited ? "Lines: 2 (M)" : "Lines: unlimited (M)");
    mLoops.SetText(("Loops: " + std::to_string(mLoopCount) + (mLoopCount == 0 ? " (infinite)" : "")).c_str());
    mStopMode.SetText(mFinishLoop ? "FINISH_LOOP (F)" : "IMMEDIATE (F)");
  }

  void ToggleOrientation()
  {
    mVertical = !mVertical;
    for(auto label : mLabels)
    {
      label.SetMultiLine(mVertical);
      label.SetMarqueeOrientation(mVertical ? Text::MarqueeOrientation::VERTICAL : Text::MarqueeOrientation::HORIZONTAL);
      label.SetText(CurrentText());
    }
    UpdateControls();
  }

  void CycleText()
  {
    mSource = mSource == TextScenario::BOUNDARY ? TextScenario::LONG : mSource == TextScenario::LONG ? TextScenario::SHORT
                                                                                                     : TextScenario::BOUNDARY;
    for(auto label : mLabels)
    {
      label.SetText(CurrentText());
    }
    UpdateControls();
  }

  void ToggleFit()
  {
    mLargeMinimum = !mLargeMinimum;
    for(std::size_t index = 0u; index < mLabels.size(); ++index)
    {
      ApplyFit(index);
    }
    UpdateControls();
  }

  void ToggleLimit()
  {
    mLimited = !mLimited;
    for(auto label : mLabels)
    {
      label.SetMaximumLines(mLimited ? 2 : 0);
    }
    UpdateControls();
  }

  void CycleLoops()
  {
    mLoopCount = (mLoopCount + 1) % 4;
    for(auto label : mLabels)
    {
      label.SetMarqueeLoopCount(mLoopCount);
    }
    UpdateControls();
  }

  void ToggleStopMode()
  {
    mFinishLoop = !mFinishLoop;
    for(auto label : mLabels)
    {
      label.SetMarqueeStopMode(mFinishLoop ? Text::MarqueeStopMode::FINISH_LOOP : Text::MarqueeStopMode::IMMEDIATE);
    }
    UpdateControls();
  }

  void Restart()
  {
    for(auto label : mLabels)
    {
      label.StartMarquee();
    }
  }

  void Stop()
  {
    for(auto label : mLabels)
    {
      label.StopMarquee();
    }
  }

  void CheckState()
  {
    for(std::size_t index = 0u; index < mLabels.size(); ++index)
    {
      mStates[index].SetText(mLabels[index].IsMarqueeRunning() ? "Last check: running" : "Last check: stopped");
    }
  }

  void Reset()
  {
    mWidth->Cancel();
    mHeight->Cancel();
    mVertical = mLargeMinimum = mLimited = false;
    mFinishLoop                          = true;
    mLoopCount                           = 0;
    mSource                              = TextScenario::BOUNDARY;
    mWidth->SetValue(INITIAL_WIDTH);
    mHeight->SetValue(INITIAL_HEIGHT);
    for(std::size_t index = 0u; index < mLabels.size(); ++index)
    {
      auto label = mLabels[index];
      label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
      label.StopMarquee();
      label.SetMultiLine(false);
      label.SetMarqueeOrientation(Text::MarqueeOrientation::HORIZONTAL);
      label.SetText(CurrentText());
      label.SetMaximumLines(0);
      label.SetMarqueeLoopCount(0);
      label.SetMarqueeStopMode(Text::MarqueeStopMode::FINISH_LOOP);
      ApplyFit(index);
      mStates[index].SetText("State: press Check state");
    }
    ApplySize(true, INITIAL_WIDTH);
    ApplySize(false, INITIAL_HEIGHT);
    Restart();
    UpdateControls();
  }

  void OnKey(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }
    const auto key = event.GetKeyName();
    if(IsKey(event, DALI_KEY_ESCAPE) || IsKey(event, DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(key == "r" || key == "R")
    {
      Restart();
    }
    else if(key == "s" || key == "S")
    {
      Stop();
    }
    else if(key == "f" || key == "F")
    {
      ToggleStopMode();
    }
    else if(key == "v" || key == "V")
    {
      ToggleOrientation();
    }
    else if(key == "t" || key == "T")
    {
      CycleText();
    }
    else if(key == "c" || key == "C")
    {
      ToggleFit();
    }
    else if(key == "m" || key == "M")
    {
      ToggleLimit();
    }
  }

  Application&                        mApplication;
  std::array<Label, 4u>               mLabels;
  std::array<Label, 4u>               mStates;
  std::unique_ptr<TextSample::Slider> mWidth;
  std::unique_ptr<TextSample::Slider> mHeight;
  ScrollView                          mScroll;
  TextButton                          mOrientation, mText, mFit, mLimit, mLoops, mStopMode;
  TextScenario                        mSource{TextScenario::BOUNDARY};
  int                                 mLoopCount{0};
  bool                                mVertical{false};
  bool                                mFinishLoop{true};
  bool                                mLargeMinimum{false};
  bool                                mLimited{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  FitOverflowExample example(application);
  application.MainLoop();
  return 0;
}
