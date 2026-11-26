/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include "dali-ui-components-test-utils/atspi-capture.h"

using namespace Dali;
using namespace Dali::Ui;

namespace
{
// The transport is installed before the application can create bridge clients.
// Stop the bridge while the application's singleton services still exist.
struct EventFixture
{
  ComponentsTest::AtspiCapture capture;
  UiTestApplication           application;

  EventFixture()
  {
    capture.Start(application.GetScene());
  }
  ~EventFixture()
  {
    capture.Stop();
  }
  void Render()
  {
    application.SendNotification();
    application.Render();
    application.SendNotification();
  }
  DialogContainer Container(bool onScene = true)
  {
    auto container = DialogContainer::New();
    container.SetRequestedWidth(200.0f);
    container.SetRequestedHeight(200.0f);
    if(onScene)
    {
      application.GetScene().Add(container);
    }
    return container;
  }
  View Content()
  {
    auto content = View::New();
    content.SetRequestedWidth(100.0f);
    content.SetRequestedHeight(100.0f);
    content.SetBackgroundColor(UiColor(0xffffffu));
    // BUTTON cannot bypass the emitter's modal-state filter as DIALOG can.
    content.SetAccessibilityRole(Dali::Ui::Accessibility::Role::BUTTON);
    return content;
  }
};

bool IsShowingMessage(const ComponentsTest::AtspiStateEvent& event, Actor actor, int value)
{
  // Independently derive identity from the actor, not a capture helper.
  const auto path = "/org/a11y/atspi/accessible/" + std::to_string(actor.GetProperty<int>(Actor::Property::ID));
  return event.path == path && event.interface == "org.a11y.atspi.Event.Object" &&
         event.member == "StateChanged" && event.signature == "siiv(so)" &&
         event.state == "showing" && event.value == value && event.reserved == 0 &&
         event.payload == 0 && event.sourceBus == ":1.42" && event.sourcePath == "/org/a11y/atspi/accessible/root";
}
}

// Catches missing notification requests and duplicate same-handle requests.
int UtcDaliDialogShowingBridgeSameHandleEmitsOnceP(void)
{
  EventFixture fixture;
  auto container = fixture.Container();
  auto content   = fixture.Content();
  fixture.Render();
  fixture.capture.Clear();
  container.SetModalContent(content);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], content, 1));

  container.SetModalContent(content);
  content.SetProperty(Actor::Property::VISIBLE, true);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  END_TEST;
}

// Catches unconditional true on installation, and restoring MODAL before false.
int UtcDaliDialogShowingBridgeHiddenOffsceneAndRemovalP(void)
{
  EventFixture fixture;
  auto container = fixture.Container(false);
  auto content   = fixture.Content();
  fixture.capture.Clear();
  container.SetModalContent(content);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 0u, TEST_LOCATION);

  container.SetProperty(Actor::Property::VISIBLE, false);
  fixture.application.GetScene().Add(container);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 0u, TEST_LOCATION);

  container.SetProperty(Actor::Property::VISIBLE, true);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], content, 1));

  container.SetModalContent({});
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[1], content, 0));
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}

// Catches new-content notification before the old content disappears.
int UtcDaliDialogShowingBridgeReplacementOrdersSourcesP(void)
{
  EventFixture fixture;
  auto container = fixture.Container();
  auto first     = fixture.Content();
  auto second    = fixture.Content();
  container.SetModalContent(first);
  fixture.Render();
  fixture.capture.Tick(22u);
  fixture.capture.Clear();

  container.SetModalContent(second);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], first, 0));
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[1], second, 1));
  DALI_TEST_CHECK(!first.IsAccessibilityModal());
  END_TEST;
}

// A settled transfer starts a new cooldown with false, then emits the queued
// true on the eleventh 100ms timer tick. Counts concern serialized emissions.
int UtcDaliDialogShowingBridgeTransferImmediateFalseDeferredTrueP(void)
{
  EventFixture fixture;
  auto first   = fixture.Container();
  auto second  = fixture.Container();
  auto content = fixture.Content();
  first.SetModalContent(content);
  fixture.Render();
  fixture.capture.Tick(22u);
  fixture.capture.Clear();

  second.SetModalContent(content);
  fixture.Render();
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], content, 0));
  fixture.capture.Tick(10u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  fixture.capture.Tick(1u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[1], content, 1));
  DALI_TEST_CHECK(!first.GetModalContent());
  DALI_TEST_CHECK(second.GetModalContent() == content);

  fixture.capture.Tick(11u);
  second.SetModalContent({});
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[2], content, 0));
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}

