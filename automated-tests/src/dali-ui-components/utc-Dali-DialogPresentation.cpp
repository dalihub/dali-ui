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

#include <dali.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/public-api/dialog/alert-dialog.h>
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali-ui-test-suite-utils.h>
#include <test-gesture-generator.h>
#include <cerrno>
#include <cstdlib>
#include <functional>
#include <sys/wait.h>
#include <unistd.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
std::function<void()> gContainerStyleHook;

UiStyle CreateReentrantContainerStyle()
{
  auto hook = std::move(gContainerStyleHook);
  if(hook) hook();
  return DialogContainerStyle::DefaultPreset();
}

void Advance(UiTestApplication& application)
{
  application.SendNotification();
  application.Render(500);
  application.SendNotification();
  application.Render(500);
}

int CheckPresentationMixing(unsigned operation)
{
  // Actor callbacks may assert after hierarchy mutation. Test the fatal contract
  // in a child process without unwinding a partially modified scene afterwards.
  const pid_t child = fork();
  DALI_TEST_CHECK(child >= 0);
  if(child > 0)
  {
    int status = 0;
    pid_t result;
    do { result = waitpid(child, &status, 0); } while(result < 0 && errno == EINTR);
    DALI_TEST_CHECK(result == child);
    DALI_TEST_CHECK(WIFEXITED(status));
    DALI_TEST_EQUALS(WEXITSTATUS(status), 0, TEST_LOCATION);
    END_TEST;
  }
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  auto dialog = Dialog::New();
  auto newParent = View::New();
  DialogPostOptions options;
  options.animated = false;
  unsigned hidden = 0u;
  dialog.HiddenSignal().Connect(&tracker, [&](Dialog, DialogDismissReason) { ++hidden; });
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  if(operation == 7u)
  {
    application.GetScene().Add(navigator);
    Advance(application);
    dialog.Dismiss(true);
    DALI_TEST_CHECK(dialog.IsPosted());
  }
  bool asserted = false;
  try
  {
    switch(operation)
    {
      case 0u: newParent.Add(dialog); break;
      case 1u: dialog.Unparent(); break;
      case 2u: dialog.GetParent().Remove(dialog); break;
      case 3u: DialogContainer::DownCast(dialog.GetParent()).SetModalContent({}); break;
      case 4u: DialogContainer::DownCast(dialog.GetParent()).SetModalContent(View::New()); break;
      case 5u: static_cast<Actor&>(navigator).Remove(navigator.GetModalStackItem(0u)); break;
      case 6u: newParent.Add(navigator.GetModalStackItem(0u)); break;
      case 7u: dialog.Unparent(); break;
    }
  }
  catch(const DaliException&)
  {
    asserted = true;
  }
  std::_Exit(asserted && hidden == 0u ? EXIT_SUCCESS : EXIT_FAILURE);
}
}

