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

// EXTERNAL INCLUDES
#include <dali/devel-api/actors/actor-devel.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-test-suite-utils.h>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
template<typename Control, typename Implementation>
struct TextChangedFixture
{
  TextChangedFixture()
  : control(Control::New()),
    label(Label::New())
  {
    control.SetRequestedWidth(300.0f);
    control.SetRequestedHeight(60.0f);
    control.SetCursorBlinkEnabled(false);
    label.SetRequestedWidth(300.0f);
    label.SetRequestedHeight(60.0f);
    application.GetWindow().Add(control);
    application.GetWindow().Add(label);
    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();
    application.GetRenderController().Initialize();
  }

  Implementation& GetImpl()
  {
    return static_cast<Implementation&>(control.GetImplementation());
  }

  void QueueTextChanged()
  {
    // Enter the same deferred path as input events, without IME, focus, cursor
    // movement or font-dependent geometry introducing an unrelated idle wake.
    GetImpl().TextChanged(false);
    // Changing the font size requests native relayout without changing the
    // fixed control size or emitting an immediate TextChanged signal.
    control.SetFontSize(control.GetFontSize() + 1.0f);
  }

  void ProcessEvents()
  {
    application.GetRenderController().Initialize();
    application.SendNotification();
  }

  bool IdleRequested()
  {
    return application.GetRenderController().WasCalled(TestRenderController::RequestProcessEventsOnIdleFunc);
  }

  UiTestApplication application;
  Control           control;
  Label             label;
};

using FieldFixture  = TextChangedFixture<InputField, Dali::Ui::Integration::InputFieldImpl>;
using EditorFixture = TextChangedFixture<InputEditor, Dali::Ui::Integration::InputEditorImpl>;

template<typename Fixture>
static int DeferredTextChangedUpdatesLabel(bool disconnect)
{
  Fixture           fixture;
  unsigned int      changes = 0u;
  unsigned int      layouts = 0u;
  ConnectionTracker observer;
  ConnectionTracker listener;
  DevelActor::OnRelayoutSignal(fixture.label).Connect(&observer, [&](Actor)
  { ++layouts; });
  fixture.control.TextChangedSignal().Connect(&listener, [&](View)
  {
    ++changes;
    fixture.label.SetText("updated");
    // Disconnect last: removing this connection also destroys the stored lambda.
    if(disconnect)
    {
      listener.DisconnectAll();
    }
  });

  fixture.QueueTextChanged();
  DALI_TEST_EQUALS(changes, 0u, TEST_LOCATION);
  fixture.ProcessEvents();
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.label.GetText(), String("updated"), TEST_LOCATION);
  DALI_TEST_EQUALS(layouts, 0u, TEST_LOCATION);
  DALI_TEST_CHECK(fixture.IdleRequested());

  // Consume the requested cycle only; an unconditional extra cycle would hide
  // the original bug by flushing a stranded label even without an idle request.
  fixture.ProcessEvents();
  DALI_TEST_EQUALS(layouts, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!fixture.IdleRequested());
  END_TEST;
}

template<typename Fixture>
static int DeferredTextChangedWithoutListeners()
{
  Fixture fixture;
  fixture.QueueTextChanged();
  fixture.ProcessEvents();
  DALI_TEST_CHECK(!fixture.IdleRequested());
  END_TEST;
}

template<typename Fixture>
static int DeferredTextChangedSettles()
{
  Fixture           fixture;
  unsigned int      changes = 0u;
  ConnectionTracker listener;
  fixture.control.TextChangedSignal().Connect(&listener, [&](View)
  { ++changes; });
  fixture.QueueTextChanged();
  fixture.ProcessEvents();
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(fixture.IdleRequested());
  fixture.ProcessEvents();
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!fixture.IdleRequested());
  END_TEST;
}

template<typename Fixture>
static int TextChangedSetsOwnText()
{
  Fixture           fixture;
  unsigned int      changes  = 0u;
  unsigned int      observed = 0u;
  ConnectionTracker listener;
  fixture.control.TextChangedSignal().Connect(&listener, [&](View)
  {
    ++changes;
    // Deliberately unconditional: a replay on the next idle must not be hidden
    // by a one-shot application guard.
    fixture.control.SetText("normalized");
    fixture.control.SetCursorPosition(1u);
  });
  fixture.control.TextChangedSignal().Connect(&listener, [&](View)
  { ++observed; });
  fixture.QueueTextChanged();
  fixture.ProcessEvents();
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(observed, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(fixture.IdleRequested());
  fixture.ProcessEvents();
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(observed, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.control.GetText(), String("normalized"), TEST_LOCATION);
  DALI_TEST_EQUALS(fixture.control.GetCursorPosition(), 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!fixture.IdleRequested());
  END_TEST;
}

template<typename Fixture>
static int ImmediateTextChangedDoesNotWake()
{
  Fixture           fixture;
  unsigned int      changes = 0u;
  ConnectionTracker listener;
  fixture.control.TextChangedSignal().Connect(&listener, [&](View)
  { ++changes; });
  fixture.GetImpl().TextChanged(true);
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!fixture.IdleRequested());
  fixture.ProcessEvents();
  DALI_TEST_EQUALS(changes, 1u, TEST_LOCATION);
  DALI_TEST_CHECK(!fixture.IdleRequested());
  END_TEST;
}

} // namespace

void utc_dali_editable_text_changed_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_editable_text_changed_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliInputFieldDeferredTextChangedUpdatesLabelP(void)
{
  return DeferredTextChangedUpdatesLabel<FieldFixture>(false);
}

int UtcDaliInputFieldDeferredTextChangedDisconnectsP(void)
{
  return DeferredTextChangedUpdatesLabel<FieldFixture>(true);
}

int UtcDaliInputFieldDeferredTextChangedNoListenersN(void)
{
  return DeferredTextChangedWithoutListeners<FieldFixture>();
}

int UtcDaliInputFieldDeferredTextChangedSettlesP(void)
{
  return DeferredTextChangedSettles<FieldFixture>();
}

int UtcDaliInputFieldDeferredTextChangedSetsOwnTextP(void)
{
  return TextChangedSetsOwnText<FieldFixture>();
}

int UtcDaliInputFieldImmediateTextChangedDoesNotWakeN(void)
{
  return ImmediateTextChangedDoesNotWake<FieldFixture>();
}

int UtcDaliInputEditorDeferredTextChangedUpdatesLabelP(void)
{
  return DeferredTextChangedUpdatesLabel<EditorFixture>(false);
}

int UtcDaliInputEditorDeferredTextChangedDisconnectsP(void)
{
  return DeferredTextChangedUpdatesLabel<EditorFixture>(true);
}

int UtcDaliInputEditorDeferredTextChangedNoListenersN(void)
{
  return DeferredTextChangedWithoutListeners<EditorFixture>();
}

int UtcDaliInputEditorDeferredTextChangedSettlesP(void)
{
  return DeferredTextChangedSettles<EditorFixture>();
}

int UtcDaliInputEditorDeferredTextChangedSetsOwnTextP(void)
{
  return TextChangedSetsOwnText<EditorFixture>();
}

int UtcDaliInputEditorImmediateTextChangedDoesNotWakeN(void)
{
  return ImmediateTextChangedDoesNotWake<EditorFixture>();
}