// Narrow characterization: a rapid hide/show is coalesced by the real bridge.
// It is deliberately not asserted as a lossless 1,0,1 transport sequence.
int UtcDaliDialogShowingBridgeRapidHideShowCoalescesLatestP(void)
{
  EventFixture fixture;
  auto container = fixture.Container();
  auto content   = fixture.Content();
  fixture.capture.Clear();
  container.SetModalContent(content);
  fixture.Render();
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], content, 1));

  container.SetProperty(Actor::Property::VISIBLE, false);
  fixture.Render();
  container.SetProperty(Actor::Property::VISIBLE, true);
  fixture.Render();
  fixture.capture.Tick(10u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  fixture.capture.Tick(1u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[1], content, 1));
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);
  END_TEST;
}

// Exercises fixture restart without terminating the singleton bridge; stopped
// emission must remain filtered and a later start must reach the real emitter.
int UtcDaliDialogShowingBridgeCaptureStartStopIsIdempotentP(void)
{
  EventFixture fixture;
  auto container = fixture.Container();
  auto content   = fixture.Content();
  fixture.capture.Start(fixture.application.GetScene());
  fixture.capture.Stop();
  fixture.capture.Stop();
  fixture.capture.Clear();
  container.SetModalContent(content);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 0u, TEST_LOCATION);
  container.SetModalContent({});

  fixture.capture.Start(fixture.application.GetScene());
  container.SetModalContent(content);
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], content, 1));
  END_TEST;
}

// Catches losing rendered world-alpha notifications (including inherited
// opacity), and treating a positive-to-positive alpha change as disappearance.
int UtcDaliDialogShowingBridgeWorldAlphaTransitionsP(void)
{
  EventFixture fixture;
  auto container = fixture.Container();
  auto content   = fixture.Content();
  container.SetModalContent(content);
  fixture.Render();
  fixture.capture.Tick(22u);
  fixture.capture.Clear();

  container.SetProperty(Actor::Property::OPACITY, 0.0f);
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(content.GetProperty<Vector4>(Actor::Property::WORLD_COLOR_MULTIPLIER).a, 0.0f, TEST_LOCATION);
  DALI_TEST_CHECK(content.IsOnSceneVisible());
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], content, 0));

  container.SetProperty(Actor::Property::OPACITY, 0.5f);
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[1], content, 1));
  container.SetProperty(Actor::Property::OPACITY, 1.0f);
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);

  content.SetProperty(Actor::Property::OPACITY, 0.0f);
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 3u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[2], content, 0));
  content.SetProperty(Actor::Property::OPACITY, 0.0001f);
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 4u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[3], content, 1));
  END_TEST;
}

// Move the rendered actor outside the real scene frustum; do not assign the
// read-only CULLED property or replace the accessible's GetStates calculation.
int UtcDaliDialogShowingBridgeCullingTransitionsP(void)
{
  EventFixture fixture;
  auto container = fixture.Container();
  auto content   = fixture.Content();
  container.SetModalContent(content);
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_CHECK(!content.GetProperty<bool>(DevelActor::Property::CULLED));
  fixture.capture.Clear();
  auto originalPosition = container.GetProperty<Vector3>(Actor::Property::POSITION);

  container.SetProperty(Actor::Property::POSITION, Vector3(10000.0f, 10000.0f, 0.0f));
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_CHECK(content.GetProperty<bool>(DevelActor::Property::CULLED));
  DALI_TEST_CHECK(content.IsOnSceneVisible());
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[0], content, 0));

  container.SetProperty(Actor::Property::POSITION, originalPosition);
  fixture.Render();
  fixture.Render();
  fixture.capture.Tick(22u);
  DALI_TEST_CHECK(!content.GetProperty<bool>(DevelActor::Property::CULLED));
  DALI_TEST_EQUALS(fixture.capture.Events().size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(IsShowingMessage(fixture.capture.Events()[1], content, 1));
  END_TEST;
}

