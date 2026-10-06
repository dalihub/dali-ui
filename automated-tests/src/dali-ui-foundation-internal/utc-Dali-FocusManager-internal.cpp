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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/focus-manager/focus-finder.h>
#include <dali-ui-foundation/internal/focus-manager/focus-manager-impl.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
View gFallbackTarget;
int gFallbackCallCount = 0;

FocusNavigationResult MoveFallback(View, FocusNavigationContext)
{
  ++gFallbackCallCount;
  return FocusNavigationResult::MoveTo(gFallbackTarget);
}

FocusNavigationResult NotHandledFallback(View, FocusNavigationContext)
{
  ++gFallbackCallCount;
  return FocusNavigationResult::NotHandled();
}
} // namespace

void utc_dali_focusmanager_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_focusmanager_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFocusManagerFallbackRunsWithDefaultAlgorithmDisabledInternalP(void)
{
  UiTestApplication application;

  View current = View::New();
  current.SetFocusable(true);
  View fallbackTarget = View::New();
  fallbackTarget.SetFocusable(true);
  application.GetScene().Add(current);
  application.GetScene().Add(fallbackTarget);
  application.SendNotification();
  application.Render();

  FocusManager manager = FocusManager::Get();
  DALI_TEST_CHECK(manager.RequestFocus(current));
  GetImpl(manager).EnableDefaultAlgorithm(false);
  gFallbackTarget = fallbackTarget;
  gFallbackCallCount = 0;
  manager.SetFocusNavigationFallback(FocusNavigationCallback::New(&MoveFallback));

  DALI_TEST_CHECK(manager.MoveFocus(FocusDirection::RIGHT));
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == fallbackTarget);
  DALI_TEST_CHECK(gFallbackCallCount == 1);

  manager.SetFocusNavigationFallback(FocusNavigationCallback::New(&NotHandledFallback));
  DALI_TEST_CHECK(!manager.MoveFocus(FocusDirection::LEFT));
  DALI_TEST_CHECK(manager.GetCurrentFocusView() == fallbackTarget);
  DALI_TEST_CHECK(gFallbackCallCount == 2);

  GetImpl(manager).EnableDefaultAlgorithm(true);
  manager.SetFocusNavigationFallback({});
  gFallbackTarget.Reset();
  END_TEST;
}

int UtcDaliFocusFinderSpatialAndReadingOrderP(void)
{
  UiTestApplication application;
  Actor root = Actor::New();
  root.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  root.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  root.SetProperty(Actor::Property::ALLOW_DESCENDANT_FOCUS, true);
  root.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(root);

  auto makeView = [&root](float x, float y)
  {
    View view = View::New();
    view.SetRequestedWidth(40.0f);
    view.SetRequestedHeight(40.0f);
    view.SetFocusable(true);
    view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
    view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    view.SetProperty(Actor::Property::POSITION, Vector3(x, y, 0.0f));
    view.SetProperty(Actor::Property::SIZE, Vector2(40.0f, 40.0f));
    root.Add(view);
    return view;
  };

  View up = makeView(100.0f, 20.0f);
  View left = makeView(20.0f, 100.0f);
  View center = makeView(100.0f, 100.0f);
  View right = makeView(180.0f, 100.0f);
  View down = makeView(100.0f, 180.0f);
  application.SendNotification();
  application.Render();

  // The first layout pass establishes each View's size and resets its initial Actor position.
  left.SetProperty(Actor::Property::POSITION, Vector3(20.0f, 100.0f, 0.0f));
  center.SetProperty(Actor::Property::POSITION, Vector3(100.0f, 100.0f, 0.0f));
  right.SetProperty(Actor::Property::POSITION, Vector3(180.0f, 100.0f, 0.0f));
  up.SetProperty(Actor::Property::POSITION, Vector3(100.0f, 20.0f, 0.0f));
  down.SetProperty(Actor::Property::POSITION, Vector3(100.0f, 180.0f, 0.0f));
  application.SendNotification();
  application.Render();
  namespace Finder = Dali::Ui::Internal::FocusFinder;
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::LEFT), left, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT), right, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::UP), up, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::DOWN), down, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNextFocusableViewInOrder(root, center, FocusDirection::FORWARD), right, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNextFocusableViewInOrder(root, center, FocusDirection::BACKWARD), left, TEST_LOCATION);

  right.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT));
  DALI_TEST_CHECK(!Finder::GetNextFocusableViewInOrder(root, down, FocusDirection::FORWARD));
  DALI_TEST_CHECK(!Finder::GetNearestFocusableView(Actor(), center, FocusDirection::LEFT));
  DALI_TEST_CHECK(!Finder::GetNextFocusableViewInOrder(Actor(), center, FocusDirection::FORWARD));
  DALI_TEST_EQUALS(Finder::GetNextFocusableViewInOrder(root, View(), FocusDirection::FORWARD), up, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNextFocusableViewInOrder(root, View(), FocusDirection::BACKWARD), down, TEST_LOCATION);

  right.SetProperty(Actor::Property::VISIBLE, true);
  right.SetProperty(Actor::Property::ENABLED, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT));
  right.SetProperty(Actor::Property::ENABLED, true);
  right.SetProperty(Actor::Property::FOCUSABLE, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT));
  right.SetProperty(Actor::Property::FOCUSABLE, true);
  root.SetProperty(Actor::Property::ALLOW_DESCENDANT_FOCUS, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT));
  DALI_TEST_CHECK(!Finder::GetNextFocusableViewInOrder(root, center, FocusDirection::FORWARD));
  root.SetProperty(Actor::Property::ALLOW_DESCENDANT_FOCUS, true);
  root.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!Finder::GetNextFocusableViewInOrder(root, center, FocusDirection::FORWARD));
  DALI_TEST_CHECK(!Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT));
  root.SetProperty(Actor::Property::VISIBLE, true);
  View farLeft = makeView(0.0f, 100.0f);
  View farRight = makeView(300.0f, 100.0f);
  View farUp = makeView(100.0f, 0.0f);
  View farDown = makeView(100.0f, 300.0f);
  application.SendNotification();
  application.Render();
  auto place = [](View view, float x, float y)
  {
    view.SetProperty(Actor::Property::POSITION, Vector3(x, y, 0.0f));
  };
  place(left, 20.0f, 100.0f);
  place(center, 100.0f, 100.0f);
  place(right, 180.0f, 100.0f);
  place(up, 100.0f, 20.0f);
  place(down, 100.0f, 180.0f);
  place(farLeft, 0.0f, 100.0f);
  place(farRight, 300.0f, 100.0f);
  place(farUp, 100.0f, 0.0f);
  place(farDown, 100.0f, 300.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::LEFT), left, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT), right, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::UP), up, TEST_LOCATION);
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::DOWN), down, TEST_LOCATION);
  DALI_TEST_CHECK(Finder::GetNearestFocusableView(root, View(), FocusDirection::RIGHT));
  DALI_TEST_CHECK(Finder::GetNearestFocusableView(root, View(), FocusDirection::DOWN));
  DALI_TEST_CHECK(!Finder::GetNearestFocusableView(root, center, FocusDirection::FORWARD));
  View diagonalRight = makeView(260.0f, 260.0f);
  application.SendNotification();
  application.Render();
  diagonalRight.SetProperty(Actor::Property::POSITION, Vector3(260.0f, 260.0f, 0.0f));
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(Finder::GetNearestFocusableView(root, center, FocusDirection::RIGHT), right, TEST_LOCATION);
  END_TEST;
}
