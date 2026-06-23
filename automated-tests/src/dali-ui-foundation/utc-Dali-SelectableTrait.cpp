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
#include <dali-ui-foundation/internal/views/view/selectable-trait-impl.h>
#include <dali-ui-foundation/integration-api/view-integ.h>
#include <dali-ui-foundation/public-api/selectable-view-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali/devel-api/object/type-registry.h>
#include <test-gesture-generator.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>

using namespace Dali;
using namespace Dali::Ui;
using namespace Dali::Ui::Integration;

namespace
{

// ============================================================================
// Signal callback helpers
// ============================================================================

struct ClickedSignalData
{
  ClickedSignalData()
  : called(false)
  {
  }

  bool called;
  View view;
};

struct ClickedSignalFunctor
{
  ClickedSignalFunctor(ClickedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, InputEvent event)
  {
    signalData.called = true;
    signalData.view   = view;
  }

  ClickedSignalData& signalData;
};

struct SelectionChangedSignalData
{
  SelectionChangedSignalData()
  : called(false),
    selected(false)
  {
  }

  void Reset()
  {
    called   = false;
    selected = false;
    view     = View();
    event    = InputEvent();
  }

  bool called;
  bool selected;
  View view;
  InputEvent event;
};

struct SelectionChangedSignalFunctor
{
  SelectionChangedSignalFunctor(SelectionChangedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(View view, bool selected, InputEvent event)
  {
    signalData.called   = true;
    signalData.selected = selected;
    signalData.view     = view;
    signalData.event    = event;
  }

  SelectionChangedSignalData& signalData;
};

class TestSelectableViewImpl : public SelectableViewImpl
{
public:
  static IntrusivePtr<TestSelectableViewImpl> New()
  {
    return new TestSelectableViewImpl();
  }
};

Dali::TypeRegistration testSelectableViewImplTypeReg(typeid(TestSelectableViewImpl), typeid(SelectableViewImpl), nullptr);

/**
 * @brief Helper to create a View, add it to the scene, attach selectable, and render.
 */
View CreateSelectableView(TestApplication& application, float width = 100.0f, float height = 100.0f)
{
  View view = View::New();
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);
  view.AsSelectable();

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

SelectableView CreateSelectableViewFromImpl(TestApplication& application, IntrusivePtr<TestSelectableViewImpl> impl)
{
  SelectableView view(*impl);
  impl->Initialize();

  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetPivot(Pivot::TOP_LEFT);
  view.SetParentOrigin(ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  return view;
}

} // namespace

void utc_dali_selectabletrait_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_selectabletrait_cleanup(void)
{
  test_return_value = TET_PASS;
}

// ============================================================================
// Construction / Handle tests
// ============================================================================

int UtcDaliSelectableTraitNewP(void)
{
  UiTestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(selectable);
  END_TEST;
}

int UtcDaliSelectableTraitCopyConstructorP(void)
{
  UiTestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  SelectableTrait copy(selectable);
  DALI_TEST_CHECK(copy);
  DALI_TEST_CHECK(copy == selectable);
  END_TEST;
}

int UtcDaliSelectableTraitDownCastP(void)
{
  UiTestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  BaseHandle         handle(selectable);
  SelectableTrait downcast = SelectableTrait::DownCast(handle);
  DALI_TEST_CHECK(downcast);
  END_TEST;
}

int UtcDaliSelectableTraitDownCastN(void)
{
  UiTestApplication    application;
  BaseHandle         handle;
  SelectableTrait downcast = SelectableTrait::DownCast(handle);
  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

// ============================================================================
// AsSelectable (View integration)
// ============================================================================

int UtcDaliViewAsSelectableP(void)
{
  UiTestApplication application;
  View view = View::New();

  SelectableTrait result = view.AsSelectable();
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(view.IsSelectable());
  END_TEST;
}

int UtcDaliViewAsSelectableWithConfigureP(void)
{
  UiTestApplication application;
  bool configureCalled = false;

  View view = View::New();
  SelectableTrait configuredTrait = view.AsSelectable();
  configureCalled                 = true;
  configuredTrait.EnableToggleByClick();

  DALI_TEST_CHECK(configureCalled);

  SelectableTrait selectable = view.AsSelectable();
  DALI_TEST_CHECK(selectable);
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());
  END_TEST;
}

int UtcDaliViewAsSelectableIdempotentP(void)
{
  UiTestApplication application;
  View view = View::New();

  view.AsSelectable();
  SelectableTrait first = view.AsSelectable();

  view.AsSelectable();
  SelectableTrait second = view.AsSelectable();

  DALI_TEST_CHECK(first == second);
  END_TEST;
}

int UtcDaliViewIsSelectableWithoutAttachN(void)
{
  UiTestApplication application;
  View view = View::New();

  DALI_TEST_CHECK(!view.IsSelectable());
  END_TEST;
}

int UtcDaliViewEnsureSelectableTraitP(void)
{
  UiTestApplication application;
  View view = View::New();

  SelectableTrait selectable = view.AsSelectable();
  DALI_TEST_CHECK(selectable);

  // Second call returns the same trait
  SelectableTrait again = view.AsSelectable();
  DALI_TEST_CHECK(selectable == again);
  END_TEST;
}

// ============================================================================
// Interactive + Selectable coexistence
// ============================================================================

int UtcDaliViewInteractiveAndSelectableCoexistP(void)
{
  UiTestApplication application;
  View view = View::New();

  view.AsInteractive();
  view.AsSelectable();

  DALI_TEST_CHECK(view.IsInteractive());
  DALI_TEST_CHECK(view.IsSelectable());
  END_TEST;
}

int UtcDaliViewSelectableBeforeInteractiveP(void)
{
  UiTestApplication application;
  View view = View::New();

  // Selectable first, then clickable
  view.AsSelectable();
  view.AsInteractive();

  DALI_TEST_CHECK(view.IsInteractive());
  DALI_TEST_CHECK(view.IsSelectable());
  END_TEST;
}

// ============================================================================
// State API tests
// ============================================================================

int UtcDaliSelectableTraitIsSelectedDefaultP(void)
{
  UiTestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitSetSelectedP(void)
{
  UiTestApplication application;
  View view = CreateSelectableView(application);
  SelectableTrait selectable = view.AsSelectable();

  selectable.SetSelected(true);
  DALI_TEST_CHECK(selectable.IsSelected());

  selectable.SetSelected(false);
  DALI_TEST_CHECK(!selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitSetSelectedNoChangeP(void)
{
  UiTestApplication application;
  View view = CreateSelectableView(application);
  SelectableTrait selectable = view.AsSelectable();

  SelectionChangedSignalData    data;
  SelectionChangedSignalFunctor functor(data);
  selectable.SelectionChangedSignal().Connect(&application, functor);

  // Set to same value (already false)
  selectable.SetSelected(false);
  DALI_TEST_CHECK(!data.called); // Signal should NOT fire when no actual change
  END_TEST;
}

int UtcDaliSelectableTraitIsToggleByClickEnabledDefaultP(void)
{
  UiTestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());
  END_TEST;
}

int UtcDaliSelectableTraitEnableToggleByClickP(void)
{
  UiTestApplication    application;
  SelectableTrait selectable = SelectableTrait::New();

  selectable.EnableToggleByClick();
  DALI_TEST_CHECK(selectable.IsToggleByClickEnabled());

  selectable.EnableToggleByClick(false);
  DALI_TEST_CHECK(!selectable.IsToggleByClickEnabled());
  END_TEST;
}

// ============================================================================
// SelectionChangedSignal
// ============================================================================

int UtcDaliSelectableTraitSelectionChangedSignalP(void)
{
  UiTestApplication application;
  View view = CreateSelectableView(application);
  SelectableTrait selectable = view.AsSelectable();

  SelectionChangedSignalData    data;
  SelectionChangedSignalFunctor functor(data);
  selectable.SelectionChangedSignal().Connect(&application, functor);

  selectable.SetSelected(true);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.selected);
  DALI_TEST_CHECK(data.view == view);

  data.Reset();

  selectable.SetSelected(false);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.selected);
  END_TEST;
}

int UtcDaliViewAsSelectableWithSignalLambdaP(void)
{
  UiTestApplication application;

  bool signalCalled   = false;
  bool signalSelected = false;

  View            view       = View::New();
  SelectableTrait selectable = view.AsSelectable();
  selectable.SelectionChangedSignal().Connect(&application,
    [&signalCalled, &signalSelected](View v, bool selected, InputEvent e) {
      signalCalled   = true;
      signalSelected = selected;
    });
  DALI_TEST_CHECK(selectable);

  // Manually add to scene for proper lifecycle
  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  selectable.SetSelected(true);

  DALI_TEST_CHECK(signalCalled);
  DALI_TEST_CHECK(signalSelected);
  END_TEST;
}

// ============================================================================
// Toggle-by-click interaction
// ============================================================================

int UtcDaliSelectableTraitToggleByClickP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsSelectable().EnableToggleByClick();

  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.AsSelectable();

