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
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/animated-vector-image-visual-actions-integ.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
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
void     ResetLoadCount();
uint32_t GetLoadCount();
uint32_t GetDynamicPropertyCount();
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

struct ResourceReadyCounter
{
  int& count;
  void operator()(Ui::View)
  {
    ++count;
  }
};

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
  application.GetScene().Add(view);
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
  application.GetScene().Add(view);
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

int UtcDaliLottieAnimationViewReplacementKeepsRendererUntilReady(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetSynchronousLoading(true);
  view.SetDesiredWidth(100);
  view.SetDesiredHeight(100);
  application.GetScene().Add(view);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2));
  DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);

  // Both URL replacement and Reload must retain the displayed frame until the
  // replacement has a rasterized resource, not merely loaded metadata.
  for(int replacement = 0; replacement < 2; ++replacement)
  {
    Renderer previousRenderer = view.GetRendererAt(0u);
    if(replacement == 0)
    {
      view.SetResourceUrl("replacement.json");
    }
    else
    {
      view.Reload();
    }
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(view.GetRendererAt(0u) == previousRenderer);

    application.SendNotification();
    application.Render();
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2));
    DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::READY, TEST_LOCATION);
    DALI_TEST_EQUALS(view.GetRendererCount(), 1u, TEST_LOCATION);
    DALI_TEST_CHECK(view.GetRendererAt(0u) != previousRenderer);
  }

  // An empty URL has no replacement to wait for and must remove the old frame.
  view.SetResourceUrl("");
  DALI_TEST_EQUALS(view.GetRendererCount(), 0u, TEST_LOCATION);
  auto& data = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  DALI_TEST_CHECK(!data.GetVisual(LottieAnimationView::Property::IMAGE));
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
  application.GetScene().Add(view);
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
  lottie.SetSynchronousLoading(true);
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

  // Attachment already created the visual, so the first measure pass settles.
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);

  auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(lottie));
  auto  visual   = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  // An independently triggered cycle keeps the settled layout and the same visual.
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

int UtcDaliLottieAnimationViewVisualCreatedOnAttachDoesNotWakeDuringMeasure(void)
{
  UiTestApplication application;
  Window            window = application.GetWindow();
  tet_infoline("A vector visual created on attachment settles in the first measure pass without another wake");

  int                         emitCount = 0;
  WindowLayoutFinishedCounter counter(emitCount);
  LayoutController::Get(window).LayoutFinishedSignal().Connect(&application, counter);

  LottieAnimationView lottie = LottieAnimationView::New(LOTTIE_TEST_URL);
  lottie.SetSynchronousLoading(true);
  // Explicit dimensions are available even before attachment.
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

  // Visual creation and its invalidation happened at attachment, before this pass.
  DALI_TEST_EQUALS(emitCount, 1, TEST_LOCATION);

  auto& viewData = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(lottie));
  auto  visual   = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(visual);

  // A further cycle leaves the settled layout and visual unchanged.
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
  lottie.SetSynchronousLoading(true);
  gInPassLottie              = lottie;

  View host = View::New();
  host.SetRequestedWidth(200.0f);
  host.SetRequestedHeight(200.0f);
  host.SetMeasureCallback(MeasureCallback::New(&TouchVisualDuringMeasure));
  host.Add(lottie);
  window.Add(host);

  // The visual already exists when the first measure callback sets PixelArea.
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
  lottie.SetSynchronousLoading(true);
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

