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
#include <dali-ui/ui-event-thread-callback.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/image/animated-image-view.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_animatedimageview_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_animatedimageview_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Construction

int UtcDaliAnimatedImageViewConstructorP(void)
{
  UiTestApplication application;
  AnimatedImageView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliAnimatedImageViewNewP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewNewWithUrlP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("test.gif");
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("test.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewGetNaturalSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("test.gif");
  view.SetDesiredWidth(64);
  view.SetDesiredHeight(32);
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);

  const AnimatedImageView constView(view);
  DALI_TEST_EQUALS(constView.GetNaturalSize(), Vector3(64.0f, 32.0f, 0.0f), TEST_LOCATION);

  view.Measure(200.0f, 100.0f);
  view.Arrange(LayoutRect(0.0f, 0.0f, 200.0f, 100.0f));

  DALI_TEST_EQUALS(view.GetSize(), Vector3(200.0f, 100.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(constView.GetNaturalSize(), Vector3(64.0f, 32.0f, 0.0f), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewMeasureExplicitSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("test.gif");
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(100.0f);

  const MeasuredSize measuredSize = view.Measure(400.0f, 300.0f);
  DALI_TEST_EQUALS(measuredSize.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measuredSize.height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewMeasureSingleExplicitDimensionP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("test.gif");
  view.SetDesiredWidth(64);
  view.SetDesiredHeight(32);
  view.SetRequestedWidth(200.0f);

  const MeasuredSize measuredSize = view.Measure(400.0f, 300.0f);
  DALI_TEST_EQUALS(measuredSize.width, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(measuredSize.height, 100.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewCopyConstructorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliAnimatedImageViewMoveConstructorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView moved(std::move(view));
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliAnimatedImageViewAssignmentOperatorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliAnimatedImageViewMoveAssignmentOperatorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// DownCast

int UtcDaliAnimatedImageViewDownCastP(void)
{
  UiTestApplication application;
  AnimatedImageView view     = AnimatedImageView::New();
  View              baseView = view;
  AnimatedImageView downcast = AnimatedImageView::DownCast(baseView);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliAnimatedImageViewDownCastN(void)
{
  UiTestApplication application;
  View              baseView = View::New();
  AnimatedImageView downcast = AnimatedImageView::DownCast(baseView);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// SetResourceUrl / GetResourceUrl

int UtcDaliAnimatedImageViewSetGetImageP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetResourceUrl("animation.gif");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetImageNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("animation.gif");
  view.SetResourceUrl("animation.gif"); // same url — no update
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetGetResourceUrlsP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  Dali::Vector<Dali::String> urls;
  urls.PushBack("frame1.png");
  urls.PushBack("frame2.png");
  urls.PushBack("frame3.png");

  view.SetResourceUrlList(urls);

  const auto result = view.GetResourceUrlList();
  DALI_TEST_EQUALS(result.Size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(result[0], Dali::String("frame1.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[1], Dali::String("frame2.png"), TEST_LOCATION);
  DALI_TEST_EQUALS(result[2], Dali::String("frame3.png"), TEST_LOCATION);

  // Setting URLs clears the single URL
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String(""), TEST_LOCATION);
  END_TEST;
}

// LoopCount

int UtcDaliAnimatedImageViewSetGetLoopCountP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  DALI_TEST_EQUALS(view.GetLoopCount(), -1, TEST_LOCATION); // default: infinite

  view.SetLoopCount(3);
  DALI_TEST_EQUALS(view.GetLoopCount(), 3, TEST_LOCATION);

  view.SetLoopCount(0);
  DALI_TEST_EQUALS(view.GetLoopCount(), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetLoopCountNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetLoopCount(5);
  view.SetLoopCount(5); // same value — no update
  DALI_TEST_EQUALS(view.GetLoopCount(), 5, TEST_LOCATION);
  END_TEST;
}

// ImageColor

int UtcDaliAnimatedImageViewSetGetImageColorP(void)
{
  UiTestApplication application;
  AnimatedImageView view  = AnimatedImageView::New();
  UiColor           color(1.0f, 0.5f, 0.5f, 1.0f);
  view.SetImageColor(color);
  DALI_TEST_EQUALS(view.GetImageColor().GetRgba(), color.GetRgba(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetImageColorNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view  = AnimatedImageView::New();
  UiColor           white(Color::WHITE);
  view.SetImageColor(white); // same as default — no update
  DALI_TEST_EQUALS(view.GetImageColor().GetRgba(), white.GetRgba(), TEST_LOCATION);
  END_TEST;
}

// Play / Pause / Stop (no visual — should not crash)

int UtcDaliAnimatedImageViewPlayNoVisualP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Play();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewPauseNoVisualP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Pause();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewStopNoVisualP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Stop();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetLoadingStatus

int UtcDaliAnimatedImageViewGetLoadingStatusP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("animation.gif");
  // Before scene add, status may be PREPARING, READY, or FAILED
  Ui::Visual::ResourceStatus status = view.GetLoadingStatus();
  DALI_TEST_CHECK(status == Ui::Visual::ResourceStatus::PREPARING ||
                  status == Ui::Visual::ResourceStatus::READY ||
                  status == Ui::Visual::ResourceStatus::FAILED);
  END_TEST;
}

// ResourceReadySignal

int UtcDaliAnimatedImageViewSignalsP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  // ResourceReadySignal is inherited from View — just call, don't store
  view.ResourceReadySignal();
  DALI_TEST_CHECK(true);
  END_TEST;
}

// Fluent chaining

// Property IMAGE set/get via Property::Index

int UtcDaliAnimatedImageViewPropertyImageP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  view.SetProperty(Dali::Ui::AnimatedImageView::Property::IMAGE, Dali::String("animation.gif"));
  Dali::Property::Value value = view.GetProperty(Dali::Ui::AnimatedImageView::Property::IMAGE);
  Dali::String url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_EQUALS(url, Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSettersP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetResourceUrl("animation.gif");
  view.SetLoopCount(2);
  view.SetImageColor(UiColor(1.0f, 1.0f, 1.0f, 0.5f));
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLoopCount(), 2, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// Validation Tests - New Functionality
// =============================================================================

// StopBehavior

int UtcDaliAnimatedImageViewSetGetStopBehaviorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be CURRENT_FRAME
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::CURRENT_FRAME, TEST_LOCATION);

  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::FIRST_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::FIRST_FRAME, TEST_LOCATION);

  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::LAST_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::LAST_FRAME, TEST_LOCATION);

  // Set back to default
  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::CURRENT_FRAME);
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::CURRENT_FRAME, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetStopBehaviorNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::LAST_FRAME);
  view.SetStopBehavior(Ui::AnimatedImage::StopBehavior::LAST_FRAME); // same value
  DALI_TEST_EQUALS(view.GetStopBehavior(), Ui::AnimatedImage::StopBehavior::LAST_FRAME, TEST_LOCATION);
  END_TEST;
}

// FrameSpeedFactor

int UtcDaliAnimatedImageViewSetGetFrameSpeedFactorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 1.0f
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 1.0f, TEST_LOCATION);

  view.SetFrameSpeedFactor(0.5f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 0.5f, TEST_LOCATION);

  view.SetFrameSpeedFactor(2.0f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 2.0f, TEST_LOCATION);

  // Reset to default
  view.SetFrameSpeedFactor(1.0f);
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 1.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetFrameSpeedFactorNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetFrameSpeedFactor(1.5f);
  view.SetFrameSpeedFactor(1.5f); // same value
  DALI_TEST_EQUALS(view.GetFrameSpeedFactor(), 1.5f, TEST_LOCATION);
  END_TEST;
}

// BatchSize

int UtcDaliAnimatedImageViewSetGetBatchSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 1
  DALI_TEST_EQUALS(view.GetBatchSize(), 1, TEST_LOCATION);

  view.SetBatchSize(3);
  DALI_TEST_EQUALS(view.GetBatchSize(), 3, TEST_LOCATION);

  view.SetBatchSize(10);
  DALI_TEST_EQUALS(view.GetBatchSize(), 10, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetBatchSizeNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetBatchSize(5);
  view.SetBatchSize(5); // same value
  DALI_TEST_EQUALS(view.GetBatchSize(), 5, TEST_LOCATION);
  END_TEST;
}

// CacheSize

int UtcDaliAnimatedImageViewSetGetCacheSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 1
  DALI_TEST_EQUALS(view.GetCacheSize(), 1, TEST_LOCATION);

  view.SetCacheSize(5);
  DALI_TEST_EQUALS(view.GetCacheSize(), 5, TEST_LOCATION);

  view.SetCacheSize(20);
  DALI_TEST_EQUALS(view.GetCacheSize(), 20, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetCacheSizeNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetCacheSize(10);
  view.SetCacheSize(10); // same value
  DALI_TEST_EQUALS(view.GetCacheSize(), 10, TEST_LOCATION);
  END_TEST;
}

// FrameDelay

int UtcDaliAnimatedImageViewSetGetFrameDelayP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be -1 (use file default)
  DALI_TEST_EQUALS(view.GetFrameDelay(), -1, TEST_LOCATION);

  view.SetFrameDelay(33); // 30 FPS
  DALI_TEST_EQUALS(view.GetFrameDelay(), 33, TEST_LOCATION);

  view.SetFrameDelay(100); // 10 FPS
  DALI_TEST_EQUALS(view.GetFrameDelay(), 100, TEST_LOCATION);

  // Reset to default
  view.SetFrameDelay(-1);
  DALI_TEST_EQUALS(view.GetFrameDelay(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetFrameDelayNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetFrameDelay(50);
  view.SetFrameDelay(50); // same value
  DALI_TEST_EQUALS(view.GetFrameDelay(), 50, TEST_LOCATION);
  END_TEST;
}

// DesiredWidth / DesiredHeight

int UtcDaliAnimatedImageViewSetGetDesiredWidthP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 0 (use natural size)
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 0, TEST_LOCATION);

  view.SetDesiredWidth(1920);
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 1920, TEST_LOCATION);

  view.SetDesiredWidth(640);
  DALI_TEST_EQUALS(view.GetDesiredWidth(), 640, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetGetDesiredHeightP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be 0 (use natural size)
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 0, TEST_LOCATION);

  view.SetDesiredHeight(1080);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 1080, TEST_LOCATION);

  view.SetDesiredHeight(480);
  DALI_TEST_EQUALS(view.GetDesiredHeight(), 480, TEST_LOCATION);
  END_TEST;
}

