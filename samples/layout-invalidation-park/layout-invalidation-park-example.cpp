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

// Demonstrates the PARK contract for layout invalidation raised DURING layout
// processing: the work is fully retained (dirty + pending layout root) but it
// never wakes the event loop by itself.
//
// The box view below carries an ArrangeCallback that changes a Label's TEXT on
// every arrange. Changing the text invalidates the label's measure -- from
// INSIDE the arrange pass -- so that invalidation is parked: it stays pending,
// requests no idle ProcessEvents wake, and is serviced only by the NEXT
// externally triggered event.
//
// What to observe:
//  1. After launch the app goes IDLE (CPU ~0 in top) even though every arrange
//     re-invalidates the label. Under the old behaviour this exact producer
//     kept the main loop spinning at 100% CPU.
//  2. The console prints one "arrange #N" line per EXTERNAL event (touch, key):
//     each event drains the parked work once, the callback re-parks, and the
//     loop sleeps again. The label's SIZE also lags its text by one event --
//     the newest string renders inside the previous measure, which is the
//     "dirty but not yet laid out" state made visible.
//  3. The LayoutController logs one parked-episode diagnostic (DALI_LOG_ERROR).
//     No per-View warning appears: SetText() invalidates through the
//     framework-internal path, and the park policy is route-independent.
//  4. LayoutFinished stays deferred while the producer keeps re-invalidating.
//     After ARRANGE_LIMIT arranges the callback stops mutating; the next event
//     drains, the layout finally settles, and "LayoutFinished" is printed.
//
// Press Escape or Back to quit.
//
// NOTE: this is a DIAGNOSTIC reproducer, not a pattern to copy. Two things about it are
// deliberately wrong for production code: the arrange callback mutates another view from
// inside a layout pass, which is exactly the contract violation the PARK rule exists to
// contain; and it selects ArrangePolicy::ALWAYS so the callback is guaranteed to run on
// every pass. Under the default IF_CHANGED the box is the LAST child of a vertical stack,
// so once the labels above it stop changing height its slot stops changing too and its
// producer is served from the arrange cache -- the reproducer would stop reproducing.

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/layout-controller.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>

#include <iostream>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr int ARRANGE_LIMIT = 30; ///< Mutation stops here so the settle path is observable too.

int   gArrangeCount = 0;
bool  gMutating     = true;
Label gCounterLabel; ///< The label whose text is rewritten from inside the arrange pass.

Label MakeLabel(const Dali::String& text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}

// The in-pass producer. Runs inside the box's arrange; the SetText() below
// invalidates the label's measure while the pass is on the stack, so the
// resulting layout work is PARKED (retained, no self wake).
LayoutRect BoxArrange(View /*view*/, const LayoutRect& bounds)
{
  ++gArrangeCount;
  if(gMutating && gCounterLabel)
  {
    const std::string text = "arrange #" + std::to_string(gArrangeCount) +
                             " - parked; touch to run the next pass";
    gCounterLabel.SetText(Dali::String(text.c_str()));
    std::cout << "arrange #" << gArrangeCount
              << ": label text changed in-pass -> invalidation parked, no idle wake" << std::endl;
    if(gArrangeCount >= ARRANGE_LIMIT)
    {
      gMutating = false;
      std::cout << "producer stopped mutating; the next event settles the layout" << std::endl;
    }
  }
  return bounds;
}
} // namespace

class LayoutInvalidationParkExample : public ConnectionTracker
{
public:
  explicit LayoutInvalidationParkExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &LayoutInvalidationParkExample::Create);
  }

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);
    window.KeyEventSignal().Connect(this, &LayoutInvalidationParkExample::OnKeyEvent);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(12.0f);
    root.SetPadding(Insets(24.0f, 24.0f, 24.0f, 24.0f));

    root.Add(MakeLabel("Parked in-pass invalidation", 24.0f, 0x202124u));
    root.Add(MakeLabel("Every arrange rewrites the label below from OnArrange.", 14.0f, 0x5F6368u));
    root.Add(MakeLabel("Watch: app idles between touches; one pass per touch.", 14.0f, 0x5F6368u));

    gCounterLabel = MakeLabel("arrange #0 - waiting for the first pass", 18.0f, 0x0B57D0u);
    root.Add(gCounterLabel);

    // The producer view. Its ArrangeCallback replaces its OnArrange, which is
    // fine for a childless box; the label it mutates is a SIBLING, so the
    // mutation is an in-pass invalidation of another view.
    View box = View::New();
    box.SetRequestedWidth(MATCH_PARENT);
    box.SetRequestedHeight(80.0f);
    box.SetBackgroundColor(UiColor(0xE8F0FEu));
    box.SetArrangeCallback(ArrangeCallback::New(&BoxArrange), ArrangePolicy::ALWAYS);
    root.Add(box);

    window.Add(root);

    LayoutController::Get(window).LayoutFinishedSignal().Connect(this, &LayoutInvalidationParkExample::OnLayoutFinished);
  }

  void OnLayoutFinished(Window /*window*/)
  {
    // Starved while the producer kept re-invalidating; fires once the layout
    // finally settles (after the mutation stops and one more event drains).
    std::cout << "LayoutFinished: layout settled after " << gArrangeCount << " arranges" << std::endl;
  }

  void OnKeyEvent(Window /*window*/, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application& mApplication;
};

int main(int argc, char** argv)
{
  Application                   application = Application::New(&argc, &argv);
  LayoutInvalidationParkExample example(application);
  application.MainLoop();
  return 0;
}