  // InteractiveTrait should have been auto-created by EnableToggleByClick
  InteractiveTrait clickable = view.AsInteractive();
  DALI_TEST_CHECK(clickable);

  SelectionChangedSignalData    data;
  SelectionChangedSignalFunctor functor(data);
  selectable.SelectionChangedSignal().Connect(&application, functor);

  InputEvent stateCause;
  int        stateChangedCount = 0;
  ConnectionTracker tracker;
  IntegrationView::WhenStateChanged(GetImpl(view), "observer", &tracker, [&](View, const StateEvent& e) {
    if(e.Changed(ViewState::SELECTED))
    {
      ++stateChangedCount;
      stateCause = e.GetCause();
    }
  });

  DALI_TEST_CHECK(!selectable.IsSelected());

  // First tap: select
  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.selected);
  DALI_TEST_CHECK(!data.event.IsProgrammatic());
  DALI_TEST_CHECK(!data.event.IsCancellation());
  DALI_TEST_CHECK(selectable.IsSelected());
  DALI_TEST_EQUALS(stateChangedCount, 1, TEST_LOCATION);
  DALI_TEST_CHECK(stateCause.GetInputEventType() == InputEventType::TAP_GESTURE);
  DALI_TEST_CHECK(!stateCause.IsProgrammatic());
  DALI_TEST_CHECK(!stateCause.IsCancellation());

