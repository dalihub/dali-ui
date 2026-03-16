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

#include <dali-test-suite-utils.h>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/image-view/animated-image-view.h>

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
  TestApplication application;
  AnimatedImageView view;
  DALI_TEST_CHECK(!view);
  END_TEST;
}

int UtcDaliAnimatedImageViewNewP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewNewWithUrlP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New("test.gif");
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("test.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewCopyConstructorP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView copy(view);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliAnimatedImageViewMoveConstructorP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView moved(std::move(view));
  DALI_TEST_CHECK(moved);
  END_TEST;
}

int UtcDaliAnimatedImageViewAssignmentOperatorP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView copy;
  copy = view;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(view == copy);
  END_TEST;
}

int UtcDaliAnimatedImageViewMoveAssignmentOperatorP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  AnimatedImageView moved;
  moved = std::move(view);
  DALI_TEST_CHECK(moved);
  END_TEST;
}

// DownCast

int UtcDaliAnimatedImageViewDownCastP(void)
{
  TestApplication application;
  AnimatedImageView view     = AnimatedImageView::New();
  View              baseView = view;
  AnimatedImageView downcast = AnimatedImageView::DownCast(baseView);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliAnimatedImageViewDownCastN(void)
{
  TestApplication application;
  View              baseView = View::New();
  AnimatedImageView downcast = AnimatedImageView::DownCast(baseView);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// SetResourceUrl / GetResourceUrl

int UtcDaliAnimatedImageViewSetGetImageP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetResourceUrl("animation.gif");
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetImageNoChangeP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New("animation.gif");
  view.SetResourceUrl("animation.gif"); // same url — no update
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

// LoopCount

int UtcDaliAnimatedImageViewSetGetLoopCountP(void)
{
  TestApplication application;
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
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.SetLoopCount(5);
  view.SetLoopCount(5); // same value — no update
  DALI_TEST_EQUALS(view.GetLoopCount(), 5, TEST_LOCATION);
  END_TEST;
}

// ImageColor

int UtcDaliAnimatedImageViewSetGetImageColorP(void)
{
  TestApplication application;
  AnimatedImageView view  = AnimatedImageView::New();
  UiColor           color(1.0f, 0.5f, 0.5f, 1.0f);
  view.SetImageColor(color);
  DALI_TEST_EQUALS(view.GetImageColor().Resolve(), color.Resolve(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewSetImageColorNoChangeP(void)
{
  TestApplication application;
  AnimatedImageView view  = AnimatedImageView::New();
  UiColor           white(Color::WHITE);
  view.SetImageColor(white); // same as default — no update
  DALI_TEST_EQUALS(view.GetImageColor().Resolve(), white.Resolve(), TEST_LOCATION);
  END_TEST;
}

// Play / Pause / Stop (no visual — should not crash)

int UtcDaliAnimatedImageViewPlayNoVisualP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Play();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewPauseNoVisualP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Pause();
  DALI_TEST_CHECK(view);
  END_TEST;
}

int UtcDaliAnimatedImageViewStopNoVisualP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();
  view.Stop();
  DALI_TEST_CHECK(view);
  END_TEST;
}

// GetLoadingStatus

int UtcDaliAnimatedImageViewGetLoadingStatusP(void)
{
  TestApplication application;
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
  TestApplication application;
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
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New();

  view.SetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX), // IMAGE
                   Dali::String("animation.gif"));
  Dali::Property::Value value = view.GetProperty(Dali::Property::Index(Dali::PROPERTY_REGISTRATION_START_INDEX));
  Dali::String url;
  DALI_TEST_CHECK(value.Get(url));
  DALI_TEST_EQUALS(url, Dali::String("animation.gif"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliAnimatedImageViewChainingP(void)
{
  TestApplication application;
  AnimatedImageView view = AnimatedImageView::New()
                             .SetResourceUrl("animation.gif")
                             .SetLoopCount(2)
                             .SetImageColor(UiColor(1.0f, 1.0f, 1.0f, 0.5f));
  DALI_TEST_CHECK(view);
  DALI_TEST_EQUALS(view.GetResourceUrl(), Dali::String("animation.gif"), TEST_LOCATION);
  DALI_TEST_EQUALS(view.GetLoopCount(), 2, TEST_LOCATION);
  END_TEST;
}
