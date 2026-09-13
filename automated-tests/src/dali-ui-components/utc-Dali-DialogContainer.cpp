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
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-components/public-api/styles/dialog-container-style.h>
#include <dali-ui-components/public-api/components-ui-config.h>
#include <limits>
#include <typeinfo>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
UiStyle CreateContainerFixtureStyle()
{
  return DialogContainerStyle::DefaultPreset().Configure().SetScrimColor(UiColor(Color::BLUE)).SetScrimBlurRadius(6.0f).Build();
}
BackgroundBlurEffect GetContainerBlur(View scrim)
{
  auto effect = scrim.GetRenderEffect();
  if(!effect)
  {
    return BackgroundBlurEffect();
  }

  // RenderEffect has no public typed downcast. Check the implementation type
  // against a public factory result before sharing the existing effect handle.
  auto blur = BackgroundBlurEffect::New();
  DALI_ASSERT_ALWAYS(typeid(effect.GetBaseObject()) == typeid(blur.GetBaseObject()));
  static_cast<BaseHandle&>(blur) = effect;
  return blur;
}
}

int UtcDaliDialogContainerStyleContractP(void)
{
  auto config = Components::UiConfig::New();
  config.StyleSheet().SetStyle(DialogContainerStyle::DefaultKey(), &CreateContainerFixtureStyle);
  UiTestApplication application(config);
  auto style = DialogContainerStyle::Default();
  DALI_TEST_EQUALS(style.GetScrimColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  auto container = DialogContainer::New();
  auto blur = GetContainerBlur(container.GetScrim());
  DALI_TEST_CHECK(blur);
  DALI_TEST_EQUALS(blur.GetBlurRadius(), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(DialogContainerStyle::DefaultPreset().GetScrimBlurRadius(), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(DialogContainerStyle::DefaultPreset().GetScrimColor().GetRgba(), Vector4(0.0f, 0.0f, 0.0f, 0.5f), TEST_LOCATION);
  DALI_TEST_CHECK(DialogContainerStyle::DownCast(style) == style);
  DALI_TEST_CHECK(DialogContainerStyle::StaticDownCast(style) == style);
  DALI_TEST_CHECK(!DialogContainerStyle::DownCast(BaseHandle()));
  DALI_TEST_CHECK(!DialogContainerStyle::DownCast(View::New()));
  DALI_TEST_ASSERTION(DialogContainerStyle().Configure(), "");
  auto source = style.Configure();
  auto moved = std::move(source);
  DALI_TEST_ASSERTION(source.SetScrimBlurRadius(1.0f), "");
  DialogContainerStyle::Builder assigned;
  assigned = std::move(moved);
  DALI_TEST_ASSERTION(std::move(moved).Build(), "");
  auto copy = std::move(assigned).SetScrimBlurRadius(7.9f).Build();
  DALI_TEST_EQUALS(style.GetScrimBlurRadius(), 6.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(copy.GetScrimBlurRadius(), 7.9f, TEST_LOCATION);
  DALI_TEST_EQUALS(GetContainerBlur(DialogContainer::New(copy).GetScrim()).GetBlurRadius(), 7u, TEST_LOCATION);
  DALI_TEST_ASSERTION(assigned.SetScrimColor(UiColor(Color::RED)), "");
  DALI_TEST_ASSERTION(std::move(assigned).Build(), "");
  END_TEST;
}

int UtcDaliDialogContainerStyleBlurBoundsN(void)
{
  UiTestApplication application;
  for(float invalid : {-1.0f, std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), 4097.0f, 1.0e9f, std::numeric_limits<float>::max()})
  {
    DALI_TEST_ASSERTION(DialogContainerStyle::Builder().SetScrimBlurRadius(invalid), "");
  }
  auto style = DialogContainerStyle::Builder().SetScrimBlurRadius(4096.0f).Build();
  DALI_TEST_EQUALS(style.GetScrimBlurRadius(), 4096.0f, TEST_LOCATION);
  DALI_TEST_CHECK(DialogContainer::New(style).GetScrim().GetRenderEffect());
  DALI_TEST_CHECK(!DialogContainer::New(style.Configure().SetScrimBlurRadius(0.0f).Build()).GetScrim().GetRenderEffect());
  END_TEST;
}

int UtcDaliDialogContainerStylePreservesCustomScrimP(void)
{
  UiTestApplication application;
  auto style = DialogContainerStyle::Builder().SetScrimColor(UiColor(Color::BLUE)).SetScrimBlurRadius(5.0f).Build();
  auto container = DialogContainer::New(style);
  DALI_TEST_EQUALS(container.GetScrim().GetBackgroundColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_CHECK(container.GetScrim().GetRenderEffect());
  auto custom = View::New();
  custom.SetBackgroundColor(UiColor(Color::RED));
  container.SetScrim(custom);
  DALI_TEST_EQUALS(custom.GetBackgroundColor().GetRgba(), Color::RED, TEST_LOCATION);
  DALI_TEST_CHECK(!custom.GetRenderEffect());
  auto other = DialogContainer::New(style.Configure().SetScrimColor(UiColor(Color::GREEN)).Build());
  DALI_TEST_EQUALS(other.GetScrim().GetBackgroundColor().GetRgba(), Color::GREEN, TEST_LOCATION);
  DALI_TEST_EQUALS(style.GetScrimColor().GetRgba(), Color::BLUE, TEST_LOCATION);
  DALI_TEST_ASSERTION(DialogContainer::New(DialogContainerStyle()), "");
  END_TEST;
}

void utc_dali_dialog_container_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_dialog_container_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliDialogContainerConstructorP(void)
{
  UiTestApplication application;
  DialogContainer   container;
  DALI_TEST_CHECK(!container);
  END_TEST;
}

int UtcDaliDialogContainerNewP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(container);
  END_TEST;
}

int UtcDaliDialogContainerCopyConstructorP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DialogContainer   copy(container);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(container == copy);
  END_TEST;
}

int UtcDaliDialogContainerMoveConstructor(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_EQUALS(1, container.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DialogContainer moved = std::move(container);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!container);
  END_TEST;
}

int UtcDaliDialogContainerAssignmentOperatorP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DialogContainer   copy;
  copy = container;
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(container == copy);
  END_TEST;
}

int UtcDaliDialogContainerMoveAssignment(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_EQUALS(1, container.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  DialogContainer moved;
  moved = std::move(container);
  DALI_TEST_CHECK(moved);
  DALI_TEST_EQUALS(1, moved.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(!container);
  END_TEST;
}

int UtcDaliDialogContainerDownCastP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  BaseHandle        object(container);
  DialogContainer   container2 = DialogContainer::DownCast(object);
  DialogContainer   container3 = DownCast<DialogContainer>(object);
  DALI_TEST_CHECK(container2);
  DALI_TEST_CHECK(container3);
  END_TEST;
}

int UtcDaliDialogContainerDownCastN(void)
{
  UiTestApplication application;
  BaseHandle        unInitializedObject;
  DialogContainer   container2 = DialogContainer::DownCast(unInitializedObject);
  DialogContainer   container3 = DownCast<DialogContainer>(unInitializedObject);
  DALI_TEST_CHECK(!container2);
  DALI_TEST_CHECK(!container3);
  END_TEST;
}

// A DialogContainer is created with a default scrim.
int UtcDaliDialogContainerDefaultScrimP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(container.GetScrim());
  END_TEST;
}

int UtcDaliDialogContainerSetGetModalContentP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(!container.GetModalContent());

  View content = View::New();
  container.SetModalContent(content);
  DALI_TEST_CHECK(container.GetModalContent() == content);
  END_TEST;
}