// ReleasePolicy

int UtcDaliAnimatedImageViewSetGetReleasePolicyP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be DETACHED
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DETACHED, TEST_LOCATION);

  view.SetReleasePolicy(Ui::Image::ReleasePolicy::NEVER);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::NEVER, TEST_LOCATION);

  view.SetReleasePolicy(Ui::Image::ReleasePolicy::DESTROYED);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DESTROYED, TEST_LOCATION);

  // Reset to default
  view.SetReleasePolicy(Ui::Image::ReleasePolicy::DETACHED);
  DALI_TEST_EQUALS(view.GetReleasePolicy(), Ui::Image::ReleasePolicy::DETACHED, TEST_LOCATION);
  END_TEST;
}

// SynchronousLoading

int UtcDaliAnimatedImageViewSetGetSynchronousLoadingP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.IsSynchronousLoading());

  view.SetSynchronousLoading(true);
  DALI_TEST_CHECK(view.IsSynchronousLoading());

  view.SetSynchronousLoading(false);
  DALI_TEST_CHECK(!view.IsSynchronousLoading());
  END_TEST;
}

// PreMultipliedAlpha

int UtcDaliAnimatedImageViewSetGetPreMultipliedAlphaP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.IsPreMultiplyAlphaOnLoadEnabled());

  view.SetPreMultiplyAlphaOnLoadEnabled(true);
  DALI_TEST_CHECK(view.IsPreMultiplyAlphaOnLoadEnabled());

  view.SetPreMultiplyAlphaOnLoadEnabled(false);
  DALI_TEST_CHECK(!view.IsPreMultiplyAlphaOnLoadEnabled());
  END_TEST;
}