int UtcDaliDialogPresentationOffSceneAndShownOnce(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  auto dialog = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  unsigned shown = 0;
  unsigned hidden = 0;
  dialog.ShownSignal().Connect(&tracker, [&](Dialog) { ++shown; });
  dialog.HiddenSignal().Connect(&tracker, [&](Dialog, DialogDismissReason) { ++hidden; });
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  DALI_TEST_EQUALS(shown, 0u, TEST_LOCATION);
  application.GetScene().Add(navigator);
  Advance(application);
  DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
  auto upper = Dialog::New();
  DALI_TEST_CHECK(upper.Post(navigator, options));
  DALI_TEST_CHECK(dialog.IsPosted());
  upper.Dismiss(false);
  Advance(application);
  DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(hidden, 0u, TEST_LOCATION);
  application.GetScene().Remove(navigator);
  DALI_TEST_CHECK(dialog.IsPosted());
  application.GetScene().Add(navigator);
  Advance(application);
  DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(hidden, 0u, TEST_LOCATION);
  dialog.Dismiss(false);
  DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogPresentationCloseBeforeShown(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  unsigned shown = 0;
  unsigned hidden = 0;
  dialog.ShownSignal().Connect(&tracker, [&](Dialog) { ++shown; });
  dialog.HiddenSignal().Connect(&tracker, [&](Dialog, DialogDismissReason) { ++hidden; });
  DALI_TEST_CHECK(dialog.Post(navigator));
  dialog.Dismiss(true);
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_CHECK(!dialog.Post(navigator));
  dialog.Dismiss(true);
  Advance(application);
  DALI_TEST_EQUALS(shown, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!dialog.IsPosted());
  END_TEST;
}

int UtcDaliDialogPresentationExternalRemoval(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  auto dialog = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  unsigned hidden = 0;
  bool restored = true;
  dialog.HiddenSignal().Connect(&tracker, [&](Dialog value, DialogDismissReason reason) {
    ++hidden;
    restored = restored && !value.IsPosted() && !value.IsAccessibilityModal() && !value.GetParent() && reason == DialogDismissReason::PROGRAMMATIC;
  });
  for(unsigned operation = 0; operation < 3; ++operation)
  {
    DALI_TEST_CHECK(dialog.Post(navigator, options));
    DALI_TEST_CHECK(dialog.IsAccessibilityModal());
    if(operation == 0) navigator.PopModal(false);
    if(operation == 1) navigator.Remove(navigator.GetModalStackItem(0u));
    if(operation == 2) navigator.Clear();
    DALI_TEST_EQUALS(hidden, operation + 1u, TEST_LOCATION);
    DALI_TEST_CHECK(restored);
  }
  END_TEST;
}

int UtcDaliDialogPresentationClearRepost(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  auto first = Dialog::New();
  auto second = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  bool reposted = false;
  unsigned hidden = 0;
  first.HiddenSignal().Connect(&tracker, [&](Dialog value, DialogDismissReason) {
    if(++hidden == 1u) reposted = value.Post(navigator, options);
  });
  first.Post(navigator, options);
  second.Post(navigator, options);
  navigator.Clear();
  DALI_TEST_CHECK(reposted);
  DALI_TEST_CHECK(first.IsPosted());
  DALI_TEST_CHECK(!second.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  first.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationHostDestroyed(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto dialog = Dialog::New();
  dialog.SetAccessibilityModal(true);
  DialogPostOptions options;
  options.animated = false;
  unsigned hidden = 0;
  bool reasonMatches = false;
  dialog.HiddenSignal().Connect(&tracker, [&](Dialog value, DialogDismissReason reason) {
    ++hidden;
    reasonMatches = reason == DialogDismissReason::HOST_REMOVED && !value.IsPosted();
  });
  {
    auto navigator = Navigator::New();
    DALI_TEST_CHECK(dialog.Post(navigator, options));
  }
  DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(reasonMatches);
  DALI_TEST_CHECK(dialog.IsAccessibilityModal());
  DALI_TEST_CHECK(!dialog.GetParent());
  END_TEST;
}

int UtcDaliDialogPresentationContentReparentAsserts(void)
{
  return CheckPresentationMixing(0u);
}

int UtcDaliDialogPresentationContentUnparentAsserts(void)
{
  return CheckPresentationMixing(1u);
}

int UtcDaliDialogPresentationContentRemoveAsserts(void)
{
  return CheckPresentationMixing(2u);
}

int UtcDaliDialogPresentationContentClearAsserts(void)
{
  return CheckPresentationMixing(3u);
}

int UtcDaliDialogPresentationContentReplaceAsserts(void)
{
  return CheckPresentationMixing(4u);
}

int UtcDaliDialogPresentationContainerRemoveAsserts(void)
{
  return CheckPresentationMixing(5u);
}

int UtcDaliDialogPresentationContainerReparentAsserts(void)
{
  return CheckPresentationMixing(6u);
}

int UtcDaliDialogPresentationClosingUnparentAsserts(void)
{
  return CheckPresentationMixing(7u);
}

int UtcDaliDialogPresentationPolicyAndExplicitBypass(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  navigator.SetModalTransitionAnimationEnabled(false);
  auto dialog = Dialog::New();
  unsigned requested = 0;
  DialogDismissReason reason = DialogDismissReason::PROGRAMMATIC;
  dialog.DismissRequestedSignal().Connect(&tracker, [&](Dialog, DialogDismissReason) { ++requested; return false; });
  dialog.HiddenSignal().Connect(&tracker, [&](Dialog, DialogDismissReason value) { reason = value; });
  DALI_TEST_CHECK(dialog.GetDismissPolicy() == DialogDismissPolicy::BACK_AND_SCRIM);
  dialog.SetDismissPolicy(DialogDismissPolicy::NONE);
  dialog.Post(navigator);
  DALI_TEST_CHECK(navigator.NavigateBack());
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(requested, 0u, TEST_LOCATION);
  dialog.SetDismissPolicy(DialogDismissPolicy::BACK);
  DALI_TEST_CHECK(navigator.NavigateBack());
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_EQUALS(requested, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(reason == DialogDismissReason::BACK);
  dialog.SetDismissPolicy(DialogDismissPolicy::NONE);
  dialog.Post(navigator);
  dialog.Dismiss(false);
  DALI_TEST_CHECK(reason == DialogDismissReason::PROGRAMMATIC);
  END_TEST;
}

int UtcDaliDialogPresentationRequestReentry(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  navigator.SetModalTransitionAnimationEnabled(false);
  auto first = Dialog::New();
  auto second = Dialog::New();
  first.DismissRequestedSignal().Connect(&tracker, [&](Dialog value, DialogDismissReason) {
    value.Dismiss(false);
    second.Post(navigator);
    return false;
  });
  first.Post(navigator);
  navigator.NavigateBack();
  DALI_TEST_CHECK(!first.IsPosted());
  DALI_TEST_CHECK(second.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  second.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationBasicPost(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  auto dialog = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  auto container = DialogContainer::DownCast(navigator.GetModalStackItem(0u));
  DALI_TEST_CHECK(container);
  DALI_TEST_EQUALS(container.GetModalContent(), dialog, TEST_LOCATION);
  dialog.Dismiss(false);
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_CHECK(!dialog.GetParent());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 0u, TEST_LOCATION);
  auto alert = AlertDialog::New();
  DALI_TEST_CHECK(alert.Post(navigator, options));
  alert.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationHiddenRepost(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  auto dialog = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  unsigned hiddenCount = 0;
  bool reposted = false;
  bool cleaned = false;
  dialog.HiddenSignal().Connect(&tracker, [&](Dialog value, DialogDismissReason reason) {
    ++hiddenCount;
    cleaned = !value.IsPosted() && !value.GetParent() && reason == DialogDismissReason::PROGRAMMATIC;
    if(hiddenCount == 1u)
    {
      reposted = value.Post(navigator, options);
    }
  });
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  dialog.Dismiss(false);
  DALI_TEST_CHECK(cleaned);
  DALI_TEST_CHECK(reposted);
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(hiddenCount, 1u, TEST_LOCATION);
  dialog.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationDismissCovered(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  auto first = Dialog::New();
  auto second = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  DALI_TEST_CHECK(first.Post(navigator, options));
  DALI_TEST_CHECK(second.Post(navigator, options));
  auto top = navigator.GetModalStackItem(1u);
  first.Dismiss(false);
  DALI_TEST_CHECK(!first.IsPosted());
  DALI_TEST_CHECK(second.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(navigator.GetModalStackItem(0u), top, TEST_LOCATION);
  second.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationParentAndRepost(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  auto other = Navigator::New();
  auto parent = View::New();
  auto dialog = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  parent.Add(dialog);
  bool postAsserted = false;
  bool dismissAsserted = false;
  try { dialog.Post(navigator, options); }
  catch(const DaliException&) { postAsserted = true; }
  try { dialog.Dismiss(false); }
  catch(const DaliException&) { dismissAsserted = true; }
  DALI_TEST_CHECK(postAsserted);
  DALI_TEST_CHECK(dismissAsserted);
  DALI_TEST_EQUALS(dialog.GetParent(), parent, TEST_LOCATION);
  parent.Remove(dialog);
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  auto container = navigator.GetModalStackItem(0u);
  DALI_TEST_CHECK(dialog.Post(navigator));
  DALI_TEST_CHECK(!dialog.Post(other));
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(navigator.GetModalStackItem(0u), container, TEST_LOCATION);
  dialog.Dismiss(false);
  dialog.Dismiss(false);
  parent.Add(dialog);
  DALI_TEST_EQUALS(dialog.GetParent(), parent, TEST_LOCATION);
  parent.Remove(dialog);
  END_TEST;
}

int UtcDaliDialogPresentationInternalRemovalIsTargetScoped(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  auto first = Dialog::New();
  auto second = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  first.Post(navigator, options);
  second.Post(navigator, options);
  auto firstContainer = first.GetParent();
  bool asserted = false;
  firstContainer.ChildRemovedSignal().Connect(&application, [&](Actor, Actor) {
    try { second.Unparent(); }
    catch(const DaliException&) { asserted = true; }
  });
  first.Dismiss(false);
  DALI_TEST_CHECK(asserted);
  DALI_TEST_CHECK(!first.IsPosted());
  END_TEST;
}

int UtcDaliDialogPresentationPagePushReentry(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  auto page = View::New();
  DialogPostOptions options;
  options.animated = true;
  unsigned shown = 0u;
  dialog.ShownSignal().Connect(&application, [&](Dialog) { ++shown; });
  navigator.PageWillAppearSignal().Connect(&application, [&](Navigator, View value, bool) {
    if(value == page) dialog.Post(navigator, options);
  });
  navigator.Push(page, false);
  Advance(application);
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(dialog.GetProperty<bool>(Actor::Property::VISIBLE));
  dialog.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationPagePopReentry(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto page = View::New();
  navigator.Push(page, false);
  auto dialog = Dialog::New();
  DialogPostOptions options;
  options.animated = true;
  unsigned shown = 0u;
  dialog.ShownSignal().Connect(&application, [&](Dialog) { ++shown; });
  navigator.PageWillDisappearSignal().Connect(&application, [&](Navigator, View value, bool) {
    if(value == page) dialog.Post(navigator, options);
  });
  navigator.Pop(false);
  Advance(application);
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!page.GetParent());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  dialog.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationDetachReentry(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  auto first = Dialog::New();
  auto second = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  unsigned hidden = 0u;
  first.HiddenSignal().Connect(&application, [&](Dialog, DialogDismissReason) { ++hidden; });
  first.Post(navigator, options);
  auto container = first.GetParent();
  navigator.ChildRemovedSignal().Connect(&application, [&](Actor, Actor child) {
    if(child == container) second.Post(navigator, options);
  });
  first.Dismiss(false);
  DALI_TEST_CHECK(!first.IsPosted());
  DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(second.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  second.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationNoScrimInput(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  navigator.SetPivot(Pivot::TOP_LEFT);
  navigator.SetParentOrigin(ParentOrigin::TOP_LEFT);
  navigator.SetRequestedWidth(200.0f);
  navigator.SetRequestedHeight(200.0f);
  navigator.SetModalTransitionAnimationEnabled(false);
  auto background = InteractiveView::New();
  unsigned clicks = 0u;
  background.ClickedSignal().Connect(&application, [&](View, InputEvent) { ++clicks; });
  navigator.Push(background, false);
  auto dialog = Dialog::New();
  dialog.SetLayoutParams(AbsoluteLayoutParams::New().SetBounds(LayoutRect(0, 0, 40, 40)));
  DialogPostOptions options;
  options.animated = false;
  options.containerStyle = DialogContainerStyle::NoScrimPreset();
  unsigned hidden = 0u;
  DialogDismissReason reason = DialogDismissReason::PROGRAMMATIC;
  dialog.HiddenSignal().Connect(&application, [&](Dialog, DialogDismissReason value) { ++hidden; reason = value; });
  dialog.SetDismissPolicy(DialogDismissPolicy::NONE);
  dialog.Post(navigator, options);
  application.GetScene().Add(navigator);
  navigator.Measure(200, 200);
  navigator.Arrange(LayoutRect(0, 0, 200, 200));
  Advance(application);
  TestGenerateTap(application, 150.0f, 150.0f, 100u);
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(clicks, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(hidden, 0u, TEST_LOCATION);
  dialog.SetDismissPolicy(DialogDismissPolicy::SCRIM);
  TestGenerateTap(application, 150.0f, 150.0f, 300u);
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_EQUALS(clicks, 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(reason == DialogDismissReason::SCRIM);
  Advance(application);
  TestGenerateTap(application, 150.0f, 150.0f, 500u);
  DALI_TEST_EQUALS(clicks, 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogPresentationLayoutParity(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto manualHost = Navigator::New();
  auto managedHost = Navigator::New();
  application.GetScene().Add(manualHost);
  application.GetScene().Add(managedHost);
  for(unsigned scenario = 0u; scenario < 3u; ++scenario)
  {
    UiScaleManager::Get().SetScale(scenario == 2u ? 1.5f : 1.0f);
    auto manual = AlertDialog::New();
    auto managed = AlertDialog::New();
    for(auto dialog : {manual, managed})
    {
      dialog.SetTitle("Preserved title");
      dialog.SetMessage("Long content remains owned by the application, without added scrolling or scaling.");
      if(scenario == 2u)
      {
        auto scroll = ScrollView::New();
        scroll.SetContent(Label::New("Application-provided scrolling content"));
        dialog.SetBodyView(scroll);
      }
      dialog.SetPadding(Insets(7.0f, 7.0f));
      dialog.GetBodyView().SetPadding(Insets(9.0f, 9.0f));
      if(scenario > 0u)
      {
        dialog.SetLayoutParams(AbsoluteLayoutParams::New()
          .SetBounds(LayoutRect(0.5f, 0.5f, 240.0f, 160.0f))
          .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
        dialog.SetMinimumWidth(600.0f);
        dialog.SetMaximumHeight(190.0f);
        dialog.SetMargin(Insets(11.0f, 13.0f));
      }
      if(scenario == 2u) dialog.SetProperty(Actor::Property::LAYOUT_DIRECTION, LayoutDirection::RIGHT_TO_LEFT);
    }
    const float requestedWidth = managed.GetRequestedWidth();
    const float requestedHeight = managed.GetRequestedHeight();
    const auto originalMargin = managed.GetMargin();
    auto body = managed.GetBodyView();
    auto container = DialogContainer::New();
    container.SetModalContent(manual);
    manualHost.PushModal(container, false);
    DialogPostOptions options;
    options.animated = false;
    managed.Post(managedHost, options);
    for(float width : {400.0f, 800.0f})
    {
      for(auto host : {manualHost, managedHost})
      {
        host.SetRequestedWidth(width);
        host.SetRequestedHeight(300.0f);
        host.Measure(width, 300.0f);
        host.Arrange(LayoutRect(0, 0, width, 300));
      }
      Advance(application);
      DALI_TEST_EQUALS(managed.GetCurrentSize(), manual.GetCurrentSize(), 0.01f, TEST_LOCATION);
      DALI_TEST_EQUALS(managed.GetCurrentPosition(), manual.GetCurrentPosition(), 0.01f, TEST_LOCATION);
      DALI_TEST_EQUALS(managed.GetRequestedWidth(), requestedWidth, TEST_LOCATION);
      DALI_TEST_EQUALS(managed.GetRequestedHeight(), requestedHeight, TEST_LOCATION);
      DALI_TEST_EQUALS(managed.GetMargin(), originalMargin, TEST_LOCATION);
      DALI_TEST_CHECK(managed.GetBodyView() == body);
      DALI_TEST_EQUALS(body.GetPadding(), Insets(9.0f, 9.0f), TEST_LOCATION);
    }
    managed.Dismiss(false);
    manualHost.PopModal(false);
    DALI_TEST_CHECK(!managed.IsAccessibilityModal());
    DALI_TEST_EQUALS(managed.GetRequestedWidth(), requestedWidth, TEST_LOCATION);
  }
  UiScaleManager::Get().SetScale(1.0f);
  END_TEST;
}

int UtcDaliDialogPresentationVetoAndLifetime(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  navigator.SetModalTransitionAnimationEnabled(false);
  for(bool vetoFirst : {false, true})
  {
    ConnectionTracker tracker;
    auto dialog = AlertDialog::New();
    dialog.SetAccessibilityModal(true);
    auto button = dialog.AddActionButton("Validate");
    dialog.DismissRequestedSignal().Connect(&tracker, [vetoFirst](Dialog, DialogDismissReason) { return vetoFirst; });
    dialog.DismissRequestedSignal().Connect(&tracker, [vetoFirst](Dialog, DialogDismissReason) { return !vetoFirst; });
    dialog.Post(navigator);
    navigator.NavigateBack();
    DALI_TEST_CHECK(dialog.IsPosted());
    button.ClickedSignal().Emit(button, InputEvent());
    DALI_TEST_CHECK(dialog.IsPosted());
    WeakHandle<AlertDialog> weak(dialog);
    dialog.Reset();
    DALI_TEST_CHECK(weak.GetHandle());
    navigator.Clear();
    DALI_TEST_CHECK(!weak.GetHandle());
  }
  END_TEST;
}

int UtcDaliDialogPresentationLifecycleClearRepost(void)
{
  UiTestApplication application(Components::UiConfig::New());
  for(unsigned hook = 0u; hook < 5u; ++hook)
  {
    ConnectionTracker tracker;
    auto navigator = Navigator::New();
    application.GetScene().Add(navigator);
    auto first = Dialog::New();
    auto incoming = Dialog::New();
    auto replacement = Dialog::New();
    DialogPostOptions options;
    options.animated = false;
    first.Post(navigator, options);
    bool invoked = false;
    unsigned shown = 0u;
    replacement.ShownSignal().Connect(&tracker, [&](Dialog) { ++shown; });
    auto replace = [&]() {
      if(invoked) return;
      invoked = true;
      navigator.Clear();
      replacement.Post(navigator, options);
    };
    auto pageCallback = [&](Navigator, View, bool) { replace(); };
    if(hook == 0u) navigator.PageWillAppearSignal().Connect(&tracker, pageCallback);
    if(hook == 1u) navigator.PageWillDisappearSignal().Connect(&tracker, pageCallback);
    if(hook == 2u) navigator.PageDidAppearSignal().Connect(&tracker, pageCallback);
    if(hook == 3u) navigator.PageDidDisappearSignal().Connect(&tracker, pageCallback);
    if(hook == 4u) navigator.TransitionFinishedSignal().Connect(&tracker, [&](Navigator) { replace(); });
    incoming.Post(navigator, options);
    Advance(application);
    DALI_TEST_CHECK(invoked);
    DALI_TEST_CHECK(!first.IsPosted());
    DALI_TEST_CHECK(!incoming.IsPosted());
    DALI_TEST_CHECK(replacement.IsPosted());
    DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
    DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
    replacement.Dismiss(false);
    application.GetScene().Remove(navigator);
  }
  END_TEST;
}

int UtcDaliDialogPresentationFocusParity(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto focus = FocusManager::Get();
  auto content = TextButton::New("Focus target");
  auto manual = Dialog::New();
  manual.SetBodyView(content);
  auto container = DialogContainer::New();
  container.SetModalContent(manual);
  navigator.PushModal(container, false);
  Advance(application);
  DALI_TEST_CHECK(focus.SetCurrentFocusView(content));
  DALI_TEST_CHECK(focus.GetCurrentFocusView() == content);
  navigator.PopModal(false);
  auto manualFocusAfter = focus.GetCurrentFocusView();
  container.SetModalContent({});
  DialogPostOptions options;
  options.animated = false;
  manual.Post(navigator, options);
  Advance(application);
  DALI_TEST_CHECK(focus.SetCurrentFocusView(content));
  DALI_TEST_CHECK(focus.GetCurrentFocusView() == content);
  manual.Dismiss(false);
  DALI_TEST_CHECK(focus.GetCurrentFocusView() == manualFocusAfter);
  END_TEST;
}

int UtcDaliDialogPresentationCoveredRepostPreservesOptions(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  auto dialog = Dialog::New();
  auto upper = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  options.containerStyle = DialogContainerStyle::Default().Configure()
    .SetScrimColor(UiColor(Color::BLUE)).SetScrimBlurRadius(0.0f).Build();
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  auto container = DialogContainer::DownCast(dialog.GetParent());
  DALI_TEST_EQUALS(container.GetScrim().GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_CHECK(upper.Post(navigator, options));
  auto top = navigator.GetCurrentView();
  options.containerStyle = DialogContainerStyle::NoScrimPreset();
  // Replacing options or moving the covered entry would break this contract.
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(navigator.GetCurrentView(), top, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetParent(), container, TEST_LOCATION);
  DALI_TEST_EQUALS(container.GetScrim().GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  dialog.Dismiss(false);
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  container = DialogContainer::DownCast(dialog.GetParent());
  DALI_TEST_EQUALS(container.GetScrim().GetBackgroundColor().GetRgba().a, 0.0f, TEST_LOCATION);
  DALI_TEST_CHECK(!container.GetScrim().GetRenderEffect());
  navigator.Clear();
  END_TEST;
}

int UtcDaliDialogPresentationBackRequestReentry(void)
{
  UiTestApplication application(Components::UiConfig::New());
  for(bool removeOriginal : {false, true})
  {
    ConnectionTracker tracker;
    auto navigator = Navigator::New();
    navigator.SetModalTransitionAnimationEnabled(false);
    auto first = Dialog::New();
    auto replacement = Dialog::New();
    unsigned requests = 0u;
    first.DismissRequestedSignal().Connect(&tracker, [&](Dialog, DialogDismissReason) { ++requests; return false; });
    navigator.BackRequestedSignal().Connect(&tracker, [&](Navigator, View) {
      if(removeOriginal) first.Dismiss(false);
      replacement.Post(navigator);
      return false;
    });
    first.Post(navigator);
    DALI_TEST_CHECK(navigator.NavigateBack());
    // Continuing with a stale top would close the replacement or the covered dialog.
    DALI_TEST_CHECK(replacement.IsPosted());
    DALI_TEST_EQUALS(first.IsPosted(), !removeOriginal, TEST_LOCATION);
    DALI_TEST_EQUALS(navigator.GetCurrentView(), replacement.GetParent(), TEST_LOCATION);
    DALI_TEST_EQUALS(requests, 0u, TEST_LOCATION);
    navigator.Clear();
  }
  END_TEST;
}

int UtcDaliDialogPresentationTransitionCallbackRejectsPost(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  bool invoked = false;
  bool accepted = true;
  unsigned hidden = 0u;
  dialog.HiddenSignal().Connect(&application, [&](Dialog, DialogDismissReason) { ++hidden; });
  spec.EnterSignal().Connect(&application, [&](Animation&, View) {
    invoked = true;
    accepted = dialog.Post(navigator);
  });
  navigator.SetTransitionSpec(spec);
  navigator.Push(View::New(), true);
  DALI_TEST_CHECK(invoked);
  DALI_TEST_CHECK(!accepted);
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_CHECK(!dialog.GetParent());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(hidden, 0u, TEST_LOCATION);
  // Failed registration must release ownership so a later valid Post succeeds.
  Advance(application);
  DialogPostOptions options;
  options.animated = false;
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  dialog.Dismiss(false);
  DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogPresentationCustomExitTransition(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  spec.SetDuration(0.5f);
  unsigned exits = 0u;
  unsigned hidden = 0u;
  spec.PopExitSignal().Connect(&application, [&](Animation& animation, View view) {
    ++exits;
    animation.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f);
  });
  dialog.HiddenSignal().Connect(&application, [&](Dialog, DialogDismissReason) { ++hidden; });
  DialogPostOptions options;
  options.animated = false;
  options.transitionSpec = spec;
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  dialog.Dismiss(true);
  DALI_TEST_EQUALS(exits, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(hidden, 0u, TEST_LOCATION);
  Advance(application);
  // Deliver the finish event queued by the last render of the 0.5s transition.
  application.SendNotification();
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  dialog.Dismiss(false);
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_EQUALS(exits, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(hidden, 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogPresentationTransitionTargetsContent(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  View entered, exited, snapped, appeared;
  spec.EnterSignal().Connect(&application, [&](Animation& animation, View view) {
    entered = view;
    animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.8f, 0.8f, 1.0f));
  });
  spec.PopExitSignal().Connect(&application, [&](Animation& animation, View view) {
    exited = view;
    animation.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f);
  });
  spec.SnapIncomingSignal().Connect(&application, [&](View view) { snapped = view; });
  navigator.PageDidAppearSignal().Connect(&application, [&](Navigator, View view, bool) { appeared = view; });
  DialogPostOptions options;
  options.transitionSpec = spec;
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  auto container = dialog.GetParent();
  DALI_TEST_EQUALS(entered, dialog, TEST_LOCATION);
  Advance(application);
  DALI_TEST_EQUALS(snapped, dialog, TEST_LOCATION);
  DALI_TEST_EQUALS(appeared, container, TEST_LOCATION);
  DALI_TEST_EQUALS(container.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(dialog.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3(0.8f, 0.8f, 1.0f), 0.001f, TEST_LOCATION);
  dialog.Dismiss();
  DALI_TEST_EQUALS(exited, dialog, TEST_LOCATION);
  Advance(application);
  END_TEST;
}

int UtcDaliDialogPresentationScrimFadesIndependently(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto container = DialogContainer::New();
  auto content = View::New();
  auto scrim = View::New();
  scrim.SetProperty(Actor::Property::OPACITY, 0.6f);
  container.SetModalContent(content);
  container.SetScrim(scrim);
  auto spec = NavigationTransitionSpec::New();
  spec.SetDuration(1.0f);
  spec.EnterSignal().Connect(&application, [](Animation& animation, View view) {
    animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.5f, 0.5f, 1.0f));
  });
  spec.PopExitSignal().Connect(&application, [](Animation& animation, View view) {
    animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.2f, 0.2f, 1.0f));
  });
  navigator.SetPageModalTransitionSpec(container, spec);
  navigator.PushModal(container, true);
  application.SendNotification();
  application.Render(250);
  float opacity = scrim.GetCurrentProperty<float>(Actor::Property::OPACITY);
  DALI_TEST_CHECK(opacity > 0.0f && opacity < 0.6f);
  DALI_TEST_EQUALS(container.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(scrim.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, 0.001f, TEST_LOCATION);
  Advance(application);
  DALI_TEST_EQUALS(scrim.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.6f, 0.001f, TEST_LOCATION);
  navigator.PopModal(true);
  application.SendNotification();
  application.Render(250);
  opacity = scrim.GetCurrentProperty<float>(Actor::Property::OPACITY);
  DALI_TEST_CHECK(opacity > 0.0f && opacity < 0.6f);
  Advance(application);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(scrim.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.6f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(!container.GetParent());
  END_TEST;
}

int UtcDaliDialogPresentationExitRestoredBeforeRepost(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  dialog.SetProperty(Actor::Property::SCALE, Vector3(0.9f, 0.9f, 1.0f));
  auto spec = NavigationTransitionSpec::New();
  View exitTarget;
  spec.PopExitSignal().Connect(&application, [&](Animation& animation, View view) {
    exitTarget = view;
    animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.1f, 0.1f, 1.0f));
    animation.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f);
  });
  DialogPostOptions options;
  options.animated = false;
  options.transitionSpec = spec;
  bool reposted = false;
  bool restored = false;
  dialog.HiddenSignal().Connect(&application, [&](Dialog view, DialogDismissReason) {
    if(reposted) return;
    restored = view.GetProperty<Vector3>(Actor::Property::SCALE) == Vector3(0.9f, 0.9f, 1.0f) &&
               view.GetProperty<float>(Actor::Property::OPACITY) == 1.0f;
    reposted = view.Post(navigator, options);
  });
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  Advance(application);
  dialog.Dismiss(true);
  DALI_TEST_EQUALS(exitTarget, dialog, TEST_LOCATION);
  Advance(application);
  DALI_TEST_CHECK(restored);
  DALI_TEST_CHECK(reposted);
  DALI_TEST_CHECK(dialog.IsPosted());
  dialog.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationUnanimatedContentSnap(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  View snapped;
  unsigned animated = 0u;
  spec.EnterSignal().Connect(&tracker, [&](Animation&, View) { ++animated; });
  spec.SnapIncomingSignal().Connect(&tracker, [&](View view) { snapped = view; });
  DialogPostOptions options;
  options.transitionSpec = spec;
  options.containerStyle = DialogContainerStyle::NoScrimPreset();
  for(bool disabled : {false, true})
  {
    navigator.SetModalTransitionAnimationEnabled(!disabled);
    options.animated = disabled;
    DALI_TEST_CHECK(dialog.Post(navigator, options));
    DALI_TEST_EQUALS(snapped, dialog, TEST_LOCATION);
    auto container = DialogContainer::DownCast(dialog.GetParent());
    DALI_TEST_EQUALS(container.GetScrim().GetProperty<float>(Actor::Property::OPACITY), 1.0f, TEST_LOCATION);
    dialog.Dismiss(false);
  }
  DALI_TEST_EQUALS(animated, 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogPresentationNestedScrimIsNotRestarted(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto lower = Dialog::New();
  auto upper = Dialog::New();
  DialogPostOptions options;
  options.animated = false;
  DALI_TEST_CHECK(lower.Post(navigator, options));
  auto lowerScrim = DialogContainer::DownCast(lower.GetParent()).GetScrim();
  lowerScrim.SetProperty(Actor::Property::OPACITY, 0.4f);
  Advance(application);
  DALI_TEST_CHECK(upper.Post(navigator));
  application.SendNotification();
  application.Render(50);
  DALI_TEST_EQUALS(lowerScrim.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.4f, 0.001f, TEST_LOCATION);
  Advance(application);
  upper.Dismiss();
  application.SendNotification();
  application.Render(50);
  DALI_TEST_EQUALS(lowerScrim.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.4f, 0.001f, TEST_LOCATION);
  Advance(application);
  DALI_TEST_CHECK(lower.IsPosted());
  DALI_TEST_CHECK(upper.Post(navigator));
  lower.Dismiss(false);
  DALI_TEST_CHECK(upper.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  upper.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationEmptyModalVisuals(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  for(bool withScrim : {false, true})
  {
    auto container = DialogContainer::New();
    if(!withScrim) container.SetScrim({});
    navigator.PushModal(container, true);
    if(withScrim)
    {
      application.SendNotification();
      application.Render(50);
      float opacity = container.GetScrim().GetCurrentProperty<float>(Actor::Property::OPACITY);
      DALI_TEST_CHECK(opacity > 0.0f && opacity < 1.0f);
    }
    Advance(application);
    DALI_TEST_EQUALS(container.GetCurrentProperty<float>(Actor::Property::OPACITY), 1.0f, TEST_LOCATION);
    navigator.PopModal(true);
    Advance(application);
    DALI_TEST_CHECK(!container.GetParent());
  }
  END_TEST;
}

int UtcDaliDialogPresentationScrimAndContentShareCompletion(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto dialog = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  spec.SetDuration(0.25f);
  spec.EnterSignal().Connect(&tracker, [](Animation& animation, View view) {
    animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.8f, 0.8f, 1.0f),
                        AlphaFunction::LINEAR, TimePeriod(0.2f, 1.0f));
  });
  unsigned shown = 0u;
  dialog.ShownSignal().Connect(&tracker, [&](Dialog) { ++shown; });
  DialogPostOptions options;
  options.transitionSpec = spec;
  DALI_TEST_CHECK(dialog.Post(navigator, options));
  auto scrim = DialogContainer::DownCast(dialog.GetParent()).GetScrim();
  application.SendNotification();
  application.Render(400);
  application.SendNotification();
  DALI_TEST_EQUALS(scrim.GetCurrentProperty<float>(Actor::Property::OPACITY), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(shown, 0u, TEST_LOCATION);
  Advance(application);
  DALI_TEST_EQUALS(dialog.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3(0.8f, 0.8f, 1.0f), 0.001f, TEST_LOCATION);
  // Deliver the completion generated by the last update, after the long track.
  application.SendNotification();
  DALI_TEST_EQUALS(shown, 1u, TEST_LOCATION);
  dialog.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationDestroyRestoresTransitionContent(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto dialog = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  spec.EnterSignal().Connect(&tracker, [](Animation& animation, View view) {
    view.SetProperty(Actor::Property::SCALE, Vector3(0.5f, 0.5f, 1.0f));
    animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3::ONE);
  });
  {
    auto navigator = Navigator::New();
    DialogPostOptions options;
    options.transitionSpec = spec;
    DALI_TEST_CHECK(dialog.Post(navigator, options));
  }
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_CHECK(!dialog.GetParent());
  DALI_TEST_EQUALS(dialog.GetProperty<Vector3>(Actor::Property::SCALE), Vector3::ONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogPresentationReentrantPageSnap(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  auto page = View::New();
  auto dialog = Dialog::New();
  page.SetProperty(Actor::Property::OPACITY, 0.0f);
  navigator.PageWillAppearSignal().Connect(&tracker, [&](Navigator, View view, bool) {
    if(view == page)
    {
      DialogPostOptions options;
      options.animated = false;
      dialog.Post(navigator, options);
    }
  });
  navigator.Push(page, true);
  DALI_TEST_EQUALS(page.GetProperty<float>(Actor::Property::OPACITY), 1.0f, TEST_LOCATION);
  dialog.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationRevealedContentDefaultSnap(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto lower = Dialog::New();
  auto upper = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  spec.ExitSignal().Connect(&tracker, [](Animation& animation, View view) {
    animation.AnimateTo(Property(view, Actor::Property::OPACITY), 0.0f);
  });
  spec.PopEnterSignal().Connect(&tracker, [](Animation& animation, View view) {
    animation.AnimateTo(Property(view, Actor::Property::OPACITY), 1.0f);
  });
  DialogPostOptions options;
  options.animated = false;
  options.transitionSpec = spec;
  DALI_TEST_CHECK(lower.Post(navigator, options));
  Advance(application);
  DALI_TEST_CHECK(upper.Post(navigator));
  Advance(application);
  DALI_TEST_EQUALS(lower.GetCurrentProperty<float>(Actor::Property::OPACITY), 0.0f, 0.001f, TEST_LOCATION);
  upper.Dismiss(true);
  Advance(application);
  application.SendNotification();
  application.Render();
  DALI_TEST_EQUALS(lower.GetCurrentProperty<float>(Actor::Property::OPACITY), 1.0f, 0.001f, TEST_LOCATION);
  lower.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationRemoveAnimatedCoveredContent(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  for(bool directRemove : {false, true})
  {
    auto lower = Dialog::New();
    auto upper = Dialog::New();
    lower.SetProperty(Actor::Property::SCALE, Vector3(0.9f, 0.9f, 1.0f));
    auto spec = NavigationTransitionSpec::New();
    spec.ExitSignal().Connect(&tracker, [](Animation& animation, View view) {
      view.SetProperty(Actor::Property::SCALE, Vector3(0.2f, 0.2f, 1.0f));
      animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.1f, 0.1f, 1.0f));
    });
    DialogPostOptions options;
    options.animated = false;
    options.transitionSpec = spec;
    DALI_TEST_CHECK(lower.Post(navigator, options));
    Advance(application);
    Vector3 hiddenScale;
    bool reposted = false;
    ConnectionTracker hiddenTracker;
    lower.HiddenSignal().Connect(&hiddenTracker, [&](Dialog view, DialogDismissReason) {
      if(reposted) return;
      hiddenScale = view.GetProperty<Vector3>(Actor::Property::SCALE);
      reposted = view.Post(navigator, options);
    });
    DALI_TEST_CHECK(upper.Post(navigator));
    if(directRemove) navigator.Remove(View::DownCast(lower.GetParent()));
    else lower.Dismiss(false);
    DALI_TEST_EQUALS(hiddenScale, Vector3(0.9f, 0.9f, 1.0f), TEST_LOCATION);
    DALI_TEST_CHECK(reposted);
    Advance(application);
    DALI_TEST_EQUALS(lower.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3(0.9f, 0.9f, 1.0f), 0.001f, TEST_LOCATION);
    lower.Dismiss(false);
    upper.Dismiss(false);
  }
  END_TEST;
}

int UtcDaliDialogPresentationRemoveKeepsHostAlive(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  auto lower = Dialog::New();
  auto upper = Dialog::New();
  unsigned lowerHidden = 0u;
  unsigned upperHidden = 0u;
  lower.HiddenSignal().Connect(&tracker, [&](Dialog, DialogDismissReason) { ++lowerHidden; });
  upper.HiddenSignal().Connect(&tracker, [&](Dialog, DialogDismissReason) { ++upperHidden; });
  DialogPostOptions options;
  options.animated = false;
  DALI_TEST_CHECK(lower.Post(navigator, options));
  auto container = View::DownCast(lower.GetParent());
  DALI_TEST_CHECK(upper.Post(navigator));
  navigator.TransitionFinishedSignal().Connect(&tracker, [&](Navigator) { navigator.Reset(); });
  navigator.Remove(container);
  DALI_TEST_CHECK(!navigator);
  DALI_TEST_CHECK(!lower.IsPosted());
  DALI_TEST_CHECK(!upper.IsPosted());
  DALI_TEST_EQUALS(lowerHidden, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(upperHidden, 1u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogPresentationRemoveSettlesReplacementTransition(void)
{
  UiTestApplication application(Components::UiConfig::New());
  ConnectionTracker tracker;
  auto navigator = Navigator::New();
  application.GetScene().Add(navigator);
  auto lower = Dialog::New();
  auto upper = Dialog::New();
  auto replacement = Dialog::New();
  auto spec = NavigationTransitionSpec::New();
  spec.ExitSignal().Connect(&tracker, [](Animation& animation, View view) {
    view.SetProperty(Actor::Property::SCALE, Vector3(0.2f, 0.2f, 1.0f));
    animation.AnimateTo(Property(view, Actor::Property::SCALE), Vector3(0.1f, 0.1f, 1.0f));
  });
  lower.SetProperty(Actor::Property::SCALE, Vector3(0.9f, 0.9f, 1.0f));
  DialogPostOptions options;
  options.animated = false;
  options.transitionSpec = spec;
  DALI_TEST_CHECK(lower.Post(navigator, options));
  Advance(application);
  DALI_TEST_CHECK(upper.Post(navigator));
  auto upperContainer = upper.GetParent();
  auto lowerContainer = View::DownCast(lower.GetParent());
  bool replaced = false;
  navigator.PageDidAppearSignal().Connect(&tracker, [&](Navigator, View view, bool) {
    if(!replaced && view == upperContainer)
    {
      replaced = true;
      upper.Dismiss(false);
      replacement.Post(navigator);
    }
  });
  navigator.Remove(lowerContainer);
  DALI_TEST_CHECK(replaced);
  DALI_TEST_CHECK(!lower.IsPosted());
  DALI_TEST_CHECK(replacement.IsPosted());
  // A fresh parent must not keep receiving the old modal's animation updates.
  auto newParent = View::New();
  application.GetScene().Add(newParent);
  newParent.Add(lower);
  lower.SetProperty(Actor::Property::SCALE, Vector3(0.9f, 0.9f, 1.0f));
  Advance(application);
  DALI_TEST_EQUALS(lower.GetCurrentProperty<Vector3>(Actor::Property::SCALE), Vector3(0.9f, 0.9f, 1.0f), 0.001f, TEST_LOCATION);
  replacement.Dismiss(false);
  END_TEST;
}

int UtcDaliDialogPresentationAccessibilityRestorePaths(void)
{
  UiTestApplication application(Components::UiConfig::New());
  for(bool originalModal : {false, true})
  {
    for(unsigned operation = 0u; operation < 5u; ++operation)
    {
      ConnectionTracker tracker;
      auto navigator = Navigator::New();
      auto dialog = Dialog::New();
      dialog.SetAccessibilityModal(originalModal);
      dialog.SetAccessibilityName("Preserved dialog name");
      const auto originalRole = dialog.GetAccessibilityRole();
      const auto originalName = dialog.GetAccessibilityName();
      DialogPostOptions options;
      options.animated = false;
      unsigned hidden = 0u;
      bool restoredAtSignal = false;
      dialog.HiddenSignal().Connect(&tracker, [&](Dialog value, DialogDismissReason reason) {
        ++hidden;
        restoredAtSignal = !value.GetParent() && !value.IsPosted() &&
          value.IsAccessibilityModal() == originalModal && value.GetAccessibilityRole() == originalRole &&
          value.GetAccessibilityName() == originalName &&
          reason == (operation == 4u ? DialogDismissReason::HOST_REMOVED : DialogDismissReason::PROGRAMMATIC);
      });
      DALI_TEST_CHECK(dialog.Post(navigator, options));
      DALI_TEST_CHECK(dialog.IsAccessibilityModal());
      if(operation == 0u) dialog.Dismiss(false);
      if(operation == 1u) navigator.PopModal(false);
      if(operation == 2u) navigator.Remove(navigator.GetCurrentView());
      if(operation == 3u) navigator.Clear();
      if(operation == 4u) navigator.Reset();
      DALI_TEST_EQUALS(hidden, 1u, TEST_LOCATION);
      DALI_TEST_CHECK(restoredAtSignal);
    }
  }
  END_TEST;
}

int UtcDaliDialogPresentationInvalidArguments(void)
{
  UiTestApplication application(Components::UiConfig::New());
  auto dialog = Dialog::New();
  DALI_TEST_ASSERTION(dialog.Post(Navigator()), "");
  DALI_TEST_ASSERTION(dialog.SetDismissPolicy(static_cast<DialogDismissPolicy>(4u)), "");
  DALI_TEST_CHECK(!dialog.IsPosted());
  DALI_TEST_CHECK(!dialog.GetParent());
  DALI_TEST_CHECK(dialog.GetDismissPolicy() == DialogDismissPolicy::BACK_AND_SCRIM);
  END_TEST;
}

int UtcDaliDialogPresentationStyleCreatorReentry(void)
{
  auto config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(DialogContainerStyle::DefaultKey(), &CreateReentrantContainerStyle);
  UiTestApplication application(config);
  auto navigator = Navigator::New();
  auto dialog = Dialog::New();
  bool nestedAccepted = true;
  gContainerStyleHook = [&]() {
    DialogPostOptions nested;
    nested.animated = false;
    // Avoid recursive style lookup: exercise Post reentry, not style recursion.
    nested.containerStyle = DialogContainerStyle::NoScrimPreset();
    nestedAccepted = dialog.Post(navigator, nested);
  };
  DialogPostOptions options;
  options.animated = false;
  const bool accepted = dialog.Post(navigator, options);
  gContainerStyleHook = {};
  DALI_TEST_CHECK(accepted);
  DALI_TEST_CHECK(!nestedAccepted);
  DALI_TEST_CHECK(dialog.IsPosted());
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 1u, TEST_LOCATION);
  dialog.Dismiss(false);
  DALI_TEST_EQUALS(navigator.GetModalStackCount(), 0u, TEST_LOCATION);
  DALI_TEST_CHECK(!dialog.GetParent());
  END_TEST;
}
