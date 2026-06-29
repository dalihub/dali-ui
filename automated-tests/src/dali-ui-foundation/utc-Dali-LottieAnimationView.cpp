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

#include <dali-ui-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/lottie-animation-view.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_lottieanimationview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_lottieanimationview_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Construction

int UtcDaliLottieAnimationViewConstructorP(void)
{
  UiTestApplication application;
  LottieAnimationView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliLottieAnimationViewNewP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliLottieAnimationViewNewWithUrlP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("test.json");
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliLottieAnimationViewCopyConstructorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  LottieAnimationView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliLottieAnimationViewMoveConstructorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  LottieAnimationView moved(std::move(view));
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliLottieAnimationViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  LottieAnimationView copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliLottieAnimationViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  LottieAnimationView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// DownCast

int UtcDaliLottieAnimationViewDownCastP(void)
{
  UiTestApplication application;
  LottieAnimationView view     = LottieAnimationView::New();
  View                baseView = view;
  LottieAnimationView downcast = LottieAnimationView::DownCast(baseView);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliLottieAnimationViewDownCastN(void)
{
  UiTestApplication application;
  View                baseView = View::New();
  LottieAnimationView downcast = LottieAnimationView::DownCast(baseView);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// SetResourceUrl

int UtcDaliLottieAnimationViewSetImageP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  view.SetResourceUrl("animation.json");
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Play / Pause / Stop (no visual — should not crash)

int UtcDaliLottieAnimationViewPlayNoVisualP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  view.Play();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliLottieAnimationViewPauseNoVisualP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  view.Pause();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliLottieAnimationViewStopNoVisualP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();
  view.Stop();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Property IMAGE set/get via Property::Index

int UtcDaliLottieAnimationViewPropertyImageP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  view.SetProperty(Dali::Ui::LottieAnimationView::Property::IMAGE, Dali::String("animation.json"));
  Dali::Property::Value value = view.GetProperty(Dali::Ui::LottieAnimationView::Property::IMAGE);
  Dali::String url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_EQUALS(url, Dali::String("animation.json"), TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Validation Tests - New Functionality
// =============================================================================

// LoopCount

int UtcDaliLottieAnimationViewSetGetLoopCountP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be -1
  DALI_TEST_EQUALS(view.GetLoopCount(), -1, TEST_LOCATION);

  view.SetLoopCount(3);
  DALI_TEST_EQUALS(view.GetLoopCount(), 3, TEST_LOCATION);

  view.SetLoopCount(0);
  DALI_TEST_EQUALS(view.GetLoopCount(), 0, TEST_LOCATION);
  END_TEST;
}

// GetResourceUrl

int UtcDaliLottieAnimationViewGetResourceUrlP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New("test.json");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("test.json"), TEST_LOCATION);

  view.SetResourceUrl("animation.json");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.json"), TEST_LOCATION);
  END_TEST;
}

// StopBehavior

int UtcDaliLottieAnimationViewSetGetStopBehaviorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be CURRENT_FRAME
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::CURRENT_FRAME, TEST_LOCATION);

  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::FIRST_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::FIRST_FRAME, TEST_LOCATION);

  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::LAST_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::LAST_FRAME, TEST_LOCATION);
  END_TEST;
}

// LoopingMode

int UtcDaliLottieAnimationViewSetGetLoopingModeP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be RESTART
  DALI_TEST_EQUALS(view.GetLoopingMode(), Ui::LottieAnimation::LoopingMode::RESTART, TEST_LOCATION);

  view.SetLoopingMode(Ui::LottieAnimation::LoopingMode::AUTO_REVERSE);
  DALI_TEST_EQUALS(view.GetLoopingMode(), Ui::LottieAnimation::LoopingMode::AUTO_REVERSE, TEST_LOCATION);

  view.SetLoopingMode(Ui::LottieAnimation::LoopingMode::RESTART);
  DALI_TEST_EQUALS(view.GetLoopingMode(), Ui::LottieAnimation::LoopingMode::RESTART, TEST_LOCATION);
  END_TEST;
}

// FrameSpeedFactor

int UtcDaliLottieAnimationViewSetGetFrameSpeedFactorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be 1.0f
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 1.0f, TEST_LOCATION);

  view.SetFrameSpeedFactor(0.5f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 0.5f, TEST_LOCATION);

  view.SetFrameSpeedFactor(2.0f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 2.0f, TEST_LOCATION);
  END_TEST;
}

// ImageColor

int UtcDaliLottieAnimationViewSetGetImageColorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be WHITE
  DALI_TEST_EQUALS(view.GetImageColor().GetRgba(), Color::WHITE, TEST_LOCATION);

  UiColor red(1.0f, 0.0f, 0.0f, 1.0f);
  view.SetImageColor(red);
  DALI_TEST_EQUALS(view.GetImageColor().GetRgba(), red.GetRgba(), TEST_LOCATION);
  END_TEST;
}

