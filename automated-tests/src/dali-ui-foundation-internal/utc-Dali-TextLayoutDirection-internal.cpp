/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/integration-api/input-editor-impl.h>
#include <dali-ui-foundation/integration-api/input-field-impl.h>
#include <dali-ui-foundation/integration-api/label-impl.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-loader-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-event-thread-callback.h>
#include <initializer_list>

using namespace Dali;
using namespace Dali::Ui;

void utc_dali_text_layout_direction_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_layout_direction_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
constexpr auto LTR = Dali::LayoutDirection::LEFT_TO_RIGHT;
constexpr auto RTL = Dali::LayoutDirection::RIGHT_TO_LEFT;
const Size     BOX(320.0f, 100.0f);

// Inspect the production layout and worker output without changing access
// levels or adding test-only APIs to the library.
template<typename Tag, typename Tag::Type member>
struct MemberAccess
{
  friend typename Tag::Type GetMember(Tag)
  {
    return member;
  }
};

struct LabelController
{
  using Type = Text::ControllerPtr Ui::Integration::LabelImpl::*;
  friend Type                      GetMember(LabelController);
};
struct FieldController
{
  using Type = Text::ControllerPtr Ui::Integration::InputFieldImpl::*;
  friend Type                      GetMember(FieldController);
};
struct EditorController
{
  using Type = Text::ControllerPtr Ui::Integration::InputEditorImpl::*;
  friend Type                      GetMember(EditorController);
};
struct LabelParameters
{
  using Type = Text::AsyncTextParameters (Ui::Integration::LabelImpl::*)(Ui::Integration::Text::Async::RequestType,
                                                                         const Vector2&, const Insets&, Dali::LayoutDirection::Type);
  friend Type GetMember(LabelParameters);
};
struct WorkerModel
{
  using Type = const Text::Model* (Text::Internal::AsyncTextLoader::*)() const;
  friend Type GetMember(WorkerModel);
};

template struct MemberAccess<LabelController, &Ui::Integration::LabelImpl::mController>;
template struct MemberAccess<FieldController, &Ui::Integration::InputFieldImpl::mController>;
template struct MemberAccess<EditorController, &Ui::Integration::InputEditorImpl::mController>;
template struct MemberAccess<LabelParameters, &Ui::Integration::LabelImpl::GetAsyncTextParameters>;
template struct MemberAccess<WorkerModel, &Text::Internal::AsyncTextLoader::GetRenderTextModel>;

Text::ControllerPtr ControllerFor(Label control)
{
  return static_cast<Ui::Integration::LabelImpl&>(GetImpl(control)).*GetMember(LabelController{});
}
Text::ControllerPtr ControllerFor(InputField control)
{
  return static_cast<Ui::Integration::InputFieldImpl&>(GetImpl(control)).*GetMember(FieldController{});
}
Text::ControllerPtr ControllerFor(InputEditor control)
{
  return static_cast<Ui::Integration::InputEditorImpl&>(GetImpl(control)).*GetMember(EditorController{});
}

void CheckAlignment(const Text::Model& model, Dali::LayoutDirection::Type expected)
{
  // Verify actual laid-out text, not just the actor direction property: those
  // properties already reported LTR in the original failing Korean panel.
  const auto& lines = model.mVisualModel->mLines;
  DALI_TEST_EQUALS(lines.Count(), 1u, TEST_LOCATION);
  if(lines.Empty())
  {
    return;
  }
  DALI_TEST_CHECK(lines[0].width > 0.0f && lines[0].width < BOX.width - 100.0f);
  if(expected == LTR)
  {
    DALI_TEST_EQUALS(lines[0].alignmentOffset, 0.0f, 0.01f, TEST_LOCATION);
  }
  else
  {
    DALI_TEST_CHECK(lines[0].alignmentOffset > 100.0f);
  }
}

