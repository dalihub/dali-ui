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

#include "manual-test-case.h"

#include <dali/public-api/adaptor-framework/timer.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr uint32_t COLOR_TARGET  = 0x2196F3;
constexpr uint32_t COLOR_RESET   = 0x4CAF50;
constexpr uint32_t COLOR_FAIL    = 0xF44336;
constexpr uint32_t COLOR_WHITE   = 0xFFFFFF;
constexpr float    TARGET_W      = 300.0f;
constexpr float    TARGET_H      = 150.0f;
constexpr float    RESET_H       = 80.0f;
constexpr float    LOG_H         = 300.0f;
constexpr float    PAD           = 30.0f;
constexpr float    SPACING       = 20.0f;
constexpr uint32_t DISABLE_DELAY = 1500;
} // namespace

/**
 * @brief Verifies that PRESSED state is cleared when a view becomes DISABLED.
 *
 * A blue InteractiveView auto-disables 1.5 s after being pressed.
 * The state log shows transitions; if PRESSED is properly cleared the log
 * displays "PRESSED cleared by DISABLED!".
 */
class TcDisabledClearsPressed : public ManualTest::TestCase, public ConnectionTracker
{
public:
  Dali::String GetName() const override
  {
    return "Disabled clears Pressed";
  }

  Dali::String GetDescription() const override
  {
    return "PRESSED state must be cleared when a view becomes DISABLED (touch & key)";
  }

  void OnEnter(View contentArea) override
  {
    mLog = Label::New("State log:\n")
      .SetMultiLine(true)
      .SetFontSize(20.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(LOG_H);

    mTarget = InteractiveView::New();
    mTarget
      .SetBackgroundColor(UiColor(COLOR_TARGET))
      .SetRequestedWidth(TARGET_W)
      .SetRequestedHeight(TARGET_H);

    mTarget.StateChangedSignal().Connect(this, &TcDisabledClearsPressed::OnStateChanged);
    mTarget.PressedChangedSignal().Connect(this, &TcDisabledClearsPressed::OnPressedChanged);

    mTarget.Add(Label::New("Press & Hold me")
      .SetFontSize(28.0f)
      .SetTextColor(UiColor(COLOR_WHITE)));

    mResetButton = InteractiveView::New();
    mResetButton
      .SetBackgroundColor(UiColor(COLOR_RESET))
      .SetRequestedWidth(TARGET_W)
      .SetRequestedHeight(RESET_H)
      .ConnectClickedSignal(this, [this](View, InputEvent) -> bool {
        mTarget.SetEnabled(true);
        mTarget.SetBackgroundColor(UiColor(COLOR_TARGET));
        AppendLog("-- Reset: re-enabled --\n");
        return true;
      });

    mResetButton.Add(Label::New("Re-enable")
      .SetFontSize(22.0f)
      .SetTextColor(UiColor(COLOR_WHITE)));

    mDisableTimer = Timer::New(DISABLE_DELAY);
    mDisableTimer.TickSignal().Connect(this, &TcDisabledClearsPressed::OnDisableTimerTick);

    contentArea.Add(StackLayout::New()
      .SetSpacing(SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetPadding(Extents(PAD, PAD, PAD, PAD))
      .Children({mTarget, mResetButton, mLog}));
  }

  void OnExit() override
  {
    mDisableTimer.Stop();
  }

private:
  void OnStateChanged(View, StateEvent e)
  {
    String line = "[" + e.GetPrev().ToString() + "] -> [" + e.GetCurrent().ToString() + "]\n";
    AppendLog(line);

    if(e.Removed(ViewState::PRESSED) && e.Added(ViewState::DISABLED))
    {
      mTarget.SetBackgroundColor(UiColor(COLOR_FAIL));
      AppendLog("  => PRESSED cleared by DISABLED!\n");
    }
  }

  void OnPressedChanged(View, bool pressed, InputEvent)
  {
    if(pressed)
    {
      mDisableTimer.Start();
    }
    else
    {
      mDisableTimer.Stop();
    }
  }

  bool OnDisableTimerTick()
  {
    AppendLog("-- Timer: disabling while pressed --\n");
    mTarget.SetEnabled(false);
    return false;
  }

  void AppendLog(const String& text)
  {
    mLogText += text;
    mLog.SetText(mLogText);
  }

  InteractiveView mTarget;
  InteractiveView mResetButton;
  Label           mLog;
  Timer           mDisableTimer;
  String          mLogText{"State log:\n"};
};

REGISTER_MANUAL_TEST(TcDisabledClearsPressed)