// JumpToFrame

int UtcDaliAnimatedImageViewJumpToFrameP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Should not crash even without visual
  view.JumpToFrame(5);
  DALI_TEST_CHECK(view);

  view.JumpToFrame(0);
  DALI_TEST_CHECK(view);

  view.JumpToFrame(100);
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetPlayState

int UtcDaliAnimatedImageViewGetPlayStateP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

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

// GetCurrentFrameNumber / GetTotalFrameCount

int UtcDaliAnimatedImageViewGetCurrentFrameP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Without visual, should return 0
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewGetTotalFrameP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Without visual, should return 0
  DALI_TEST_EQUALS(view.GetTotalFrameCount(), 0, TEST_LOCATION);
  END_TEST;
}

// AnimationFinishedSignal

int UtcDaliAnimatedImageViewAnimationFinishedSignalP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Just call to ensure it doesn't crash
  view.AnimationFinishedSignal();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// Property registration - all new properties

int UtcDaliAnimatedImageViewPropertyLoopCountP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::LOOP_COUNT;

  view.SetProperty(index, 5);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 5, TEST_LOCATION);

  view.SetProperty(index, -1);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyStopBehaviorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::STOP_BEHAVIOR;

  view.SetProperty(index, static_cast<int>(Ui::AnimatedImage::StopBehavior::LAST_FRAME));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(),
                   static_cast<int>(Ui::AnimatedImage::StopBehavior::LAST_FRAME), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyFrameSpeedFactorP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::FRAME_SPEED_FACTOR;

  view.SetProperty(index, 1.5f);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<float>(), 1.5f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyBatchSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::BATCH_SIZE;

  view.SetProperty(index, 3);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyCacheSizeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::CACHE_SIZE;

  view.SetProperty(index, 5);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 5, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyFrameDelayP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::FRAME_DELAY;

  view.SetProperty(index, 33);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 33, TEST_LOCATION);

  view.SetProperty(index, -1);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), -1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyDesiredWidthP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::DESIRED_WIDTH;

  view.SetProperty(index, 1920);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 1920, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyDesiredHeightP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::DESIRED_HEIGHT;

  view.SetProperty(index, 1080);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(), 1080, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyReleasePolicyP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::RELEASE_POLICY;

  view.SetProperty(index, static_cast<int>(Ui::Image::ReleasePolicy::NEVER));
  DALI_TEST_EQUALS(view.GetProperty(index).Get<int>(),
                   static_cast<int>(Ui::Image::ReleasePolicy::NEVER), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertySynchronousLoadingP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::SYNCHRONOUS_LOADING;

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyPreMultipliedAlphaP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::PRE_MULTIPLY_ALPHA_ON_LOAD;

  view.SetProperty(index, true);
  DALI_TEST_CHECK(view.GetProperty(index).Get<bool>());

  view.SetProperty(index, false);
  DALI_TEST_CHECK(!view.GetProperty(index).Get<bool>());
  END_TEST;
}