// DesiredWidth / DesiredHeight

int UtcDaliLottieAnimationViewSetGetDesiredWidthP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be 0
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 0, TEST_LOCATION);

  view.SetDesiredWidth(1920);
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 1920, TEST_LOCATION);

  view.SetDesiredWidth(640);
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 640, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewSetGetDesiredHeightP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be 0
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 0, TEST_LOCATION);

  view.SetDesiredHeight(1080);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 1080, TEST_LOCATION);

  view.SetDesiredHeight(480);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 480, TEST_LOCATION);
  END_TEST;
}

// ReleasePolicy

int UtcDaliLottieAnimationViewSetGetReleasePolicyP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be DETACHED
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DETACHED, TEST_LOCATION);

  view.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::NEVER, TEST_LOCATION);

  view.SetReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DESTROYED, TEST_LOCATION);
  END_TEST;
}

// SynchronousLoading

int UtcDaliLottieAnimationViewSetGetSynchronousLoadingP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.IsSynchronousLoading());

  view.SetSynchronousLoading(true);
  DALI_TEST_CHECK(view.IsSynchronousLoading());

  view.SetSynchronousLoading(false);
  DALI_TEST_CHECK(!view.IsSynchronousLoading());
  END_TEST;
}

// JumpToFrame

int UtcDaliLottieAnimationViewJumpToFrameP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Should not crash even without visual
  view.JumpToFrame(10);
  DALI_TEST_CHECK(view);

  view.JumpToFrame(0);
  DALI_TEST_CHECK(view);

  view.JumpToFrame(50);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// SetMinMaxFrame

int UtcDaliLottieAnimationViewSetMinMaxFrameP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Should not crash even without visual
  view.SetMinMaxFrame(0, 30);
  DALI_TEST_CHECK(view);

  view.SetMinMaxFrame(10, 50);
  DALI_TEST_CHECK(view);

  view.SetMinMaxFrame(0, 100);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// SetMinMaxFrameByMarker

int UtcDaliLottieAnimationViewSetMinMaxFrameByMarkerP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Should not crash even without visual
  view.SetMinMaxFrameByMarker("marker1");
  DALI_TEST_CHECK(view);

  view.SetMinMaxFrameByMarker("marker1", "marker2");
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetPlayState

int UtcDaliLottieAnimationViewGetPlayStateP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Without visual, should return STOPPED
  DALI_TEST_EQUALS(view.GetPlayState(), Ui::AnimatedImage::PlayState::STOPPED, TEST_LOCATION);

  view.Play();
  DALI_TEST_CHECK(view);

  view.Pause();
  DALI_TEST_CHECK(view);

  view.Stop();
  DALI_TEST_EQUALS(view.GetPlayState(), Ui::AnimatedImage::PlayState::STOPPED, TEST_LOCATION);
  END_TEST;
}

// GetCurrentFrame / GetTotalFrame

int UtcDaliLottieAnimationViewGetCurrentFrameP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Without visual, should return 0
  DALI_TEST_EQUALS(view.GetCurrentFrame(), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewGetTotalFrameP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Without visual, should return 0
  DALI_TEST_EQUALS(view.GetTotalFrame(), 0, TEST_LOCATION);
  END_TEST;
}

// RedrawOnScaleDown / RedrawOnScaleUp

int UtcDaliLottieAnimationViewSetGetRedrawOnScaleDownP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be true
  DALI_TEST_CHECK(view.IsRedrawOnScaleDown());

  view.SetRedrawOnScaleDown(false);
  DALI_TEST_CHECK(!view.IsRedrawOnScaleDown());

  view.SetRedrawOnScaleDown(true);
  DALI_TEST_CHECK(view.IsRedrawOnScaleDown());
  END_TEST;
}

int UtcDaliLottieAnimationViewSetGetRedrawOnScaleUpP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be true
  DALI_TEST_CHECK(view.IsRedrawOnScaleUp());

  view.SetRedrawOnScaleUp(false);
  DALI_TEST_CHECK(!view.IsRedrawOnScaleUp());

  view.SetRedrawOnScaleUp(true);
  DALI_TEST_CHECK(view.IsRedrawOnScaleUp());
  END_TEST;
}

// EnableFrameCache

int UtcDaliLottieAnimationViewSetGetEnableFrameCacheP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.IsFrameCacheEnabled());

  view.SetFrameCacheEnabled(true);
  DALI_TEST_CHECK(view.IsFrameCacheEnabled());

  view.SetFrameCacheEnabled(false);
  DALI_TEST_CHECK(!view.IsFrameCacheEnabled());
  END_TEST;
}

// NotifyAfterRasterization

