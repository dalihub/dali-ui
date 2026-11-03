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
#include <dali.h>
#include <dali/devel-api/animation/animation-devel.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <set>
#include <string>
#include <vector>

// Install the callback mock before production headers.
#include <dali-ui/ui-event-thread-callback.h>

// Load task types before other headers introduce Internal::Text.
#include <dali-ui-foundation/internal/text/async-text/async-text-manager-impl.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/extension-api/layout-scale-manager.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/text/text-visual.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/gradient/linear-gradient.h>
#include <dali-ui-foundation/public-api/text/label-properties.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-async-task-manager.h>

using namespace Dali;

namespace
{
namespace Text                      = Ui::Text;
using Task                          = Ui::Internal::TextLoadingTaskPtr;
constexpr const char* LONG_TEXT     = "A complete marquee loop must survive pending asynchronous text publication and layout changes";
constexpr const char* VERTICAL_TEXT = "First long line\nSecond long line\nThird long line\nFourth long line\nFifth long line\nSixth long line";

void UpdateCore(UiTestApplication& application, unsigned milliseconds)
{
  // Lifecycle, animation and uniform assertions need the real update/event
  // pipeline, but do not need draw calls or graphics-program reflection.
  application.UpdateOnly(milliseconds);
  // Retire updated-property flags and discarded resources even when drawing
  // is skipped. Leaving these queued across updates can retain stale owners.
  application.GetCore().PostRender();
}

// Capture the real loader tasks and drive the real core animation clock. No
// worker sleeps, polling retries, private state writes or fabricated results.
struct Lifecycle
{
  UiTestApplication&    application;
  ConnectionTracker     tracker;
  Actor                 parent;
  Ui::Label             label;
  WeakHandle<Animation> latestAnimation;
  unsigned              completed         = 0u;
  unsigned              finished          = 0u;
  unsigned              animationsCreated = 0u;
  bool                  asynchronous;
  bool                  trackMarqueeAnimations = true;

  explicit Lifecycle(UiTestApplication& app, bool async = true, bool vertical = false, bool automatic = false, int loops = 0)
  : application(app),
    asynchronous(async)
  {
    Test::AsyncTaskManager::SetTaskCaptureEnabled(true);
    application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&tracker, [&](BaseHandle handle)
    {
      Animation animation = Animation::DownCast(handle);
      if(animation && trackMarqueeAnimations)
      {
        latestAnimation = animation;
        ++animationsCreated;
        animation.FinishedSignal().Connect(&tracker, [&](Animation)
        { ++finished; });
      }
    });
    parent = Actor::New();
    label  = Ui::Label::New(vertical ? VERTICAL_TEXT : LONG_TEXT);
    label.SetRequestedWidth(180.0f);
    label.SetRequestedHeight(48.0f);
    label.SetFontSize(20.0f);
    label.SetMultiLine(vertical);
    label.SetMarqueeOrientation(vertical ? Text::MarqueeOrientation::VERTICAL : Text::MarqueeOrientation::HORIZONTAL);
    label.SetMarqueeLoopCount(loops);
    label.SetMarqueeLoopDelay(0.0f);
    label.SetMarqueeGap(0);
    label.SetMarqueeSpeed(200);
    label.SetMarqueeStopMode(Text::MarqueeStopMode::FINISH_LOOP);
    label.SetMarqueeTriggerPolicy(automatic ? Text::MarqueeTriggerPolicy::ON_OVERFLOW : Text::MarqueeTriggerPolicy::MANUAL);
    label.SetAsyncRendering(async);
    label.AsyncRenderFinishedSignal().Connect(&tracker, [&](Ui::View, float, float)
    { ++completed; });
    parent.Add(label);
    application.GetScene().Add(parent);
    Drain();
  }

  ~Lifecycle()
  {
    tracker.DisconnectAll();
    if(label) label.Unparent();
    label.Reset();
    parent.Unparent();
    parent.Reset();
    Frame();
    Test::AsyncTaskManager::SetTaskCaptureEnabled(false);
  }

  void Frame(unsigned milliseconds = 0u)
  {
    application.SendNotification();
    UpdateCore(application, milliseconds);
    application.SendNotification();
  }

  std::vector<Task> Take()
  {
    std::vector<Task> output;
    for(auto& task : Test::AsyncTaskManager::TakeCapturedTasks())
    {
      output.emplace_back(static_cast<Ui::Internal::TextLoadingTask*>(task.Get()));
    }
    return output;
  }

  Task Dispatch()
  {
    Frame();
    auto tasks = Take();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    return tasks.empty() ? Task{} : tasks.front();
  }

  void Deliver(Task task)
  {
    DALI_TEST_CHECK(task);
    if(task)
    {
      auto manager = Text::AsyncTextManager::Get();
      Text::GetImplementation(manager).LoadComplete(task);
    }
  }

  void Complete(Task task)
  {
    DALI_TEST_CHECK(task && task->IsReady());
    if(task && task->IsReady())
    {
      task->Process();
      Deliver(task);
    }
  }

  void Drain()
  {
    // Bounded event/relayout quiescence, not a retry of a failing assertion.
    for(unsigned round = 0u; round < 8u; ++round)
    {
      Frame();
      auto tasks = Take();
      if(tasks.empty()) return;
      for(auto task : tasks) Complete(task);
    }
    tet_printf("Marquee did not reach event-thread quiescence in eight relayouts");
    tet_result(TET_FAIL);
  }

  Animation Current()
  {
    auto animation = latestAnimation.GetHandle();
    DALI_TEST_CHECK(animation);
    return animation;
  }

  void Start()
  {
    label.StartMarquee();
    Drain();
    DALI_TEST_CHECK(label.IsMarqueeRunning());
  }

  void Advance(unsigned milliseconds)
  {
    // Core counts at most one loop boundary per update. Bound each step by
    // the animation's period, so render work depends on the number of loops
    // rather than on font-dependent text width or machine rendering speed.
    while(milliseconds)
    {
      auto     animation = latestAnimation.GetHandle();
      unsigned step      = milliseconds;
      if(animation && animation.GetState() == Animation::PLAYING)
      {
        const unsigned period = std::max(1u, static_cast<unsigned>(animation.GetDuration() * 1000.0f));
        step                  = std::min(step, period);
      }
      Frame(step);
      milliseconds -= step;
    }
  }

  unsigned DurationMs()
  {
    return static_cast<unsigned>(std::ceil(Current().GetDuration() * 1000.0f));
  }

  void CheckTexture()
  {
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    if(label.GetRendererCount())
    {
      auto textures = label.GetRendererAt(0u).GetTextures();
      DALI_TEST_CHECK(textures && textures.GetTextureCount() && textures.GetTexture(0u));
    }
  }
};

void FinishLoopAt(UiTestApplication& application, unsigned phase, bool async, bool vertical, int loops)
{
  tet_printf("finish-loop phase=%u async=%d vertical=%d loops=%d", phase, async, vertical, loops);
  Lifecycle test(application, async, vertical, false, loops);
  test.Start();
  Animation      animation  = test.Current();
  auto           shader     = test.label.GetRendererAt(0u).GetShader();
  auto           textures   = test.label.GetRendererAt(0u).GetTextures();
  const unsigned duration   = test.DurationMs();
  const unsigned beforeStop = phase == 0u ? 1u : phase == 1u ? duration / 2u
                                               : phase == 2u ? duration - 2u
                                                             : duration + 2u;
  test.Advance(beforeStop);
  const float    progress        = animation.GetCurrentProgress();
  const unsigned beforeCompleted = test.completed;
  test.label.StopMarquee();
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  DALI_TEST_CHECK(test.Current() == animation);
  DALI_TEST_EQUALS(animation.GetCurrentProgress(), progress, 0.0001f, TEST_LOCATION);
  test.Frame();
  DALI_TEST_CHECK(test.Take().empty());
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() == shader);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == textures);
  DALI_TEST_EQUALS(test.completed, beforeCompleted, TEST_LOCATION);
  // Check both sides of the actual boundary instead of polling every simulated
  // millisecond. Progress and duration come from Core, not platform font metrics.
  const unsigned remaining = static_cast<unsigned>(std::ceil((1.0f - progress) * animation.GetDuration() * 1000.0f));
  if(remaining > 1u)
  {
    test.Advance(remaining - 1u);
    DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() == shader);
    DALI_TEST_EQUALS(test.completed, beforeCompleted, TEST_LOCATION);
  }
  test.Advance(2u);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, beforeCompleted + (async ? 1u : 0u), TEST_LOCATION);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() != shader);
  test.CheckTexture();
  const auto created = test.animationsCreated;
  test.Advance(20u);
  test.Drain();
  DALI_TEST_EQUALS(test.animationsCreated, created, TEST_LOCATION);
}
} // namespace

int UtcDaliMarqueeLifecycleFinishLoopBoundariesSyncHorizontalP(void)
{
  UiTestApplication application;
  for(unsigned phase = 0u; phase < 4u; ++phase) FinishLoopAt(application, phase, false, false, 0);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopBoundariesSyncVerticalP(void)
{
  UiTestApplication application;
  for(unsigned phase = 0u; phase < 4u; ++phase) FinishLoopAt(application, phase, false, true, 0);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopBoundariesAsyncHorizontalP(void)
{
  UiTestApplication application;
  for(unsigned phase = 0u; phase < 4u; ++phase) FinishLoopAt(application, phase, true, false, 0);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopBoundariesAsyncVerticalP(void)
{
  UiTestApplication application;
  for(unsigned phase = 0u; phase < 4u; ++phase) FinishLoopAt(application, phase, true, true, 0);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopFiniteP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
    for(bool vertical : {false, true}) FinishLoopAt(application, 3u, async, vertical, 3);
  END_TEST;
}

namespace
{
void CheckFiniteLoops(UiTestApplication& application, bool async, bool vertical)
{
  for(int loops : {0, 1, 2, 3, 32})
  {
    Lifecycle test(application, async, vertical, false, loops);
    test.Start();
    Animation      animation  = test.Current();
    const auto     before     = test.completed;
    const unsigned duration   = test.DurationMs();
    const int      boundaries = loops ? loops : 4;
    for(int loop = 1; loop <= boundaries; ++loop)
    {
      test.Advance(duration + 1u);
      tet_printf("finite async=%d vertical=%d configured=%d boundary=%d current=%d progress=%.4f", async, vertical, loops, loop, animation.GetCurrentLoop(), animation.GetCurrentProgress());
      DALI_TEST_EQUALS(animation.GetCurrentLoop(), loops ? loop : 0, TEST_LOCATION);
      DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), !loops || loop < loops, TEST_LOCATION);
      DALI_TEST_EQUALS(test.finished, loops && loop == loops ? 1u : 0u, TEST_LOCATION);
    }
    test.Drain();
    DALI_TEST_EQUALS(test.completed, before + (async && loops ? 1u : 0u), TEST_LOCATION);
    test.CheckTexture();
    if(loops)
    {
      const auto created = test.animationsCreated;
      test.Drain();
      DALI_TEST_EQUALS(test.animationsCreated, created, TEST_LOCATION);
      test.label.SetMarqueeLoopCount(2);
      test.Start();
      DALI_TEST_EQUALS(test.Current().GetLoopCount(), 2, TEST_LOCATION);
      DALI_TEST_EQUALS(test.label.GetMarqueeLoopCount(), 2, TEST_LOCATION);
    }
  }
}
} // namespace

int UtcDaliMarqueeLifecycleFiniteLoopsSyncHorizontalP(void)
{
  UiTestApplication application;
  CheckFiniteLoops(application, false, false);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFiniteLoopsSyncVerticalP(void)
{
  UiTestApplication application;
  CheckFiniteLoops(application, false, true);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFiniteLoopsAsyncHorizontalP(void)
{
  UiTestApplication application;
  CheckFiniteLoops(application, true, false);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFiniteLoopsAsyncVerticalP(void)
{
  UiTestApplication application;
  CheckFiniteLoops(application, true, true);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFiniteRepeatedPublicationP(void)
{
  UiTestApplication application;
  for(bool vertical : {false, true})
  {
    Lifecycle test(application, true, vertical, false, 3);
    test.Start();
    test.Advance(test.DurationMs() + 20u);
    DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 1, TEST_LOCATION);
    test.label.SetRequestedWidth(170.0f);
    test.Drain();
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), 2, TEST_LOCATION);
    test.label.SetRequestedWidth(160.0f);
    test.Drain();
    // Each replacement uses the configured count and the preceding animation's local count.
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), 3, TEST_LOCATION);
    test.Advance(test.DurationMs() + 1u);
    test.label.SetRequestedWidth(150.0f);
    test.Drain();
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), 2, TEST_LOCATION);
    test.Advance(test.DurationMs() + 1u);
    test.Advance(test.DurationMs() + 1u);
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(test.label.GetMarqueeLoopCount(), 3, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliMarqueeLifecycleFiniteCompletionCrossingP(void)
{
  UiTestApplication application;
  for(bool automatic : {false, true})
    for(bool deliverBeforeFinish : {false, true})
    {
      Lifecycle test(application, true, false, automatic, 2);
      test.Start();
      test.Advance(test.DurationMs() + 20u);
      DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 1, TEST_LOCATION);
      test.label.SetText("Updated content during the final loop must not replenish completed loops");
      Task task = test.Dispatch();
      DALI_TEST_CHECK(task);
      if(!task) continue;
      task->Process();
      const auto completed = test.completed;
      if(deliverBeforeFinish)
      {
        test.Deliver(task);
        test.Frame();
        DALI_TEST_EQUALS(test.Current().GetLoopCount(), 1, TEST_LOCATION);
      }
      test.Advance(test.DurationMs() + 5u);
      if(!deliverBeforeFinish) test.Deliver(task);
      test.Drain();
      DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
      DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
      DALI_TEST_EQUALS(test.completed, completed + (deliverBeforeFinish ? 2u : 1u), TEST_LOCATION);
      const auto created = test.animationsCreated;
      test.Deliver(task);
      test.Advance(50u);
      test.Drain();
      DALI_TEST_EQUALS(test.animationsCreated, created, TEST_LOCATION);
    }
  END_TEST;
}

int UtcDaliMarqueeLifecycleRuntimeLoopConfigurationP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
  {
    Lifecycle test(application, async, false, false, 3);
    test.Start();
    auto original = test.Current();
    test.label.SetMarqueeLoopCount(2);
    test.Drain();
    DALI_TEST_CHECK(test.Current() == original);
    DALI_TEST_EQUALS(original.GetLoopCount(), 3, TEST_LOCATION);
    DALI_TEST_EQUALS(test.label.GetMarqueeLoopCount(), 2, TEST_LOCATION);
    test.label.SetMarqueeLoopCount(-1);
    DALI_TEST_EQUALS(test.label.GetMarqueeLoopCount(), 2, TEST_LOCATION);
    test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
    test.label.StopMarquee();
    test.Drain();
    test.Start();
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), 2, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliMarqueeLifecycleRuntimeLoopChangeAtPublicationP(void)
{
  UiTestApplication application;
  for(int configured : {0, 5})
  {
    Lifecycle test(application, true, false, false, 3);
    test.Start();
    test.Advance(test.DurationMs() + 20u);
    DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 1, TEST_LOCATION);
    test.label.SetMarqueeLoopCount(configured);
    // The existing setter leaves the playing animation alone. The next
    // content publication does consume the new configured loop count.
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), 3, TEST_LOCATION);
    test.label.SetRequestedWidth(160.0f);
    test.Drain();
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), configured ? configured - 1 : 0, TEST_LOCATION);
  }
  END_TEST;
}

