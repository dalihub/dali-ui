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

// Verifies the Windows backend WIN_CALLBACK_EVENT routing fix (PostWinCallback ->
// window message + WinProc). A Timer mutates the scene every tick; on the Windows
// backend the redraw is driven by Core::ProcessEvents(), which is scheduled by an
// idle WIN_CALLBACK_EVENT. Before the fix that event was a thread message a nested
// modal loop (window move/resize) would drop, freezing rendering while dragging.
//
// Run it and drag / resize the window: the background must keep animating and the
// per-second tick log must keep advancing.

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <dali/public-api/adaptor-framework/timer.h>

#include <cmath>
#include <cstdint>
#include <iostream>

using namespace Dali;
using namespace Dali::Ui;

class TimerRenderController : public ConnectionTracker
{
public:
  explicit TimerRenderController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TimerRenderController::Create);
  }

  void Create(Application application)
  {
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Color::BLACK);

    // A real renderable view whose colour the timer changes every tick, so there
    // is actual content to re-render (a bare window background colour adds no
    // renderer and DALi would just report "Rendering skip").
    mBox = View::New();
    mBox.SetRequestedWidth(400_spx);
    mBox.SetRequestedHeight(400_spx);
    mBox.SetBackgroundColor(UiColor(0.2f, 0.2f, 0.2f));
    mWindow.Add(mBox);

    mTimer = Timer::New(16u); // ~60 Hz
    mTimer.TickSignal().Connect(this, &TimerRenderController::OnTick);
    mTimer.Start();

    std::cout << "[timer-render] started a 16ms timer that recolours the window every tick.\n"
                 "[timer-render] Drag or resize the window - it must keep animating (no render stall)."
              << std::endl;
  }

  bool OnTick()
  {
    ++mTick;

    const float phase = mTick * 0.05f;
    const float r     = 0.5f + 0.5f * std::sin(phase);
    const float g     = 0.5f + 0.5f * std::sin(phase + 2.094f); // +120 deg
    const float b     = 0.5f + 0.5f * std::sin(phase + 4.188f); // +240 deg
    mBox.SetBackgroundColor(UiColor(r, g, b));

    if(mTick % 60u == 0u)
    {
      std::cout << "[timer-render] tick " << mTick << std::endl;
    }
    return true; // keep the timer running
  }

private:
  Application& mApplication;
  Window       mWindow;
  View         mBox;
  Timer        mTimer;
  uint32_t     mTick = 0u;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application           application = Application::New(&argc, &argv);
  UiConfig              config      = UiConfig::New();
  config.Apply();
  TimerRenderController controller(application);
  application.MainLoop();
  return 0;
}