int UtcDaliDialogContainerSetGetScrimP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();

  View scrim = View::New();
  container.SetScrim(scrim);
  DALI_TEST_CHECK(container.GetScrim() == scrim);
  END_TEST;
}

// The ScrimClicked signal is accessible.
int UtcDaliDialogContainerScrimClickedSignalP(void)
{
  UiTestApplication application;
  DialogContainer   container = DialogContainer::New();
  DALI_TEST_CHECK(container.ScrimClickedSignal().GetConnectionCount() == 0);
  END_TEST;
}

int UtcDaliDialogContainerDefaultScrimDoesNotTakeFocusP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  auto scrim = InteractiveView::DownCast(container.GetScrim());
  DALI_TEST_CHECK(scrim);
  DALI_TEST_CHECK(!scrim.IsFocusable());
  DALI_TEST_CHECK(!scrim.IsFocusOnTouchEnabled());
  int calls = 0;
  container.ScrimClickedSignal().Connect(&application, [&calls](DialogContainer) { ++calls; });
  scrim.ClickedSignal().Emit(scrim, InputEvent::Programmatic());
  DALI_TEST_EQUALS(calls, 1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliDialogContainerReplacedScrimDoesNotNotifyP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  auto oldScrim = InteractiveView::DownCast(container.GetScrim());
  auto newScrim = InteractiveView::New();
  int calls = 0;
  int independentCalls = 0;
  container.ScrimClickedSignal().Connect(&application, [&calls](DialogContainer) { ++calls; });
  oldScrim.ClickedSignal().Connect(&application, [&independentCalls](View, InputEvent) { ++independentCalls; });
  container.SetScrim(newScrim);
  View newParent = View::New();
  newParent.Add(oldScrim);
  oldScrim.ClickedSignal().Emit(oldScrim, InputEvent::Programmatic());
  DALI_TEST_EQUALS(calls, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(independentCalls, 1, TEST_LOCATION);
  newScrim.ClickedSignal().Emit(newScrim, InputEvent::Programmatic());
  DALI_TEST_EQUALS(calls, 1, TEST_LOCATION);
  container.SetScrim(newScrim);
  newScrim.ClickedSignal().Emit(newScrim, InputEvent::Programmatic());
  DALI_TEST_EQUALS(calls, 2, TEST_LOCATION);
  container.SetScrim(View());
  newScrim.ClickedSignal().Emit(newScrim, InputEvent::Programmatic());
  DALI_TEST_EQUALS(calls, 2, TEST_LOCATION);
  container.SetScrim(oldScrim);
  oldScrim.ClickedSignal().Emit(oldScrim, InputEvent::Programmatic());
  DALI_TEST_EQUALS(calls, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(independentCalls, 2, TEST_LOCATION);
  DALI_TEST_CHECK(newScrim.IsFocusable());
  END_TEST;
}

#include <dali/devel-api/atspi-interfaces/accessible.h>

int UtcDaliDialogContainerModalStateOwnershipP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  View content = View::New();
  content.SetAccessibilityRole(Dali::Ui::Accessibility::Role::BUTTON);
  container.SetModalContent(content);
  DALI_TEST_CHECK(content.IsAccessibilityModal());
  DALI_TEST_CHECK(content.GetAccessibilityRole() == Dali::Ui::Accessibility::Role::BUTTON);

  // Reinstalling the same handle must not replace the original false snapshot.
  container.SetModalContent(content);
  container.SetModalContent(View());
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  DALI_TEST_CHECK(!content.GetParent());
  DALI_TEST_CHECK(content.GetAccessibilityRole() == Dali::Ui::Accessibility::Role::BUTTON);
  END_TEST;
}

int UtcDaliDialogContainerModalReplacementRestoresOriginalP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  View original = View::New();
  original.SetAccessibilityModal(true);
  View replacement = View::New();
  container.SetModalContent(original);
  container.SetModalContent(replacement);
  DALI_TEST_CHECK(original.IsAccessibilityModal());
  DALI_TEST_CHECK(!original.GetParent());
  DALI_TEST_CHECK(replacement.IsAccessibilityModal());
  container.SetModalContent(View());
  DALI_TEST_CHECK(!replacement.IsAccessibilityModal());
  END_TEST;
}

