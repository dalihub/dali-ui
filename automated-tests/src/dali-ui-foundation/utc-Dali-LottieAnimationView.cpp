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

  view.SetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX), // IMAGE
                   Dali::String("animation.json"));
  Dali::Property::Value value = view.GetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX));
  Dali::String url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_EQUALS(url, Dali::String("animation.json"), TEST_LOCATION);
  END_TEST;
}

