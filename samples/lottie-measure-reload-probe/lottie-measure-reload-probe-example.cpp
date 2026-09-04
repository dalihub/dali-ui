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

// On-device probe: what happens when a view RELOADS a Lottie child from inside its own
// measure pass?
//
// MeasureReloadProbeView (measure-reload-probe-view.h) creates a LottieAnimationView in
// its construction path, sets its resource with SetResourceUrl() and adds it as a child.
// It then installs a measure callback with View::SetMeasureCallback(), and from inside
// that callback it touches the same child before measuring it -- either with
// SetResourceUrl() carrying the url the child already has, or with Reload().
//
// What to observe on a device:
//  1. In the DEFAULT mode the callback issues SetResourceUrl() with the current url,
//     which is a NO-OP: the child is not dirtied, the layout settles, "LayoutFinished"
//     prints once, and the application goes idle. CPU for this process in `top` should
//     fall back to the idle baseline and the per-pass lines should stop.
//  2. Press 5 to switch the callback to Reload(). That is the explicit reload, and it
//     rebuilds the child's visual on every pass. The passes, the CPU reading and the
//     framework diagnostics below are what this probe exists to show.
//  3. CPU for this process in `top` while nothing is touched. Press 3 first: the per-pass
//     console line is the largest cost in the loop and would otherwise dominate the
//     reading.
//  4. The console "[rate]" lines. They report wall-clock time per 25 measure passes, so
//     they show the pass rate directly. With per-pass logging on they are a LOWER bound.
//  5. Whether passes keep arriving with NO input at all. That is the actual question:
//     an idle application prints nothing.
//  6. "LayoutFinished" is printed only when layout really settles. In Reload() mode the
//     producer keeps re-invalidating and it stays starved; press 1 to stop the producer,
//     or 5 to go back to the no-op call, and the next event should drain the pending work
//     and settle.
//  7. In Reload() mode two framework diagnostics are expected and are not defects: one
//     'View::InvalidateMeasure() called ... while a Measure/Arrange pass is running'
//     error line per view (the Lottie child's in-pass invalidation), and one
//     LayoutController 'layout root(s) ... remain pending' line per parked episode.
//
// Keys:
//  1 - toggle the in-measure call (the pattern under test); on by default
//  2 - print the current counters
//  3 - toggle the one-line-per-pass console trace; on by default
//  4 - play / pause the Lottie animation; paused by default
//  5 - toggle explicit Reload() instead of the same-url SetResourceUrl(); off by default
//  Escape / Back - quit
//
// Mechanism, stated factually:
//  - SetResourceUrl() with the url that is already set is a NO-OP since this change: the
//    Lottie view leaves its visual clean and raises no InvalidateMeasure(), so a pass that
//    only re-applies the current url converges.
//  - Reload() is the explicit reload. It marks the visual dirty and calls
//    InvalidateMeasure(). Raised from inside a Measure pass that is a contract violation:
//    it is RETAINED and PARKED -- caches are revoked and the layout root stays pending,
//    but it requests no idle ProcessEvents wake. It also re-dirties the probe while the
//    probe's own producer is running, so the probe declines to publish its measure cache
//    and its callback runs again on the next pass.
//  - The child's Measure() in the same callback rebuilds the vector visual, because the
//    dirty flag is consumed there.
//  - The animated vector image visual parks ITS main loop wake too while a
//    Measure/Arrange pass is on the stack: it still registers its once post-processor, it
//    just no longer calls RequestProcessEventsAndUpdate() from inside the pass.
//  - What remains is asynchronous. Each rebuilt visual loads on its own, and when it
//    reaches READY the Lottie view requests a re-layout AT EVENT TIME, which is a legal
//    wake. Whether that closes the loop into a self-sustaining main loop on real hardware
//    is exactly what this sample is here to show. This file asserts no result.
//
// NOTE: this is a DIAGNOSTIC reproducer of a contract violation, not a pattern to copy.
// Mutating a view from inside a measure pass is precisely what the measure contract
// forbids: a measure implementation must be a pure function of its constraints, the
// view's effective scale, its own layout-tracked state, the effective layout direction
// and its children's measured sizes.

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