int UtcDaliDialogContainerModalTransferOwnershipP(void)
{
  UiTestApplication application;
  DialogContainer first = DialogContainer::New();
  DialogContainer second = DialogContainer::New();
  View content = View::New();
  first.SetModalContent(content);
  second.SetModalContent(content);
  DALI_TEST_CHECK(!first.GetModalContent());
  DALI_TEST_CHECK(second.GetModalContent() == content);
  DALI_TEST_CHECK(content.IsAccessibilityModal());
  DALI_TEST_CHECK(content.GetParent() == second);
  first.Reset();
  DALI_TEST_CHECK(content.IsAccessibilityModal());
  second.SetModalContent(View());
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}

int UtcDaliDialogContainerExternalReparentReleasesModalP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  View content = View::New();
  View parent = View::New();
  container.SetModalContent(content);
  parent.Add(content);
  DALI_TEST_CHECK(!container.GetModalContent());
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  DALI_TEST_CHECK(content.GetParent() == parent);
  content.SetAccessibilityModal(true);
  container.Reset();
  DALI_TEST_CHECK(content.IsAccessibilityModal());
  END_TEST;
}

int UtcDaliDialogContainerDestructionRestoresModalP(void)
{
  UiTestApplication application;
  View content = View::New();
  {
    DialogContainer container = DialogContainer::New();
    container.SetModalContent(content);
    DALI_TEST_CHECK(content.IsAccessibilityModal());
  }
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}

