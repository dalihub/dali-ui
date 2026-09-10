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
 *
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali/devel-api/adaptor-framework/vector-animation-renderer.h>
#include <dali-ui/ui-event-thread-callback.h>

using namespace Dali;
using namespace Dali::Ui;

namespace Test
{
namespace UiVectorAnimationRenderer
{
void     ResetLastSize();
uint32_t GetLastWidth();
uint32_t GetLastHeight();
} // namespace UiVectorAnimationRenderer
} // namespace Test

namespace
{
Dali::Property::Value TestFillColor(int32_t,
                                    Ui::LottieAnimation::ContentProperty,
                                    uint32_t)
{
  return Dali::Property::Value(Dali::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}

bool WasProcessEventsOnIdleRequested(UiTestApplication& application)
{
  return application.GetRenderController().WasCalled(TestRenderController::RequestProcessEventsOnIdleFunc);
}

// Delivers the idle ProcessEvents request that mounted/event-time work armed. Reset
// first, because the real adaptor consumes that wake as it enters ProcessEvents: any
// request observed afterwards was made by the pass itself.
void SendRequestedProcessEvents(UiTestApplication& application)
{
  DALI_TEST_CHECK(WasProcessEventsOnIdleRequested(application));
  application.GetRenderController().Initialize();
  application.SendNotification();
}

// Drives ProcessEvents for an unrelated external reason.
void SendIndependentProcessEvents(UiTestApplication& application)
{
  application.GetRenderController().Initialize();
  application.SendNotification();
}

struct WindowLayoutFinishedCounter
{
  explicit WindowLayoutFinishedCounter(int& count)
  : count(count)
  {
  }
  void operator()(Dali::Window)
  {
    ++count;
  }
  int& count;
};

// DALI_TEST_* throws on failure, so a test that assigns one of the globals below must
// clear it from a destructor: a plain trailing Reset() is skipped by a failing assertion
// and would leave a stale handle outliving the UiTestApplication.
struct ScopedGlobalHandleReset
{
  explicit ScopedGlobalHandleReset(Ui::LottieAnimationView& handle)
  : mHandle(handle)
  {
  }
  ~ScopedGlobalHandleReset()
  {
    mHandle.Reset();
  }
  Ui::LottieAnimationView& mHandle;
};

const char* const LOTTIE_TEST_URL = "animation.json";

Ui::LottieAnimationView gInPassLottie;
int                     gInPassProducerCount = 0;

MeasuredSize SetSameUrlDuringMeasure(View, float widthConstraint, float heightConstraint)
{
  ++gInPassProducerCount;
  if(gInPassLottie)
  {
    gInPassLottie.SetResourceUrl(LOTTIE_TEST_URL); // the anti-pattern under test
    gInPassLottie.Measure(widthConstraint, heightConstraint);
  }
  return MeasuredSize(120.0f, 120.0f);
}

MeasuredSize TouchVisualDuringMeasure(View, float widthConstraint, float heightConstraint)
{
  ++gInPassProducerCount;
  if(gInPassLottie)
  {
    // Unguarded setter -> Visual::Base::DoAction(UPDATE_PROPERTY) -> OnDoAction ->
    // TriggerVectorRasterization: a visual action raised from inside a measure pass.
    gInPassLottie.SetPixelArea(Dali::Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    gInPassLottie.Measure(widthConstraint, heightConstraint);
  }
  return MeasuredSize(120.0f, 120.0f);
}

Ui::LottieAnimationView gLayoutFinishedLottie;
int                     gLayoutFinishedSlotCount = 0;

struct PlayOnLayoutFinished
{
  void operator()(Dali::Window)
  {
    if(gLayoutFinishedSlotCount++ == 0 && gLayoutFinishedLottie)
    {
      gLayoutFinishedLottie.Play(); // post phase: this wake is deliberately NOT gated
    }
  }
};
} // namespace

void utc_dali_lottie_animation_view_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_lottie_animation_view_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLottieAnimationViewJumpToFramePreservedAfterDesiredSizeChange(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetSynchronousLoading(true);

  application.GetScene().Add(view);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetTotalFrameCount(), 5, TEST_LOCATION);

  int minFrame = -1;
  int maxFrame = -1;
  view.GetMinMaxFrame(minFrame, maxFrame);
  DALI_TEST_EQUALS(minFrame, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(maxFrame, 5, TEST_LOCATION);

  view.Stop();
  view.JumpToFrame(3);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 3, TEST_LOCATION);

  auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  auto  visualBeforeDesiredSizeChange = viewData.GetVisual(LottieAnimationView::Property::IMAGE);

  Test::UiVectorAnimationRenderer::ResetLastSize();
  view.SetDesiredWidth(200);
  view.SetDesiredHeight(200);
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 5), true, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastWidth(), 200u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastHeight(), 200u, TEST_LOCATION);

  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), visualBeforeDesiredSizeChange, TEST_LOCATION);

  view.SetResourceUrl("other-animation.json");
  view.Measure(100.0f, 100.0f);
  DALI_TEST_CHECK(viewData.GetVisual(LottieAnimationView::Property::IMAGE) != visualBeforeDesiredSizeChange);
  END_TEST;
}

