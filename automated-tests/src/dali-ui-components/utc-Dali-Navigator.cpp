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

int UtcDaliNavigationTransitionSpecHandleP(void)
{
  UiTestApplication application;

  NavigationTransitionSpec empty;
  DALI_TEST_CHECK(!empty);

  NavigationTransitionSpec spec = NavigationTransitionSpec::New();
  DALI_TEST_CHECK(spec);
  DALI_TEST_EQUALS(0.0f, spec.GetDuration(), TEST_LOCATION);
  DALI_TEST_CHECK(spec.EnterSignal().Empty());
  DALI_TEST_CHECK(spec.ExitSignal().Empty());
  DALI_TEST_CHECK(spec.PopEnterSignal().Empty());
  DALI_TEST_CHECK(spec.PopExitSignal().Empty());
  DALI_TEST_CHECK(spec.SnapIncomingSignal().Empty());
  DALI_TEST_CHECK(spec.SnapOutgoingSignal().Empty());

  spec.SetDuration(-1.0f);
  DALI_TEST_EQUALS(0.0f, spec.GetDuration(), TEST_LOCATION);
  spec.SetDuration(0.42f);
  DALI_TEST_EQUALS(0.42f, spec.GetDuration(), TEST_LOCATION);

  NavigationTransitionSpec copy(spec);
  DALI_TEST_CHECK(copy == spec);
  DALI_TEST_EQUALS(0.42f, copy.GetDuration(), TEST_LOCATION);

  BaseHandle               object(spec);
  NavigationTransitionSpec downcast  = NavigationTransitionSpec::DownCast(object);
  NavigationTransitionSpec downcast2 = DownCast<NavigationTransitionSpec>(object);
  DALI_TEST_CHECK(downcast == spec);
  DALI_TEST_CHECK(downcast2 == spec);

  NavigationTransitionSpec invalid = NavigationTransitionSpec::DownCast(View::New());
  DALI_TEST_CHECK(!invalid);
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

// A BackRequested callback can consume Back so the current page is not popped.
int UtcDaliNavigatorBackRequestedSignalP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  View p2 = View::New();
  navigator.Push(p1, false);
  navigator.Push(p2, false);

  bool falseListenerCalled = false;
  bool consumed            = false;
  navigator.BackRequestedSignal().Connect(&application, [&falseListenerCalled](Navigator, View)
  {
    falseListenerCalled = true;
    return false;
  });
  navigator.BackRequestedSignal().Connect(&application, [&consumed, navigator, p2](Navigator source, View page)
  {
    consumed = (source == navigator && page == p2);
    return consumed;
  });

  bool handled = navigator.NavigateBack();
  DALI_TEST_CHECK(handled);
  DALI_TEST_CHECK(falseListenerCalled);
  DALI_TEST_CHECK(consumed);
  // Consumed -> page must NOT be popped.
  DALI_TEST_EQUALS(2u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorBackRequestedSignalFallsThroughP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  View p2 = View::New();
  navigator.Push(p1, false);
  navigator.Push(p2, false);

  bool callbackCalled = false;
  navigator.BackRequestedSignal().Connect(&application, [&callbackCalled, p2](Navigator, View page)
  {
    callbackCalled = (page == p2);
    return false;
  });

  DALI_TEST_CHECK(navigator.NavigateBack());
  DALI_TEST_CHECK(callbackCalled);
  DALI_TEST_EQUALS(1u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetCurrentView() == p1);
  END_TEST;
}

int UtcDaliNavigatorBackRequestedSignalModalP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  navigator.Push(View::New(), false);
  View modal = View::New();
  navigator.PushModal(modal, false);

  bool modalReceived = false;
  navigator.BackRequestedSignal().Connect(&application, [&modalReceived, modal](Navigator, View current)
  {
    modalReceived = (current == modal);
    return modalReceived;
  });

  DALI_TEST_CHECK(navigator.NavigateBack());
  DALI_TEST_CHECK(modalReceived);
  DALI_TEST_EQUALS(1u, navigator.GetModalStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetCurrentView() == modal);
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

int UtcDaliNavigatorClearTransitionSpecsP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View root = View::New();
  navigator.Push(root, false);

  int                      enterCount = 0;
  NavigationTransitionSpec spec       = NavigationTransitionSpec::New();
  spec.EnterSignal().Connect(&application, [&enterCount](Animation&, View)
  {
    ++enterCount;
  });

  navigator.SetTransitionSpec(spec);
  navigator.ClearTransitionSpec();
  navigator.Push(View::New(), true);
  DALI_TEST_EQUALS(0, enterCount, TEST_LOCATION);
  navigator.Pop(false);

  View page = View::New();
  navigator.SetPageTransitionSpec(page, spec);
  navigator.ClearPageTransitionSpec(page);
  navigator.Push(page, true);
  DALI_TEST_EQUALS(0, enterCount, TEST_LOCATION);
  navigator.Pop(false);

  navigator.SetModalTransitionSpec(spec);
  navigator.ClearModalTransitionSpec();
  navigator.PushModal(View::New(), true);
  DALI_TEST_EQUALS(0, enterCount, TEST_LOCATION);
  navigator.PopModal(false);

  View modal = View::New();
  navigator.SetPageModalTransitionSpec(modal, spec);
  navigator.ClearPageModalTransitionSpec(modal);
  navigator.PushModal(modal, true);
  DALI_TEST_EQUALS(0, enterCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorTransitionSpecReentrantClearP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  navigator.Push(View::New(), false);

  int                      firstEnterCount  = 0;
  int                      secondEnterCount = 0;
  int                      exitCount        = 0;
  int                      snapCount        = 0;
  bool                     pushIgnored      = false;
  NavigationTransitionSpec animatorSpec     = NavigationTransitionSpec::New();
  animatorSpec.EnterSignal().Connect(&application, [&navigator, &firstEnterCount, &pushIgnored](Animation&, View)
  {
    ++firstEnterCount;
    navigator.ClearTransitionSpec();
    const uint32_t oldCount = navigator.GetNavigationStackCount();
    navigator.Push(View::New(), false);
    pushIgnored = (navigator.GetNavigationStackCount() == oldCount);
  });
  animatorSpec.EnterSignal().Connect(&application, [&secondEnterCount](Animation&, View)
  {
    ++secondEnterCount;
  });
  animatorSpec.ExitSignal().Connect(&application, [&exitCount](Animation&, View)
  {
    ++exitCount;
  });
  animatorSpec.SnapIncomingSignal().Connect(&application, [&snapCount](View)
  {
    ++snapCount;
  });

  navigator.SetTransitionSpec(animatorSpec);
  animatorSpec.Reset(); // Navigator is now the only persistent owner.
  navigator.Push(View::New(), true);

  DALI_TEST_EQUALS(1, firstEnterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, secondEnterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, exitCount, TEST_LOCATION);
  DALI_TEST_CHECK(pushIgnored);

  // Completing the in-flight transition still uses the original snap snapshot,
  // even though its animator callback cleared Navigator's stored spec.
  navigator.Push(View::New(), false);
  DALI_TEST_EQUALS(1, snapCount, TEST_LOCATION);

  int                      firstSnapCount  = 0;
  int                      secondSnapCount = 0;
  NavigationTransitionSpec snapSpec        = NavigationTransitionSpec::New();
  snapSpec.SnapIncomingSignal().Connect(&application, [&navigator, &firstSnapCount](View)
  {
    ++firstSnapCount;
    navigator.ClearTransitionSpec();
  });
  snapSpec.SnapIncomingSignal().Connect(&application, [&secondSnapCount](View)
  {
    ++secondSnapCount;
  });

  navigator.SetTransitionSpec(snapSpec);
  snapSpec.Reset(); // Exercise re-entrant destruction during SnapView().
  navigator.Push(View::New(), false);

  DALI_TEST_EQUALS(1, firstSnapCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, secondSnapCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorGlobalTransitionSpecP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  navigator.Push(p1, false);

  int enterCount = 0;
  int exitCount  = 0;

  NavigationTransitionSpec spec = NavigationTransitionSpec::New();
  spec.EnterSignal().Connect(&application, [&enterCount](Animation& anim, View view)
  {
    ++enterCount;
    view.SetProperty(Actor::Property::OPACITY, 0.0f);
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
  });
  spec.ExitSignal().Connect(&application, [&exitCount](Animation& anim, View view)
  {
    ++exitCount;
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.5f);
  });
  spec.SnapIncomingSignal().Connect(&application, [](View view)
  {
    view.SetProperty(Actor::Property::OPACITY, 1.0f);
  });
  spec.SnapOutgoingSignal().Connect(&application, [](View view)
  {
    view.SetProperty(Actor::Property::OPACITY, 1.0f);
  });

  navigator.SetTransitionSpec(spec);

  View p2 = View::New();
  navigator.Push(p2, true);

  DALI_TEST_EQUALS(1, enterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, exitCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorPageTransitionSpecPriorityP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  navigator.Push(p1, false);

  int globalEnterCount = 0;
  int globalExitCount  = 0;
  int pageEnterCount   = 0;

  NavigationTransitionSpec globalSpec = NavigationTransitionSpec::New();
  globalSpec.EnterSignal().Connect(&application, [&globalEnterCount](Animation& anim, View view)
  {
    ++globalEnterCount;
    view.SetProperty(Actor::Property::OPACITY, 0.0f);
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
  });
  globalSpec.ExitSignal().Connect(&application, [&globalExitCount](Animation& anim, View view)
  {
    ++globalExitCount;
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.5f);
  });
  globalSpec.SnapIncomingSignal().Connect(&application, [](View view)
  {
    view.SetProperty(Actor::Property::OPACITY, 1.0f);
  });
  globalSpec.SnapOutgoingSignal().Connect(&application, [](View view)
  {
    view.SetProperty(Actor::Property::OPACITY, 1.0f);
  });
  navigator.SetTransitionSpec(globalSpec);

  View                     p2       = View::New();
  NavigationTransitionSpec pageSpec = NavigationTransitionSpec::New();
  pageSpec.EnterSignal().Connect(&application, [&pageEnterCount](Animation& anim, View view)
  {
    ++pageEnterCount;
    view.SetProperty(Actor::Property::OPACITY, 0.0f);
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
  });
  pageSpec.SnapIncomingSignal().Connect(&application, [](View view)
  {
    view.SetProperty(Actor::Property::OPACITY, 1.0f);
  });
  navigator.SetPageTransitionSpec(p2, pageSpec);

  navigator.Push(p2, true);

  DALI_TEST_EQUALS(0, globalEnterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, pageEnterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, globalExitCount, TEST_LOCATION);

  navigator.ClearPageTransitionSpec(p2);
  navigator.Pop(false);

  globalEnterCount = 0;
  globalExitCount  = 0;
  pageEnterCount   = 0;
  navigator.Push(p2, true);

  DALI_TEST_EQUALS(1, globalEnterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(0, pageEnterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, globalExitCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorModalTransitionSpecIsSeparatedP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View page = View::New();
  navigator.Push(page, false);

  int navExitCount    = 0;
  int modalEnterCount = 0;
  int modalExitCount  = 0;

  NavigationTransitionSpec navSpec = NavigationTransitionSpec::New();
  navSpec.ExitSignal().Connect(&application, [&navExitCount](Animation& anim, View view)
  {
    ++navExitCount;
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.5f);
  });
  navigator.SetTransitionSpec(navSpec);

  View builtInModal = View::New();
  navigator.PushModal(builtInModal, true);
  DALI_TEST_EQUALS(0, navExitCount, TEST_LOCATION);
  navigator.PopModal(false);

  NavigationTransitionSpec modalSpec = NavigationTransitionSpec::New();
  modalSpec.EnterSignal().Connect(&application, [&modalEnterCount](Animation& anim, View view)
  {
    ++modalEnterCount;
    view.SetProperty(Actor::Property::OPACITY, 0.0f);
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
  });
  modalSpec.ExitSignal().Connect(&application, [&modalExitCount](Animation& anim, View view)
  {
    ++modalExitCount;
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
  });
  navigator.SetModalTransitionSpec(modalSpec);

  View modal = View::New();
  navigator.PushModal(modal, true);

  DALI_TEST_EQUALS(0, navExitCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, modalEnterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(1, modalExitCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorPageTransitionAnimationEnabledP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View p1 = View::New();
  navigator.Push(p1, false);

  int enterCount = 0;

  NavigationTransitionSpec spec = NavigationTransitionSpec::New();
  spec.EnterSignal().Connect(&application, [&enterCount](Animation& anim, View view)
  {
    ++enterCount;
    view.SetProperty(Actor::Property::OPACITY, 0.0f);
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
  });
  navigator.SetTransitionSpec(spec);

  navigator.SetPageTransitionAnimationEnabled(false);
  DALI_TEST_CHECK(!navigator.IsPageTransitionAnimationEnabled());

  View p2 = View::New();
  navigator.Push(p2, true);
  DALI_TEST_EQUALS(0, enterCount, TEST_LOCATION);
  DALI_TEST_EQUALS(2u, navigator.GetNavigationStackCount(), TEST_LOCATION);
  DALI_TEST_CHECK(navigator.GetCurrentView() == p2);

  navigator.SetPageTransitionAnimationEnabled(true);
  DALI_TEST_CHECK(navigator.IsPageTransitionAnimationEnabled());

  View p3 = View::New();
  navigator.Push(p3, true);
  DALI_TEST_EQUALS(1, enterCount, TEST_LOCATION);
  END_TEST;
}

int UtcDaliNavigatorModalTransitionAnimationEnabledP(void)
{
  UiTestApplication application;
  Navigator         navigator = Navigator::New();

  View page = View::New();
  navigator.Push(page, false);

  int exitCount = 0;

  NavigationTransitionSpec spec = NavigationTransitionSpec::New();
  spec.PopExitSignal().Connect(&application, [&exitCount](Animation& anim, View view)
  {
    ++exitCount;
    anim.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f);
  });
  navigator.SetModalTransitionSpec(spec);

  View modal = View::New();
  navigator.PushModal(modal, false);

  navigator.SetPageTransitionAnimationEnabled(false);
  DALI_TEST_CHECK(!navigator.IsPageTransitionAnimationEnabled());

  navigator.PopModal(true);
  DALI_TEST_EQUALS(1, exitCount, TEST_LOCATION);
  DALI_TEST_EQUALS(0u, navigator.GetModalStackCount(), TEST_LOCATION);

  navigator.SetPageTransitionAnimationEnabled(true);
  DALI_TEST_CHECK(navigator.IsPageTransitionAnimationEnabled());

  View modal2 = View::New();
  navigator.PushModal(modal2, false);

  navigator.SetModalTransitionAnimationEnabled(false);
  DALI_TEST_CHECK(!navigator.IsModalTransitionAnimationEnabled());

  navigator.PopModal(true);
  DALI_TEST_EQUALS(1, exitCount, TEST_LOCATION);
  DALI_TEST_EQUALS(0u, navigator.GetModalStackCount(), TEST_LOCATION);

  View modal3 = View::New();
  navigator.PushModal(modal3, false);

  navigator.SetModalTransitionAnimationEnabled(true);
  DALI_TEST_CHECK(navigator.IsModalTransitionAnimationEnabled());

  navigator.PopModal(true);
  DALI_TEST_EQUALS(2, exitCount, TEST_LOCATION);
  END_TEST;
}
