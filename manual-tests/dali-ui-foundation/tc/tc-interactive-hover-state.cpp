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

#include "manual-test-case.h"

#include <array>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t COLOR_BG              = 0xF6F7F9;
constexpr uint32_t COLOR_TEXT            = 0x263238;
constexpr uint32_t COLOR_SUB_TEXT        = 0x607D8B;
constexpr uint32_t COLOR_NORMAL          = 0xE3F2FD;
constexpr uint32_t COLOR_HOVERED         = 0x80CBC4;
constexpr uint32_t COLOR_PRESSED         = 0xFFAB91;
constexpr uint32_t COLOR_DISABLED        = 0xCFD8DC;
constexpr uint32_t COLOR_PSEUDO_DISABLED = 0xB0BEC5;
constexpr uint32_t COLOR_BUTTON          = 0x455A64;
constexpr uint32_t COLOR_BUTTON_ALT      = 0x00796B;
constexpr uint32_t COLOR_WHITE           = 0xFFFFFF;

constexpr float PAD             = 20.0f;
constexpr float GAP             = 12.0f;
constexpr float TARGET_H        = 136.0f;
constexpr float BUTTON_H        = 56.0f;
constexpr float LOG_H           = 220.0f;
constexpr float FONT_TITLE      = 20.0f;
constexpr float FONT_BODY       = 15.0f;
constexpr float FONT_LOG        = 14.0f;
constexpr float FONT_BUTTON     = 16.0f;
constexpr uint32_t TARGET_COUNT = 3u;

enum TargetIndex : uint32_t
{
  NORMAL_TARGET,
  CLICKABLE_FALSE_TARGET,
  POLICY_TARGET,
};

Dali::String BoolText(bool value)
{
  return value ? "true" : "false";
}

Dali::String InputEventTypeText(InputEvent event)
{
  if(!event)
  {
    return "invalid";
  }

  switch(event.GetInputEventType())
  {
    case InputEventType::NONE:
    {
      return event.IsProgrammatic() ? "programmatic" : "none";
    }
    case InputEventType::TOUCH_EVENT:
    {
      return "touch";
    }
    case InputEventType::KEY_EVENT:
    {
      return "key";
    }
    case InputEventType::TAP_GESTURE:
    {
      return "tap";
    }
    case InputEventType::LONG_PRESS_GESTURE:
    {
      return "long-press";
    }
    case InputEventType::WHEEL_EVENT:
    {
      return "wheel";
    }
    case InputEventType::HOVER_EVENT:
    {
      return "hover";
    }
    case InputEventType::RESERVED:
    {
      return "reserved";
    }
  }

  return "unknown";
}
} // namespace

/**
 * @brief Verifies InteractiveTrait HOVERED state with press and disabled policies.
 */
class TcInteractiveHoverState : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Interactive Hover State";
  }

  Dali::String GetDescription() const override
  {
    return "Verifies HOVERED state, hover signal, clickability, disabled, and pseudo-disabled policies";
  }

  void OnEnter(View contentArea) override
  {
    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetBackgroundColor(UiColor(COLOR_BG));
    root.SetPadding(Extents(PAD, PAD, PAD, PAD));
    root.SetSpacing(GAP);

    root.Add(MakeHeader());
    root.Add(CreateTarget(NORMAL_TARGET, "Normal interactive target", "Hover, then press while the pointer remains over this view.", COLOR_NORMAL));
    root.Add(CreateTarget(CLICKABLE_FALSE_TARGET, "Clickable=false target", "Hover should still turn on even though click is blocked.", COLOR_NORMAL));
    root.Add(CreateTarget(POLICY_TARGET, "Policy target", "Use the buttons below while this target is hovered.", COLOR_NORMAL));
    root.Add(MakeButtonRow());

    mLog = Label::New("Event log:\n");
    mLog.SetMultiLine(true);
    mLog.SetFontSize(FONT_LOG);
    mLog.SetTextColor(UiColor(COLOR_TEXT));
    mLog.SetRequestedWidth(MATCH_PARENT);
    mLog.SetRequestedHeight(LOG_H);
    mLog.SetBackgroundColor(UiColor(0xFFFFFF));
    mLog.SetPadding(Extents(12.0f, 12.0f, 10.0f, 10.0f));
    root.Add(mLog);

    mTraits[CLICKABLE_FALSE_TARGET].SetClickable(false);
    UpdateAllTargets();

    contentArea.Add(root);
  }