namespace
{
void CheckFinishLoopCompletionOrder(bool automatic, bool vertical)
{
  UiTestApplication application;
  for(unsigned when = 0u; when < 5u; ++when)
  {
    Lifecycle test(application, true, vertical, automatic);
    test.Start();
    test.Advance(test.DurationMs() / 3u);
    test.label.SetText(when % 2u ? "Ab" : "Updated long asynchronous marquee text with several words\nSecond line\nThird line\nFourth line");
    Task pending = test.Dispatch();
    DALI_TEST_CHECK(pending);
    if(!pending) continue;
    pending->Process();
    if(when == 0u)
    {
      test.Deliver(pending);
      test.Frame();
    }
    Animation  moving = test.Current();
    const auto shader = test.label.GetRendererAt(0u).GetShader();
    const auto count  = test.completed;
    test.label.StopMarquee();
    const unsigned duration = test.DurationMs();
    if(when == 1u) test.Deliver(pending);
    if(when == 2u)
    {
      test.Advance(duration / 8u);
      test.Deliver(pending);
    }
    if(when == 3u)
    {
      const unsigned remaining = static_cast<unsigned>((1.0f - moving.GetCurrentProgress()) * duration);
      if(remaining > 3u) test.Advance(remaining - 3u);
      test.Deliver(pending);
    }
    if(when != 4u)
    {
      DALI_TEST_CHECK(test.label.IsMarqueeRunning());
      DALI_TEST_CHECK(test.Current() == moving);
      DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() == shader);
      DALI_TEST_EQUALS(test.completed, count, TEST_LOCATION);
    }
    test.Advance(duration + 5u);
    if(when == 4u) test.Deliver(pending);
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(test.completed, count + 1u, TEST_LOCATION);
    auto finalTexture = test.label.GetRendererAt(0u).GetTextures();
    test.Deliver(pending);
    test.Frame();
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == finalTexture);
    DALI_TEST_EQUALS(test.completed, count + 1u, TEST_LOCATION);
  }
}
} // namespace

int UtcDaliMarqueeLifecycleFinishLoopCompletionOrderManualHorizontalP(void)
{
  CheckFinishLoopCompletionOrder(false, false);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopCompletionOrderManualVerticalP(void)
{
  CheckFinishLoopCompletionOrder(false, true);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopCompletionOrderAutomaticHorizontalP(void)
{
  CheckFinishLoopCompletionOrder(true, false);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopCompletionOrderAutomaticVerticalP(void)
{
  CheckFinishLoopCompletionOrder(true, true);
  END_TEST;
}

namespace
{
void CheckFinishLoopAdditionalRequests(bool async, bool automatic)
{
  UiTestApplication application;
  for(unsigned change = 0u; change < 6u; ++change)
  {
    Lifecycle test(application, async, false, automatic);
    test.Start();
    test.Advance(test.DurationMs() / 4u);
    const auto moving = test.Current();
    const auto shader = test.label.GetRendererAt(0u).GetShader();
    test.label.StopMarquee();
    switch(change)
    {
      case 0u:
        test.label.StartMarquee();
        break;
      case 1u:
        test.label.StopMarquee();
        break;
      case 2u:
        test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
        test.label.StopMarquee();
        break;
      case 3u:
        test.label.SetText("Ab");
        break;
      case 4u:
        test.label.SetRequestedWidth(150.0f);
        break;
      case 5u:
        test.label.SetProperty(Actor::Property::VISIBLE, false);
        break;
    }
    test.Drain();
    const bool immediate = change == 2u || change == 5u;
    DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), !immediate, TEST_LOCATION);
    if(!immediate)
    {
      DALI_TEST_CHECK(test.Current() == moving);
      DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() == shader);
      test.Advance(test.DurationMs() + 5u);
      test.Drain();
      DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
      DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
    }
    // Existing Start-while-pending semantics leave FINISH_LOOP in force;
    // a new explicit Start after completion begins the next run.
    test.label.SetProperty(Actor::Property::VISIBLE, true);
    test.label.SetText(LONG_TEXT);
    test.Start();
  }
}
} // namespace

int UtcDaliMarqueeLifecycleFinishLoopAdditionalRequestsSyncManualP(void)
{
  CheckFinishLoopAdditionalRequests(false, false);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopAdditionalRequestsSyncAutomaticP(void)
{
  CheckFinishLoopAdditionalRequests(false, true);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopAdditionalRequestsAsyncManualP(void)
{
  CheckFinishLoopAdditionalRequests(true, false);
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopAdditionalRequestsAsyncAutomaticP(void)
{
  CheckFinishLoopAdditionalRequests(true, true);
  END_TEST;
}

int UtcDaliMarqueeLifecycleOverflowTransitionsP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
    for(bool vertical : {false, true})
    {
      Lifecycle test(application, async, vertical, true);
      DALI_TEST_CHECK(test.label.IsMarqueeRunning());
      for(unsigned cycle = 0u; cycle < 3u; ++cycle)
      {
        test.label.SetText("Ab");
        test.Drain();
        DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
        test.CheckTexture();
        test.label.SetText(vertical ? VERTICAL_TEXT : LONG_TEXT);
        test.Drain();
        DALI_TEST_CHECK(test.label.IsMarqueeRunning());
        const auto animation = test.Current();
        const auto created   = test.animationsCreated;
        test.Advance(100u);
        test.Drain();
        DALI_TEST_CHECK(test.Current() == animation);
        DALI_TEST_EQUALS(test.animationsCreated, created, TEST_LOCATION);
      }
      test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
      test.label.StopMarquee();
      test.Drain();
      DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
      test.label.SetRequestedWidth(160.0f);
      test.Drain();
      DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    }
  END_TEST;
}

int UtcDaliMarqueeLifecycleOverflowPropertyReevaluationP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
    for(bool vertical : {false, true})
    {
      Lifecycle test(application, async, vertical, true);
      for(unsigned property = 0u; property < 8u; ++property)
      {
        if(property < 4u)
        {
          // Size/font changes lift ON_OVERFLOW suppression; style-only changes
          // are tested while running because they do not promise a restart.
          test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
          test.label.StopMarquee();
          test.Drain();
          DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
        }
        switch(property)
        {
          case 0u:
            test.label.SetRequestedWidth(140.0f);
            break;
          case 1u:
            test.label.SetRequestedHeight(40.0f);
            break;
          case 2u:
            test.label.SetFontSize(22.0f);
            break;
          case 3u:
            test.label.SetFontFamily("DejaVu Sans");
            break;
          case 4u:
            test.label.SetHorizontalTextAlignment(Text::Alignment::END);
            break;
          case 5u:
            test.label.SetTextOverflowMode(Text::OverflowMode::CLIP);
            break;
          case 6u:
            test.label.SetRenderScale(1.5f);
            break;
          case 7u:
            test.label.SetTextFit(Text::Fit::Range(18.0f, 22.0f, 2.0f));
            break;
        }
        test.Drain();
        tet_printf("overflow reevaluation async=%d vertical=%d property=%u running=%d", async, vertical, property, test.label.IsMarqueeRunning());
        DALI_TEST_CHECK(test.label.IsMarqueeRunning());
        test.CheckTexture();
      }
      if(vertical)
        test.label.SetRequestedHeight(2000.0f);
      else
        test.label.SetRequestedWidth(2000.0f);
      test.Drain();
      DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
      if(vertical)
        test.label.SetRequestedHeight(40.0f);
      else
        test.label.SetRequestedWidth(140.0f);
      test.Drain();
      DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    }
  END_TEST;
}

int UtcDaliMarqueeLifecycleGradientFinishLoopP(void)
{
  UiTestApplication application;
  const char*       sources[] = {LONG_TEXT, "مرحبا بالعالم هذا نص طويل لاختبار التمرير وإيقاف الحركة بعد الحلقة", "Marquee مرحبا 123 mixed direction text with a long final English section"};
  for(unsigned variant = 0u; variant < 12u; ++variant)
  {
    const bool  vertical = variant % 2u;
    Lifecycle   test(application, variant % 3u != 0u, vertical);
    std::string source = sources[variant % 3u];
    if(vertical) source += "\nSecond line\nThird line\nFourth line\nFifth line";
    test.label.SetText(source.c_str());
    Ui::Gradient::Linear gradient(Vector2::ZERO, variant % 4u == 0u ? Vector2(140.0f, 0.0f) : Vector2::XAXIS);
    gradient.SetUnits(variant % 4u == 0u ? Ui::Gradient::Units::USER_SPACE : Ui::Gradient::Units::OBJECT_BOUNDING_BOX);
    gradient.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(Color::RED)), Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
    test.label.SetTextGradient(gradient);
    if(variant % 4u >= 2u) test.label.SetTextGradientOverlay(gradient);
    test.label.SetRenderScale(variant % 3u == 0u ? 1.0f : 1.5f);
    test.Start();
    test.Advance(test.DurationMs() / 3u);
    auto       moving   = test.Current();
    const auto shader   = test.label.GetRendererAt(0u).GetShader();
    const auto textures = test.label.GetRendererAt(0u).GetTextures();
    test.label.StopMarquee();
    test.label.SetRequestedWidth(160.0f);
    test.Drain();
    DALI_TEST_CHECK(test.Current() == moving);
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() == shader);
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == textures);
    test.Advance(test.DurationMs() + 5u);
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    test.CheckTexture();
    test.Start();
  }
  END_TEST;
}

int UtcDaliMarqueeLifecycleVisibilityAndSceneP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
    for(bool automatic : {false, true})
      for(unsigned change = 0u; change < 3u; ++change)
      {
        Lifecycle test(application, async, false, automatic);
        test.Start();
        test.Advance(100u);
        if(change == 0u) test.label.SetProperty(Actor::Property::VISIBLE, false);
        if(change == 1u) test.parent.SetProperty(Actor::Property::VISIBLE, false);
        if(change == 2u) test.label.Unparent();
        test.Drain();
        // Label subscribes to OnSceneVisibilityChangedSignal, so detachment
        // follows the same immediate retirement path as hiding the label.
        DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
        test.label.SetRequestedWidth(140.0f);
        test.label.SetText("A fresh long source requested while hidden or disconnected");
        test.Drain();
        if(change == 0u) test.label.SetProperty(Actor::Property::VISIBLE, true);
        if(change == 1u) test.parent.SetProperty(Actor::Property::VISIBLE, true);
        if(change == 2u) test.parent.Add(test.label);
        test.Drain();
        DALI_TEST_CHECK(test.label.IsMarqueeRunning());
        test.CheckTexture();
      }
  END_TEST;
}

int UtcDaliMarqueeLifecycleFinishLoopSceneReconnectP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
    for(bool automatic : {false, true})
      for(bool finishDetached : {false, true})
      {
        Lifecycle test(application, async, false, automatic);
        test.Start();
        const unsigned duration = test.DurationMs();
        test.Advance(duration / 4u);
        const auto moving = test.Current();
        test.label.StopMarquee();
        test.label.Unparent();
        test.Drain();
        // OnSceneVisibilityChanged(false) explicitly uses IMMEDIATE even when
        // FINISH_LOOP was pending. Preserve that established scene contract.
        DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
        DALI_TEST_EQUALS(moving.GetState(), Animation::STOPPED, TEST_LOCATION);
        DALI_TEST_CHECK(test.label.GetMarqueeStopMode() == Text::MarqueeStopMode::FINISH_LOOP);
        test.Advance(finishDetached ? duration : duration / 4u);
        test.parent.Add(test.label);
        test.Drain();
        // ON_OVERFLOW reevaluates on visibility restoration. An explicitly
        // stopped MANUAL run stays stopped until another Start request.
        DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), automatic, TEST_LOCATION);
        DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
        test.CheckTexture();
      }
  END_TEST;
}

int UtcDaliMarqueeLifecycleNaturalFinishVisibilityP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
    for(bool automatic : {false, true})
    {
      Lifecycle test(application, async, false, automatic, 1);
      test.Start();
      test.Advance(test.DurationMs() + 5u);
      test.Drain();
      DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
      test.label.SetProperty(Actor::Property::VISIBLE, false);
      test.Drain();
      test.label.SetProperty(Actor::Property::VISIBLE, true);
      test.Drain();
      DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), automatic, TEST_LOCATION);
      test.Start();
    }
  END_TEST;
}

int UtcDaliMarqueeLifecycleAsyncSyncModeCrossingP(void)
{
  UiTestApplication application;
  for(bool vertical : {false, true})
  {
    Lifecycle test(application, true, vertical);
    test.Start();
    test.label.SetRequestedWidth(160.0f);
    Task pending = test.Dispatch();
    DALI_TEST_CHECK(pending);
    if(!pending) continue;
    pending->Process();
    const auto completed = test.completed;
    test.label.SetAsyncRendering(false);
    test.Drain();
    const auto currentTexture = test.label.GetRendererAt(0u).GetTextures();
    test.Deliver(pending);
    DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == currentTexture);
    DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    test.label.SetAsyncRendering(true);
    test.Drain();
    DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
    test.CheckTexture();
  }
  END_TEST;
}

int UtcDaliMarqueeLifecycleHiddenCompletionP(void)
{
  UiTestApplication application;
  for(bool automatic : {false, true})
    for(unsigned change = 0u; change < 3u; ++change)
    {
      Lifecycle test(application, true, false, automatic);
      test.Start();
      test.label.SetText("Pending result while the label becomes hidden, with enough text to overflow");
      Task task = test.Dispatch();
      DALI_TEST_CHECK(task);
      if(!task) continue;
      task->Process();
      if(change == 0u) test.label.SetProperty(Actor::Property::VISIBLE, false);
      if(change == 1u) test.parent.SetProperty(Actor::Property::VISIBLE, false);
      if(change == 2u) test.label.Unparent();
      test.Deliver(task);
      test.Drain();
      if(change != 2u)
      {
        DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
      }
      if(change == 0u) test.label.SetProperty(Actor::Property::VISIBLE, true);
      if(change == 1u) test.parent.SetProperty(Actor::Property::VISIBLE, true);
      if(change == 2u) test.parent.Add(test.label);
      test.Drain();
      DALI_TEST_CHECK(test.label.IsMarqueeRunning());
      test.CheckTexture();
    }
  END_TEST;
}

int UtcDaliMarqueeLifecycleManualDispatchEntryPointsP(void)
{
  UiTestApplication application;
  Lifecycle         test(application);
  auto&             impl   = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  using Request            = void (Ui::Integration::LabelImpl::*)(float, float);
  const Request requests[] = {&Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedSize,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedWidth,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedHeight,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithConstraints};
  for(auto request : requests)
  {
    const auto count = test.completed;
    (impl.*request)(160.0f, 48.0f);
    // No notification/relayout occurs between the API call and this check.
    auto tasks = test.Take();
    DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(test.completed, count, TEST_LOCATION);
    if(!tasks.empty()) test.Complete(tasks.front());
    DALI_TEST_EQUALS(test.completed, count + 1u, TEST_LOCATION);
    test.Drain();
    DALI_TEST_EQUALS(test.completed, count + 1u, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliMarqueeLifecycleManualShaderReentryOwnershipP(void)
{
  UiTestApplication application;
  Lifecycle         test(application);
  auto&             impl      = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  const auto        shader    = test.label.GetRendererAt(0u).GetShader();
  const auto        textures  = test.label.GetRendererAt(0u).GetTextures();
  const auto        completed = test.completed;
  bool              fired     = false;
  std::vector<Task> nested;
  test.application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&test.tracker, [&](BaseHandle object)
  {
    if(!fired && Shader::DownCast(object))
    {
      fired = true;
      // This manual request changes ownership without a text/property change.
      impl.RequestAsyncRenderWithFixedSize(130.0f, 48.0f);
      nested = test.Take();
    }
  });
  Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2::XAXIS);
  gradient.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(Color::RED)), Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
  test.label.SetTextGradient(gradient); // Cold ordinary text shader variant.
  Task first = test.Dispatch();
  test.Complete(first);
  DALI_TEST_CHECK(fired);
  DALI_TEST_EQUALS(nested.size(), 1u, TEST_LOCATION);
  const bool previousPublicationPreserved = test.completed == completed &&
                                            test.label.GetRendererAt(0u).GetShader() == shader &&
                                            test.label.GetRendererAt(0u).GetTextures() == textures;
  // Release the nested loader before a failing assertion unwinds the fixture.
  if(!nested.empty()) test.Complete(nested.front());
  test.Drain();
  DALI_TEST_CHECK(previousPublicationPreserved);
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  test.CheckTexture();
  END_TEST;
}

int UtcDaliMarqueeLifecycleCompletionIntentReentryP(void)
{
  UiTestApplication application;
  for(unsigned intent = 0u; intent < 3u; ++intent)
  {
    Lifecycle test(application);
    bool      fired = false;
    test.label.AsyncRenderFinishedSignal().Connect(&test.tracker, [&](Ui::View, float, float)
    {
      if(!fired)
      {
        fired = true;
        if(intent == 0u)
        {
          test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
          test.label.StopMarquee();
        }
        if(intent == 1u) test.label.StopMarquee();
        if(intent == 2u) test.label.SetProperty(Actor::Property::VISIBLE, false);
      }
    });
    test.label.StartMarquee();
    test.Drain();
    DALI_TEST_CHECK(fired);
    DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), intent == 1u, TEST_LOCATION);
    if(intent == 1u)
    {
      test.Advance(test.DurationMs() + 5u);
      test.Drain();
    }
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    test.label.SetProperty(Actor::Property::VISIBLE, true);
    test.Start();
  }
  END_TEST;
}