// These assertions query the real accessible state. They do not capture AT-SPI
// bus events; bridge notification count/order needs a separate bridge-up test.
int UtcDaliDialogContainerModalAccessibleShowingStateP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  container.SetRequestedWidth(200.0f);
  container.SetRequestedHeight(200.0f);
  View content = View::New();
  content.SetRequestedWidth(100.0f);
  content.SetRequestedHeight(100.0f);
  content.SetBackgroundColor(UiColor(0xffffffu));
  container.SetModalContent(content);
  auto* accessible = Dali::Accessibility::Accessible::Get(content);
  DALI_TEST_CHECK(accessible);
  using AccessibleState = Dali::Integration::Accessibility::State;
  DALI_TEST_CHECK(accessible->GetStates()[AccessibleState::MODAL]);
  DALI_TEST_CHECK(!accessible->GetStates()[AccessibleState::SHOWING]);

  container.SetProperty(Actor::Property::VISIBLE, false);
  application.GetScene().Add(container);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!accessible->GetStates()[AccessibleState::SHOWING]);
  container.SetProperty(Actor::Property::VISIBLE, true);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(accessible->GetStates()[AccessibleState::SHOWING]);

  content.SetProperty(Actor::Property::OPACITY, 0.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(!accessible->GetStates()[AccessibleState::SHOWING]);
  content.SetProperty(Actor::Property::OPACITY, 1.0f);
  application.SendNotification();
  application.Render();
  DALI_TEST_CHECK(accessible->GetStates()[AccessibleState::SHOWING]);

  container.SetModalContent(View());
  DALI_TEST_CHECK(!accessible->GetStates()[AccessibleState::MODAL]);
  DALI_TEST_CHECK(!accessible->GetStates()[AccessibleState::SHOWING]);
  END_TEST;
}

#include <dali-ui-components/integration-api/dialog/dialog-container-impl.h>

namespace
{
class DerivedDialogContainerImpl : public Dali::Ui::Integration::DialogContainerImpl
{
public:
  DerivedDialogContainerImpl() = default;
  ~DerivedDialogContainerImpl() override = default;
};
} // namespace

int UtcDaliDialogContainerDerivedInitializationUsesDefaultStyleP(void)
{
  UiTestApplication application;
  IntrusivePtr<DerivedDialogContainerImpl> impl = new DerivedDialogContainerImpl();
  View view(*impl);
  impl->Initialize();
  DALI_TEST_CHECK(impl->GetScrim());
  DALI_TEST_CHECK(impl->GetScrim().GetParent() == view);
  DALI_TEST_CHECK(impl->GetScrim().GetBackgroundColor() == DialogContainerStyle::Default().GetScrimColor());
  END_TEST;
}

#include <dali/devel-api/actors/actor-devel.h>

