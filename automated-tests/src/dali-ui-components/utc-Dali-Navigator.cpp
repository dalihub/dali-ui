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

#include <stdlib.h>
#include <iostream>
#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_navigator_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_navigator_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliNavigatorConstructorP(void)
{
  UiTestApplication application;
  Navigator         navigator;
  DALI_TEST_CHECK(!navigator);
  END_TEST;
}

int UtcDaliNavigatorNewP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();
  DALI_TEST_CHECK(navigator);
  END_TEST;
}

int UtcDaliNavigatorCopyConstructorP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();
  Navigator         copy(navigator);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(navigator == copy);
  END_TEST;
}

int UtcDaliNavigatorMoveConstructor(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();
  DALI_TEST_EQUALS(1, navigator.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Navigator moved = std::move(navigator);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!navigator);
  END_TEST;
}

int UtcDaliNavigatorAssignmentOperatorP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();
  Navigator         copy;
  copy = navigator;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(navigator == copy);
  END_TEST;
}

int UtcDaliNavigatorMoveAssignment(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();
  DALI_TEST_EQUALS(1, navigator.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  Navigator moved;
  moved = std::move(navigator);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!navigator);
  END_TEST;
}

int UtcDaliNavigatorDownCastP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();
  BaseHandle        object(navigator);
  Navigator         navigator2 = Navigator::DownCast(object);
  Navigator         navigator3 = DownCast<Navigator>(object);
  DALI_TEST_CHECK(navigator2);
  DALI_TEST_CHECK(navigator3);
  END_TEST;
}

int UtcDaliNavigatorDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  Navigator         navigator2 = Navigator::DownCast(unInitializedObject);
  Navigator         navigator3 = DownCast<Navigator>(unInitializedObject);
  DALI_TEST_CHECK(!navigator2);
  DALI_TEST_CHECK(!navigator3);
  END_TEST;
}

int UtcDaliNavigatorPushPopP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  navigator.Push(p1, false);
  DALI_TEST_EQUALS(1u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetCurrentView() == p1);

  View p2 = View::New();
  navigator.Push(p2, false);
  DALI_TEST_EQUALS(2u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetCurrentView() == p2);

  View popped = navigator.Pop(false);
  DALI_TEST_CHECK(popped == p2);
  DALI_TEST_EQUALS(1u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetCurrentView() == p1);
  END_TEST;
}

// The last page can be popped, leaving the stack empty; only an already-empty
// stack returns an empty handle.
int UtcDaliNavigatorPopLastP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  navigator.Push(p1, false);

  View popped = navigator.Pop(false);
  DALI_TEST_CHECK(popped == p1);
  DALI_TEST_EQUALS(0u, navigator.GetNavigationStackCount(), TEST_LOCATION);

  // Popping an already-empty stack returns an empty handle.
  View none = navigator.Pop(false);
  DALI_TEST_CHECK(!none);
  END_TEST;
}

// Pushing the same page twice is ignored.
int UtcDaliNavigatorDuplicatePushP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  navigator.Push(p1, false);
  navigator.Push(p1, false);
  DALI_TEST_EQUALS(1u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorStackItemP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  View p2 = View::New();
  navigator.Push(p1, false);
  navigator.Push(p2, false);

  DALI_TEST_CHECK(navigator.GetNavigationStackItem(0) == p1);
  DALI_TEST_CHECK(navigator.GetNavigationStackItem(1) == p2);
  DALI_TEST_CHECK(!navigator.GetNavigationStackItem(2)); // out of range
  END_TEST;
}

int UtcDaliNavigatorInsertBeforeP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  View p3 = View::New();
  navigator.Push(p1, false);
  navigator.Push(p3, false);

  View p2 = View::New();
  navigator.InsertBefore(p2, p3);

  DALI_TEST_EQUALS(3u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetNavigationStackItem(0) == p1);
  DALI_TEST_CHECK(navigator.GetNavigationStackItem(1) == p2);
  DALI_TEST_CHECK(navigator.GetNavigationStackItem(2) == p3);
  END_TEST;
}

int UtcDaliNavigatorModalP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View page = View::New();
  navigator.Push(page, false);

  View modal = View::New();
  navigator.PushModal(modal, false);
  DALI_TEST_EQUALS(1u, navigator.GetModalStackCount(), TEST_LOCATION);
  // Current view prioritises the modal stack.
  DALI_TEST_CHECK(navigator.GetCurrentView() == modal);

  View popped = navigator.PopModal(false);
  DALI_TEST_CHECK(popped == modal);
  DALI_TEST_EQUALS(0u, navigator.GetModalStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetCurrentView() == page);
  END_TEST;
}

int UtcDaliNavigatorNavigateBackP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  navigator.Push(p1, false);
  // Nothing to go back to with a single page.
  DALI_TEST_CHECK(navigator.NavigateBack() == false);

  View p2 = View::New();
  navigator.Push(p2, false);
  DALI_TEST_CHECK(navigator.NavigateBack() == true);
  DALI_TEST_EQUALS(1u, navigator.GetNavigationStackCount(), TEST_LOCATION);

  View modal = View::New();
  navigator.PushModal(modal, false);
  DALI_TEST_CHECK(navigator.NavigateBack() == true);
  DALI_TEST_EQUALS(0u, navigator.GetModalStackCount(), TEST_LOCATION);
  END_TEST;
}

// A page's back handler can consume Back so the page is not popped.
int UtcDaliNavigatorBackHandlerP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  View p2 = View::New();
  navigator.Push(p1, false);
  navigator.Push(p2, false);

  bool consumed = false;
  navigator.SetBackHandler(p2, [&consumed]() {
    consumed = true;
    return true; // consume
  });

  bool handled = navigator.NavigateBack();
  DALI_TEST_CHECK(handled);
  DALI_TEST_CHECK(consumed);
  // Consumed -> page must NOT be popped.
  DALI_TEST_EQUALS(2u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorClearP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  navigator.Push(View::New(), false);
  navigator.Push(View::New(), false);
  navigator.PushModal(View::New(), false);

  navigator.Clear();
  DALI_TEST_EQUALS(0u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, navigator.GetModalStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!navigator.GetCurrentView());
  END_TEST;
}

// A view already in one stack must not be accepted into the other stack.
int UtcDaliNavigatorCrossStackRejectN(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View shared = View::New();
  navigator.Push(shared, false);
  navigator.PushModal(shared, false); // must be rejected (already in nav stack)
  DALI_TEST_EQUALS(1u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, navigator.GetModalStackCount(), TEST_LOCATION);

  View modalOnly = View::New();
  navigator.PushModal(modalOnly, false);
  navigator.Push(modalOnly, false); // must be rejected (already in modal stack)
  DALI_TEST_EQUALS(1u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, navigator.GetModalStackCount(), TEST_LOCATION);
  END_TEST;
}