int UtcDaliMarqueeLifecycleEmptyAndZeroSizeP(void)
{
  UiTestApplication application;
  for(bool automatic : {false, true})
    for(bool zeroSize : {false, true})
    {
      Lifecycle test(application, true, false, automatic);
      test.Start();
      test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
      test.label.StopMarquee();
      test.Drain();
      const auto completed = test.completed;
      if(zeroSize)
        test.label.SetRequestedWidth(0.0f);
      else
        test.label.SetText("");
      test.Drain();
      DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
      DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
      DALI_TEST_CHECK(test.Take().empty());
      test.label.SetRequestedWidth(180.0f);
      test.label.SetText(LONG_TEXT);
      test.Start();
    }
  END_TEST;
}

int UtcDaliMarqueeLifecycleResourceRetirementP(void)
{
  UiTestApplication application;
  for(bool async : {false, true})
    for(unsigned finish = 0u; finish < 4u; ++finish)
    {
      Lifecycle test(application, async, false, false, finish == 0u ? 1 : 0);
      test.Start();
      WeakHandle<Animation> animation = test.latestAnimation;
      WeakHandle<Ui::Label> label(test.label);
      if(finish == 0u) test.Advance(test.DurationMs() + 5u);
      if(finish == 1u)
      {
        test.label.StopMarquee();
        test.Advance(test.DurationMs() + 5u);
      }
      if(finish == 2u)
      {
        test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
        test.label.StopMarquee();
      }
      if(finish == 3u) test.label.StopMarquee(); // destruction while FINISH_LOOP is pending
      test.Drain();
      tet_printf("resources async=%d finish=%u animations_before_destroy=%u running=%d", async, finish, DevelAnimation::GetAnimationCount(), test.label.IsMarqueeRunning());
      test.label.Unparent();
      test.label.Reset();
      test.Frame(20u);
      DALI_TEST_CHECK(!label.GetHandle());
      DALI_TEST_CHECK(!animation.GetHandle());
      DALI_TEST_EQUALS(DevelAnimation::GetAnimationCount(), 0u, TEST_LOCATION);
    }
  END_TEST;
}

namespace
{
void CheckSeededTransitions(uint32_t seed)
{
  UiTestApplication application;
  Lifecycle test(application);
  uint32_t  state = seed;
  for(unsigned step = 0u; step < 80u; ++step)
  {
    state ^= state << 13u;
    state ^= state >> 17u;
    state ^= state << 5u;
    const unsigned action = state % 8u;
    tet_printf("seed=%u step=%u action=%u", seed, step, action);
    switch(action)
    {
      case 0u:
        test.label.StartMarquee();
        break;
      case 1u:
        test.label.SetMarqueeStopMode(Text::MarqueeStopMode::FINISH_LOOP);
        test.label.StopMarquee();
        break;
      case 2u:
        test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
        test.label.StopMarquee();
        break;
      case 3u:
        test.label.SetText(step % 2u ? LONG_TEXT : "Ab");
        break;
      case 4u:
        test.label.SetRequestedWidth(100.0f + (state % 200u));
        break;
      case 5u:
        test.label.SetProperty(Actor::Property::VISIBLE, false);
        test.Drain();
        test.label.SetProperty(Actor::Property::VISIBLE, true);
        break;
      case 6u:
        test.label.SetMarqueeLoopCount(state % 4u);
        break;
      case 7u:
        test.Advance(50u);
        break;
    }
    test.Drain();
    test.CheckTexture();
    DALI_TEST_CHECK(DevelAnimation::GetAnimationCount() <= 1u);
  }
  // A reproducible terminal intent, after the randomized prefix, must win.
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.label.StopMarquee();
  test.label.SetText("Final static source");
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  const auto completed = test.completed;
  test.Advance(100u);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
}
} // namespace

int UtcDaliMarqueeLifecycleSeededTransitionsFirstP(void)
{
  CheckSeededTransitions(0x13579bdu);
  END_TEST;
}

int UtcDaliMarqueeLifecycleSeededTransitionsSecondP(void)
{
  CheckSeededTransitions(0x2468aceu);
  END_TEST;
}

int UtcDaliMarqueeLifecycleSeededTransitionsThirdP(void)
{
  CheckSeededTransitions(0xc0ffeeu);
  END_TEST;
}

int UtcDaliMarqueeLifecycleSeededTransitionsFourthP(void)
{
  CheckSeededTransitions(0x5eed1234u);
  END_TEST;
}

namespace
{
void ReplaceFiniteContent(Lifecycle& test, bool vertical, unsigned serial, int expected)
{
  auto previous = test.Current();
  test.label.SetRequestedWidth(150.0f + serial % 20u);
  std::string text = vertical ? VERTICAL_TEXT : LONG_TEXT;
  text += serial % 2u ? " revised" : " updated";
  test.label.SetText(text.c_str());
  test.Drain();
  DALI_TEST_CHECK(test.Current() != previous);
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), expected, TEST_LOCATION);
  DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(test.Current().GetCurrentProgress(), 0.0f, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
  test.CheckTexture();
}

// Replacement may reset the configured budget; a finite animation must still
// finish once publications stop. Explicit Stop is covered independently.
void FinitePublicationBudget(UiTestApplication& application, int loops, bool vertical, bool automatic = false, bool gradient = false)
{
  Lifecycle test(application, true, vertical, automatic, loops);
  if(gradient)
  {
    Ui::Gradient::Linear fill(Vector2::ZERO, Vector2(140.0f, 0.0f));
    fill.SetUnits(Ui::Gradient::Units::USER_SPACE);
    fill.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(Color::RED)), Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
    test.label.SetTextGradient(fill);
    test.label.SetTextGradientOverlay(fill);
    test.label.SetRenderScale(1.5f);
  }
  test.Start();
  unsigned serial = 0u;
  for(unsigned repeat = 0u; repeat < 3u; ++repeat)
  {
    test.Advance(test.DurationMs() / 3u);
    ReplaceFiniteContent(test, vertical, serial++, loops);
  }
  if(loops != 1)
  {
    test.Advance(test.DurationMs() + 1u);
    DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), loops ? 1 : 0, TEST_LOCATION);
    ReplaceFiniteContent(test, vertical, serial++, loops ? loops - 1 : 0);
    ReplaceFiniteContent(test, vertical, serial++, loops);
    ReplaceFiniteContent(test, vertical, serial++, loops);
  }
  // Publish during the last partial loop. The preceding animation has spent
  // loops-1, so the replacement has one loop left, with a fresh phase.
  if(loops)
  {
    for(int loop = 1; loop < loops; ++loop) test.Advance(test.DurationMs() + 1u);
    test.Advance(test.DurationMs() / 3u);
    ReplaceFiniteContent(test, vertical, serial++, 1);
    auto last = test.Current();
    test.Advance(test.DurationMs() - 2u);
    DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
    test.Advance(4u);
    DALI_TEST_EQUALS(last.GetCurrentLoop(), 1, TEST_LOCATION);
    DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  }
  else
  {
    for(unsigned loop = 0u; loop < 4u; ++loop) test.Advance(test.DurationMs() + 1u);
    DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
  }
  const auto movingShader = test.label.GetRendererAt(0u).GetShader();
  test.Drain();
  if(loops)
  {
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() != movingShader);
  }
  test.CheckTexture();
  DALI_TEST_EQUALS(test.label.GetMarqueeLoopCount(), loops, TEST_LOCATION);
  if(loops)
  {
    test.Start();
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), loops, TEST_LOCATION);
    DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 0, TEST_LOCATION);
  }
}

void RuntimeFiniteChange(UiTestApplication& application, int from, int to, bool vertical, bool afterReplacement = false)
{
  Lifecycle test(application, true, vertical, false, from);
  test.Start();
  test.Advance(test.DurationMs() + 1u);
  if(afterReplacement) ReplaceFiniteContent(test, vertical, 0u, from - 1);
  // 5 -> 1 deliberately reduces below the already completed local count.
  if(from == 5 && to == 1) test.Advance(test.DurationMs() + 1u);
  auto      original       = test.Current();
  const int spent          = original.GetCurrentLoop();
  const int originalBudget = original.GetLoopCount();
  test.label.SetMarqueeLoopCount(to);
  test.Drain();
  DALI_TEST_CHECK(test.Current() == original);
  DALI_TEST_EQUALS(original.GetLoopCount(), originalBudget, TEST_LOCATION);
  const int expected = to ? std::max(1, to - spent) : 0;
  ReplaceFiniteContent(test, vertical, 1u, expected);
  // A second content replacement starts with the configured count.
  ReplaceFiniteContent(test, vertical, 2u, to);
  DALI_TEST_EQUALS(test.label.GetMarqueeLoopCount(), to, TEST_LOCATION);
  auto animation = test.Current();
  for(int i = 1; i <= (to ? to : 3); ++i)
  {
    test.Advance(test.DurationMs() + 1u);
    DALI_TEST_EQUALS(animation.GetCurrentLoop(), to ? i : 0, TEST_LOCATION);
    DALI_TEST_EQUALS(test.finished, to && i == to ? 1u : 0u, TEST_LOCATION);
  }
  test.Drain();
  DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), to == 0, TEST_LOCATION);
  test.CheckTexture();
}

