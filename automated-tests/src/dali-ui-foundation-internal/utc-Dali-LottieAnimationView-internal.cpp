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

#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>
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
                                    Dali::VectorAnimationRenderer::VectorProperty,
                                    uint32_t)
{
  return Dali::Property::Value(Dali::Vector4(1.0f, 0.0f, 0.0f, 1.0f));
}
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
  DALI_TEST_EQUALS(view.GetTotalFrame(), 5, TEST_LOCATION);

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
  DALI_TEST_EQUALS(view.GetCurrentFrame(), 3, TEST_LOCATION);

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

  DALI_TEST_EQUALS(view.GetCurrentFrame(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), visualBeforeDesiredSizeChange, TEST_LOCATION);

  view.SetResourceUrl("other-animation.json");
  view.Measure(100.0f, 100.0f);
  DALI_TEST_CHECK(viewData.GetVisual(LottieAnimationView::Property::IMAGE) != visualBeforeDesiredSizeChange);
  END_TEST;
}

int UtcDaliLottieAnimationViewSameResourceUrlReloadsVisual(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("animation.json");
  view.Measure(100.0f, 100.0f);

  auto& viewData       = Ui::Internal::ViewDataImpl::Get(Ui::GetImpl(view));
  auto  originalVisual = viewData.GetVisual(LottieAnimationView::Property::IMAGE);
  DALI_TEST_CHECK(originalVisual);

  view.SetResourceUrl("animation.json");
  view.Measure(100.0f, 100.0f);

  DALI_TEST_CHECK(viewData.GetVisual(LottieAnimationView::Property::IMAGE) != originalVisual);
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
  DALI_TEST_EQUALS(Test::WaitForEventThreadTrigger(1, 5), true, TEST_LOCATION);

  Ui::LottieAnimation::DynamicPropertyInfo info;
  info.id       = 1;
  info.keyPath  = "**";
  info.property = Ui::LottieAnimation::VectorProperty::FILL_COLOR;
  info.callback = MakeCallback(&TestFillColor);
  view.SetDynamicProperty(info);

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
  view.SetFrameCacheEnabled(true);
  view.SetNotifyAfterRasterization(true);
  view.SetRenderScale(0.5f);
  view.SetAspectFitEnabled(false);
  view.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);
  view.SetSynchronousLoading(true);
  view.Measure(100.0f, 100.0f);

  DALI_TEST_EQUALS(viewData.GetVisual(LottieAnimationView::Property::IMAGE), originalVisual, TEST_LOCATION);
  END_TEST;
}