int UtcDaliIndependentFocusNotifiedWhenAccessibilityBridgeStartsP(void)
{
  EventFixture fixture;
  using Bridge = Dali::Integration::Accessibility::Bridge;
  auto bridge = Bridge::GetCurrentBridge();
  bridge->ForceDown();
  DALI_TEST_CHECK(!Dali::Integration::Accessibility::IsUp());

  auto window = Window::New(PositionSize(0, 0, 480, 800), "independent-focus-accessibility");
  window.Show();
  auto previous = fixture.Content();
  auto current  = fixture.Content();
  window.Add(previous);
  window.Add(current);
  previous.SetFocusable(true);
  current.SetFocusable(true);
  auto staleDescendant   = fixture.Content();
  auto clearedDescendant = fixture.Content();
  auto activeDescendant  = fixture.Content();
  previous.Add(staleDescendant);
  current.Add(clearedDescendant);
  current.Add(activeDescendant);

  auto manager = FocusManager::Get();
  DALI_TEST_CHECK(manager.SetIndependentFocusEnabled(window, true));
  DALI_TEST_CHECK(manager.SetCurrentFocusView(previous));
  previous.NotifyAccessibilityActiveDescendantChanged(staleDescendant);
  DALI_TEST_CHECK(manager.SetCurrentFocusView(current));
  current.NotifyAccessibilityActiveDescendantChanged(clearedDescendant);
  current.NotifyAccessibilityActiveDescendantChanged(activeDescendant);
  DALI_TEST_CHECK(manager.GetCurrentFocusView(window) == current);
  auto registerWindows = [&]()
  {
    bridge->AddTopLevelWindow(Dali::Accessibility::Accessible::Get(fixture.application.GetScene().GetRootLayer()));
    bridge->AddTopLevelWindow(Dali::Accessibility::Accessible::Get(window.GetRootLayer()));
  };
  registerWindows();
  fixture.capture.Clear();

  DALI_TEST_CHECK(bridge->ForceUp() == Bridge::ForceUpResult::JUST_STARTED);
  DALI_TEST_CHECK(manager.SetCurrentFocusView(current));

  const auto ownerPath      = "/org/a11y/atspi/accessible/" + std::to_string(current.GetProperty<int>(Actor::Property::ID));
  const auto descendantPath = "/org/a11y/atspi/accessible/" + std::to_string(activeDescendant.GetProperty<int>(Actor::Property::ID));
  unsigned int focusEvents      = 0;
  unsigned int descendantEvents = 0;
  for(const auto& event : fixture.capture.Events())
  {
    if(event.state == "focused" && event.value == 1)
    {
      DALI_TEST_EQUALS(event.path, ownerPath, TEST_LOCATION);
      DALI_TEST_EQUALS(descendantEvents, 0u, TEST_LOCATION);
      ++focusEvents;
    }
    else if(event.member == "ActiveDescendantChanged")
    {
      DALI_TEST_EQUALS(event.path, ownerPath, TEST_LOCATION);
      DALI_TEST_EQUALS(event.activeDescendantPath, descendantPath, TEST_LOCATION);
      DALI_TEST_EQUALS(focusEvents, 1u, TEST_LOCATION);
      ++descendantEvents;
    }
  }
  DALI_TEST_EQUALS(focusEvents, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(descendantEvents, 1u, TEST_LOCATION);

  bridge->ForceDown();
  current.NotifyAccessibilityActiveDescendantChanged(clearedDescendant);
  current.NotifyAccessibilityActiveDescendantChanged({});
  registerWindows();
  fixture.capture.Clear();
  DALI_TEST_CHECK(bridge->ForceUp() == Bridge::ForceUpResult::JUST_STARTED);
  for(const auto& event : fixture.capture.Events())
  {
    DALI_TEST_CHECK(event.state != "focused");
    DALI_TEST_CHECK(event.member != "ActiveDescendantChanged");
  }
  fixture.capture.Stop();
  END_TEST;
}