template<typename Control>
void CheckAlignment(Control control, Dali::LayoutDirection::Type expected)
{
  CheckAlignment(*Text::Controller::Impl::GetImplementation(*ControllerFor(control)).mModel, expected);
}

template<typename Control>
void Configure(Control control)
{
  control.SetRequestedWidth(BOX.width);
  control.SetRequestedHeight(BOX.height);
  control.SetFontSize(16.0f);
  control.SetText("Hello Alex");
  control.SetHorizontalTextAlignment(Text::Alignment::START);
  control.SetLayoutDirectionMode(Text::LayoutDirectionMode::INHERIT);
}

void Relayout(UiTestApplication& application)
{
  application.SendNotification();
  application.Render();
}

void SetRootDirection(UiTestApplication& application, Dali::LayoutDirection::Type direction)
{
  // Reproduce the root-direction update and locale notification delivered by
  // SetApplicationLocale(), without mutating the test process's C locale.
  application.GetScene().GetRootLayer().SetLayoutDirection(direction);
  application.GetAdaptor().LocaleChangedSignal().Emit(direction == RTL ? "ar_AE" : "en_US");
  Relayout(application);
}

View Panel(Dali::LayoutDirection::Type direction)
{
  View panel = View::New();
  panel.SetRequestedWidth(BOX.width);
  panel.SetRequestedHeight(BOX.height);
  panel.SetLayoutDirection(direction);
  return panel;
}

template<typename Control>
void FixedLtrBoundary(UiTestApplication& application, Control control, bool initiallyRtl)
{
  SetRootDirection(application, initiallyRtl ? RTL : LTR);
  View panel = Panel(LTR);
  Configure(control);
  unsigned          directionChanges = 0u;
  ConnectionTracker tracker;
  control.LayoutDirectionChangedSignal().Connect(&tracker, [&directionChanges](Actor, Dali::LayoutDirection::Type)
  {
    ++directionChanges;
  });
  panel.Add(control);
  application.GetScene().Add(panel);
  Relayout(application);
  CheckAlignment(control, LTR);

  for(auto direction : {RTL, LTR, RTL, LTR})
  {
    SetRootDirection(application, direction);
    DALI_TEST_EQUALS(control.GetEffectiveLayoutDirection(), LTR, TEST_LOCATION);
    DALI_TEST_EQUALS(directionChanges, 0u, TEST_LOCATION);
    CheckAlignment(control, LTR);
  }
  panel.Unparent();
}

template<typename Control>
void PolicyMatrix(UiTestApplication& application, Control control)
{
  Configure(control);
  View panel = Panel(LTR);
  panel.Add(control);
  application.GetScene().Add(panel);
  for(auto rootDirection : {LTR, RTL})
  {
    for(auto panelDirection : {LTR, RTL, Dali::LayoutDirection::INHERIT})
    {
      panel.SetLayoutDirection(panelDirection);
      SetRootDirection(application, rootDirection);
      for(auto mode : {Text::LayoutDirectionMode::INHERIT, Text::LayoutDirectionMode::LOCALE, Text::LayoutDirectionMode::CONTENTS})
      {
        control.SetLayoutDirectionMode(mode);
        Relayout(application);
        const auto expected = mode == Text::LayoutDirectionMode::CONTENTS ? LTR
                              : mode == Text::LayoutDirectionMode::LOCALE ? rootDirection
                                                                          : panel.GetEffectiveLayoutDirection();
        CheckAlignment(control, expected);
      }
    }
  }
  panel.Unparent();
}
} // namespace

int UtcDaliTextLayoutDirectionFixedLtrBoundaryP(void)
{
  UiTestApplication application;
  FixedLtrBoundary(application, Label::New(), false);
  END_TEST;
}

int UtcDaliTextLayoutDirectionInputFieldFixedLtrBoundaryP(void)
{
  UiTestApplication application;
  FixedLtrBoundary(application, InputField::New(), false);
  END_TEST;
}