#include <iostream>

#include "measure-reload-probe-view.h"

using namespace Dali;
using namespace Dali::Ui;
using namespace LottieProbeSample;

namespace
{
Label MakeLabel(const Dali::String& text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}
} // namespace

class LottieMeasureReloadProbeExample : public ConnectionTracker
{
public:
  explicit LottieMeasureReloadProbeExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &LottieMeasureReloadProbeExample::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &LottieMeasureReloadProbeExample::OnKeyEvent);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(12.0f);
    root.SetPadding(Insets(24.0f, 24.0f, 24.0f, 24.0f));

    root.Add(MakeLabel("Lottie reload from inside Measure", 24.0f, 0x202124u));
    root.Add(MakeLabel("The probe below touches its Lottie child from its measure callback.", 14.0f, 0x5F6368u));
    root.Add(MakeLabel("Keys: 1 call on/off, 2 counters, 3 per-pass log, 4 play/pause, 5 Reload().", 14.0f, 0x5F6368u));

    mProbe = MeasureReloadProbeView::New();
    root.Add(mProbe);

    window.Add(root);

    LayoutController::Get(window).LayoutFinishedSignal().Connect(this, &LottieMeasureReloadProbeExample::OnLayoutFinished);

    std::cout << "probe started: in-measure call " << (mProbe.IsReloadInMeasure() ? "ON" : "OFF")
              << ", explicit Reload() " << (mProbe.IsExplicitReload() ? "ON" : "OFF")
              << ", per-pass log " << (mProbe.IsPerPassLogging() ? "ON" : "OFF")
              << ", playback " << (mProbe.IsPlaying() ? "ON" : "OFF") << std::endl;
  }

  void OnLayoutFinished(Window /*window*/)
  {
    // Starved while the producer keeps re-invalidating; fires once layout finally settles.
    std::cout << "LayoutFinished: settled after " << mProbe.GetMeasureCount()
              << " measure passes, " << mProbe.GetReloadCount() << " in-pass calls" << std::endl;
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(event.GetKeyName() == "1")
    {
      mProbe.SetReloadInMeasure(!mProbe.IsReloadInMeasure());
      std::cout << "in-measure call " << (mProbe.IsReloadInMeasure() ? "ON" : "OFF") << std::endl;
    }
    else if(event.GetKeyName() == "2")
    {
      std::cout << "counters: " << mProbe.GetMeasureCount() << " measure passes, "
                << mProbe.GetReloadCount() << " in-pass calls" << std::endl;
    }
    else if(event.GetKeyName() == "3")
    {
      mProbe.SetPerPassLogging(!mProbe.IsPerPassLogging());
      std::cout << "per-pass log " << (mProbe.IsPerPassLogging() ? "ON" : "OFF") << std::endl;
    }
    else if(event.GetKeyName() == "4")
    {
      mProbe.SetPlaying(!mProbe.IsPlaying());
      std::cout << "playback " << (mProbe.IsPlaying() ? "ON" : "OFF") << std::endl;
    }
    else if(event.GetKeyName() == "5")
    {
      mProbe.SetExplicitReload(!mProbe.IsExplicitReload());
      std::cout << "explicit Reload() " << (mProbe.IsExplicitReload() ? "ON" : "OFF")
                << " (the in-measure call is now "
                << (mProbe.IsExplicitReload() ? "Reload()" : "SetResourceUrl() with the current url")
                << ")" << std::endl;
    }
    else if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
  }

private:
  Application&           mApplication;
  MeasureReloadProbeView mProbe;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application                     application = Application::New(&argc, &argv);
  LottieMeasureReloadProbeExample example(application);
  application.MainLoop();
  return 0;
}