int UtcDaliDialogContainerModalSceneCallbackCanClearP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  application.GetScene().Add(container);
  View content = View::New();
  DevelActor::OnSceneVisibilityChangedSignal(content).Connect(&application, [&](Actor, bool visible)
  {
    if(visible)
    {
      container.SetModalContent({});
    }
  });
  container.SetModalContent(content);
  DALI_TEST_CHECK(!container.GetModalContent());
  DALI_TEST_CHECK(!content.GetParent());
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}

int UtcDaliDialogContainerModalRemovalCallbackCanReplaceP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  application.GetScene().Add(container);
  View first = View::New();
  View replacement = View::New();
  View callbackContent = View::New();
  container.SetModalContent(first);
  DevelActor::OnSceneVisibilityChangedSignal(first).Connect(&application, [&](Actor, bool visible)
  {
    if(!visible)
    {
      container.SetModalContent(callbackContent);
    }
  });
  container.SetModalContent(replacement);
  // The newer nested request must not be silently released by the older call.
  DALI_TEST_CHECK(container.GetModalContent() == callbackContent);
  DALI_TEST_CHECK(callbackContent.GetParent() == container);
  DALI_TEST_CHECK(callbackContent.IsAccessibilityModal());
  DALI_TEST_CHECK(!replacement.GetParent());
  DALI_TEST_CHECK(!first.IsAccessibilityModal());
  END_TEST;
}

int UtcDaliDialogContainerRejectedAttachmentReleasesModalP(void)
{
  UiTestApplication application;
  DialogContainer container = DialogContainer::New();
  View parent = View::New();
  View content = View::New();
  parent.Add(content);
  parent.ChildRemovedSignal().Connect(&application, [&](Actor, Actor child)
  {
    parent.Add(child);
  });
  container.SetModalContent(content);
  // Core respects the callback's reparenting and declines the attempted Add.
  DALI_TEST_CHECK(content.GetParent() == parent);
  DALI_TEST_CHECK(!container.GetModalContent());
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  container.Reset();
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}

int UtcDaliDialogContainerAttachmentCallbackTransfersToThirdOwnerP(void)
{
  UiTestApplication application;
  DialogContainer requested = DialogContainer::New();
  DialogContainer callbackOwner = DialogContainer::New();
  View parent = View::New();
  View content = View::New();
  parent.Add(content);
  parent.ChildRemovedSignal().Connect(&application, [&](Actor, Actor)
  {
    callbackOwner.SetModalContent(content);
  });
  requested.SetModalContent(content);
  DALI_TEST_CHECK(!requested.GetModalContent());
  DALI_TEST_CHECK(callbackOwner.GetModalContent() == content);
  DALI_TEST_CHECK(content.GetParent() == callbackOwner);
  DALI_TEST_CHECK(content.IsAccessibilityModal());
  requested.Reset();
  DALI_TEST_CHECK(content.IsAccessibilityModal());
  callbackOwner.SetModalContent({});
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}

int UtcDaliDialogContainerDisconnectionCallbackTransfersOwnedModalP(void)
{
  UiTestApplication application;
  DialogContainer first = DialogContainer::New();
  DialogContainer third = DialogContainer::New();
  application.GetScene().Add(first);
  application.GetScene().Add(third);
  View content = View::New();
  first.SetModalContent(content);
  bool transferred = false;
  DevelActor::OnSceneVisibilityChangedSignal(content).Connect(&application, [&](Actor, bool visible)
  {
    if(!visible && !transferred)
    {
      transferred = true;
      third.SetModalContent(content);
    }
  });
  first.SetModalContent({});
  DALI_TEST_CHECK(transferred);
  DALI_TEST_CHECK(!first.GetModalContent());
  DALI_TEST_CHECK(third.GetModalContent() == content);
  DALI_TEST_CHECK(content.GetParent() == third);
  DALI_TEST_CHECK(content.IsAccessibilityModal());
  third.SetModalContent({});
  DALI_TEST_CHECK(!content.IsAccessibilityModal());
  END_TEST;
}