private:
  View MakeHeader()
  {
    Label label = Label::New("Move a mouse/stylus across the targets and watch the states.");
    label.SetMultiLine(true);
    label.SetFontSize(FONT_BODY);
    label.SetTextColor(UiColor(COLOR_SUB_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    return label;
  }

  View CreateTarget(uint32_t index, const Dali::String& title, const Dali::String& description, uint32_t baseColor)
  {
    mBaseColors[index] = baseColor;
    mTitles[index]     = title;

    mTargets[index] = InteractiveView::New();
    mTargets[index].SetRequestedWidth(MATCH_PARENT);
    mTargets[index].SetRequestedHeight(TARGET_H);
    mTargets[index].SetBackgroundColor(UiColor(baseColor));
    mTargets[index].SetPadding(Extents(16.0f, 16.0f, 12.0f, 12.0f));
    mTargets[index].SetFocusable(true);

    mTraits[index] = mTargets[index].AsInteractive();

    StackLayout content = StackLayout::New(StackOrientation::VERTICAL);
    content.SetRequestedWidth(MATCH_PARENT);
    content.SetRequestedHeight(MATCH_PARENT);
    content.SetSpacing(4.0f);

    Label titleLabel = Label::New(title);
    titleLabel.SetFontSize(FONT_TITLE);
    titleLabel.SetTextColor(UiColor(COLOR_TEXT));
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(WRAP_CONTENT);

    Label descriptionLabel = Label::New(description);
    descriptionLabel.SetMultiLine(true);
    descriptionLabel.SetFontSize(FONT_BODY);
    descriptionLabel.SetTextColor(UiColor(COLOR_SUB_TEXT));
    descriptionLabel.SetRequestedWidth(MATCH_PARENT);
    descriptionLabel.SetRequestedHeight(WRAP_CONTENT);

    mStateLabels[index] = Label::New();
    mStateLabels[index].SetMultiLine(true);
    mStateLabels[index].SetFontSize(FONT_BODY);
    mStateLabels[index].SetTextColor(UiColor(COLOR_TEXT));
    mStateLabels[index].SetRequestedWidth(MATCH_PARENT);
    mStateLabels[index].SetRequestedHeight(WRAP_CONTENT);

    content.Add(titleLabel);
    content.Add(descriptionLabel);
    content.Add(mStateLabels[index]);
    mTargets[index].Add(content);

    mTargets[index].StateChangedSignal().Connect(this, [this, index](View, StateEvent event)
    {
      AppendLog(mTitles[index] + ": state [" + event.GetPrev().ToString() + "] -> [" + event.GetCurrent().ToString() + "] cause=" + InputEventTypeText(event.GetCause()) + "\n");
      UpdateTarget(index);
    });

    mTraits[index].HoveredChangedSignal().Connect(this, [this, index](View, bool hovered, InputEvent event)
    {
      AppendLog(mTitles[index] + ": hovered=" + BoolText(hovered) + " cause=" + InputEventTypeText(event) + "\n");
      UpdateTarget(index);
    });

    mTraits[index].PressedChangedSignal().Connect(this, [this, index](View, bool pressed, InputEvent event)
    {
      AppendLog(mTitles[index] + ": pressed=" + BoolText(pressed) + " cause=" + InputEventTypeText(event) + "\n");
      UpdateTarget(index);
    });

    return mTargets[index];
  }

  View MakeButtonRow()
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(BUTTON_H);
    row.SetSpacing(GAP);
    row.Add(MakeButton("Reset", COLOR_BUTTON, [this]()
    {
      mPolicyEnabled        = true;
      mPolicyPseudoDisabled = false;
      mTargets[POLICY_TARGET].SetEnabled(true);
      mTraits[POLICY_TARGET].SetPseudoDisabled(false);
      AppendLog("-- reset policy target --\n");
      UpdateAllTargets();
    }));
    row.Add(MakeButton("Toggle enabled", COLOR_BUTTON_ALT, [this]()
    {
      mPolicyEnabled = !mPolicyEnabled;
      mTargets[POLICY_TARGET].SetEnabled(mPolicyEnabled);
      AppendLog(Dali::String("-- policy enabled=") + BoolText(mPolicyEnabled) + " --\n");
      UpdateAllTargets();
    }));
    row.Add(MakeButton("Toggle pseudo-disabled", COLOR_BUTTON_ALT, [this]()
    {
      mPolicyPseudoDisabled = !mPolicyPseudoDisabled;
      mTraits[POLICY_TARGET].SetPseudoDisabled(mPolicyPseudoDisabled);
      AppendLog(Dali::String("-- policy pseudoDisabled=") + BoolText(mPolicyPseudoDisabled) + " --\n");
      UpdateAllTargets();
    }));
    return row;
  }

  View MakeButton(const Dali::String& text, uint32_t color, std::function<void()> action)
  {
    InteractiveView button = InteractiveView::New();
    button.SetRequestedHeight(MATCH_PARENT);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    button.SetBackgroundColor(UiColor(color));
    button.ClickedSignal().Connect(this, [action](View, InputEvent)
    {
      action();
    });

    Label label = Label::New(text);
    label.SetFontSize(FONT_BUTTON);
    label.SetTextColor(UiColor(COLOR_WHITE));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(MATCH_PARENT);
    label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
    label.SetVerticalTextAlignment(Text::Alignment::CENTER);
    button.Add(label);
    return button;
  }

  void UpdateAllTargets()
  {
    for(uint32_t index = 0u; index < TARGET_COUNT; ++index)
    {
      UpdateTarget(index);
    }
  }

  void UpdateTarget(uint32_t index)
  {
    const ViewState state   = mTargets[index].GetState();
    const bool      hovered = mTraits[index].IsHovered();
    const bool      pressed = mTraits[index].IsPressed();

    if(state.Contains(ViewState::DISABLED))
    {
      mTargets[index].SetBackgroundColor(UiColor(COLOR_DISABLED));
    }
    else if(state.Contains(ViewState::PSEUDO_DISABLED))
    {
      mTargets[index].SetBackgroundColor(UiColor(COLOR_PSEUDO_DISABLED));
    }
    else if(pressed)
    {
      mTargets[index].SetBackgroundColor(UiColor(COLOR_PRESSED));
    }
    else if(hovered)
    {
      mTargets[index].SetBackgroundColor(UiColor(COLOR_HOVERED));
    }
    else
    {
      mTargets[index].SetBackgroundColor(UiColor(mBaseColors[index]));
    }

    Dali::String text = "State: [" + state.ToString() + "]";
    text += "\nIsHovered: " + BoolText(hovered);
    text += "  IsPressed: " + BoolText(pressed);
    text += "  Clickable: " + BoolText(mTraits[index].IsClickable());
    mStateLabels[index].SetText(text);
  }

  void AppendLog(const Dali::String& text)
  {
    mLogText += text;
    mLog.SetText(mLogText);
  }

  std::array<InteractiveView, TARGET_COUNT>    mTargets;
  std::array<InteractiveTrait, TARGET_COUNT>   mTraits;
  std::array<Label, TARGET_COUNT>              mStateLabels;
  std::array<Dali::String, TARGET_COUNT>       mTitles;
  std::array<uint32_t, TARGET_COUNT>           mBaseColors{};
  Label                                        mLog;
  Dali::String                                 mLogText{"Event log:\n"};
  bool                                         mPolicyEnabled{true};
  bool                                         mPolicyPseudoDisabled{false};
};

REGISTER_MANUAL_TEST(TcInteractiveHoverState)