int UtcDaliLottieAnimationViewSameResourceUrlKeepsVisual(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.Measure(100.0f, 100.0f);

  auto& viewData       = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  auto  originalVisual = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(originalVisual);

  view.SetResourceUrl("animation.json");
  view.Measure(100.0f, 100.0f);

  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), originalVisual, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewReloadRecreatesVisual(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.Measure(100.0f, 100.0f);

  auto& viewData       = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  auto  originalVisual = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(originalVisual);

  view.Reload();
  view.Measure(100.0f, 100.0f);

  DALI_TEST_CHECK(viewData.GetVisual(LottieAnimationView::Property::IMAGE) != originalVisual);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.json"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewReloadWithoutUrlIsNoOp(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  view.Reload();
  view.Measure(100.0f, 100.0f);

  auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  DALI_TEST_CHECK(!viewData.GetVisual(LottieAnimationView::Property::IMAGE));
  END_TEST;
}

int UtcDaliLottieAnimationViewDynamicPropertyRendersWhilePaused(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetSynchronousLoading(true);

  application.GetScene().Add(view);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(2), true, TEST_LOCATION);

  view.Play();
  view.Pause();
  DALI_TEST_EQUALS(view.GetPlayState(), Ui::AnimatedImage::PlayState::PAUSED, TEST_LOCATION);
  application.SendNotification();
  application.Render();

  Ui::LottieAnimation::DynamicProperty info(1,
                                               "**",
                                               Ui::LottieAnimation::ContentProperty::FILL_COLOR,
                                               Ui::LottieAnimation::DynamicPropertyCallback::New(&TestFillColor));
  view.SetDynamicProperty(std::move(info));
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 5), true, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewRuntimePropertiesDoNotRecreateVisual(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.Measure(100.0f, 100.0f);

  auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  auto  originalVisual = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(originalVisual);

  view.SetLoopCount(2);
  view.SetMinMaxFrame(1, 4);
  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::FIRST_FRAME);
  view.SetLoopingMode(Ui::LottieAnimation::LoopingMode::AUTO_REVERSE);
  view.SetFrameSpeedFactor(0.5f);
  view.SetRedrawOnScaleDown(false);
  view.SetRedrawOnScaleUp(false);
  view.SetNotifyAfterRasterizationEnabled(true);
  view.SetRenderScale(0.5f);
  view.SetAspectFitEnabled(false);
  view.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);
  view.SetSynchronousLoading(true);
  view.Measure(100.0f, 100.0f);

  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), originalVisual, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewSetSameUrlInMeasureDoesNotWakeIdle(void)
{
  UiTestApplication       application;
  Window                  window = application.GetWindow();
  ScopedGlobalHandleReset resetGlobal(gInPassLottie);
  tet_infoline("SetResourceUrl with the current URL from a measure producer neither rebuilds the visual nor wakes the main loop");

  gInPassProducerCount = 0;
  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  LottieAnimationView lottie = LottieAnimationView::New(LOTTIE_TEST_URL);
  gInPassLottie              = lottie;

  View host = View::New();
  host.SetRequestedWidth(200.0f);
  host.SetRequestedHeight(200.0f);
  host.SetMeasureCallback(MeasureCallback::New(&SetSameUrlDuringMeasure));
  host.Add(lottie);
  window.Add(host);

  // The event-time mount request wakes the first pass; that pass must not arm another.
  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gInPassProducerCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // The first pass CREATES the visual, and putting it on scene raises an in-pass
  // RelayoutRequest -> InvalidateMeasure, so this pass parks a follow-up instead of
  // settling. What matters is that it parks WITHOUT arming a wake (asserted above).
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);

  auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(lottie));
  auto  visual   = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  // An independently triggered cycle drains the parked follow-up. The same-URL setter
  // is now a no-op, so nothing is rebuilt, nothing re-parks, no wake is armed, and the
  // layout settles.
  SendIndependentProcessEvents(application);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), visual, TEST_LOCATION);

  // ...and it STAYS settled: a further cycle neither re-runs layout nor wakes.
  SendIndependentProcessEvents(application);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), visual, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLottieAnimationViewVisualCreatedInMeasureDoesNotWakeIdle(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A vector visual created inside a measure pass registers its rasterization without waking the main loop");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  LottieAnimationView lottie = LottieAnimationView::New(LOTTIE_TEST_URL);
  // A fixed desired size makes the natural size deterministic: the animation loads on the
  // vector thread, and without this the fitting transform applied at LayoutFinished could
  // observe a size that changed between the arrange and the emit and re-trigger a wake at
  // pass depth 0. Neither call creates the visual at event time: the visual is still built
  // by the first measure pass, which is what this test exercises.
  lottie.SetDesiredWidth(100);
  lottie.SetDesiredHeight(100);

  View host = View::New();
  host.SetRequestedWidth(200.0f);
  host.SetRequestedHeight(200.0f);
  host.Add(lottie);
  window.Add(host);

  // The event-time mount request wakes the first pass; that pass must not arm another.
  SendRequestedProcessEvents(application);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // The first pass CREATES the visual, and putting it on scene raises an in-pass
  // RelayoutRequest -> InvalidateMeasure, so this pass parks a follow-up instead of
  // settling. What matters is that it parks WITHOUT a wake even though DoSetProperties,
  // DoSetOnScene and OnSetTransform all reach TriggerVectorRasterization.
  DALI_TEST_EQUALS(emitCount, 0, TEST_LOCATION);

  auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(lottie));
  auto  visual   = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  // An independently triggered cycle drains the parked follow-up: nothing is rebuilt,
  // nothing re-parks, no wake is armed, and the layout settles.
  SendIndependentProcessEvents(application);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), visual, TEST_LOCATION);

  // ...and it STAYS settled: a further cycle neither re-runs layout nor wakes.
  SendIndependentProcessEvents(application);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), visual, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLottieAnimationViewVisualActionInMeasureDoesNotWakeIdle(void)
{
  UiTestApplication       application;
  Window                  window = application.GetWindow();
  ScopedGlobalHandleReset resetGlobal(gInPassLottie);
  tet_infoline("A visual action raised from inside a measure pass registers its rasterization without waking the main loop");

  gInPassProducerCount = 0;

  LottieAnimationView lottie = LottieAnimationView::New(LOTTIE_TEST_URL);
  gInPassLottie              = lottie;

  View host = View::New();
  host.SetRequestedWidth(200.0f);
  host.SetRequestedHeight(200.0f);
  host.SetMeasureCallback(MeasureCallback::New(&TouchVisualDuringMeasure));
  host.Add(lottie);
  window.Add(host);

  // First pass creates the visual (SetPixelArea is a no-op until then).
  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gInPassProducerCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));
  DALI_TEST_CHECK(Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(lottie)).GetVisual(LottieAnimationView::Property::IMAGE));

  // Re-arm a pass from EVENT time so the producer runs again with the visual in place.
  host.SetRequestedWidth(210.0f);
  SendRequestedProcessEvents(application);
  DALI_TEST_EQUALS(gInPassProducerCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  END_TEST;
}

int UtcDaliLottieAnimationViewPlayFromLayoutFinishedRequestsIdleWake(void)
{
  UiTestApplication       application;
  Window                  window = application.GetWindow();
  ScopedGlobalHandleReset resetGlobal(gLayoutFinishedLottie);
  tet_infoline("Play() from a LayoutFinished slot still wakes the main loop: only the pass half of the window is gated");

  gLayoutFinishedSlotCount = 0;

  LottieAnimationView lottie = LottieAnimationView::New(LOTTIE_TEST_URL);
  gLayoutFinishedLottie      = lottie;
  lottie.SetRequestedWidth(100.0f);
  lottie.SetRequestedHeight(100.0f);
  window.Add(lottie);

  // Baseline: settle with no slot connected -> the pass raises no wake of its own.
  SendRequestedProcessEvents(application);
  DALI_TEST_CHECK(!WasProcessEventsOnIdleRequested(application));

  // Now connect the slot and drive one more settled episode from event time.
  PlayOnLayoutFinished slot;
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, slot);
  lottie.SetRequestedWidth(120.0f);
  SendRequestedProcessEvents(application);

  DALI_TEST_EQUALS(gLayoutFinishedSlotCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(WasProcessEventsOnIdleRequested(application));

  END_TEST;
}