int UtcDaliLottieAnimationViewSetGetNotifyAfterRasterizationP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.IsNotifyAfterRasterizationEnabled());

  view.SetNotifyAfterRasterization(true);
  DALI_TEST_CHECK(view.IsNotifyAfterRasterizationEnabled());

  view.SetNotifyAfterRasterization(false);
  DALI_TEST_CHECK(!view.IsNotifyAfterRasterizationEnabled());
  END_TEST;
}

// RenderScale

int UtcDaliLottieAnimationViewSetGetRenderScaleP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be 1.0f
  DALI_TEST_EQUALS(view.GetRenderScale(), 1.0f, TEST_LOCATION);

  view.SetRenderScale(0.5f);
  DALI_TEST_EQUALS(view.GetRenderScale(), 0.5f, TEST_LOCATION);

  view.SetRenderScale(2.5f);
  DALI_TEST_EQUALS(view.GetRenderScale(), 2.5f, TEST_LOCATION);
  END_TEST;
}

// EnableAspectFit

int UtcDaliLottieAnimationViewSetGetEnableAspectFitP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Default should be true
  DALI_TEST_EQUALS(view.IsAspectFitEnabled(), true, TEST_LOCATION);

  view.SetAspectFitEnabled(false);
  DALI_TEST_EQUALS(view.IsAspectFitEnabled(), false, TEST_LOCATION);

  view.SetAspectFitEnabled(true);
  DALI_TEST_EQUALS(view.IsAspectFitEnabled(), true, TEST_LOCATION);
  END_TEST;
}

// GetContentInfo / GetMarkerInfo

int UtcDaliLottieAnimationViewGetContentInfoP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Should not crash even without visual
  Dali::Property::Map contentInfo = view.GetContentInfo();
  DALI_TEST_CHECK(contentInfo.Count() == 0); // Should be empty without visual
  END_TEST;
}

int UtcDaliLottieAnimationViewGetMarkerInfoP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Should not crash even without visual
  Dali::Property::Map markerInfo = view.GetMarkerInfo();
  DALI_TEST_CHECK(markerInfo.Count() == 0); // Should be empty without visual
  END_TEST;
}



// AnimationFinishedSignal

int UtcDaliLottieAnimationViewAnimationFinishedSignalP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  // Just call to ensure it doesn't crash
  view.AnimationFinishedSignal();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Property registration - all new properties

int UtcDaliLottieAnimationViewPropertyLoopCountP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::LOOP_COUNT;

  view.SetProperty(index, 5);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 5, TEST_LOCATION);

  view.SetProperty(index, -1);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyLoopingModeP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::LOOPING_MODE;

  view.SetProperty(index, static_cast<int>(Ui::LottieAnimation::LoopingMode::AUTO_REVERSE));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(),
                   static_cast<int>(Ui::LottieAnimation::LoopingMode::AUTO_REVERSE), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyStopBehaviorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::STOP_BEHAVIOR;

  view.SetProperty(index, static_cast<int>(Ui::AnimatedImage::StopBehavior::LAST_FRAME));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(),
                   static_cast<int>(Ui::AnimatedImage::StopBehavior::LAST_FRAME), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyFrameSpeedFactorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::FRAME_SPEED_FACTOR;

  view.SetProperty(index, 1.5f);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<float>(), 1.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyImageColorP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::IMAGE_COLOR;

  view.SetProperty(index, Vector4(1.0f, 0.5f, 0.5f, 1.0f));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<Vector4>(), Vector4(1.0f, 0.5f, 0.5f, 1.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyDesiredWidthP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::DESIRED_WIDTH;

  view.SetProperty(index, 1920);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 1920, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyDesiredHeightP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::DESIRED_HEIGHT;

  view.SetProperty(index, 1080);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 1080, TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyReleasePolicyP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::RELEASE_POLICY;

  view.SetProperty(index, static_cast<int>(Ui::Image::ReleasePolicy::NEVER));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(),
                   static_cast<int>(Ui::Image::ReleasePolicy::NEVER), TEST_LOCATION);
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertySynchronousLoadingP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::SYNCHRONOUS_LOADING;

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyRedrawInScalingDownP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::REDRAW_IN_SCALING_DOWN;

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyRedrawInScalingUpP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::REDRAW_IN_SCALING_UP;

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyEnableFrameCacheP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::ENABLE_FRAME_CACHE;

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyNotifyAfterRasterizationP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::NOTIFY_AFTER_RASTERIZATION;

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());
  END_TEST;
}

int UtcDaliLottieAnimationViewPropertyRenderScaleP(void)
{
  UiTestApplication application;
  LottieAnimationView view = LottieAnimationView::New();

  const int index = LottieAnimationView::Property::RENDER_SCALE;

  view.SetProperty(index, 1.5f);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<float>(), 1.5f, TEST_LOCATION);

  view.SetProperty(index, 0.2f);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<float>(), 0.2f, TEST_LOCATION);
  END_TEST;
}