  data.Reset();

  // Second tap: deselect
  TestGenerateTap(application, 50.0f, 50.0f, 300);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(!data.selected);
  DALI_TEST_CHECK(!data.event.IsProgrammatic());
  DALI_TEST_CHECK(!data.event.IsCancellation());
  DALI_TEST_CHECK(!selectable.IsSelected());
  DALI_TEST_EQUALS(stateChangedCount, 2, TEST_LOCATION);
  DALI_TEST_CHECK(stateCause.GetInputEventType() == InputEventType::TAP_GESTURE);
  DALI_TEST_CHECK(!stateCause.IsProgrammatic());
  DALI_TEST_CHECK(!stateCause.IsCancellation());
  END_TEST;
}

int UtcDaliSelectableTraitToggleByClickAutoCreatesInteractiveP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  // No clickable trait exists yet
  DALI_TEST_CHECK(!view.IsInteractive());

  // AsSelectable with toggle-by-click should auto-create clickable
  view.AsSelectable().EnableToggleByClick();

  application.SendNotification();
  application.Render();

  // InteractiveTrait should now exist
  DALI_TEST_CHECK(view.IsInteractive());
  END_TEST;
}

int UtcDaliSelectableTraitToggleByClickWithExistingInteractiveP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  // Attach clickable first
  view.AsInteractive();
  InteractiveTrait existingInteractive = view.AsInteractive();

  // Then attach selectable with toggle
  view.AsSelectable().EnableToggleByClick();

  application.SendNotification();
  application.Render();

  // Should reuse the existing clickable, not create a new one
  InteractiveTrait clickableAfter = view.AsInteractive();
  DALI_TEST_CHECK(existingInteractive == clickableAfter);

  // Toggle should still work
  SelectableTrait selectable = view.AsSelectable();
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitToggleByClickDoesNotConsumeClickP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsInteractive();
  view.AsSelectable().EnableToggleByClick();

  application.SendNotification();
  application.Render();

  // Connect a separate clicked handler
  bool appClickedCalled = false;
  view.AsInteractive().ClickedSignal().Connect(
    &application,
    [&appClickedCalled](View v, InputEvent e) -> bool {
      appClickedCalled = true;
      return false;
    });

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  // Both should fire: selectable toggle AND app clicked handler
  DALI_TEST_CHECK(view.AsSelectable().IsSelected());
  DALI_TEST_CHECK(appClickedCalled);
  END_TEST;
}

