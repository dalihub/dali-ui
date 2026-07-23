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
#include <dali-ui-foundation/public-api/views/image/lottie-animation-view.h>
#include <dali-ui-foundation/public-api/views/image/selectable-image-interface.h>
#include <dali-ui-foundation/public-api/views/image/selectable-lottie-animation-view.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/public-api/actors/actor.h>
#include <dali.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
using FrameRange = SelectableLottieImage::FrameRange;

SelectableLottieAnimationView NewGlyph()
{
  return SelectableLottieAnimationView::New(
    SelectableLottieImage("checkbox.json", FrameRange(0, 30), FrameRange(30, 48)));
}
} // namespace

void utc_dali_selectable_lottie_animation_view_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectable_lottie_animation_view_cleanup(void)
{
  test_return_value = TET_PASS;
}

// An uninitialized handle is empty.
int UtcDaliSelectableLottieAnimationViewConstructorP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

// New(SelectableLottieImage) produces an initialized handle.
int UtcDaliSelectableLottieAnimationViewNewP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetView() returns the composed drawing view (a LottieAnimationView) carrying the url.
int UtcDaliSelectableLottieAnimationViewGetViewP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  Ui::View drawingView = view.GetView();
  DALI_TEST_CHECK(drawingView);

  LottieAnimationView asLottie = LottieAnimationView::DownCast(drawingView);
  DALI_TEST_CHECK(asLottie);
  DALI_TEST_EQUALS(asLottie.GetResourceUrl(), std::string("checkbox.json"), TEST_LOCATION);

  END_TEST;
}

// DownCasts from a base handle through the SelectableImageInterface handle chain.
int UtcDaliSelectableLottieAnimationViewDownCastP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  BaseHandle                    base      = view;
  SelectableLottieAnimationView roundTrip = SelectableLottieAnimationView::DownCast(base);
  DALI_TEST_CHECK(roundTrip);

  SelectableImageInterface asInterface = SelectableImageInterface::DownCast(base);
  DALI_TEST_CHECK(asInterface);

  END_TEST;
}

// DownCast of an empty or unrelated handle yields an empty handle.
int UtcDaliSelectableLottieAnimationViewDownCastN(void)
{
  UiTestApplication application;

  DALI_TEST_CHECK(!SelectableLottieAnimationView::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!SelectableLottieAnimationView::DownCast(Actor::New()));

  END_TEST;
}

// SetSelected snap/animate variants run without a live visual (headless).
int UtcDaliSelectableLottieAnimationViewSetSelectedP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  view.SetSelected(true);
  DALI_TEST_CHECK(view);

  view.SetSelected(false, false);
  DALI_TEST_CHECK(view);

  view.SetSelected(true, true);
  DALI_TEST_CHECK(view);

  END_TEST;
}

// SetStateColors configures the glyph without crashing; IsTransitioning is false headless.
int UtcDaliSelectableLottieAnimationViewConfigureP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  view.SetStateColors(Vector4(0.f, 0.f, 0.f, 1.f), Vector4(1.f, 1.f, 1.f, 1.f));
  view.SetSelected(true, false);
  DALI_TEST_CHECK(!view.IsTransitioning());

  END_TEST;
}

// The transition-finished signal is accessible through the interface.
int UtcDaliSelectableLottieAnimationViewTransitionFinishedSignalP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  // Simply accessing the signal must not crash (it forwards the composed view's signal).
  (void)view.TransitionFinishedSignal();
  DALI_TEST_CHECK(view);

  END_TEST;
}

// Copy and move preserve the underlying image.
int UtcDaliSelectableLottieAnimationViewCopyMoveP(void)
{
  UiTestApplication             application;
  SelectableLottieAnimationView view = NewGlyph();

  SelectableLottieAnimationView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy.GetView());

  SelectableLottieAnimationView moved(std::move(copy));
  DALI_TEST_CHECK(moved);

  END_TEST;
}