int UtcDaliTextLayoutDirectionInputEditorFixedLtrBoundaryP(void)
{
  UiTestApplication application;
  FixedLtrBoundary(application, InputEditor::New(), false);
  END_TEST;
}

int UtcDaliTextLayoutDirectionInitiallyRtlSceneP(void)
{
  UiTestApplication application;
  FixedLtrBoundary(application, Label::New(), true);
  END_TEST;
}

int UtcDaliTextLayoutDirectionInputFieldInitiallyRtlSceneP(void)
{
  UiTestApplication application;
  FixedLtrBoundary(application, InputField::New(), true);
  END_TEST;
}

int UtcDaliTextLayoutDirectionInputEditorInitiallyRtlSceneP(void)
{
  UiTestApplication application;
  FixedLtrBoundary(application, InputEditor::New(), true);
  END_TEST;
}

int UtcDaliTextLayoutDirectionPolicyMatrixP(void)
{
  UiTestApplication application;
  PolicyMatrix(application, Label::New());
  PolicyMatrix(application, InputField::New());
  PolicyMatrix(application, InputEditor::New());
  END_TEST;
}

int UtcDaliTextLayoutDirectionReparentP(void)
{
  UiTestApplication application;
  SetRootDirection(application, RTL);
  View ltrPanel = Panel(LTR);
  View rtlPanel = Panel(RTL);
  View nested   = Panel(Dali::LayoutDirection::INHERIT);
  auto label    = Label::New();
  Configure(label);
  nested.Add(label);
  ltrPanel.Add(nested);
  application.GetScene().Add(ltrPanel);
  application.GetScene().Add(rtlPanel);
  Relayout(application);
  CheckAlignment(label, LTR);
  rtlPanel.Add(nested);
  Relayout(application);
  CheckAlignment(label, RTL);
  ltrPanel.Add(nested);
  Relayout(application);
  CheckAlignment(label, LTR);
  ltrPanel.Unparent();
  application.GetScene().Add(ltrPanel);
  Relayout(application);
  CheckAlignment(label, LTR);
  END_TEST;
}