// ============================================================================
// Toggle-by-click via key event
// ============================================================================

int UtcDaliSelectableTraitToggleByKeyP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsSelectable().EnableToggleByClick();
  view.AsInteractive().SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);

  application.SendNotification();
  application.Render();

  FocusManager::Get().SetCurrentFocusView(view);
  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.AsSelectable();
  DALI_TEST_CHECK(!selectable.IsSelected());

  // Key press + release → click → toggle
  Dali::Integration::KeyEvent keyDown(
    "Return", "", "", 0, 0, 100, Dali::Integration::KeyEvent::DOWN, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyDown);

  Dali::Integration::KeyEvent keyUp(
    "Return", "", "", 0, 0, 120, Dali::Integration::KeyEvent::UP, "", "", Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(keyUp);

  DALI_TEST_CHECK(selectable.IsSelected());
  END_TEST;
}

// ============================================================================
// Enable/disable toggle-by-click after attachment
// ============================================================================

int UtcDaliSelectableTraitEnableToggleByClickAfterAttachP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  // Attach selectable WITHOUT toggle-by-click (explicitly disable, since default is true)
  view.AsSelectable().EnableToggleByClick(false);

  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.AsSelectable();

  // Tap should NOT toggle
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(!selectable.IsSelected());

  // Now enable toggle-by-click
  selectable.EnableToggleByClick();

  // Tap should now toggle
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(selectable.IsSelected());
  END_TEST;
}

int UtcDaliSelectableTraitDisableToggleByClickP(void)
{
  UiTestApplication application;
  View view = View::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  application.GetScene().Add(view);

  view.AsSelectable().EnableToggleByClick();

  application.SendNotification();
  application.Render();

  SelectableTrait selectable = view.AsSelectable();

  // First tap works
  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(selectable.IsSelected());

  // Disable toggle
  selectable.EnableToggleByClick(false);

  // Tap should no longer toggle
  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(selectable.IsSelected()); // Still true, not toggled back
  END_TEST;
}

// ============================================================================
// Chaining API
// ============================================================================

int UtcDaliViewAsInteractiveAsSelectableChainingP(void)
{
  UiTestApplication application;
  View view = View::New();

  InteractiveTrait interactive = view.AsInteractive();
  SelectableTrait  result      = view.AsSelectable();
  DALI_TEST_CHECK(interactive);
  DALI_TEST_CHECK(result);

  DALI_TEST_CHECK(view.IsInteractive());
  DALI_TEST_CHECK(view.IsSelectable());
  END_TEST;
}

// ============================================================================
// SelectableView
// ============================================================================

int UtcDaliSelectableViewNewP(void)
{
  UiTestApplication application;
  SelectableView    view = SelectableView::New();

  DALI_TEST_CHECK(view);
  DALI_TEST_CHECK(view.IsInteractive());
  DALI_TEST_CHECK(view.IsSelectable());
  END_TEST;
}