// PixelArea

int UtcDaliAnimatedImageViewSetGetPixelAreaP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  Vector4 area(0.1f, 0.1f, 0.8f, 0.8f);
  view.SetPixelArea(area);
  DALI_TEST_EQUALS(view.GetPixelArea(), area, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewPropertyPixelAreaP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  const int index = AnimatedImageView::Property::PIXEL_AREA;

  Vector4 area(0.2f, 0.2f, 0.6f, 0.6f);
  view.SetProperty(index, area);
  DALI_TEST_EQUALS(view.GetProperty(index).Get<Vector4>(), area, TEST_LOCATION);
  END_TEST;
}

// =============================================================================
// AlphaMask
// =============================================================================

int UtcDaliAnimatedImageViewSetGetAlphaMaskUrlP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be empty
  DALI_TEST_EQUALS(view.GetAlphaMaskUrl(), Dali::String(""), TEST_LOCATION);

  view.SetAlphaMaskUrl("mask.png");
  DALI_TEST_EQUALS(view.GetAlphaMaskUrl(), Dali::String("mask.png"), TEST_LOCATION);

  view.SetAlphaMaskUrl("circle-mask.png");
  DALI_TEST_EQUALS(view.GetAlphaMaskUrl(), Dali::String("circle-mask.png"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetGetCropToMaskP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  // Default should be false
  DALI_TEST_CHECK(!view.IsCropToMask());

  view.SetCropToMask(true);
  DALI_TEST_CHECK(view.IsCropToMask());

  view.SetCropToMask(false);
  DALI_TEST_CHECK(!view.IsCropToMask());
  END_TEST;
}

int UtcDaliAnimatedImageViewSetGetMaskingModeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  view.SetMaskingPolicy(Ui::Image::MaskingPolicy::ON_LOADING);
  DALI_TEST_EQUALS(view.GetMaskingPolicy(), Ui::Image::MaskingPolicy::ON_LOADING, TEST_LOCATION);

  view.SetMaskingPolicy(Ui::Image::MaskingPolicy::ON_RENDERING);
  DALI_TEST_EQUALS(view.GetMaskingPolicy(), Ui::Image::MaskingPolicy::ON_RENDERING, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewAlphaMaskNoChangeP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  view.SetAlphaMaskUrl("mask.png");
  view.SetAlphaMaskUrl("mask.png"); // same value
  DALI_TEST_EQUALS(view.GetAlphaMaskUrl(), Dali::String("mask.png"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewRealGifPlaybackP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New("../samples/visual-base/res/animatedLoading.gif");
  view.SetSynchronousLoading(true);
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  DALI_TEST_CHECK(view.GetCurrentFrameNumber() >= 0);

  view.Play();
  DALI_TEST_EQUALS(view.GetPlayState(), AnimatedImage::PlayState::PLAYING, TEST_LOCATION);
  view.JumpToFrame(1);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 1, TEST_LOCATION);

  view.Pause();
  DALI_TEST_EQUALS(view.GetPlayState(), AnimatedImage::PlayState::PAUSED, TEST_LOCATION);
  view.Stop();
  DALI_TEST_EQUALS(view.GetPlayState(), AnimatedImage::PlayState::STOPPED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAnimatedImageViewImageSequenceCacheP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetResourceUrlList({
    "../samples/image-view/res/dog-anim-001.png",
    "../samples/image-view/res/dog-anim-002.png",
    "../samples/image-view/res/dog-anim-003.png",
    "../samples/image-view/res/dog-anim-004.png"});
  view.SetSynchronousLoading(true);
  view.SetBatchSize(2);
  view.SetCacheSize(2);
  view.SetFrameDelay(100);
  view.SetRequestedWidth(200.0f);
  view.SetRequestedHeight(200.0f);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  for(int attempt = 0; attempt < 4 && view.GetRendererCount() == 0u; ++attempt)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    application.SendNotification();
    application.Render();
  }
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 0, TEST_LOCATION);

  view.Play();
  DALI_TEST_EQUALS(view.GetPlayState(), AnimatedImage::PlayState::PLAYING, TEST_LOCATION);
  for(int frame = 1; frame < 4; ++frame)
  {
    view.JumpToFrame(frame);
    application.SendNotification();
    application.Render();
    DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), frame, TEST_LOCATION);
  }

  view.Stop();
  DALI_TEST_EQUALS(view.GetPlayState(), AnimatedImage::PlayState::STOPPED, TEST_LOCATION);

  view.SetStopBehavior(AnimatedImage::StopBehavior::FIRST_FRAME);
  view.Stop();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 0, TEST_LOCATION);

  view.SetStopBehavior(AnimatedImage::StopBehavior::LAST_FRAME);
  view.Stop();
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(view.GetCurrentFrameNumber(), 3, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewRealGifAlphaMaskPoliciesP(void)
{
  UiTestApplication application;
  const Dali::String gifUrl("../samples/visual-base/res/animatedLoading.gif");
  const Dali::String maskUrl("../samples/visual-base/res/mask.png");

  AnimatedImageView renderMasked = AnimatedImageView::New(gifUrl);
  renderMasked.SetAlphaMaskUrl(maskUrl);
  renderMasked.SetMaskingPolicy(Image::MaskingPolicy::ON_RENDERING);
  renderMasked.SetSynchronousLoading(true);
  renderMasked.SetRequestedWidth(160.0f);
  renderMasked.SetRequestedHeight(160.0f);
  application.GetScene().Add(renderMasked);

  AnimatedImageView loadMasked = AnimatedImageView::New(gifUrl);
  loadMasked.SetAlphaMaskUrl(maskUrl);
  loadMasked.SetMaskingPolicy(Image::MaskingPolicy::ON_LOADING);
  loadMasked.SetCropToMask(true);
  loadMasked.SetSynchronousLoading(true);
  loadMasked.SetRequestedWidth(160.0f);
  loadMasked.SetRequestedHeight(160.0f);
  application.GetScene().Add(loadMasked);

  application.SendNotification();
  application.Render();
  for(int attempt = 0; attempt < 4 &&
      (renderMasked.GetRendererCount() == 0u || loadMasked.GetRendererCount() == 0u); ++attempt)
  {
    DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, 5));
    application.SendNotification();
    application.Render();
  }

  DALI_TEST_CHECK(renderMasked.GetRendererCount() > 0u);
  DALI_TEST_CHECK(loadMasked.GetRendererCount() > 0u);
  DALI_TEST_EQUALS(renderMasked.GetRendererAt(0u).GetTextures().GetTextureCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(loadMasked.GetRendererAt(0u).GetTextures().GetTextureCount(), 1u, TEST_LOCATION);

  renderMasked.JumpToFrame(1);
  loadMasked.JumpToFrame(1);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(renderMasked.GetCurrentFrameNumber(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(loadMasked.GetCurrentFrameNumber(), 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewLoadingAndFittingOptionsP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetLoadPolicy(Image::LoadPolicy::ATTACHED);
  DALI_TEST_EQUALS(view.GetLoadPolicy(), Image::LoadPolicy::ATTACHED, TEST_LOCATION);
  view.SetSamplingMode(Image::SamplingMode::NEAREST);
  DALI_TEST_EQUALS(view.GetSamplingMode(), Image::SamplingMode::NEAREST, TEST_LOCATION);
  view.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  DALI_TEST_EQUALS(view.GetFittingMode(), Image::FittingMode::FIT_KEEP_ASPECT_RATIO, TEST_LOCATION);
  view.SetImageLoadWithViewSizeEnabled(true);
  DALI_TEST_CHECK(view.IsImageLoadWithViewSizeEnabled());
  view.SetPlaceholderUrl("../samples/image-view/res/sample.jpg");
  DALI_TEST_EQUALS(view.GetPlaceholderUrl(), Dali::String("../samples/image-view/res/sample.jpg"), TEST_LOCATION);

  view.SetResourceUrl("../samples/visual-base/res/animatedLoading.gif");
  view.SetSynchronousLoading(true);
  view.SetRequestedWidth(180.0f);
  view.SetRequestedHeight(120.0f);
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);

  view.SetFittingMode(Image::FittingMode::CENTER);
  DALI_TEST_EQUALS(view.GetFittingMode(), Image::FittingMode::CENTER, TEST_LOCATION);
  view.SetSamplingMode(Image::SamplingMode::LINEAR);
  DALI_TEST_EQUALS(view.GetSamplingMode(), Image::SamplingMode::LINEAR, TEST_LOCATION);
  view.SetLoadPolicy(Image::LoadPolicy::IMMEDIATE);
  DALI_TEST_EQUALS(view.GetLoadPolicy(), Image::LoadPolicy::IMMEDIATE, TEST_LOCATION);
  view.SetImageLoadWithViewSizeEnabled(false);
  DALI_TEST_CHECK(!view.IsImageLoadWithViewSizeEnabled());
  view.SetPlaceholderUrl("");
  DALI_TEST_EQUALS(view.GetPlaceholderUrl(), Dali::String(""), TEST_LOCATION);
  view.Reload();
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(view.GetRendererCount() > 0u);
  END_TEST;
}

int UtcDaliAnimatedImageViewRegisteredPropertyMatrixP(void)
{
  UiTestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  struct PropertyCase
  {
    Property::Index index;
    Property::Value value;
  };
  const PropertyCase cases[] = {
    {AnimatedImageView::Property::IMAGE, Dali::String("missing-frame.gif")},
    {AnimatedImageView::Property::LOOP_COUNT, 2},
    {AnimatedImageView::Property::IMAGE_COLOR, Vector4(0.5f, 0.6f, 0.7f, 1.0f)},
    {AnimatedImageView::Property::STOP_BEHAVIOR, static_cast<int>(AnimatedImage::StopBehavior::FIRST_FRAME)},
    {AnimatedImageView::Property::FRAME_SPEED_FACTOR, 1.5f},
    {AnimatedImageView::Property::BATCH_SIZE, 3},
    {AnimatedImageView::Property::CACHE_SIZE, 5},
    {AnimatedImageView::Property::FRAME_DELAY, 80},
    {AnimatedImageView::Property::DESIRED_WIDTH, 48},
    {AnimatedImageView::Property::DESIRED_HEIGHT, 32},
    {AnimatedImageView::Property::LOAD_POLICY, static_cast<int>(Image::LoadPolicy::ATTACHED)},
    {AnimatedImageView::Property::RELEASE_POLICY, static_cast<int>(Image::ReleasePolicy::DETACHED)},
    {AnimatedImageView::Property::SYNCHRONOUS_LOADING, false},
    {AnimatedImageView::Property::PRE_MULTIPLY_ALPHA_ON_LOAD, true},
    {AnimatedImageView::Property::FITTING_MODE, static_cast<int>(Image::FittingMode::FILL)},
    {AnimatedImageView::Property::SAMPLING_MODE, static_cast<int>(Image::SamplingMode::BOX_THEN_LINEAR)},
    {AnimatedImageView::Property::IMAGE_LOAD_WITH_VIEW_SIZE, true},
    {AnimatedImageView::Property::ALPHA_MASK_URL, Dali::String("missing-mask.png")},
    {AnimatedImageView::Property::CROP_TO_MASK, true},
    {AnimatedImageView::Property::MASKING_POLICY, static_cast<int>(Image::MaskingPolicy::ON_LOADING)},
    {AnimatedImageView::Property::PLACEHOLDER_IMAGE, Dali::String("missing-placeholder.png")},
    {AnimatedImageView::Property::PIXEL_AREA, Vector4(0.0f, 0.0f, 1.0f, 1.0f)},
  };
  for(const auto& property : cases)
  {
    view.SetProperty(property.index, property.value);
    DALI_TEST_CHECK(view.GetProperty(property.index).GetType() != Property::NONE);
  }

  Property::Array urls;
  urls.PushBack(Dali::String("first.png"));
  urls.PushBack(5);
  urls.PushBack(Dali::String("second.png"));
  view.SetProperty(AnimatedImageView::Property::IMAGE_URLS, urls);
  Property::Array result;
  DALI_TEST_CHECK(view.GetProperty(AnimatedImageView::Property::IMAGE_URLS).Get(result));
  DALI_TEST_EQUALS(result.Count(), 2u, TEST_LOCATION);
  END_TEST;
}