int UtcDaliTextLayoutDirectionOffSceneP(void)
{
  UiTestApplication application;
  auto              label      = Label::New();
  auto              controller = ControllerFor(label);
  Configure(label);
  Actor actor = label;
  DALI_TEST_EQUALS(controller->GetLayoutDirection(actor), LTR, TEST_LOCATION);
  label.SetLayoutDirection(RTL);
  DALI_TEST_EQUALS(controller->GetLayoutDirection(actor), RTL, TEST_LOCATION);
  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::LOCALE);
  DALI_TEST_EQUALS(controller->GetLayoutDirection(actor), LTR, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextLayoutDirectionAsyncWorkerP(void)
{
  UiTestApplication application;
  View              panel = Panel(LTR);
  auto              label = Label::New();
  Configure(label);
  panel.Add(label);
  application.GetScene().Add(panel);
  auto worker = Text::AsyncTextLoader::New();
  worker.SetLocale("en_US");
  for(auto rootDirection : {LTR, RTL, LTR})
  {
    SetRootDirection(application, rootDirection);
    auto& implementation = static_cast<Ui::Integration::LabelImpl&>(GetImpl(label));
    Actor actor          = label;
    auto  parameters     = (implementation.*GetMember(LabelParameters{}))(
      Ui::Integration::Text::Async::RENDER_FIXED_SIZE, BOX, Insets(0.0f, 0.0f, 0.0f, 0.0f), ControllerFor(label)->GetLayoutDirection(actor));
    auto output = worker.RenderText(parameters, false, Size::ZERO);
    DALI_TEST_CHECK(output.textPixelData);
    CheckAlignment(*(Text::GetImplementation(worker).*GetMember(WorkerModel{}))(), LTR);
  }
  END_TEST;
}

int UtcDaliTextLayoutDirectionAsyncRenderingP(void)
{
  UiTestApplication application;
  SetRootDirection(application, RTL);
  View panel = Panel(LTR);
  auto label = Label::New();
  Configure(label);
  label.SetLayoutMode(LayoutMode::STANDALONE);
  label.SetMultiLine(true);
  label.SetAsyncRendering(true);

  uint32_t          completions    = 0u;
  int               completedLines = 0;
  Vector2           renderedSize;
  ConnectionTracker tracker;
  label.AsyncRenderFinishedSignal().Connect(&tracker, [&](View view, float width, float height)
  {
    DALI_TEST_CHECK(view == label);
    ++completions;
    // Capture the published render's line count here, so an old completion or
    // an async size computation cannot satisfy the next text update's wait.
    completedLines = label.GetAsyncLineCount();
    renderedSize   = Vector2(width, height);
  });
  panel.Add(label);
  application.GetScene().Add(panel);

  constexpr int      ASYNC_TEXT_THREAD_TIMEOUT = 5;
  constexpr uint32_t MAX_TRIGGER_COUNT         = 4u;
  const auto         directions                = {RTL, LTR, RTL};
  const char*        texts[]                   = {"Hello Alex", "Hello Alex\nFirst update", "Hello Alex\nFirst update\nSecond update"};
  uint32_t           phase                     = 0u;
  for(auto direction : directions)
  {
    if(phase > 0u)
    {
      SetRootDirection(application, direction);
      // Root changes need not emit a direction signal for this LTR label.
      // Request fresh work through the public API after the prior render ends.
      label.SetText(texts[phase]);
    }
    const uint32_t previousCompletions = completions;
    const int      expectedLines       = static_cast<int>(phase + 1u);
    Relayout(application);
    // Nonempty, fixed-size content must wait for the worker completion pump.
    DALI_TEST_EQUALS(completions, previousCompletions, TEST_LOCATION);
    for(uint32_t trigger = 0u; trigger < MAX_TRIGGER_COUNT &&
                               (completions == previousCompletions || completedLines != expectedLines);
        ++trigger)
    {
      DALI_TEST_CHECK(Test::WaitForEventThreadTrigger(1, ASYNC_TEXT_THREAD_TIMEOUT));
      Relayout(application);
    }
    DALI_TEST_CHECK(completions > previousCompletions);
    DALI_TEST_EQUALS(completedLines, expectedLines, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetAsyncLineCount(), expectedLines, TEST_LOCATION);
    DALI_TEST_EQUALS(renderedSize, Vector2(BOX.width, BOX.height), TEST_LOCATION);
    DALI_TEST_EQUALS(application.GetScene().GetRootLayer().GetEffectiveLayoutDirection(), direction, TEST_LOCATION);
    DALI_TEST_EQUALS(label.GetEffectiveLayoutDirection(), LTR, TEST_LOCATION);
    DALI_TEST_CHECK(label.IsAsyncRendering());
    DALI_TEST_CHECK(label.IsResourceReady());
    DALI_TEST_CHECK(label.GetRendererCount() > 0u);
    TextureSet textures = label.GetRendererAt(0u).GetTextures();
    DALI_TEST_CHECK(textures && textures.GetTextureCount() > 0u);
    Texture texture = textures.GetTexture(0u);
    DALI_TEST_CHECK(texture && texture.GetWidth() > 0u && texture.GetHeight() > 0u);
    ++phase;
  }

  const uint32_t settledCompletions = completions;
  Relayout(application);
  DALI_TEST_EQUALS(completions, settledCompletions, TEST_LOCATION);
  DALI_TEST_EQUALS(label.GetAsyncLineCount(), 3, TEST_LOCATION);
  label.Unparent();
  Relayout(application);
  DALI_TEST_EQUALS(completions, settledCompletions, TEST_LOCATION);
  END_TEST;
}