int UtcDaliLottieAnimationViewDeferredCommandsDoNotLoad(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetLoadCount();
  Test::UiVectorAnimationRenderer::ResetLastSize();
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetSynchronousLoading(true);
  view.SetDesiredWidth(64);
  view.SetDesiredHeight(32);
  view.SetMinMaxFrame(1, 4);
  view.Play();
  view.Pause();
  view.JumpToFrame(3);
  view.SetDynamicProperty(Ui::LottieAnimation::DynamicProperty(1, "**",
    Ui::LottieAnimation::ContentProperty::FILL_COLOR,
    Ui::LottieAnimation::DynamicPropertyCallback::New(&TestFillColor)));
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f));
  application.SendNotification();
  application.Render();

  auto& data = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  DALI_TEST_EQUALS(view.GetLoadPolicy(), Ui::Image::LoadPolicy::ATTACHED, TEST_LOCATION);
  DALI_TEST_CHECK(!data.GetVisual(LottieAnimationView::Property::IMAGE));
  // Readiness considers registered visuals only, as in ImageView and AnimatedImageView.
  DALI_TEST_CHECK(view.IsResourceReady());
  DALI_TEST_EQUALS(view.GetNaturalSize(), Vector3(64.0f, 32.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastWidth(), 0u, TEST_LOCATION);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2));
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastWidth(), 64u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastHeight(), 32u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPlayState(), Ui::AnimatedImage::PlayState::PAUSED, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetDynamicPropertyCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewHiddenAncestorDefersCreation(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetLoadCount();
  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::VISIBLE, false);
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetSynchronousLoading(true);
  view.SetRequestedWidth(WRAP_CONTENT);
  view.SetRequestedHeight(WRAP_CONTENT);
  parent.Add(view);
  application.GetScene().Add(parent);
  view.JumpToFrame(2);
  view.Play();
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f));
  application.SendNotification();
  application.Render();
  auto& data = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  DALI_TEST_CHECK(!data.GetVisual(LottieAnimationView::Property::IMAGE));
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetNaturalSize(), Vector3::ZERO, TEST_LOCATION);
  DALI_TEST_EQUALS(view.Measure(100.0f, 100.0f).GetWidth(), 0.0f, TEST_LOCATION);

  parent.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_CHECK(data.GetVisual(LottieAnimationView::Property::IMAGE));
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetNaturalSize(), Vector3(100.0f, 100.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(view.Measure(100.0f, 100.0f).GetWidth(), 100.0f, TEST_LOCATION);
  application.GetScene().Remove(parent);
  application.GetScene().Add(parent);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewHiddenReloadDefersNewResource(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetLoadCount();
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetSynchronousLoading(true);
  view.SetProperty(Actor::Property::VISIBLE, false);
  application.GetScene().Add(view);
  view.Reload();
  view.JumpToFrame(3);
  view.SetResourceUrl("replacement.json");
  view.Play();
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 0u, TEST_LOCATION);
  view.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  view.SetProperty(Actor::Property::VISIBLE, false);
  auto& data = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  auto previousVisual = data.GetVisual(LottieAnimationView::Property::IMAGE);
  view.Reload();
  DALI_TEST_CHECK(data.GetVisual(LottieAnimationView::Property::IMAGE) == previousVisual);
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  view.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_CHECK(data.GetVisual(LottieAnimationView::Property::IMAGE) != previousVisual);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewImmediateLoadsOffScene(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetLoadCount();
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetSynchronousLoading(true);
  view.SetProperty(Actor::Property::VISIBLE, false);
  view.SetProperty(LottieAnimationView::Property::LOAD_POLICY, static_cast<int>(Ui::Image::LoadPolicy::IMMEDIATE));
  DALI_TEST_EQUALS(view.GetLoadPolicy(), Ui::Image::LoadPolicy::IMMEDIATE, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetTotalFrameCount(), 5, TEST_LOCATION);
  view.SetLoadPolicy(Ui::Image::LoadPolicy::ATTACHED);
  view.Reload();
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  view.SetLoadPolicy(Ui::Image::LoadPolicy::IMMEDIATE);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationVisualLoadPolicyGuardsActions(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetLoadCount();
  Test::UiVectorAnimationRenderer::ResetLastSize();
  Dali::Property::Map map;
  map.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::LOTTIE_ANIMATION);
  map.Insert(Ui::Integration::ImageVisual::Property::URL, "animation.json");
  map.Insert(Ui::Integration::ImageVisual::Property::SYNCHRONOUS_LOADING, true);
  map.Insert("loadPolicy", "ATTACHED");
  auto visual = Ui::Integration::VisualFactory::Get().CreateVisual(map);
  visual.DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::JUMP_TO, 3);
  visual.DoAction(Ui::Integration::AnimatedVectorImageVisual::Action::PLAY, Dali::Property::Map());
  visual.CreatePropertyMap(map);
  DALI_TEST_EQUALS(map.Find(Ui::Integration::ImageVisual::Property::CONTENT_INFO)->Get<Dali::Property::Map>().Count(), 0u, TEST_LOCATION);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLastWidth(), 0u, TEST_LOCATION);

  View owner = View::New();
  owner.SetProperty(Actor::Property::VISIBLE, false);
  auto& data = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(owner));
  data.RegisterVisual(LottieAnimationView::Property::IMAGE, visual);
  application.GetScene().Add(owner);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 0u, TEST_LOCATION);
  owner.SetProperty(Actor::Property::VISIBLE, true);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  visual.CreatePropertyMap(map);
  DALI_TEST_EQUALS(map.Find(Ui::Integration::ImageVisual::Property::TOTAL_FRAME_COUNT)->Get<int>(), 5, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewAsyncDeferredLoadCompletes(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetLoadCount();
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.SetDesiredWidth(64);
  view.SetDesiredHeight(32);
  view.JumpToFrame(3);
  view.Pause();
  DALI_TEST_CHECK(!view.IsSynchronousLoading());
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 0u, TEST_LOCATION);
  application.GetScene().Add(view);
  view.Measure(100.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 100.0f, 100.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1)); // load metadata, then schedule pending commands
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(2)); // rasterization and upload
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetPlayState(), Ui::AnimatedImage::PlayState::PAUSED, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsResourceReady());
  END_TEST;
}

int UtcDaliLottieAnimationViewDeferredResourceUsesRegisteredVisualReadiness(void)
{
  UiTestApplication application;
  Test::UiVectorAnimationRenderer::ResetLoadCount();
  LottieAnimationView view = LottieAnimationView::New("invalid.json");
  view.SetSynchronousLoading(true);
  int readyCount = 0;
  view.ResourceReadySignal().Connect(&application, ResourceReadyCounter{readyCount});

  Dali::Property::Map map;
  map.Insert(Ui::Integration::Visual::Property::TYPE, Ui::Integration::InternalVisualType::COLOR);
  auto background = Ui::Integration::VisualFactory::Get().CreateVisual(map);
  auto& data = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  data.RegisterVisual(Ui::Integration::View::Property::BACKGROUND, background);
  // Deliver background completion while the Lottie view is still off scene.
  Ui::GetImplementation(background).ResourceReady(Ui::Visual::ResourceStatus::READY);
  // Background readiness may emit while the main visual is still deferred.
  DALI_TEST_EQUALS(readyCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsResourceReady());
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::PREPARING, TEST_LOCATION);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 0u, TEST_LOCATION);

  application.GetScene().Add(view);
  DALI_TEST_EQUALS(Test::UiVectorAnimationRenderer::GetLoadCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLoadingStatus(), Ui::Visual::ResourceStatus::FAILED, TEST_LOCATION);
  DALI_TEST_CHECK(view.IsResourceReady());
  DALI_TEST_CHECK(readyCount > 1);
  END_TEST;
}