void FinishedBeforePublication(UiTestApplication& application, bool vertical)
{
  Lifecycle test(application, true, vertical, false, 1);
  test.Start();
  test.label.SetRequestedWidth(160.0f);
  Task task = test.Dispatch();
  task->Process();
  auto original = test.Current();
  // Core has finished, but the event-thread FinishedSignal is still queued.
  UpdateCore(application, test.DurationMs() + 1u);
  DALI_TEST_EQUALS(original.GetCurrentLoop(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
  test.Deliver(task);
  test.Drain();
  // With no explicit Stop, content replacement can start another finite run.
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  DALI_TEST_CHECK(test.Current() != original);
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(test.animationsCreated, 2u, TEST_LOCATION);
  test.Advance(test.DurationMs() + 1u);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  test.CheckTexture();
}
} // namespace

int UtcDaliMarqueeLifecyclePublicationInfiniteHorizontalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 0, false);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationOneHorizontalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 1, false);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationTwoHorizontalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 2, false);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationThreeHorizontalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 3, false);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationLargeHorizontalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 32, false);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationInfiniteVerticalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 0, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationOneVerticalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 1, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationTwoVerticalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 2, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationThreeVerticalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 3, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationLargeVerticalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 32, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationOverflowHorizontalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 3, false, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationOverflowVerticalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 3, true, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationGradientHorizontalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 3, false, false, true);
  END_TEST;
}
int UtcDaliMarqueeLifecyclePublicationGradientVerticalP(void)
{
  UiTestApplication app;
  FinitePublicationBudget(app, 3, true, false, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeThreeToInfiniteP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 3, 0, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeThreeToFiveP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 3, 5, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeFiveToOneP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 5, 1, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeInfiniteToFiniteP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 0, 3, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeVerticalThreeToInfiniteP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 3, 0, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeVerticalThreeToFiveP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 3, 5, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeVerticalFiveToOneP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 5, 1, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeVerticalInfiniteToFiniteP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 0, 3, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleRuntimeAfterReducedBudgetP(void)
{
  UiTestApplication app;
  RuntimeFiniteChange(app, 3, 5, false, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleSameConfigurationReplacementP(void)
{
  UiTestApplication app;
  Lifecycle         test(app, true, false, false, 3);
  test.Start();
  test.Advance(test.DurationMs() + 1u);
  ReplaceFiniteContent(test, false, 0u, 2);
  test.label.SetMarqueeLoopCount(3);
  ReplaceFiniteContent(test, false, 1u, 3);
  test.label.SetMarqueeLoopCount(5);
  test.label.SetMarqueeLoopCount(0);
  test.label.SetMarqueeLoopCount(3);
  test.label.SetMarqueeLoopCount(-1);
  ReplaceFiniteContent(test, false, 2u, 3);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFinishedQueuedPublicationHorizontalP(void)
{
  UiTestApplication app;
  FinishedBeforePublication(app, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFinishedQueuedPublicationVerticalP(void)
{
  UiTestApplication app;
  FinishedBeforePublication(app, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleSyncContentStartsNewSessionP(void)
{
  UiTestApplication app;
  for(bool vertical : {false, true})
  {
    Lifecycle test(app, false, vertical, false, 3);
    test.Start();
    test.Advance(test.DurationMs() + 1u);
    auto original = test.Current();
    DALI_TEST_EQUALS(original.GetCurrentLoop(), 1, TEST_LOCATION);
    test.label.SetRequestedWidth(150.0f);
    test.Drain();
    DALI_TEST_CHECK(test.Current() != original);
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), 3, TEST_LOCATION);
    DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 0, TEST_LOCATION);
    test.label.SetMarqueeLoopCount(5);
    test.label.SetText(vertical ? "First\nSecond\nThird\nFourth\nFifth\nSixth\nSeventh" : "A different long synchronous content starts a new marquee execution session");
    test.Drain();
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), 5, TEST_LOCATION);
    test.CheckTexture();
  }
  END_TEST;
}

namespace
{
void ReducedBudgetSessionEnd(UiTestApplication& app, unsigned ending)
{
  Lifecycle test(app, true, false, ending == 4u, 3);
  test.Start();
  test.Advance(test.DurationMs() + 1u);
  ReplaceFiniteContent(test, false, 0u, 2);
  ReplaceFiniteContent(test, false, 1u, 3);
  test.Advance(test.DurationMs() / 3u);
  auto       original = test.Current();
  const auto shader   = test.label.GetRendererAt(0u).GetShader();
  if(ending == 0u || ending == 1u)
  {
    if(ending == 1u) test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
    test.label.SetRequestedWidth(140.0f);
    Task pending = test.Dispatch();
    pending->Process();
    test.label.StopMarquee();
    test.Deliver(pending);
    if(ending == 0u)
    {
      DALI_TEST_CHECK(test.label.IsMarqueeRunning());
      DALI_TEST_CHECK(test.Current() == original);
      DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetShader() == shader);
      test.Advance(test.DurationMs());
    }
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
    test.Start();
  }
  else if(ending == 2u)
  {
    test.label.SetProperty(Actor::Property::VISIBLE, false);
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    test.label.SetProperty(Actor::Property::VISIBLE, true);
    test.Drain();
  }
  else if(ending == 3u)
  {
    test.label.Unparent();
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    test.parent.Add(test.label);
    test.Drain();
  }
  else
  {
    test.label.SetText("Fit");
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    test.label.SetText(LONG_TEXT);
    test.Drain();
  }
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  DALI_TEST_CHECK(test.Current() != original);
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 0, TEST_LOCATION);
  test.CheckTexture();
}
} // namespace

int UtcDaliMarqueeLifecycleReducedBudgetFinishLoopRestartP(void)
{
  UiTestApplication app;
  ReducedBudgetSessionEnd(app, 0u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleReducedBudgetImmediateRestartP(void)
{
  UiTestApplication app;
  ReducedBudgetSessionEnd(app, 1u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleReducedBudgetVisibilityRestartP(void)
{
  UiTestApplication app;
  ReducedBudgetSessionEnd(app, 2u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleReducedBudgetSceneRestartP(void)
{
  UiTestApplication app;
  ReducedBudgetSessionEnd(app, 3u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleReducedBudgetOverflowRestartP(void)
{
  UiTestApplication app;
  ReducedBudgetSessionEnd(app, 4u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFinishedQueuedConfigurationChangeP(void)
{
  UiTestApplication app;
  Lifecycle         test(app, true, false, false, 1);
  test.Start();
  test.label.SetMarqueeLoopCount(3);
  test.label.SetRequestedWidth(160.0f);
  Task pending = test.Dispatch();
  pending->Process();
  UpdateCore(app, test.DurationMs() + 1u);
  DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 1, TEST_LOCATION);
  test.Deliver(pending);
  test.Drain();
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(test.animationsCreated, 2u, TEST_LOCATION);
  test.Advance(test.DurationMs() + 1u);
  test.Advance(test.DurationMs() + 1u);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.CheckTexture();
  END_TEST;
}

namespace
{
void FiniteNaturalStopRace(UiTestApplication& app, bool finishQueued)
{
  Lifecycle test(app, true, false, false, 3);
  test.Start();
  test.Advance(test.DurationMs() + 1u);
  ReplaceFiniteContent(test, false, 0u, 2);
  test.Advance(test.DurationMs() + 1u);
  ReplaceFiniteContent(test, false, 1u, 2);
  test.Advance(test.DurationMs() + 1u);
  auto       animation = test.Current();
  const auto duration  = test.DurationMs();
  if(finishQueued)
  {
    UpdateCore(app, duration + 1u);
    DALI_TEST_EQUALS(animation.GetCurrentLoop(), 2, TEST_LOCATION);
    DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
  }
  else
  {
    test.Advance(duration - 2u);
  }
  test.label.StopMarquee();
  test.label.SetRequestedWidth(140.0f);
  test.Advance(4u);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(test.animationsCreated, 3u, TEST_LOCATION);
  test.CheckTexture();
}
} // namespace

int UtcDaliMarqueeLifecycleFiniteNaturalFinishLoopRaceP(void)
{
  UiTestApplication app;
  FiniteNaturalStopRace(app, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFiniteQueuedFinishLoopRaceP(void)
{
  UiTestApplication app;
  FiniteNaturalStopRace(app, true);
  END_TEST;
}

namespace
{
// These callbacks are emitted by the real FocusManager/KeyInputFocusManager.
// The scale animation is independent of the marquee animation under test.
struct FocusLifecycle : Lifecycle
{
  Ui::View          item  = Ui::View::New();
  Ui::View          other = Ui::View::New();
  Ui::View          focusTarget;
  Actor             scaleTarget;
  Animation         scale;
  std::vector<bool> focusEvents;
  bool              startIfOverflow = true;
  float             upScale         = 1.1f;
  float             scaleSeconds    = 0.1f;

  FocusLifecycle(UiTestApplication& app, int ancestry = 1, bool automatic = false, int loops = 0, bool async = true)
  : Lifecycle(app, async, false, automatic, loops)
  {
    label.Unparent();
    item.SetRequestedWidth(180.0f);
    item.SetRequestedHeight(48.0f);
    parent.Add(item);
    item.Add(label);
    other.SetFocusable(true);
    application.GetWindow().Add(other);
    focusTarget = ancestry == 0 ? Ui::View(label) : item;
    focusTarget.SetFocusable(true);
    scaleTarget = ancestry == 0 ? Actor(label) : ancestry == 1 ? Actor(item)
                                                               : parent;
    focusTarget.FocusChangedSignal().Connect(&tracker, [this](Ui::View, bool focused)
    {
      focusEvents.push_back(focused);
      if(scale) scale.Stop();
      trackMarqueeAnimations = false;
      scale                  = Animation::New(scaleSeconds);
      trackMarqueeAnimations = true;
      scale.AnimateTo(Property(scaleTarget, Actor::Property::SCALE), Vector3::ONE * (focused ? upScale : 1.0f));
      scale.Play();
      if(focused)
      {
        // The app policy deliberately compares layout units, not WORLD_SCALE.
        const auto natural  = label.GetNaturalSize();
        const bool overflow = label.GetMarqueeOrientation() == Text::MarqueeOrientation::VERTICAL
                                ? natural.height > label.GetRequestedHeight()
                                : natural.width > label.GetRequestedWidth();
        if(startIfOverflow && overflow) label.StartMarquee();
      }
      else
        label.StopMarquee();
    });
    Drain();
  }
  ~FocusLifecycle()
  {
    tracker.DisconnectAll();
    Ui::FocusManager::Get().ClearFocus();
    if(scale) scale.Clear();
    other.Unparent();
  }
  void Focus(bool gained)
  {
    const auto count = focusEvents.size();
    DALI_TEST_CHECK(Ui::FocusManager::Get().SetCurrentFocusView(gained ? focusTarget : other));
    DALI_TEST_EQUALS(focusEvents.size(), count + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(static_cast<bool>(focusEvents.back()), gained, TEST_LOCATION);
  }
  void CheckScale(float expected)
  {
    DALI_TEST_EQUALS(scaleTarget.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE * expected, 0.0001f, TEST_LOCATION);
  }
};

void FocusScaleOnly(UiTestApplication& app, int ancestry, float scale)
{
  FocusLifecycle test(app, ancestry);
  test.label.SetText("Fit");
  test.upScale = scale;
  test.Drain();
  auto        texture   = test.label.GetRendererAt(0u).GetTextures();
  const auto  completed = test.completed;
  const auto  natural   = test.label.GetNaturalSize();
  const auto  size      = test.label.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  const float effective = Ui::Extension::View::GetEffectiveLayoutScale(Ui::GetImpl(test.label));
  test.Focus(true);
  test.Advance(150u);
  DALI_TEST_CHECK(test.Take().empty());
  test.CheckScale(scale);
  DALI_TEST_EQUALS(test.label.GetCurrentProperty<Vector3>(Actor::Property::SIZE), size, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(test.label.GetNaturalSize(), natural, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(Ui::GetImpl(test.label)), effective, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == texture);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.Focus(false);
  test.Advance(150u);
  test.CheckScale(1.0f);
  DALI_TEST_CHECK(test.Take().empty());
}

void FocusFinish(UiTestApplication& app, unsigned loop, bool scaleFirst)
{
  FocusLifecycle test(app, 1, false, 3);
  test.Focus(true);
  test.Drain();
  test.Advance(loop * test.DurationMs() + test.DurationMs() / 2u);
  test.scaleSeconds     = scaleFirst ? 0.01f : test.Current().GetDuration() * 2.0f;
  auto        animation = test.Current();
  const float progress  = animation.GetCurrentProgress();
  test.Focus(false);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  DALI_TEST_CHECK(test.Current() == animation);
  test.Advance(20u);
  DALI_TEST_CHECK(test.Take().empty());
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  if(scaleFirst) test.CheckScale(1.0f);
  test.Advance(static_cast<unsigned>((1.0f - progress) * test.DurationMs()) + 20u);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(test.animationsCreated, 1u, TEST_LOCATION);
  if(!scaleFirst)
  {
    DALI_TEST_CHECK(test.scale.GetState() == Animation::PLAYING);
  }
  test.Advance(static_cast<unsigned>(test.scaleSeconds * 1000) + 20u);
  test.CheckScale(1.0f);
  test.CheckTexture();
}
} // namespace

int UtcDaliMarqueeLifecycleFocusLabelScaleOnlyP(void)
{
  UiTestApplication app;
  for(float scale : {1.05f, 1.1f, 1.2f}) FocusScaleOnly(app, 0, scale);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusParentScaleOnlyP(void)
{
  UiTestApplication app;
  for(float scale : {1.05f, 1.1f, 1.2f}) FocusScaleOnly(app, 1, scale);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusGrandparentScaleOnlyP(void)
{
  UiTestApplication app;
  FocusScaleOnly(app, 2, 1.2f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusImmediateScaleP(void)
{
  UiTestApplication app;
  FocusLifecycle    test(app);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.Focus(true);
  test.Drain();
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  auto texture = test.label.GetRendererAt(0u).GetTextures();
  test.Advance(30u);
  DALI_TEST_CHECK(test.Take().empty());
  test.Focus(false);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.Drain();
  test.Advance(150u);
  test.CheckScale(1.0f);
  DALI_TEST_CHECK(test.Take().empty());
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() != texture);
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusFinishScaleFirstP(void)
{
  UiTestApplication app;
  FocusFinish(app, 0, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusFinishMarqueeFirstP(void)
{
  UiTestApplication app;
  FocusFinish(app, 0, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusFinishMiddleLoopP(void)
{
  UiTestApplication app;
  FocusFinish(app, 1, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusFinishLastLoopP(void)
{
  UiTestApplication app;
  FocusFinish(app, 2, true);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusPendingLatestP(void)
{
  UiTestApplication app;
  FocusLifecycle    test(app);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  auto     initial   = test.label.GetRendererAt(0u).GetTextures();
  unsigned completed = test.completed;
  test.Focus(true);
  Task first = test.Dispatch();
  first->Process();
  test.Focus(false);
  Task second = test.Dispatch();
  second->Process();
  test.Focus(true);
  Task third = test.Dispatch();
  third->Process();
  test.Deliver(first);
  test.Deliver(second);
  DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == initial);
  test.Deliver(third);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  test.Advance(150u);
  test.CheckScale(1.1f);
  DALI_TEST_CHECK(test.Take().empty());
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusRapidFiveTransitionsP(void)
{
  UiTestApplication app;
  FocusLifecycle    test(app);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  std::vector<Task> tasks;
  unsigned          completed = test.completed;
  for(bool focused : {true, false, true, false, true})
  {
    test.Focus(focused);
    tasks.push_back(test.Dispatch());
    tasks.back()->Process();
  }
  for(auto task : tasks) test.Deliver(task);
  tasks.clear();
  test.Drain();
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(test.animationsCreated, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusFinishPendingRefocusP(void)
{
  UiTestApplication app;
  FocusLifecycle    test(app);
  test.Focus(true);
  test.Drain();
  test.Advance(test.DurationMs() / 3u);
  auto original = test.Current();
  test.Focus(false);
  test.Focus(true);
  test.Drain();
  DALI_TEST_CHECK(test.Current() == original);
  test.Advance(test.DurationMs() + 10u);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.animationsCreated, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  // Refocus does not revoke a previously accepted FINISH_LOOP stop.
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusLayoutDuringScaleP(void)
{
  UiTestApplication app;
  FocusLifecycle    test(app);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.Focus(true);
  test.Drain();
  unsigned completed = test.completed;
  test.Advance(30u);
  DALI_TEST_CHECK(test.Take().empty());
  test.item.SetRequestedWidth(160.0f);
  test.label.SetRequestedWidth(160.0f);
  Task old = test.Dispatch();
  old->Process();
  test.Focus(false);
  Task latest = test.Dispatch();
  latest->Process();
  test.Deliver(old);
  DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  test.Deliver(latest);
  test.Drain();
  test.Advance(150u);
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.CheckScale(1.0f);
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusOverflowVisibilityP(void)
{
  UiTestApplication app;
  FocusLifecycle    test(app, 1, true);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.Focus(true);
  test.Drain();
  test.Focus(false);
  test.item.SetProperty(Actor::Property::VISIBLE, false);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.item.SetProperty(Actor::Property::VISIBLE, true);
  test.Focus(true);
  test.Drain();
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusGradientRenderScaleP(void)
{
  UiTestApplication    app;
  FocusLifecycle       test(app);
  Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2(100.0f, 0.0f));
  gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
  gradient.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(Color::RED)), Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
  test.label.SetTextGradient(gradient);
  test.label.SetTextGradientOverlay(gradient);
  test.label.SetRenderScale(1.5f);
  test.Drain();
  test.Focus(true);
  test.Drain();
  auto     texture   = test.label.GetRendererAt(0u).GetTextures();
  unsigned completed = test.completed;
  test.Advance(150u);
  DALI_TEST_CHECK(test.Take().empty());
  DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == texture);
  test.CheckScale(1.1f);
  test.Focus(false);
  test.Advance(test.DurationMs() + 20u);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFinishedPendingCallbackStateP(void)
{
  UiTestApplication app;
  Lifecycle         test(app, true, false, false, 1);
  test.Start();
  auto original = test.Current();
  auto texture  = test.label.GetRendererAt(0u).GetTextures();
  test.label.SetRequestedWidth(160.0f);
  Task pending = test.Dispatch();
  pending->Process();
  UpdateCore(app, test.DurationMs() + 1u);
  DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
  unsigned callbacks = test.completed;
  test.Deliver(pending);
  DALI_TEST_EQUALS(test.completed, callbacks + 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(test.finished, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() != texture);
  DALI_TEST_CHECK(test.Current() != original);
  test.CheckTexture();
  test.Advance(test.DurationMs() + 1u);
  test.Drain();
  DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.animationsCreated, 2u, TEST_LOCATION);
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFinishedPendingDisconnectP(void)
{
  UiTestApplication app;
  Lifecycle         test(app, true, false, false, 1);
  test.Start();
  test.label.SetRequestedWidth(160.0f);
  Task pending = test.Dispatch();
  pending->Process();
  UpdateCore(app, test.DurationMs() + 1u);
  test.Deliver(pending);
  test.label.Unparent();
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.parent.Add(test.label);
  test.Drain();
  test.Start();
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), 1, TEST_LOCATION);
  test.CheckTexture();
  END_TEST;
}

namespace
{
void FocusOverflowBoundary(UiTestApplication& app, bool automatic, float delta)
{
  FocusLifecycle test(app, 1, automatic);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.label.StopMarquee();
  test.label.SetText("Boundary text");
  const float natural = test.label.GetNaturalSize().width;
  test.label.SetRequestedWidth(natural - delta);
  test.item.SetRequestedWidth(natural - delta);
  test.startIfOverflow = !automatic;
  test.Drain();
  test.Focus(true);
  test.Drain();
  tet_printf("Focus boundary automatic=%d delta=%.3f natural=%.3f actualWidth=%.3f running=%d", automatic, delta, natural,
             test.label.GetCurrentProperty<Vector3>(Actor::Property::SIZE).width, test.label.IsMarqueeRunning());
  DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), delta > 0.0f, TEST_LOCATION);
  const auto completed = test.completed;
  test.Advance(150u);
  DALI_TEST_CHECK(test.Take().empty());
  DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  test.CheckScale(1.1f);
  test.CheckTexture();
}
} //namespace
int UtcDaliMarqueeLifecycleFocusManualFitBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, false, -1.0f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusManualEqualBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, false, 0.0f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusManualFractionBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, false, 0.25f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusManualOnePixelBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, false, 1.0f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusAutoFitBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, true, -1.0f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusAutoEqualBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, true, 0.0f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusAutoFractionBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, true, 0.25f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusAutoOnePixelBoundaryP(void)
{
  UiTestApplication app;
  FocusOverflowBoundary(app, true, 1.0f);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusNaturalSizeBeforeCompletionP(void)
{
  UiTestApplication app;
  FocusLifecycle    test(app);
  test.label.SetText("Fit");
  test.Drain();
  float before = test.label.GetNaturalSize().width;
  test.label.SetText(LONG_TEXT);
  Task stale = test.Dispatch();
  stale->Process();
  // Synchronous natural-size measurement sees current controller text even
  // while the previous rendered texture is still visible.
  DALI_TEST_CHECK(test.label.GetNaturalSize().width > before);
  test.Focus(true);
  Task current = test.Dispatch();
  current->Process();
  tet_printf("Focus stale task=%u current task=%u", stale->GetId(), current->GetId());
  unsigned callbacks = test.completed;
  test.Deliver(stale);
  DALI_TEST_EQUALS(test.completed, callbacks, TEST_LOCATION);
  test.Deliver(current);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, callbacks + 1u, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  END_TEST;
}
int UtcDaliMarqueeLifecycleFinishedPendingHideP(void)
{
  UiTestApplication app;
  Lifecycle         test(app, true, false, false, 1);
  test.Start();
  test.label.SetRequestedWidth(160.0f);
  Task pending = test.Dispatch();
  pending->Process();
  UpdateCore(app, test.DurationMs() + 1u);
  test.Deliver(pending);
  test.label.SetProperty(Actor::Property::VISIBLE, false);
  test.Drain();
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.label.SetProperty(Actor::Property::VISIBLE, true);
  test.Drain();
  test.Start();
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), 1, TEST_LOCATION);
  test.CheckTexture();
  END_TEST;
}
int UtcDaliMarqueeLifecycleFinishedPendingDestroyP(void)
{
  UiTestApplication app;
  Lifecycle         test(app, true, false, false, 1);
  test.Start();
  test.label.SetRequestedWidth(160.0f);
  Task pending = test.Dispatch();
  pending->Process();
  UpdateCore(app, test.DurationMs() + 1u);
  test.Deliver(pending);
  WeakHandle<Ui::Label> weak(test.label);
  test.label.Unparent();
  test.label.Reset();
  pending.Reset();
  test.Drain();
  DALI_TEST_CHECK(!weak.GetHandle());
  DALI_TEST_EQUALS(DevelAnimation::GetAnimationCount(), 0u, TEST_LOCATION);
  END_TEST;
}
int UtcDaliMarqueeLifecycleCompletionCallbackDestroyP(void)
{
  UiTestApplication app;
  Lifecycle         test(app);
  test.Start();
  test.label.SetRequestedWidth(160.0f);
  Task pending = test.Dispatch();
  pending->Process();
  WeakHandle<Ui::Label> weak(test.label);
  test.label.AsyncRenderFinishedSignal().Connect(&test.tracker, [&](Ui::View, float, float)
  {
    test.label.Unparent();
    test.label.Reset();
  });
  test.Deliver(pending);
  pending.Reset();
  test.Drain();
  DALI_TEST_CHECK(!weak.GetHandle());
  DALI_TEST_EQUALS(DevelAnimation::GetAnimationCount(), 0u, TEST_LOCATION);
  END_TEST;
}

namespace
{
void FocusCachedAsyncMeasurement(UiTestApplication& app, bool automatic)
{
  FocusLifecycle test(app, 1, automatic);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.label.SetText("Fit");
  test.Drain();
  float cachedWidth = -1.0f;
  test.label.AsyncNaturalSizeComputedSignal().Connect(&test.tracker, [&](Ui::View, float width, float)
  { cachedWidth = width; });
  test.label.RequestAsyncNaturalSize();
  auto measures = test.Take();
  DALI_TEST_EQUALS(measures.size(), 1u, TEST_LOCATION);
  measures.front()->Process();
  test.label.SetText(LONG_TEXT);
  Task rendering = test.Dispatch();
  rendering->Process();
  // A measurement requested for the old text remains a measurement of that
  // snapshot. The app must invalidate its own cached size when text changes.
  test.Deliver(measures.front());
  measures.clear();
  DALI_TEST_CHECK(cachedWidth > 0.0f && cachedWidth < test.label.GetRequestedWidth());
  test.startIfOverflow = cachedWidth > test.label.GetRequestedWidth();
  test.Focus(true);
  test.Deliver(rendering);
  rendering.Reset();
  test.Drain();
  DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), automatic, TEST_LOCATION);
  test.CheckTexture();

  test.label.RequestAsyncNaturalSize();
  measures = test.Take();
  DALI_TEST_EQUALS(measures.size(), 1u, TEST_LOCATION);
  test.Complete(measures.front());
  measures.clear();
  DALI_TEST_CHECK(cachedWidth > test.label.GetRequestedWidth());
  test.startIfOverflow = true;
  test.Focus(false);
  test.Focus(true);
  test.Drain();
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  test.CheckTexture();
}
} //namespace
int UtcDaliMarqueeLifecycleFocusManualCachedAsyncNaturalSizeP(void)
{
  UiTestApplication app;
  FocusCachedAsyncMeasurement(app, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusAutoCachedAsyncNaturalSizeP(void)
{
  UiTestApplication app;
  FocusCachedAsyncMeasurement(app, true);
  END_TEST;
}

namespace
{
// Verify the public call sequence an app can use when refocus should restart
// immediately, as well as refocus after a completed stop or finite run.
void CheckFocusRestartContract(unsigned transition, bool async = true, bool automatic = false)
{
  UiTestApplication app;
  const int         loops = transition == 3u ? 1 : 3;
  FocusLifecycle    test(app, 1, automatic, loops, async);
  if(transition == 0u) test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.Focus(true);
  test.Drain();
  auto original = test.Current();
  test.Advance(transition == 3u ? test.DurationMs() + 10u : test.DurationMs() / 4u);
  test.Drain();
  if(transition == 3u)
  {
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  }
  test.Focus(false);
  if(transition == 2u)
  {
    test.Advance(test.DurationMs() + 10u);
    test.Drain();
    DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(test.finished, 1u, TEST_LOCATION);
  }
  test.Focus(true);
  if(transition == 1u)
  {
    DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    DALI_TEST_CHECK(test.Current() == original);
    const auto stopMode = test.label.GetMarqueeStopMode();
    test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
    test.label.StopMarquee();
    test.label.SetMarqueeStopMode(stopMode);
    test.label.StartMarquee();
  }
  test.Drain();
  DALI_TEST_CHECK(test.label.IsMarqueeRunning());
  DALI_TEST_CHECK(test.Current() != original);
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), loops, TEST_LOCATION);
  DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(test.focusEvents.size(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(test.focusEvents.back());
  if(transition == 1u)
  {
    DALI_TEST_CHECK(test.label.GetMarqueeStopMode() == Text::MarqueeStopMode::FINISH_LOOP);
  }
  test.Advance(150u);
  test.CheckScale(1.1f);
  test.CheckTexture();
}
} // namespace

int UtcDaliMarqueeLifecycleFocusImmediateRefocusSessionP(void)
{
  CheckFocusRestartContract(0u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusFinishPendingExplicitRestartP(void)
{
  CheckFocusRestartContract(1u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusAfterFinishRefocusP(void)
{
  CheckFocusRestartContract(2u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusAfterFiniteRefocusP(void)
{
  CheckFocusRestartContract(3u);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusSyncFinishPendingExplicitRestartP(void)
{
  CheckFocusRestartContract(1u, false);
  END_TEST;
}
int UtcDaliMarqueeLifecycleFocusOverflowFinishPendingExplicitRestartP(void)
{
  CheckFocusRestartContract(1u, true, true);
  END_TEST;
}

namespace
{
// All tasks are the production loader/manager requests. Holding a completed
// task postpones event-thread delivery without depending on worker speed.
struct StopRace : FocusLifecycle
{
  ConnectionTracker          raceTracker;
  std::set<const RefObject*> liveMarquees;
  unsigned                   destroyed = 0u;

  explicit StopRace(UiTestApplication& app, int ancestry = 1, bool automatic = false, bool async = true)
  : FocusLifecycle(app, ancestry, automatic, 0, async)
  {
    label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
    label.StopMarquee();
    Drain();
    application.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&raceTracker, [this](BaseHandle h)
    {
      if(trackMarqueeAnimations && Animation::DownCast(h)) liveMarquees.insert(h.GetObjectPtr());
    });
    application.GetCore().GetObjectRegistry().ObjectDestroyedSignal().Connect(&raceTracker, [this](const RefObject* object)
    {
      if(liveMarquees.erase(object)) ++destroyed;
    });
  }

  Ui::Integration::Visual::Base Visual()
  {
    return Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(label)).GetVisual(Text::LabelPropertyIndex::TEXT);
  }

  bool Enabled()
  {
    return Ui::Internal::TextVisual::GetController(Visual())->IsMarqueeEnabled();
  }

  bool Pending()
  {
    return Ui::Internal::TextVisual::IsAsyncRenderPending(Visual());
  }

  void Stopped(unsigned created)
  {
    DALI_TEST_CHECK(!Enabled());
    DALI_TEST_CHECK(!label.IsMarqueeRunning());
    DALI_TEST_EQUALS(animationsCreated, created, TEST_LOCATION);
    auto animation = latestAnimation.GetHandle();
    if(animation)
    {
      DALI_TEST_CHECK(animation.GetState() != Animation::PLAYING);
    }
  }

  Task Request(bool marquee)
  {
    Task task = Dispatch();
    DALI_TEST_CHECK(task && task->GetId() != 0u);
    const auto& parameters = task->mParameters;
    const bool  automatic  = parameters.marqueeTriggerPolicy == Text::MarqueeTriggerPolicy::ON_OVERFLOW && !parameters.suppressAutoMarquee;
    DALI_TEST_EQUALS(static_cast<bool>(parameters.isMarqueeEnabled || automatic), marquee, TEST_LOCATION);
    DALI_TEST_CHECK(Pending());
    return task;
  }

  void Raster(Task task)
  {
    DALI_TEST_CHECK(task && task->IsReady());
    task->Process();
  }

  void StaticFrames(unsigned created)
  {
    // First update applies Stop's existing end action; subsequent updates must
    // not advance or replace the stopped marquee. Scale animates separately.
    Frame();
    auto  animation = latestAnimation.GetHandle();
    float progress  = animation ? animation.GetCurrentProgress() : 0.0f;
    // Observe the next frame, focus-scale completion, and a later idle update
    // without polling every intermediate frame.
    const unsigned scaleEnd = static_cast<unsigned>(std::ceil(scaleSeconds * 1000.0f)) + 1u;
    for(unsigned elapsed : {16u, scaleEnd, 1024u})
    {
      Frame(elapsed);
      Stopped(created);
      DALI_TEST_CHECK(Take().empty());
      DALI_TEST_CHECK(!Pending());
      if(animation) DALI_TEST_EQUALS(animation.GetCurrentProgress(), progress, 0.0001f, TEST_LOCATION);
      CheckTexture();
    }
    CheckScale(1.0f);
  }
};

struct StopPublication
{
  Renderer             renderer;
  Shader               shader;
  Geometry             geometry;
  TextureSet           textures;
  std::vector<Texture> slots;
  Vector2              size, offset;

  explicit StopPublication(Ui::Label label)
  : renderer(label.GetRendererAt(0u)),
    shader(renderer.GetShader()),
    geometry(renderer.GetGeometry()),
    textures(renderer.GetTextures()),
    size(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE)),
    offset(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET))
  {
    for(unsigned i = 0u; i < textures.GetTextureCount(); ++i) slots.push_back(textures.GetTexture(i));
  }

  void Check(Ui::Label label) const
  {
    DALI_TEST_EQUALS(label.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(label.GetRendererAt(0u) == renderer);
    DALI_TEST_CHECK(renderer.GetShader() == shader);
    DALI_TEST_CHECK(renderer.GetGeometry() == geometry);
    DALI_TEST_CHECK(renderer.GetTextures() == textures);
    DALI_TEST_EQUALS(textures.GetTextureCount(), slots.size(), TEST_LOCATION);
    for(unsigned i = 0u; i < slots.size(); ++i) DALI_TEST_CHECK(textures.GetTexture(i) == slots[i]);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), size, TEST_LOCATION);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_OFFSET), offset, TEST_LOCATION);
  }
};

void StopBeforeDispatch(int ancestry)
{
  UiTestApplication app;
  StopRace          test(app, ancestry);
  const unsigned    completed = test.completed;
  test.Focus(true);
  DALI_TEST_CHECK(test.Enabled());
  DALI_TEST_CHECK(!test.Pending());
  test.Focus(false);
  test.Stopped(0u);
  Task latest = test.Request(false);
  test.Complete(latest);
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  test.StaticFrames(0u);
}

void StopPending(unsigned order, unsigned variant = 0u)
{
  UiTestApplication app;
  StopRace          test(app, 1, variant == 3u);
  if(variant == 1u)
  {
    test.label.SetText(VERTICAL_TEXT);
    test.label.SetMultiLine(true);
    test.label.SetMarqueeOrientation(Text::MarqueeOrientation::VERTICAL);
  }
  if(variant == 2u)
  {
    Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2(140.0f, 0.0f));
    gradient.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(Color::RED)), Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
    gradient.SetUnits(Ui::Gradient::Units::USER_SPACE);
    test.label.SetTextGradient(gradient);
    test.label.SetTextGradientOverlay(gradient);
    test.label.SetRenderScale(1.5f);
  }
  test.Drain();
  StopPublication initial(test.label);
  const unsigned  completed = test.completed;
  const unsigned  created   = test.animationsCreated;
  test.Focus(true);
  Task old = test.Request(true);
  if(order != 0u) test.Raster(old);
  test.Focus(false);
  const bool pendingAfterStop = test.Pending();
  // Release the captured loader before assertions can unwind the fixture.
  // Processing alone does not deliver B or change its event-thread ownership.
  if(order == 0u) test.Raster(old);
  test.Stopped(created);
  DALI_TEST_CHECK(!pendingAfterStop);
  initial.Check(test.label);
  // Orders 0/1 deliver B before the relayout that dispatches static C.
  if(order < 2u)
  {
    test.Deliver(old);
    test.Stopped(created);
    initial.Check(test.label);
    DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  }
  Task latest = test.Request(false);
  DALI_TEST_CHECK(latest->GetId() != old->GetId());
  test.Raster(latest);
  if(order == 2u)
  {
    test.Deliver(old);
    test.Stopped(created);
    DALI_TEST_CHECK(test.Pending());
    initial.Check(test.label);
    DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  }
  test.Deliver(latest);
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  StopPublication final(test.label);
  if(order == 3u)
  {
    test.Deliver(old);
    test.Stopped(created);
    final.Check(test.label);
    DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  }
  test.StaticFrames(created);
}

void StopRunning(bool asynchronous)
{
  UiTestApplication app;
  StopRace          test(app, 1, false, asynchronous);
  test.Focus(true);
  test.Drain();
  DALI_TEST_CHECK(test.Enabled() && test.label.IsMarqueeRunning());
  test.Advance(test.DurationMs() / 3u);
  const unsigned created   = test.animationsCreated;
  const unsigned completed = test.completed;
  const unsigned finished  = test.finished;
  test.Focus(false);
  test.Stopped(created);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, completed + (asynchronous ? 1u : 0u), TEST_LOCATION);
  test.StaticFrames(created);
  // Core Stop delivers its existing FinishedSignal once; Label must stay stopped.
  DALI_TEST_EQUALS(test.finished, finished + 1u, TEST_LOCATION);
  test.label.Unparent();
  test.label.Reset();
  test.Frame();
  DALI_TEST_CHECK(test.liveMarquees.empty());
  DALI_TEST_EQUALS(test.destroyed, created, TEST_LOCATION);
}

void StopRapid(unsigned transitions)
{
  UiTestApplication app;
  StopRace          test(app);
  const unsigned    completed = test.completed;
  std::vector<Task> tasks;
  for(unsigned i = 0u; i < transitions; ++i)
  {
    test.Focus(i % 2u == 0u);
    Task task = test.Request(i % 2u == 0u);
    test.Raster(task); // Release the loader even with a one-loader pool.
    tasks.push_back(task);
  }
  // Latest delivery first, then every obsolete completion in reverse order.
  test.Deliver(tasks.back());
  StopPublication latest(test.label);
  const unsigned  created = test.animationsCreated;
  for(unsigned i = transitions - 1u; i > 0u; --i)
  {
    test.Deliver(tasks[i - 1u]);
    latest.Check(test.label);
    DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(test.animationsCreated, created, TEST_LOCATION);
    if(transitions % 2u == 0u) test.Stopped(0u);
  }
  if(transitions % 2u == 0u)
    test.StaticFrames(0u);
  else
  {
    DALI_TEST_CHECK(test.Enabled() && test.label.IsMarqueeRunning());
    DALI_TEST_EQUALS(created, 1u, TEST_LOCATION);
    test.Advance(test.DurationMs() / 4u);
    DALI_TEST_CHECK(test.Current().GetCurrentProgress() > 0.0f);
    test.Focus(false);
    test.Drain();
    test.StaticFrames(created);
  }
}

void StopDeliveryPermutations(unsigned staticPosition, unsigned firstStale)
{
  // Four UTCs pin the first stale delivery and cover all 24 orders together.
  std::array<unsigned, 4u> order{{0u, 1u, 2u, 3u}};
  std::swap(order[0u], order[firstStale]);
  std::sort(order.begin() + 1, order.end());
  unsigned                 permutations = 0u;
  UiTestApplication        app;
  do
  {
    StopRace       test(app);
    const unsigned completed = test.completed;
    test.Focus(true);
    std::array<Task, 4u> stale;
    for(unsigned i = 0u; i < stale.size(); ++i)
    {
      if(i) test.label.SetText(i % 2u ? LONG_TEXT : "Another complete long marquee request waiting for event-thread delivery");
      stale[i] = test.Request(true);
      test.Raster(stale[i]);
    }
    test.Focus(false);
    test.Stopped(0u);
    Task current = test.Request(false);
    test.Raster(current);
    StopPublication expected(test.label);
    for(unsigned i = 0u; i <= order.size(); ++i)
    {
      if(i == staticPosition)
      {
        test.Deliver(current);
        expected = StopPublication(test.label);
      }
      if(i < order.size()) test.Deliver(stale[order[i]]);
      expected.Check(test.label);
      test.Stopped(0u);
      DALI_TEST_EQUALS(test.completed, completed + (i >= staticPosition ? 1u : 0u), TEST_LOCATION);
    }
    test.StaticFrames(0u);
    ++permutations;
  } while(std::next_permutation(order.begin() + 1, order.end()));
  DALI_TEST_EQUALS(permutations, 6u, TEST_LOCATION);
}

void StopInCompletion(bool changeStaticProperty)
{
  UiTestApplication app;
  StopRace          test(app);
  bool              stopped = false;
  bool              changed = false;
  unsigned          atStop  = 0u;
  test.label.AsyncRenderFinishedSignal().Connect(&test.raceTracker, [&](Ui::View, float, float)
  {
    if(!stopped)
    {
      stopped = true;
      test.Focus(false);
      atStop = test.animationsCreated;
      test.Stopped(atStop);
    }
    else if(changeStaticProperty && !changed)
    {
      changed = true;
      test.label.SetText("Latest static content requested by the static completion callback");
    }
  });
  const unsigned completed = test.completed;
  test.Focus(true);
  Task moving = test.Request(true);
  test.Complete(moving);
  DALI_TEST_CHECK(stopped);
  test.Stopped(atStop);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, completed + (changeStaticProperty ? 3u : 2u), TEST_LOCATION);
  DALI_TEST_EQUALS(changed, changeStaticProperty, TEST_LOCATION);
  test.StaticFrames(atStop);
}
} // namespace

int UtcDaliMarqueeFocusStopRaceBeforeDispatchParentP(void)
{
  StopBeforeDispatch(1);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceBeforeDispatchLabelP(void)
{
  StopBeforeDispatch(0);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceBeforeWorkerP(void)
{
  StopPending(0u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceAfterWorkerBeforeDeliveryP(void)
{
  StopPending(1u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceOldBeforeStaticP(void)
{
  StopPending(2u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceStaticBeforeOldP(void)
{
  StopPending(3u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceVerticalP(void)
{
  StopPending(3u, 1u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceGradientRenderScaleP(void)
{
  StopPending(3u, 2u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceOverflowP(void)
{
  StopPending(3u, 3u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceAfterPublicationP(void)
{
  StopRunning(true);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceSyncAfterPublicationP(void)
{
  StopRunning(false);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceRapidFinalOutTwoP(void)
{
  StopRapid(2u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceRapidFinalInThreeP(void)
{
  StopRapid(3u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceRapidFinalOutFourP(void)
{
  StopRapid(4u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceRapidFinalInFiveP(void)
{
  StopRapid(5u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticFirstOrder0P(void)
{
  StopDeliveryPermutations(0u, 0u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticFirstOrder1P(void)
{
  StopDeliveryPermutations(0u, 1u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticFirstOrder2P(void)
{
  StopDeliveryPermutations(0u, 2u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticFirstOrder3P(void)
{
  StopDeliveryPermutations(0u, 3u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticMiddleOrder0P(void)
{
  StopDeliveryPermutations(2u, 0u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticMiddleOrder1P(void)
{
  StopDeliveryPermutations(2u, 1u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticMiddleOrder2P(void)
{
  StopDeliveryPermutations(2u, 2u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticMiddleOrder3P(void)
{
  StopDeliveryPermutations(2u, 3u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticLastOrder0P(void)
{
  StopDeliveryPermutations(4u, 0u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticLastOrder1P(void)
{
  StopDeliveryPermutations(4u, 1u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticLastOrder2P(void)
{
  StopDeliveryPermutations(4u, 2u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceFourStaleStaticLastOrder3P(void)
{
  StopDeliveryPermutations(4u, 3u);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceCompletionFocusLostP(void)
{
  StopInCompletion(false);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceStaticCompletionPropertyP(void)
{
  StopInCompletion(true);
  END_TEST;
}

namespace
{
void StopThreeFocusItems(bool finalIn)
{
  UiTestApplication app;
  StopRace          test(app);
  auto              third = Ui::View::New();
  third.SetFocusable(true);
  third.Add(Ui::Label::New("Short item C"));
  test.other.Add(Ui::Label::New("Short item B"));
  app.GetWindow().Add(third);
  test.Drain();
  unsigned          completed = test.completed;
  std::vector<Task> tasks;
  test.Focus(true); // A
  tasks.push_back(test.Request(true));
  test.Raster(tasks.back());
  test.Focus(false); // B
  tasks.push_back(test.Request(false));
  test.Raster(tasks.back());
  DALI_TEST_CHECK(Ui::FocusManager::Get().SetCurrentFocusView(third)); // C
  test.Frame();
  DALI_TEST_CHECK(test.Take().empty());
  test.Stopped(0u);
  test.Focus(true); // A again
  tasks.push_back(test.Request(true));
  test.Raster(tasks.back());
  if(!finalIn)
  {
    test.Focus(false);
    tasks.push_back(test.Request(false));
    test.Raster(tasks.back());
  }
  test.Deliver(tasks.back());
  StopPublication latest(test.label);
  const unsigned  created = test.animationsCreated;
  for(unsigned i = 0u; i + 1u < tasks.size(); ++i)
  {
    test.Deliver(tasks[i]);
    latest.Check(test.label);
    DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(test.animationsCreated, created, TEST_LOCATION);
    DALI_TEST_EQUALS(test.Enabled(), finalIn, TEST_LOCATION);
    DALI_TEST_EQUALS(test.label.IsMarqueeRunning(), finalIn, TEST_LOCATION);
  }
  if(finalIn)
  {
    DALI_TEST_EQUALS(created, 1u, TEST_LOCATION);
    test.Focus(false);
    test.Drain();
  }
  test.StaticFrames(created);
  third.Unparent();
}
} // namespace

int UtcDaliMarqueeFocusStopRaceThreeItemsFinalOutP(void)
{
  StopThreeFocusItems(false);
  END_TEST;
}

int UtcDaliMarqueeFocusStopRaceThreeItemsFinalInP(void)
{
  StopThreeFocusItems(true);
  END_TEST;
}

namespace
{
enum class FocusTextTiming
{
  PUBLISHED,
  BEFORE_RELAYOUT,
  AFTER_RELAYOUT,
  AFTER_STATIC
};

void CheckFocusStopTextTiming(FocusTextTiming timing, bool textFirst, bool adjacent = false)
{
  constexpr const char* STATIC_TEXT = "Latest static C text replaces the prior marquee after focus loss";
  UiTestApplication     app;
  StopRace              test(app);
  test.Focus(true);
  Task old = test.Request(true);
  test.Raster(old);
  if(timing == FocusTextTiming::PUBLISHED)
  {
    test.Deliver(old);
    test.Advance(test.DurationMs() / 3u);
    DALI_TEST_CHECK(test.label.IsMarqueeRunning());
    DALI_TEST_CHECK(test.Current().GetCurrentProgress() > 0.0f);
  }
  const unsigned  completed = test.completed;
  const unsigned  created   = test.animationsCreated;
  const unsigned  finished  = test.finished;
  StopPublication previous(test.label);
  if(textFirst) test.label.SetText(STATIC_TEXT);
  test.Focus(false);
  test.Stopped(created);
  DALI_TEST_CHECK(!test.Pending());
  Task intermediate;
  if(adjacent)
  {
    // Stop's static request belongs to the preceding event/relayout cycle.
    intermediate = test.Request(false);
    test.Raster(intermediate);
    test.Stopped(created);
    previous.Check(test.label);
  }
  if(!textFirst) test.label.SetText(STATIC_TEXT);
  DALI_TEST_EQUALS(test.Pending(), adjacent, TEST_LOCATION);
  DALI_TEST_CHECK(test.Take().empty());
  previous.Check(test.label);
  if(timing == FocusTextTiming::BEFORE_RELAYOUT)
  {
    test.Deliver(old);
    test.Stopped(created);
    previous.Check(test.label);
    DALI_TEST_EQUALS(test.Pending(), adjacent, TEST_LOCATION);
    DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  }
  Task current = test.Request(false);
  DALI_TEST_CHECK(current->GetId() != old->GetId());
  DALI_TEST_EQUALS(current->mParameters.text, std::string(STATIC_TEXT), TEST_LOCATION);
  test.Raster(current);
  if(timing == FocusTextTiming::AFTER_RELAYOUT)
  {
    test.Deliver(old);
    test.Stopped(created);
    previous.Check(test.label);
    DALI_TEST_CHECK(test.Pending());
    DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  }
  test.Frame(16u);
  DALI_TEST_CHECK(test.Take().empty());
  test.Stopped(created);
  previous.Check(test.label);
  test.Deliver(current);
  DALI_TEST_CHECK(!test.Pending());
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  StopPublication latest(test.label);
  DALI_TEST_CHECK(latest.textures != previous.textures);
  if(timing == FocusTextTiming::AFTER_STATIC) test.Deliver(old);
  if(intermediate) test.Deliver(intermediate);
  latest.Check(test.label);
  DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  test.StaticFrames(created);
  DALI_TEST_EQUALS(test.finished, finished + (timing == FocusTextTiming::PUBLISHED ? 1u : 0u), TEST_LOCATION);
  test.label.Unparent();
  test.label.Reset();
  test.Frame();
  DALI_TEST_CHECK(test.liveMarquees.empty());
  DALI_TEST_EQUALS(test.destroyed, created, TEST_LOCATION);
}
} // namespace

int UtcDaliMarqueeFocusTextPublishedStopTextP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::PUBLISHED, false, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextPublishedTextStopP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::PUBLISHED, true, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextBeforeRelayoutStopTextP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::BEFORE_RELAYOUT, false, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextBeforeRelayoutTextStopP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::BEFORE_RELAYOUT, true, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextAfterRelayoutStopTextP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::AFTER_RELAYOUT, false, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextAfterRelayoutTextStopP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::AFTER_RELAYOUT, true, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextAfterStaticStopTextP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::AFTER_STATIC, false, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextAfterStaticTextStopP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::AFTER_STATIC, true, false);
  END_TEST;
}

int UtcDaliMarqueeFocusTextAdjacentBeforeRelayoutP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::BEFORE_RELAYOUT, false, true);
  END_TEST;
}

int UtcDaliMarqueeFocusTextAdjacentAfterRelayoutP(void)
{
  CheckFocusStopTextTiming(FocusTextTiming::AFTER_RELAYOUT, false, true);
  END_TEST;
}

namespace
{
struct MarqueeUiScaleGuard
{
  const float scale    = Ui::Extension::LayoutScaleManager::Get().GetUiScale();
  const bool  scalable = Ui::Extension::LayoutScaleManager::Get().IsScalable();

  MarqueeUiScaleGuard()
  {
    Ui::Extension::LayoutScaleManager::Get().SetScalable(true);
    Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.0f);
  }

  ~MarqueeUiScaleGuard()
  {
    Ui::Extension::LayoutScaleManager::Get().SetUiScale(scale);
    Ui::Extension::LayoutScaleManager::Get().SetScalable(scalable);
  }
};

float CheckScaledMarqueeMotion(Lifecycle& test, float uiScale, bool advance = true)
{
  const bool vertical = test.label.GetMarqueeOrientation() == Text::MarqueeOrientation::VERTICAL;
  auto       visual   = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(test.label)).GetVisual(Text::LabelPropertyIndex::TEXT);
  DALI_TEST_EQUALS(Ui::Internal::TextVisual::GetController(visual)->GetUiScale(), uiScale, TEST_LOCATION);
  auto          shader      = test.label.GetRendererAt(0u).GetShader();
  const Vector2 textureSize = shader.GetProperty<Vector2>(shader.GetPropertyIndex("uTextureSize"));
  const Vector3 controlSize = test.label.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
  const float   distance    = vertical ? std::max(textureSize.y, controlSize.y) : std::max(textureSize.x, controlSize.x);
  const float   speed       = test.label.GetMarqueeSpeed() * uiScale;
  const float   delay       = test.label.GetMarqueeLoopDelay();
  auto          animation   = test.Current();
  const float   duration    = animation.GetDuration();
  DALI_TEST_CHECK(std::isfinite(duration) && duration > 0.0f);
  DALI_TEST_EQUALS(duration, delay + distance / speed, 0.00001f, TEST_LOCATION);
  if(advance)
  {
    DALI_TEST_CHECK(distance / speed > 0.1f);
    const auto  deltaIndex      = shader.GetPropertyIndex("uDelta");
    const float initial         = shader.GetCurrentProperty<float>(deltaIndex);
    const float initialProgress = animation.GetCurrentProgress();
    if(delay > 0.0f)
    {
      test.Advance(static_cast<unsigned>(std::round(delay * 1000.0f)));
      DALI_TEST_EQUALS(shader.GetCurrentProperty<float>(deltaIndex), initial, 0.001f, TEST_LOCATION);
    }
    // Accumulate travel across wrap boundaries when a replacement keeps progress.
    float previous = shader.GetCurrentProperty<float>(deltaIndex);
    float delta    = 0.0f;
    // Each 50ms sample is shorter than half the asserted travel duration.
    for(unsigned step = 0u; step < 2u; ++step)
    {
      test.Advance(50u);
      const float current = shader.GetCurrentProperty<float>(deltaIndex);
      const float segment = std::fabs(current - previous);
      delta += segment > distance * 0.5f ? distance - segment : segment;
      previous = current;
    }
    DALI_TEST_EQUALS(delta, speed * 0.1f, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(animation.GetCurrentProgress(), std::fmod(initialProgress + (delay + 0.1f) / duration, 1.0f), 0.0001f, TEST_LOCATION);
    tet_printf("UISCALE async=%d vertical=%d ui=%.2f render=%.2f distance=%.3f duration=%.6f delta100ms=%.6f speed=%d",
               test.asynchronous, vertical, uiScale, test.label.GetRenderScale(), distance, duration, delta, test.label.GetMarqueeSpeed());
  }
  test.CheckTexture();
  return duration;
}

void CheckMarqueeUiScaleMatrix(bool async, bool vertical)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  for(float scale : {1.0f, 1.25f, 1.5f, 2.0f})
    for(float renderScale : {1.0f, 1.5f, 2.0f})
      for(float actorScale : {1.0f, 1.5f})
      {
        Ui::Extension::LayoutScaleManager::Get().SetUiScale(scale);
        Lifecycle test(app, async, vertical);
        test.label.SetMarqueeGap(20);
        test.label.SetRenderScale(renderScale);
        test.parent.SetProperty(Actor::Property::SCALE, Vector3::ONE * actorScale);
        test.Start();
        CheckScaledMarqueeMotion(test, scale);
        DALI_TEST_EQUALS(test.label.GetMarqueeSpeed(), 200, TEST_LOCATION);
        DALI_TEST_EQUALS(test.label.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), Vector3::ONE * actorScale, 0.0001f, TEST_LOCATION);
      }
}

void CheckMarqueeUiScalePolicy(bool async)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  Lifecycle test(app, async);
  auto      branch = Ui::View::New();
  auto      root   = Ui::View::New();
  root.SetRequestedWidth(600.0f);
  root.SetRequestedHeight(300.0f);
  branch.SetRequestedWidth(400.0f);
  branch.SetRequestedHeight(100.0f);
  root.SetUiScalePolicy(Ui::UiScalePolicy::DISABLED);
  root.SetProperty(Actor::Property::SCALE, Vector3::ONE * 1.5f);
  branch.SetProperty(Actor::Property::SCALE, Vector3::ONE * 1.5f);
  root.Add(branch);
  branch.Add(test.label);
  test.parent.Add(root);
  for(unsigned policy = 0u; policy < 5u; ++policy)
  {
    if(policy == 1u) test.label.SetUiScalePolicy(Ui::UiScalePolicy::ENABLED);
    if(policy == 2u) test.label.SetUiScalePolicy(Ui::UiScalePolicy::DISABLED);
    if(policy == 3u)
    {
      test.label.SetUiScalePolicy(Ui::UiScalePolicy::INHERIT);
      branch.SetUiScalePolicy(Ui::UiScalePolicy::ENABLED);
    }
    if(policy == 4u) Ui::Extension::LayoutScaleManager::Get().SetScalable(false);
    test.Start();
    CheckScaledMarqueeMotion(test, policy == 1u || policy == 3u ? 2.0f : 1.0f);
    DALI_TEST_EQUALS(test.label.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), Vector3::ONE * 2.25f, 0.0001f, TEST_LOCATION);
  }
}

void CheckMarqueeUiScaleRuntime(bool async, bool vertical)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Lifecycle           test(app, async, vertical, false, 3);
  test.Start();
  for(float scale : {2.0f, 1.0f})
  {
    test.Advance(test.DurationMs() + 1u);
    auto       old       = test.Current();
    const auto oldBudget = old.GetLoopCount();
    DALI_TEST_EQUALS(old.GetCurrentLoop(), 1, TEST_LOCATION);
    Ui::Extension::LayoutScaleManager::Get().SetUiScale(scale);
    DALI_TEST_CHECK(test.Current() == old);
    if(async)
    {
      Task pending = test.Dispatch();
      DALI_TEST_CHECK(test.Current() == old);
      DALI_TEST_EQUALS(old.GetLoopCount(), oldBudget, TEST_LOCATION);
      test.Complete(pending);
    }
    test.Drain();
    DALI_TEST_CHECK(test.Current() != old);
    DALI_TEST_EQUALS(test.Current().GetLoopCount(), async ? 2 : 3, TEST_LOCATION);
    DALI_TEST_EQUALS(test.Current().GetCurrentLoop(), 0, TEST_LOCATION);
    CheckScaledMarqueeMotion(test, scale);
  }
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.label.StopMarquee();
  test.Drain();
  test.Start();
  DALI_TEST_EQUALS(test.Current().GetLoopCount(), 3, TEST_LOCATION);
}

void CheckMarqueeUiScaleManualSnapshot(bool measure)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Lifecycle           test(app);
  test.label.StartMarquee();
  auto& impl = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  impl.RequestAsyncRenderWithFixedSize(180.0f, 48.0f);
  auto tasks = test.Take();
  DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
  Task pending = tasks.front();
  pending->Process();
  DALI_TEST_EQUALS(pending->mParameters.decorationUiScale, 1.0f, TEST_LOCATION);
  const auto completed = test.completed;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  if(measure) test.Frame();
  test.Deliver(pending);
  DALI_TEST_EQUALS(test.completed, completed + (measure ? 0u : 1u), TEST_LOCATION);
  if(!measure)
  {
    DALI_TEST_EQUALS(Ui::Extension::View::GetEffectiveLayoutScale(Ui::GetImpl(test.label)), 2.0f, TEST_LOCATION);
    // Before measure the accepted geometry and controller still use scale 1.
    CheckScaledMarqueeMotion(test, 1.0f, false);
  }
  test.Drain();
  CheckScaledMarqueeMotion(test, 2.0f);
}

void CheckMarqueeUiScaleOptions(bool async, bool vertical)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  for(int speed : {0, -20, 200, 1000000, (std::numeric_limits<int>::max)()})
  {
    Lifecycle test(app, async, vertical);
    test.label.SetMarqueeSpeed(speed);
    test.label.SetMarqueeGap(20);
    test.label.SetMarqueeLoopDelay(0.25f);
    test.Start();
    DALI_TEST_EQUALS(test.label.GetMarqueeSpeed(), std::max(1, speed), TEST_LOCATION);
    CheckScaledMarqueeMotion(test, 2.0f, speed < 1000000);
  }
  for(bool fit : {false, true})
    for(float fontScale : {1.0f, 2.0f})
    {
      Lifecycle test(app, async, vertical);
      test.label.SetSystemFontSizeScaleEnabled(false);
      test.label.SetMinimumFontSizeScale(fontScale);
      test.label.SetMaximumFontSizeScale(fontScale);
      if(fit) test.label.SetTextFit(Text::Fit::Range(12.0f, 20.0f, 2.0f));
      test.Start();
      CheckScaledMarqueeMotion(test, 2.0f);
      auto old = test.Current();
      test.label.SetMarqueeSpeed(300);
      test.Drain();
      DALI_TEST_CHECK(test.Current() == old);
      test.label.SetRequestedWidth(160.0f);
      test.Drain();
      DALI_TEST_CHECK(test.Current() != old);
      CheckScaledMarqueeMotion(test, 2.0f);
      test.label.SetText(vertical ? "First\nSecond\nThird\nFourth\nFifth\nSixth" : "Changed long content must retain the authored speed with UI scaling");
      test.Drain();
      CheckScaledMarqueeMotion(test, 2.0f);
    }
}

void CheckMarqueeUiScaleFocusStop(unsigned order)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  StopRace test(app);
  test.Focus(true);
  Task stale = test.Request(true);
  test.Raster(stale);
  test.Focus(false);
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(1.0f);
  const auto      completed = test.completed;
  StopPublication previous(test.label);
  if(order == 0u) test.Deliver(stale);
  previous.Check(test.label);
  test.Stopped(0u);
  Task current = test.Request(false);
  test.Raster(current);
  if(order == 1u) test.Deliver(stale);
  previous.Check(test.label);
  DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
  test.Deliver(current);
  StopPublication latest(test.label);
  if(order == 2u) test.Deliver(stale);
  latest.Check(test.label);
  test.StaticFrames(0u);
  test.Focus(true);
  test.Drain();
  CheckScaledMarqueeMotion(test, 1.0f);
  test.Focus(false);
  test.Drain();
  test.StaticFrames(1u);
}
} // namespace

int UtcDaliMarqueeUiScaleSyncHorizontalP(void)
{
  CheckMarqueeUiScaleMatrix(false, false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleSyncVerticalP(void)
{
  CheckMarqueeUiScaleMatrix(false, true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleAsyncHorizontalP(void)
{
  CheckMarqueeUiScaleMatrix(true, false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleAsyncVerticalP(void)
{
  CheckMarqueeUiScaleMatrix(true, true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleSyncPolicyP(void)
{
  CheckMarqueeUiScalePolicy(false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleAsyncPolicyP(void)
{
  CheckMarqueeUiScalePolicy(true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleSyncRuntimeHorizontalP(void)
{
  CheckMarqueeUiScaleRuntime(false, false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleSyncRuntimeVerticalP(void)
{
  CheckMarqueeUiScaleRuntime(false, true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleAsyncRuntimeHorizontalP(void)
{
  CheckMarqueeUiScaleRuntime(true, false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleAsyncRuntimeVerticalP(void)
{
  CheckMarqueeUiScaleRuntime(true, true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleManualBeforeMeasureP(void)
{
  CheckMarqueeUiScaleManualSnapshot(false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleManualAfterMeasureP(void)
{
  CheckMarqueeUiScaleManualSnapshot(true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleSyncHorizontalOptionsP(void)
{
  CheckMarqueeUiScaleOptions(false, false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleSyncVerticalOptionsP(void)
{
  CheckMarqueeUiScaleOptions(false, true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleAsyncHorizontalOptionsP(void)
{
  CheckMarqueeUiScaleOptions(true, false);
  END_TEST;
}
int UtcDaliMarqueeUiScaleAsyncVerticalOptionsP(void)
{
  CheckMarqueeUiScaleOptions(true, true);
  END_TEST;
}
int UtcDaliMarqueeUiScaleDirectionP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  for(bool async : {false, true})
    for(const char* text : {"مرحبا بالعالم هذا نص طويل لاختبار التمرير", "English مرحبا 123 mixed text with a long final section"})
    {
      Lifecycle test(app, async);
      test.label.SetText(text);
      test.Start();
      CheckScaledMarqueeMotion(test, 2.0f);
    }
  END_TEST;
}
namespace
{
void CheckMarqueeUiScaleFinitePublication(int loops, bool vertical)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  FinitePublicationBudget(app, loops, vertical);
}
} // namespace

int UtcDaliMarqueeUiScaleFinitePublicationInfiniteHorizontalP(void)
{
  CheckMarqueeUiScaleFinitePublication(0, false);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationInfiniteVerticalP(void)
{
  CheckMarqueeUiScaleFinitePublication(0, true);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationOneHorizontalP(void)
{
  CheckMarqueeUiScaleFinitePublication(1, false);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationOneVerticalP(void)
{
  CheckMarqueeUiScaleFinitePublication(1, true);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationTwoHorizontalP(void)
{
  CheckMarqueeUiScaleFinitePublication(2, false);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationTwoVerticalP(void)
{
  CheckMarqueeUiScaleFinitePublication(2, true);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationThreeHorizontalP(void)
{
  CheckMarqueeUiScaleFinitePublication(3, false);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationThreeVerticalP(void)
{
  CheckMarqueeUiScaleFinitePublication(3, true);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationThirtyTwoHorizontalP(void)
{
  CheckMarqueeUiScaleFinitePublication(32, false);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinitePublicationThirtyTwoVerticalP(void)
{
  CheckMarqueeUiScaleFinitePublication(32, true);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFinishLoopP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  for(bool async : {false, true})
    for(bool vertical : {false, true})
      for(unsigned phase = 0u; phase < 4u; ++phase) FinishLoopAt(app, phase, async, vertical, 3);
  END_TEST;
}
int UtcDaliMarqueeUiScaleFinishedPendingP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  for(bool vertical : {false, true}) FinishedBeforePublication(app, vertical);
  END_TEST;
}
int UtcDaliMarqueeUiScaleSessionRestartP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  for(unsigned ending = 0u; ending < 5u; ++ending) ReducedBudgetSessionEnd(app, ending);
  END_TEST;
}
int UtcDaliMarqueeUiScaleFocusStopBeforeRelayoutP(void)
{
  CheckMarqueeUiScaleFocusStop(0u);
  END_TEST;
}
int UtcDaliMarqueeUiScaleFocusStopAfterRelayoutP(void)
{
  CheckMarqueeUiScaleFocusStop(1u);
  END_TEST;
}
int UtcDaliMarqueeUiScaleFocusStopAfterStaticP(void)
{
  CheckMarqueeUiScaleFocusStop(2u);
  END_TEST;
}
int UtcDaliMarqueeUiScaleInvalidInputP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  Lifecycle test(app);
  test.Start();
  auto original = test.Current();
  for(float invalid : {0.0f, -1.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::denorm_min()})
  {
    Ui::Extension::LayoutScaleManager::Get().SetUiScale(invalid);
    test.Drain();
    DALI_TEST_CHECK(test.Current() == original);
    DALI_TEST_EQUALS(Ui::Extension::LayoutScaleManager::Get().GetUiScale(), 2.0f, TEST_LOCATION);
  }
  CheckScaledMarqueeMotion(test, 2.0f);
  END_TEST;
}

int UtcDaliMarqueeUiScaleFocusRapidP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  StopRace            test(app);
  std::vector<Task>   tasks;
  const auto          completed = test.completed;
  for(bool focused : {true, false, true, false})
  {
    test.Focus(focused);
    Ui::Extension::LayoutScaleManager::Get().SetUiScale(focused ? 2.0f : 1.0f);
    tasks.push_back(test.Request(focused));
    test.Raster(tasks.back());
  }
  test.Deliver(tasks.back());
  StopPublication latest(test.label);
  for(unsigned i = 0u; i + 1u < tasks.size(); ++i)
  {
    test.Deliver(tasks[i]);
    latest.Check(test.label);
    test.Stopped(0u);
    DALI_TEST_EQUALS(test.completed, completed + 1u, TEST_LOCATION);
  }
  test.StaticFrames(0u);
  END_TEST;
}

int UtcDaliMarqueeUiScaleLocalScaleAndFontChangeP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  Ui::Extension::LayoutScaleManager::Get().SetUiScale(2.0f);
  for(bool async : {false, true})
  {
    Lifecycle test(app, async);
    test.Start();
    const auto animation = test.Current();
    const auto textures  = test.label.GetRendererAt(0u).GetTextures();
    test.label.SetProperty(Actor::Property::SCALE, Vector3::ONE * 1.5f);
    test.Drain();
    DALI_TEST_CHECK(test.Take().empty());
    DALI_TEST_CHECK(test.Current() == animation);
    DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == textures);
    CheckScaledMarqueeMotion(test, 2.0f);
    DALI_TEST_EQUALS(test.label.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE), Vector3::ONE * 1.5f, 0.0001f, TEST_LOCATION);
    for(float fontScale : {2.0f, 1.0f})
    {
      test.label.SetSystemFontSizeScaleEnabled(false);
      test.label.SetMinimumFontSizeScale(fontScale);
      test.label.SetMaximumFontSizeScale(fontScale);
      test.Drain();
      CheckScaledMarqueeMotion(test, 2.0f);
    }
  }
  END_TEST;
}

int UtcDaliMarqueeUiScaleResourcesP(void)
{
  UiTestApplication   app;
  MarqueeUiScaleGuard guard;
  for(bool async : {false, true})
    for(float uiScale : {1.0f, 2.0f})
    {
      Ui::Extension::LayoutScaleManager::Get().SetUiScale(uiScale);
      Lifecycle test(app, async);
      test.Start();
      const auto renderer = test.label.GetRendererAt(0u);
      const auto geometry = renderer.GetGeometry();
      const auto created  = test.animationsCreated;
      // Sync retires for the size change and rebuilds for the changed content.
      const unsigned replacements = async ? 1u : 2u;
      for(unsigned update = 0u; update < 8u; ++update)
      {
        test.label.SetRequestedWidth(update % 2u ? 180.0f : 160.0f);
        test.label.SetText(update % 2u ? LONG_TEXT : "A different long text for the same repeated publication resource workload");
        test.Drain();
        DALI_TEST_CHECK(test.label.GetRendererAt(0u) == renderer);
        DALI_TEST_CHECK(renderer.GetGeometry() == geometry);
        DALI_TEST_EQUALS(test.animationsCreated, created + (update + 1u) * replacements, TEST_LOCATION);
        DALI_TEST_EQUALS(DevelAnimation::GetAnimationCount(), 1u, TEST_LOCATION);
      }
      const auto textures  = renderer.GetTextures();
      const auto completed = test.completed;
      test.Advance(100u);
      test.Drain();
      DALI_TEST_CHECK(test.Take().empty());
      DALI_TEST_CHECK(renderer.GetTextures() == textures);
      DALI_TEST_EQUALS(test.completed, completed, TEST_LOCATION);
      DALI_TEST_EQUALS(test.animationsCreated, created + 8u * replacements, TEST_LOCATION);
      test.label.Unparent();
      test.label.Reset();
      test.Frame();
      DALI_TEST_EQUALS(DevelAnimation::GetAnimationCount(), 0u, TEST_LOCATION);
    }
  END_TEST;
}

// Dispatch-time publication and callback ownership regressions.

static void Process(Task t)
{
  DALI_TEST_CHECK(t && t->IsReady());
  t->Process();
}

static void SetterWindow(UiTestApplication& app, bool manual)
{
  Lifecycle t(app);
  auto&     impl         = static_cast<Ui::Integration::LabelImpl&>(t.label.GetImplementation());
  unsigned  intermediate = 0;
  for(unsigned i = 0; i < 24; ++i)
  {
    t.label.SetText((std::string("Older B ") + std::to_string(i)).c_str());
    Task b;
    if(manual)
    {
      impl.RequestAsyncRenderWithFixedSize(170, 48);
      auto ts = t.Take();
      DALI_TEST_CHECK(ts.size() == 1);
      b = ts.front();
    }
    else
      b = t.Dispatch();
    Process(b);
    unsigned before = t.completed;
    t.label.SetText((std::string("Final C ") + std::to_string(i)).c_str());
    t.label.SetRequestedWidth(160.0f + i);
    t.Deliver(b);
    intermediate += t.completed - before;
    Task c = t.Dispatch();
    DALI_TEST_CHECK(c->mParameters.text == std::string("Final C ") + std::to_string(i));
    Process(c);
    before = t.completed;
    t.Deliver(c);
    DALI_TEST_CHECK(t.completed == before + 1);
    auto final = t.label.GetRendererAt(0).GetTextures();
    t.Deliver(b);
    t.Frame();
    DALI_TEST_CHECK(t.label.GetRendererAt(0).GetTextures() == final);
    DALI_TEST_CHECK(t.Take().empty());
  }
  tet_printf("RESULT updates=24 intermediate=%u final_loss=0 manual=%d\n", intermediate, manual);
}
static void Stop(UiTestApplication& app, bool automatic, bool running, bool textFirst)
{
  FocusLifecycle t(app, 1, automatic);
  t.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  t.label.StopMarquee();
  t.Drain();
  t.Focus(true);
  Task b = t.Dispatch();
  Process(b);
  if(running)
  {
    t.Deliver(b);
    t.Frame();
    t.Advance(20);
    t.label.SetText("A pending long moving update before focus is lost and new static text arrives");
    b = t.Dispatch();
    Process(b);
  }
  const auto created   = t.animationsCreated;
  unsigned   completed = t.completed;
  if(textFirst) t.label.SetText("Final long content C after focus out, which must not restore an old animation");
  t.Focus(false);
  DALI_TEST_CHECK(!t.label.IsMarqueeRunning());
  if(!textFirst) t.label.SetText("Final long content C after focus out, which must not restore an old animation");
  auto old = t.label.GetRendererAt(0).GetTextures();
  t.Deliver(b);
  const bool restarted = t.label.IsMarqueeRunning();
  tet_printf("OBS stop auto=%d running=%d text_first=%d old_callbacks=%u old_texture_changed=%d old_animation_restart=%d created_delta=%u\n", automatic, running, textFirst, t.completed - completed, t.label.GetRendererAt(0).GetTextures() != old, restarted, t.animationsCreated - created);
  DALI_TEST_CHECK(!restarted);
  DALI_TEST_CHECK(t.animationsCreated == created);
  t.Drain();
  // ON_OVERFLOW deliberately reevaluates on new content; log separately from an old completion.
  tet_printf("OBS latest_C_running=%d auto=%d\n", t.label.IsMarqueeRunning(), automatic);
  if(!automatic)
  {
    DALI_TEST_CHECK(!t.label.IsMarqueeRunning());
  }
  t.Focus(true);
  t.Drain();
  DALI_TEST_CHECK(t.label.IsMarqueeRunning());
  t.CheckTexture();
}
static void QueuedStop(UiTestApplication& app)
{
  Lifecycle t(app, true, false, false, 1);
  t.Start();
  t.label.SetRequestedWidth(160);
  Task b = t.Dispatch();
  Process(b);
  auto a = t.Current();
  UpdateCore(app, t.DurationMs() + 1);
  DALI_TEST_CHECK(t.finished == 0);
  t.label.StopMarquee();
  t.Deliver(b);
  t.Drain();
  DALI_TEST_CHECK(!t.label.IsMarqueeRunning());
  DALI_TEST_CHECK(t.finished == 1);
  t.Start();
  tet_printf("RESULT queued_stop_finished=%u restart=1\n", t.finished);
}

static void LateManual(UiTestApplication& app, unsigned api)
{
  Lifecycle t(app);
  auto&     impl           = static_cast<Ui::Integration::LabelImpl&>(t.label.GetImplementation());
  using Request            = void (Ui::Integration::LabelImpl::*)(float, float);
  const Request requests[] = {&Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedSize, &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedWidth, &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedHeight, &Ui::Integration::LabelImpl::RequestAsyncRenderWithConstraints};
  t.label.SetText("Manual B snapshot");
  (impl.*requests[api])(170, 48);
  auto tasks = t.Take();
  DALI_TEST_CHECK(tasks.size() == 1);
  Task b = tasks.front();
  b->Process();
  t.label.SetText("Latest automatic C must eventually publish after the earlier manual request");
  t.Frame();
  tasks                 = t.Take();
  const unsigned before = t.completed;
  t.Deliver(b);
  unsigned currentPublished = 0;
  for(unsigned round = 0; round < 8; ++round)
  {
    for(auto task : tasks)
    {
      const auto count = t.completed;
      t.Complete(task);
      if(t.completed > count && task->mParameters.text.find("Latest automatic C") == 0) ++currentPublished;
    }
    t.Frame();
    tasks = t.Take();
  }
  tet_printf("RESULT manual_api=%u current_publications=%u callbacks=%u task_queue=%zu\n", api, currentPublished, t.completed - before, tasks.size());
  DALI_TEST_CHECK(currentPublished == 1);
  DALI_TEST_CHECK(tasks.empty());
}
static void RestartStatic(UiTestApplication& app, bool vertical)
{
  Lifecycle t(app, true, vertical);
  t.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  t.Start();
  t.label.StopMarquee();
  Task b = t.Dispatch();
  b->Process();
  t.label.StartMarquee();
  unsigned count = t.completed;
  t.Deliver(b);
  unsigned intermediate = t.completed - count;
  t.Drain();
  DALI_TEST_CHECK(t.label.IsMarqueeRunning());
  auto created = t.animationsCreated;
  auto texture = t.label.GetRendererAt(0).GetTextures();
  t.Deliver(b);
  t.Frame();
  DALI_TEST_CHECK(t.label.IsMarqueeRunning());
  DALI_TEST_CHECK(t.animationsCreated == created);
  DALI_TEST_CHECK(t.label.GetRendererAt(0).GetTextures() == texture);
  tet_printf("RESULT restart_vertical=%d intermediate_static=%u final_running=1\n", vertical, intermediate);
}
static void AdjacentStop(UiTestApplication& app)
{
  Lifecycle t(app);
  t.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  t.Start();
  t.label.StopMarquee();
  Task b = t.Dispatch();
  b->Process();
  t.label.SetText("Latest static C after the stop request's relayout");
  t.Deliver(b);
  t.Drain();
  DALI_TEST_CHECK(!t.label.IsMarqueeRunning());
  t.CheckTexture();
  tet_printf("RESULT adjacent_static_final_stopped=1");
}
static void NoopStop(UiTestApplication& app, bool manual)
{
  Lifecycle  t(app);
  const auto original = t.label.GetRendererAt(0).GetTextures();
  const auto count    = t.completed;
  t.label.SetText("The last static result must survive a no-op marquee stop");
  Task task;
  if(manual)
  {
    auto& impl = static_cast<Ui::Integration::LabelImpl&>(t.label.GetImplementation());
    impl.RequestAsyncRenderWithFixedSize(137, 48);
    auto tasks = t.Take();
    DALI_TEST_CHECK(tasks.size() == 1u);
    task = tasks.front();
  }
  else
    task = t.Dispatch();
  task->Process();
  DALI_TEST_CHECK(!t.label.IsMarqueeRunning());
  for(unsigned i = 0u; i < 3u; ++i) t.label.StopMarquee();
  t.Deliver(task);
  t.Drain();
  tet_printf("RESULT noop_stop manual=%d completions=%u texture_changed=%d\n", manual, t.completed - count, t.label.GetRendererAt(0).GetTextures() != original);
  DALI_TEST_CHECK(t.completed == count + 1u);
  DALI_TEST_CHECK(t.label.GetRendererAt(0).GetTextures() != original);
  DALI_TEST_CHECK(!t.label.IsMarqueeRunning());
}



int UtcDaliAsyncPolicyIntermediateThenFinalP(void)
{
  UiTestApplication app;
  SetterWindow(app, false);
  END_TEST;
}

int UtcDaliAsyncPolicyManualIntermediateThenFinalP(void)
{
  UiTestApplication app;
  SetterWindow(app, true);
  END_TEST;
}



int UtcDaliAsyncPolicyFinishedQueuedStopP(void)
{
  UiTestApplication app;
  QueuedStop(app);
  END_TEST;
}

int UtcDaliAsyncPolicyManualFocusStopP(void)
{
  UiTestApplication app;
  Stop(app, false, false, false);
  END_TEST;
}

int UtcDaliAsyncPolicyManualFocusStopRunningP(void)
{
  UiTestApplication app;
  Stop(app, false, true, false);
  END_TEST;
}

int UtcDaliAsyncPolicyManualTextThenStopP(void)
{
  UiTestApplication app;
  Stop(app, false, false, true);
  END_TEST;
}

int UtcDaliAsyncPolicyOverflowStopThenTextP(void)
{
  UiTestApplication app;
  Stop(app, true, false, false);
  END_TEST;
}

int UtcDaliAsyncPolicyOverflowTextThenStopP(void)
{
  UiTestApplication app;
  Stop(app, true, false, true);
  END_TEST;
}

int UtcDaliAsyncPolicyOverflowRunningStopThenTextP(void)
{
  UiTestApplication app;
  Stop(app, true, true, false);
  END_TEST;
}

int UtcDaliAsyncPolicyManualFixedSizeThenAutomaticP(void)
{
  UiTestApplication app;
  LateManual(app, 0);
  END_TEST;
}

int UtcDaliAsyncPolicyManualFixedWidthThenAutomaticP(void)
{
  UiTestApplication app;
  LateManual(app, 1);
  END_TEST;
}

int UtcDaliAsyncPolicyManualFixedHeightThenAutomaticP(void)
{
  UiTestApplication app;
  LateManual(app, 2);
  END_TEST;
}

int UtcDaliAsyncPolicyManualConstraintsThenAutomaticP(void)
{
  UiTestApplication app;
  LateManual(app, 3);
  END_TEST;
}

int UtcDaliAsyncPolicyStaticCompletionBeforeRestartHorizontalP(void)
{
  UiTestApplication app;
  RestartStatic(app, false);
  END_TEST;
}

int UtcDaliAsyncPolicyStaticCompletionBeforeRestartVerticalP(void)
{
  UiTestApplication app;
  RestartStatic(app, true);
  END_TEST;
}

int UtcDaliAsyncPolicyAdjacentStopTextP(void)
{
  UiTestApplication app;
  AdjacentStop(app);
  END_TEST;
}

int UtcDaliAsyncPolicyNoopStopStaticAutomaticP(void)
{
  UiTestApplication app;
  NoopStop(app, false);
  END_TEST;
}

int UtcDaliAsyncPolicyNoopStopStaticManualP(void)
{
  UiTestApplication app;
  NoopStop(app, true);
  END_TEST;
}

int UtcDaliAsyncPolicyPendingTextNoopStopP(void)
{
  UiTestApplication app;
  Lifecycle         test(app);
  test.label.SetText("Intermediate B remains valid before the next dispatch");
  Task b = test.Dispatch();
  b->Process();
  const auto before = test.completed;
  test.label.SetText("Final C must replace B after an unrelated no-op Stop");
  test.label.StopMarquee();
  test.Deliver(b);
  DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
  Task c = test.Dispatch();
  test.Complete(c);
  const auto latest = test.label.GetRendererAt(0u).GetTextures();
  test.Deliver(b);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, before + 2u, TEST_LOCATION);
  DALI_TEST_CHECK(test.label.GetRendererAt(0u).GetTextures() == latest);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  END_TEST;
}

namespace
{
void CheckShaderReentry(unsigned action)
{
  UiTestApplication     app;
  Lifecycle             test(app);
  const auto            renderer         = test.label.GetRendererAt(0u);
  const auto            originalTextures = renderer.GetTextures();
  const auto            originalShader   = renderer.GetShader();
  const auto            originalSize     = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  const auto            before           = test.completed;
  WeakHandle<Ui::Label> weak(test.label);
  bool                  fired = false;
  TextureSet            nestedTextures;
  Shader                nestedShader;
  Vector2               nestedSize;
  app.GetCore().GetObjectRegistry().ObjectCreatedSignal().Connect(&test.tracker, [&](BaseHandle handle)
  {
    if(fired || !Shader::DownCast(handle)) return;
    fired = true;
    if(action == 3u)
    {
      test.label.Unparent();
      test.label.Reset();
      return;
    }
    if(action == 4u) test.label.Unparent();
    test.label.SetText("Final C from the shader observer");
    if(action == 1u)
    {
      test.label.SetTextUnderline(Ui::Text::Underline::None());
      auto& impl = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
      impl.RequestAsyncRenderWithFixedSize(137.0f, 48.0f);
      auto tasks = test.Take();
      DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
      test.Complete(tasks.front());
      nestedTextures = renderer.GetTextures();
      nestedShader   = renderer.GetShader();
      nestedSize     = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
    }
    if(action == 2u) test.label.SetAsyncRendering(false);
    if(action == 5u)
    {
      Ui::Gradient::Linear gradient(Vector2::ZERO, Vector2::XAXIS);
      gradient.SetStopNodes({Ui::Gradient::StopNode(0.0f, Ui::UiColor(Color::RED)), Ui::Gradient::StopNode(1.0f, Ui::UiColor(Color::BLUE))});
      test.label.SetTextGradient(gradient);
    }
  });
  test.label.SetTextUnderline(Ui::Text::Underline());
  test.label.SetRequestedWidth(140.0f);
  Task old = test.Dispatch();
  test.Complete(old);
  DALI_TEST_CHECK(fired);
  if(action == 3u)
  {
    old.Reset();
    test.Drain();
    DALI_TEST_CHECK(!weak.GetHandle());
    DALI_TEST_EQUALS(test.completed, before, TEST_LOCATION);
    return;
  }
  if(action == 1u)
  {
    DALI_TEST_CHECK(renderer.GetTextures() == nestedTextures);
    DALI_TEST_CHECK(renderer.GetShader() == nestedShader);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), nestedSize, TEST_LOCATION);
  }
  else if(action != 4u)
  {
    DALI_TEST_CHECK(renderer.GetTextures() == originalTextures);
    DALI_TEST_CHECK(renderer.GetShader() == originalShader);
    DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), originalSize, TEST_LOCATION);
    DALI_TEST_EQUALS(test.completed, before, TEST_LOCATION);
  }
  if(action == 4u) test.parent.Add(test.label);
  test.Drain();
  test.CheckTexture();
  DALI_TEST_EQUALS(test.completed, before + (action == 2u ? 0u : 1u), TEST_LOCATION);
  auto finalTextures = renderer.GetTextures();
  auto finalShader   = renderer.GetShader();
  auto finalSize     = renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE);
  test.Deliver(old);
  test.Drain();
  DALI_TEST_CHECK(renderer.GetTextures() == finalTextures);
  DALI_TEST_CHECK(renderer.GetShader() == finalShader);
  DALI_TEST_EQUALS(renderer.GetProperty<Vector2>(VisualRenderer::Property::TRANSFORM_SIZE), finalSize, TEST_LOCATION);
  DALI_TEST_EQUALS(test.completed, before + (action == 2u ? 0u : 1u), TEST_LOCATION);
}
} // namespace

int UtcDaliAsyncPolicyShaderTextP(void)
{
  CheckShaderReentry(0u);
  END_TEST;
}

int UtcDaliAsyncPolicyShaderNestedManualP(void)
{
  CheckShaderReentry(1u);
  END_TEST;
}

int UtcDaliAsyncPolicyShaderDisableAsyncP(void)
{
  CheckShaderReentry(2u);
  END_TEST;
}

int UtcDaliAsyncPolicyShaderDestroyP(void)
{
  CheckShaderReentry(3u);
  END_TEST;
}

int UtcDaliAsyncPolicyShaderDetachP(void)
{
  CheckShaderReentry(4u);
  END_TEST;
}

int UtcDaliAsyncPolicyShaderGradientP(void)
{
  CheckShaderReentry(5u);
  END_TEST;
}

namespace
{
void CheckSuppressedManualStop(unsigned api)
{
  UiTestApplication app;
  Lifecycle         test(app, true, false, true);
  test.label.SetMarqueeStopMode(Text::MarqueeStopMode::IMMEDIATE);
  test.label.StopMarquee();
  test.Drain();
  auto& impl               = static_cast<Ui::Integration::LabelImpl&>(test.label.GetImplementation());
  using Request            = void (Ui::Integration::LabelImpl::*)(float, float);
  const Request requests[] = {&Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedSize,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedWidth,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithFixedHeight,
                              &Ui::Integration::LabelImpl::RequestAsyncRenderWithConstraints};
  const auto    before     = test.completed;
  (impl.*requests[api])(137.0f, 48.0f);
  auto tasks = test.Take();
  DALI_TEST_EQUALS(tasks.size(), 1u, TEST_LOCATION);
  auto pending = tasks.front();
  pending->Process();
  for(unsigned i = 0u; i < 3u; ++i) test.label.StopMarquee();
  test.Deliver(pending);
  DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
  test.Drain();
  DALI_TEST_EQUALS(test.completed, before + 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!test.label.IsMarqueeRunning());
  test.CheckTexture();
}
} // namespace

int UtcDaliAsyncPolicySuppressedNoopFixedSizeP(void)
{
  CheckSuppressedManualStop(0u);
  END_TEST;
}

int UtcDaliAsyncPolicySuppressedNoopFixedWidthP(void)
{
  CheckSuppressedManualStop(1u);
  END_TEST;
}

int UtcDaliAsyncPolicySuppressedNoopFixedHeightP(void)
{
  CheckSuppressedManualStop(2u);
  END_TEST;
}

int UtcDaliAsyncPolicySuppressedNoopConstraintsP(void)
{
  CheckSuppressedManualStop(3u);
  END_TEST;
}