int UtcDaliSelectableViewDownCastP(void)
{
  UiTestApplication application;
  SelectableView    view     = SelectableView::New();
  BaseHandle        handle   = view;
  SelectableView    downcast = SelectableView::DownCast(handle);

  DALI_TEST_CHECK(downcast);
  DALI_TEST_CHECK(downcast == view);
  END_TEST;
}

int UtcDaliSelectableViewDownCastN(void)
{
  UiTestApplication application;
  View              view     = View::New();
  SelectableView    downcast = SelectableView::DownCast(view);

  DALI_TEST_CHECK(!downcast);
  END_TEST;
}

int UtcDaliSelectableViewSelectionApiP(void)
{
  UiTestApplication application;
  SelectableView    view = SelectableView::New();

  DALI_TEST_CHECK(!view.IsSelected());

  view.SetSelected(true);
  DALI_TEST_CHECK(view.IsSelected());

  view.SetSelected(false);
  DALI_TEST_CHECK(!view.IsSelected());
  END_TEST;
}

int UtcDaliSelectableViewSelectionChangedSignalP(void)
{
  UiTestApplication application;
  SelectableView    view = SelectableView::New();

  SelectionChangedSignalData    data;
  SelectionChangedSignalFunctor functor(data);
  view.SelectionChangedSignal().Connect(&application, functor);

  view.SetSelected(true);

  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.selected);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

int UtcDaliSelectableViewToggleByClickP(void)
{
  UiTestApplication application;
  SelectableView    view = SelectableView::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.EnableToggleByClick();

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  TestGenerateTap(application, 50.0f, 50.0f, 100);

  DALI_TEST_CHECK(view.IsSelected());
  END_TEST;
}

int UtcDaliSelectableViewInheritedInteractiveApiP(void)
{
  UiTestApplication application;
  SelectableView    view = SelectableView::New();
  view.SetRequestedWidth(100.0f);
  view.SetRequestedHeight(100.0f);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);

  application.GetScene().Add(view);
  application.SendNotification();
  application.Render();

  ClickedSignalData    data;
  ClickedSignalFunctor functor(data);
  view.ClickedSignal().Connect(&application, functor);

  view.SetClickable(false);
  DALI_TEST_CHECK(!view.IsClickable());

  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(!data.called);

  view.SetClickable(true);
  view.SetKeyClickPolicy(KeyClickPolicy::ON_RELEASE);
  DALI_TEST_EQUALS(static_cast<uint32_t>(view.GetKeyClickPolicy()),
                   static_cast<uint32_t>(KeyClickPolicy::ON_RELEASE),
                   TEST_LOCATION);

  TestGenerateTap(application, 50.0f, 50.0f, 300);
  DALI_TEST_CHECK(data.called);
  DALI_TEST_CHECK(data.view == view);
  END_TEST;
}

int UtcDaliSelectableViewImplSubclassSmokeP(void)
{
  UiTestApplication application;

  auto           impl = TestSelectableViewImpl::New();
  SelectableView view = CreateSelectableViewFromImpl(application, impl);

  DALI_TEST_CHECK(view.IsInteractive());
  DALI_TEST_CHECK(view.IsSelectable());
  DALI_TEST_CHECK(!view.IsSelected());

  SelectionChangedSignalData    selectionData;
  SelectionChangedSignalFunctor selectionFunctor(selectionData);
  view.SelectionChangedSignal().Connect(&application, selectionFunctor);

  view.SetSelected(true);
  DALI_TEST_CHECK(view.IsSelected());
  DALI_TEST_CHECK(selectionData.called);
  DALI_TEST_CHECK(selectionData.selected);
  DALI_TEST_CHECK(selectionData.view == view);

  ClickedSignalData    clickData;
  ClickedSignalFunctor clickFunctor(clickData);
  view.ClickedSignal().Connect(&application, clickFunctor);

  TestGenerateTap(application, 50.0f, 50.0f, 100);
  DALI_TEST_CHECK(clickData.called);
  DALI_TEST_CHECK(clickData.view == view);
  END_TEST;
}
