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

#include <dali-ui-foundation/integration-api/text/text-anchor-control-interface.h>
#include <dali-ui-foundation/internal/text/async-text/async-text-manager-impl.h>
#include <dali-ui-foundation/internal/text/async-text/text-loading-task.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl-model-updater.h>
#include <dali-ui-foundation/internal/text/controller/text-controller-impl.h>
#include <dali-ui-foundation/internal/text/controller/text-controller.h>
#include <dali-ui-foundation/internal/text/decorator/text-decorator.h>
#include <dali-ui-foundation/internal/text/hidden-text.h>
#include <dali-ui-foundation/internal/text/input-filter-processor.h>
#include <dali-ui-foundation/internal/text/rendering/text-typesetter.h>
#include <dali-ui-foundation/internal/text/rendering/view-model.h>
#include <dali-ui-foundation/internal/text/segmentation.h>
#include <dali-ui-test-suite-utils.h>
#include <dali-ui/ui-async-task-manager.h>
#include <dali/integration-api/adaptor-framework/input-method-context-integ.h>
#include <dali/integration-api/debug.h>
#include <dali.h>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;
namespace Dali::Integration::InputMethodContext
{
std::vector<PreeditStyle> gTestPreeditStyles;

void GetPreeditStyle(Dali::InputMethodContext, PreEditAttributeDataContainer& attrs)
{
  for(auto style : gTestPreeditStyles)
  {
    PreeditAttributeData attribute;
    attribute.preeditType = style;
    attribute.startIndex = 0u;
    attribute.endIndex = 1u;
    attrs.PushBack(attribute);
  }
}
} // namespace Dali::Integration::InputMethodContext

namespace
{
struct HiddenTextObserver : public Text::HiddenText::Observer
{
  void DisplayTimeExpired() override
  {
    ++expirationCount;
  }

  int expirationCount{0};
};

void CheckCharacters(const Vector<Text::Character>& actual, const char* expected)
{
  for(std::size_t index = 0u; expected[index] != '\0'; ++index)
  {
    DALI_TEST_EQUALS(actual[index], static_cast<Text::Character>(expected[index]), TEST_LOCATION);
  }
}
} // namespace

void utc_dali_text_input_internals_internal_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_text_input_internals_internal_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliHiddenTextModeAndRevealMatrixP(void)
{
  UiTestApplication application;
  HiddenTextObserver observer;
  Text::HiddenText hidden(&observer);
  Vector<Text::Character> source;
  source.PushBack('a');
  source.PushBack('b');
  source.PushBack('c');
  source.PushBack('d');
  Vector<Text::Character> output;

  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "abcd");
  DALI_TEST_EQUALS(hidden.GetPasswordMode(), Text::PasswordMode::NONE, TEST_LOCATION);

  hidden.HideAll();
  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "****");
  DALI_TEST_EQUALS(hidden.GetMode(), Text::HiddenText::Mode::HIDE_ALL, TEST_LOCATION);

  hidden.HideFirstCharacters(2u);
  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "**cd");
  DALI_TEST_EQUALS(hidden.GetSubstituteCount(), 2u, TEST_LOCATION);

  hidden.ShowFirstCharacters(2u);
  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "ab**");
  DALI_TEST_EQUALS(hidden.GetPasswordMode(), Text::PasswordMode::NONE, TEST_LOCATION);

  hidden.SetPasswordMaskCharacter('#');
  DALI_TEST_EQUALS(hidden.GetPasswordMaskCharacter(), static_cast<uint32_t>('#'), TEST_LOCATION);
  hidden.SetPasswordMode(Text::PasswordMode::HIDE_ALL);
  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "####");

  hidden.SetPasswordMode(Text::PasswordMode::REVEAL_LAST_CHARACTER);
  hidden.SetPasswordRevealDuration(1000);
  DALI_TEST_EQUALS(hidden.GetPasswordRevealDuration(), 1000, TEST_LOCATION);
  hidden.InitPreviousTextCount();
  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "###d");
  DALI_TEST_CHECK(!hidden.OnTick());
  DALI_TEST_EQUALS(observer.expirationCount, 1, TEST_LOCATION);
  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "####");

  hidden.ClearHiddenText();
  hidden.Substitute(source, output, 4u);
  CheckCharacters(output, "abcd");
  END_TEST;
}

int UtcDaliInputFilterProcessorAllowDenyMatrixP(void)
{
  Text::InputFilterProcessor filter;
  std::string text("a1b2");
  DALI_TEST_CHECK(filter.IsAllowed("a"));
  DALI_TEST_CHECK(!filter.IsDenied("a"));
  DALI_TEST_CHECK(!filter.ApplyAllowPattern(text));
  DALI_TEST_CHECK(!filter.ApplyDenyPattern(text));

  filter.SetAllowPattern("[0-9]");
  DALI_TEST_EQUALS(filter.GetAllowPattern(), std::string("[0-9]"), TEST_LOCATION);
  DALI_TEST_CHECK(filter.IsAllowed("7"));
  DALI_TEST_CHECK(!filter.IsAllowed("a"));
  DALI_TEST_CHECK(filter.ApplyAllowPattern(text));
  DALI_TEST_EQUALS(text, std::string("12"), TEST_LOCATION);
  DALI_TEST_CHECK(!filter.ApplyAllowPattern(text));

  filter.SetDenyPattern("[5-9]");
  DALI_TEST_EQUALS(filter.GetDenyPattern(), std::string("[5-9]"), TEST_LOCATION);
  DALI_TEST_CHECK(filter.IsDenied("7"));
  DALI_TEST_CHECK(!filter.IsDenied("2"));
  text = "12573";
  DALI_TEST_CHECK(filter.ApplyDenyPattern(text));
  DALI_TEST_EQUALS(text, std::string("123"), TEST_LOCATION);
  DALI_TEST_CHECK(!filter.ApplyDenyPattern(text));
  END_TEST;
}

int UtcDaliTextSegmentationFullAndPartialUpdatesP(void)
{
  UiTestApplication application;
  auto segmentation = TextAbstraction::Segmentation::New();
  Vector<Text::Character> text;
  Vector<Text::LineBreakInfo> lines;
  Vector<Text::WordBreakInfo> words;

  Text::SetLineBreakInfo(segmentation, text, 0u, 0u, lines);
  Text::SetWordBreakInfo(segmentation, text, 0u, 0u, words);
  DALI_TEST_EQUALS(lines.Count(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(words.Count(), 0u, TEST_LOCATION);

  text.PushBack('a');
  text.PushBack(' ');
  text.PushBack('b');
  text.PushBack('\n');
  text.PushBack('c');
  Text::SetLineBreakInfo(segmentation, text, 0u, 5u, lines);
  Text::SetWordBreakInfo(segmentation, text, 0u, 5u, words);
  DALI_TEST_EQUALS(lines.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(words.Count(), 5u, TEST_LOCATION);

  Text::SetLineBreakInfo(segmentation, text, 1u, 2u, lines);
  Text::SetWordBreakInfo(segmentation, text, 1u, 2u, words);
  DALI_TEST_EQUALS(lines.Count(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(words.Count(), 5u, TEST_LOCATION);
#if defined(DEBUG_ENABLED)
  Debug::Filter::SetGlobalLogLevel(Debug::Verbose);
  Text::SetLineBreakInfo(segmentation, text, 0u, 5u, lines);
  Text::SetWordBreakInfo(segmentation, text, 0u, 5u, words);
  Debug::Filter::SetGlobalLogLevel(Debug::NoLogging);
  DALI_TEST_EQUALS(words.Count(), 5u, TEST_LOCATION);
#endif
  END_TEST;
}

int UtcDaliTextControllerLayoutAndHiddenOptionsP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText("alpha beta\ngamma delta");
  controller->SetFontSizeScale(1.25f);
  DALI_TEST_EQUALS(controller->GetFontSizeScale(), 1.25f, 0.001f, TEST_LOCATION);

  DALI_TEST_CHECK(controller->SetDefaultLineSpacing(3.0f));
  DALI_TEST_CHECK(!controller->SetDefaultLineSpacing(3.0f));
  DALI_TEST_EQUALS(controller->GetDefaultLineSpacing(), 3.0f, 0.001f, TEST_LOCATION);
  controller->SetRemoveFrontInset(true);
  controller->SetRemoveBackInset(true);
  controller->SetCursorInsetEnabled(false);
  DALI_TEST_CHECK(controller->IsRemoveFrontInset());
  DALI_TEST_CHECK(controller->IsRemoveBackInset());
  DALI_TEST_CHECK(!controller->IsCursorInsetEnabled());

  controller->SetDisabledColorOpacity(0.4f);
  DALI_TEST_EQUALS(controller->GetDisabledColorOpacity(), 0.4f, 0.001f, TEST_LOCATION);
  controller->SetUserInteractionEnabled(false);
  DALI_TEST_CHECK(!controller->IsUserInteractionEnabled());
  controller->SetUserInteractionEnabled(true);
  DALI_TEST_CHECK(controller->IsUserInteractionEnabled());

  const Size layoutSize(240.0f, 100.0f);
  controller->Relayout(layoutSize);
  Vector2 targetSize;
  static_cast<Text::Decorator::ControllerInterface&>(*controller).GetTargetSize(targetSize);
  DALI_TEST_EQUALS(targetSize, Vector2(240.0f, 100.0f), TEST_LOCATION);
  DALI_TEST_CHECK(controller->GetLineBoundingRectangle(0u).height > 0.0f);
  DALI_TEST_EQUALS(controller->GetLineBoundingRectangle(99u).height, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(controller->GetDefaultFontLineHeight() > 0.0f);
  DALI_TEST_CHECK(controller->GetDefaultLineBoxHeight() > 0.0f);
  float scrollPosition = 0.0f;
  float controlHeight = 0.0f;
  float layoutHeight = 0.0f;
  DALI_TEST_CHECK(!controller->GetTextScrollInfo(scrollPosition, controlHeight, layoutHeight));
  DALI_TEST_EQUALS(controlHeight, 100.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_CHECK(layoutHeight > 0.0f);

  controller->HideFirstCharacters(2u);
  DALI_TEST_EQUALS(controller->GetHiddenTextMode(), Text::HiddenText::Mode::HIDE_COUNT, TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetHiddenTextSubstituteCount(), 2u, TEST_LOCATION);
  controller->ShowFirstCharacters(3u);
  DALI_TEST_EQUALS(controller->GetHiddenTextSubstituteCount(), 3u, TEST_LOCATION);
  controller->HideAllText();
  DALI_TEST_EQUALS(controller->GetHiddenTextMode(), Text::HiddenText::Mode::HIDE_ALL, TEST_LOCATION);
  controller->ClearHiddenText();
  DALI_TEST_EQUALS(controller->GetHiddenTextMode(), Text::HiddenText::Mode::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextViewModelDelegationP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText("Latin אבג second line");
  controller->Relayout(Size(260.0f, 100.0f));
  auto& impl = Text::Controller::Impl::GetImplementation(*controller.Get());
  Text::TypesetterPtr typesetter = Text::Typesetter::New(impl.mModel.Get());
  Text::ViewModel* view = typesetter->GetViewModel();
  DALI_TEST_CHECK(view != nullptr);
  DALI_TEST_EQUALS(view->GetControlSize(), impl.mModel->GetControlSize(), TEST_LOCATION);
  DALI_TEST_EQUALS(view->GetScrollPosition(), impl.mModel->GetScrollPosition(), TEST_LOCATION);
  DALI_TEST_EQUALS(view->GetNumberOfScripts(), impl.mModel->GetNumberOfScripts(), TEST_LOCATION);
  DALI_TEST_CHECK(view->GetScriptRuns() == impl.mModel->GetScriptRuns());
  DALI_TEST_EQUALS(view->GetFontRuns().Count(), impl.mModel->GetFontRuns().Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(view->GetFontDescriptionRuns().Count(), impl.mModel->GetFontDescriptionRuns().Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(view->GetNumberOfBoundedParagraphRuns(), impl.mModel->GetNumberOfBoundedParagraphRuns(), TEST_LOCATION);
  DALI_TEST_EQUALS(view->GetBoundedParagraphRuns().Count(), impl.mModel->GetBoundedParagraphRuns().Count(), TEST_LOCATION);
  DALI_TEST_EQUALS(view->GetNumberOfCharacterSpacingGlyphRuns(), impl.mModel->GetNumberOfCharacterSpacingGlyphRuns(), TEST_LOCATION);
  DALI_TEST_CHECK(!view->GetCharacterDirection(1000u));
  END_TEST;
}

int UtcDaliTextControllerSelectionHandleEventsP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetSelectionEnabled(true);
  controller->SetText("alpha beta gamma delta");
  controller->Relayout(Size(280.0f, 60.0f));
  controller->KeyboardFocusGainEvent();
  controller->Relayout(Size(280.0f, 60.0f));

  controller->SelectEvent(30.0f, 20.0f, Text::ALL);
  controller->Relayout(Size(280.0f, 60.0f));
  const auto selection = controller->GetTextSelectionRange();
  DALI_TEST_CHECK(selection.second > selection.first);

  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::LEFT_SELECTION_HANDLE, Text::HANDLE_PRESSED, 25.0f, 20.0f);
  controller->Relayout(Size(280.0f, 60.0f));
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::LEFT_SELECTION_HANDLE, Text::HANDLE_SCROLLING, 35.0f, 20.0f);
  controller->Relayout(Size(280.0f, 60.0f));
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::LEFT_SELECTION_HANDLE, Text::HANDLE_RELEASED, 35.0f, 20.0f);
  controller->Relayout(Size(280.0f, 60.0f));

  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_PRESSED, 140.0f, 20.0f);
  controller->Relayout(Size(280.0f, 60.0f));
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_SCROLLING, 125.0f, 20.0f);
  controller->Relayout(Size(280.0f, 60.0f));
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::RIGHT_SELECTION_HANDLE, Text::HANDLE_STOP_SCROLLING, 125.0f, 20.0f);
  controller->Relayout(Size(280.0f, 60.0f));
  DALI_TEST_CHECK(controller->GetTextSelectionRange().second <= 22u);
  END_TEST;
}

int UtcDaliTextControllerGrabHandleEventsP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetText("editable content");
  controller->Relayout(Size(240.0f, 60.0f));
  controller->KeyboardFocusGainEvent();
  controller->Relayout(Size(240.0f, 60.0f));

  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::GRAB_HANDLE, Text::HANDLE_PRESSED, 20.0f, 20.0f);
  controller->Relayout(Size(240.0f, 60.0f));
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::GRAB_HANDLE, Text::HANDLE_SCROLLING, 80.0f, 20.0f);
  controller->Relayout(Size(240.0f, 60.0f));
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(Text::GRAB_HANDLE, Text::HANDLE_RELEASED, 80.0f, 20.0f);
  controller->Relayout(Size(240.0f, 60.0f));
  DALI_TEST_CHECK(controller->GetCursorPosition() <= 16u);
  END_TEST;
}

int UtcDaliTextControllerInputPropertiesWithAndWithoutEditorP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetInputLineSpacing(4.0f);
  controller->SetInputModePassword(true);
  controller->SetInputShadowProperties("shadow");
  controller->SetInputUnderlineProperties("underline");
  controller->SetInputEmbossProperties("emboss");
  controller->SetInputOutlineProperties("outline");
  DALI_TEST_EQUALS(controller->GetInputLineSpacing(), 0.0f, TEST_LOCATION);
  DALI_TEST_CHECK(!controller->IsInputModePassword());
  DALI_TEST_EQUALS(controller->GetInputShadowProperties(), std::string(), TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputUnderlineProperties(), std::string(), TEST_LOCATION);

  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetInputLineSpacing(4.0f);
  controller->SetInputModePassword(true);
  controller->SetInputShadowProperties("shadow");
  controller->SetInputUnderlineProperties("underline");
  controller->SetInputEmbossProperties("emboss");
  controller->SetInputOutlineProperties("outline");
  DALI_TEST_EQUALS(controller->GetInputLineSpacing(), 4.0f, TEST_LOCATION);
  DALI_TEST_CHECK(controller->IsInputModePassword());
  DALI_TEST_EQUALS(controller->GetInputShadowProperties(), std::string("shadow"), TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputUnderlineProperties(), std::string("underline"), TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputEmbossProperties(), std::string("emboss"), TEST_LOCATION);
  DALI_TEST_EQUALS(controller->GetInputOutlineProperties(), std::string("outline"), TEST_LOCATION);
  controller->SetInputModePassword(false);
  DALI_TEST_CHECK(!controller->IsInputModePassword());
  END_TEST;
}

int UtcDaliTextControllerPreeditStyleModelUpdateP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetText("preedit");
  controller->Relayout(Size(240.0f, 60.0f));

  auto& impl = Text::Controller::Impl::GetImplementation(*controller);
  DALI_TEST_CHECK(impl.mEventData);
  controller->SetText("preedit!");
  impl.mEventData->mPreEditFlag = true;
  impl.mEventData->mPreEditLength = 8u;
  impl.mEventData->mPrimaryCursorPosition = 8u;
  Dali::Integration::InputMethodContext::gTestPreeditStyles = {
    Dali::Integration::InputMethodContext::PreeditStyle::UNDERLINE,
    Dali::Integration::InputMethodContext::PreeditStyle::REVERSE,
    Dali::Integration::InputMethodContext::PreeditStyle::HIGHLIGHT,
    Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_1,
    Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_2,
    Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_3,
    Dali::Integration::InputMethodContext::PreeditStyle::CUSTOM_PLATFORM_STYLE_4
  };
  controller->Relayout(Size(240.0f, 60.0f));
  Dali::Integration::InputMethodContext::gTestPreeditStyles.clear();
  DALI_TEST_CHECK(impl.mModel->mVisualModel->mUnderlineRuns.Count() >= 1u);
  DALI_TEST_CHECK(impl.mModel->mLogicalModel->mBackgroundColorRuns.Count() >= 1u);
  END_TEST;
}

int UtcDaliTextControllerInputColorAcrossSelectionP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetText("color");
  controller->Relayout(Size(240.0f, 60.0f));

  auto& impl = Text::Controller::Impl::GetImplementation(*controller);
  impl.mEventData->mState = Text::EventData::SELECTING;
  impl.mEventData->mLeftSelectionPosition = 4u;
  impl.mEventData->mRightSelectionPosition = 1u;
  const Vector4 firstColor(1.0f, 0.0f, 0.0f, 1.0f);
  controller->SetInputColor(firstColor);
  DALI_TEST_EQUALS(controller->GetInputColor(), firstColor, TEST_LOCATION);
  auto& runs = impl.mModel->mLogicalModel->mColorRuns;
  DALI_TEST_CHECK(!runs.Empty());
  DALI_TEST_EQUALS(runs.Back().characterRun.characterIndex, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(runs.Back().characterRun.numberOfCharacters, 3u, TEST_LOCATION);

  impl.mEventData->mLeftSelectionPosition = 1u;
  impl.mEventData->mRightSelectionPosition = 4u;
  const Vector4 secondColor(0.0f, 0.0f, 1.0f, 1.0f);
  controller->SetInputColor(secondColor);
  DALI_TEST_EQUALS(controller->GetInputColor(), secondColor, TEST_LOCATION);
  DALI_TEST_EQUALS(runs.Back().color, secondColor, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextControllerRejectsStaleIncrementalRangeP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetText("short");
  controller->Relayout(Size(240.0f, 60.0f));

  auto& impl = Text::Controller::Impl::GetImplementation(*controller);
  impl.mTextUpdateInfo.mFullRelayoutNeeded = false;
  impl.mTextUpdateInfo.mCharacterIndex = 5u;
  impl.mTextUpdateInfo.mNumberOfCharactersToAdd = 100u;
  impl.mTextUpdateInfo.mNumberOfCharactersToRemove = 0u;
  impl.mOperationsPending = Text::Controller::GET_LINE_BREAKS;
  DALI_TEST_CHECK(!Text::ControllerImplModelUpdater::Update(impl, Text::Controller::GET_LINE_BREAKS));
  END_TEST;
}

int UtcDaliAsyncTextManagerDestroyedWaitingObserverP(void)
{
  struct Observer : Dali::Ui::TextLoadObserver
  {
    void LoadComplete(bool, const TextInformation&) override
    {
      ++completionCount;
    }
    uint32_t completionCount{0u};
  };

  UiTestApplication application;
  Text::AsyncTextManager handle = Text::AsyncTextManager::Get();
  auto& manager = Text::GetImplementation(handle);
  Test::AsyncTaskManager::GrabNotifyToReady();
  std::vector<uint32_t> runningTasks;
  auto makeParameters = []()
  {
    Text::AsyncTextParameters parameters;
    parameters.text = std::string(30000u, 'A');
    parameters.fontSize = 16.0f;
    parameters.textWidth = 200.0f;
    parameters.textHeight = 40.0f;
    parameters.originWidth = 200.0f;
    parameters.originHeight = 40.0f;
    parameters.maxTextureSize = 4096u;
    parameters.requestType = Dali::Ui::Integration::Text::Async::RENDER_FIXED_SIZE;
    return parameters;
  };
  for(uint32_t index = 0u; index < 4u; ++index)
  {
    runningTasks.push_back(manager.RequestLoad(makeParameters(), nullptr));
  }
  auto* observer = new Observer;
  const uint32_t waitingTask = manager.RequestLoad(makeParameters(), observer);
  delete observer;
  manager.RequestCancel(waitingTask);
  for(uint32_t taskId : runningTasks)
  {
    manager.RequestCancel(taskId);
  }
  Test::AsyncTaskManager::UngrabNotifyToReady();
  DALI_TEST_EQUALS(runningTasks.size(), 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliAsyncTextManagerWithoutAdaptorQueuesP(void)
{
  struct Observer : Dali::Ui::TextLoadObserver
  {
    void LoadComplete(bool, const TextInformation&) override
    {
      ++completionCount;
    }
    uint32_t completionCount{0u};
  };

  Text::AsyncTextManager handle(new Text::Internal::AsyncTextManager);
  UiTestApplication application;
  auto& manager = Text::GetImplementation(handle);
  Text::AsyncTextParameters parameters;
  parameters.text = "No loader is available";
  auto* observer = new Observer;
  const uint32_t cancelledTask = manager.RequestLoad(Text::AsyncTextParameters(parameters), observer);
  const uint32_t destroyedTask = manager.RequestLoad(std::move(parameters), observer);
  DALI_TEST_CHECK(cancelledTask != destroyedTask);
  manager.RequestCancel(cancelledTask);
  delete observer;
  manager.RequestCancel(destroyedTask);
  Dali::Ui::Internal::TextLoadingTaskPtr emptyTask(
    new Dali::Ui::Internal::TextLoadingTask(0u, Text::AsyncTextParameters{}, nullptr));
  manager.LoadComplete(emptyTask);
  END_TEST;
}

int UtcDaliTextControllerKeyEventMatrixP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetMultiLineEnabled(true);
  controller->SetSelectionEnabled(true);
  controller->SetText("alpha beta gamma delta epsilon");
  controller->Relayout(Size(100.0f, 180.0f));
  controller->KeyboardFocusGainEvent();
  controller->Relayout(Size(100.0f, 180.0f));
  Text::Controller::Impl& impl = Text::Controller::Impl::GetImplementation(*controller.Get());

  auto sendKey = [&](const char* name, int32_t code, int32_t modifiers,
                     const char* text, Dali::KeyEvent::State state = Dali::KeyEvent::DOWN)
  {
    Dali::KeyEvent event = Dali::KeyEvent::New();
    event.SetKeyName(name);
    event.SetLogicalKey(name);
    event.SetKeyString(text);
    event.SetKeyCode(code);
    event.SetKeyModifier(modifiers);
    event.SetState(state);
    const bool handled = controller->KeyEvent(event);
    controller->Relayout(Size(100.0f, 180.0f));
    return handled;
  };

  DALI_TEST_CHECK(!sendKey("", 0, 0, ""));
  DALI_TEST_CHECK(!sendKey("Escape", Dali::DALI_KEY_ESCAPE, 0, ""));
  DALI_TEST_CHECK(!sendKey("Back", Dali::DALI_KEY_BACK, 0, ""));
  DALI_TEST_CHECK(!sendKey("Control_L", Dali::DALI_KEY_CONTROL_LEFT, 0, ""));
  DALI_TEST_CHECK(!sendKey("Shift_L", Dali::DALI_KEY_SHIFT_LEFT, 0, ""));
  DALI_TEST_CHECK(!sendKey("Volume_Up", Dali::DALI_KEY_VOLUME_UP, 0, ""));

  impl.mEventData->mPrimaryCursorPosition = 0u;
  DALI_TEST_CHECK(!sendKey("Left", Dali::DALI_KEY_CURSOR_LEFT, 0, ""));
  impl.mEventData->mPrimaryCursorPosition = 5u;
  DALI_TEST_CHECK(sendKey("Right", Dali::DALI_KEY_CURSOR_RIGHT, 0, ""));
  DALI_TEST_CHECK(sendKey("Left", Dali::DALI_KEY_CURSOR_LEFT, Dali::KeyEvent::SHIFT, ""));
  impl.mEventData->mPrimaryCursorPosition = impl.mTextUpdateInfo.mPreviousNumberOfCharacters;
  DALI_TEST_CHECK(!sendKey("Right", Dali::DALI_KEY_CURSOR_RIGHT, 0, ""));
  impl.mEventData->mPrimaryCursorPosition = 4u;
  sendKey("Down", Dali::DALI_KEY_CURSOR_DOWN, 0, "");
  sendKey("Up", Dali::DALI_KEY_CURSOR_UP, 0, "");

  controller->SelectEvent(0.0f, 0.0f, Text::ALL);
  controller->Relayout(Size(100.0f, 180.0f));
  DALI_TEST_CHECK(sendKey("a", 65, Dali::KeyEvent::CTRL, ""));
  DALI_TEST_CHECK(!sendKey("q", 81, Dali::KeyEvent::CTRL, ""));

  controller->SetText("alpha beta");
  controller->Relayout(Size(200.0f, 60.0f));
  impl.mEventData->mPrimaryCursorPosition = 5u;
  sendKey("BackSpace", Dali::DALI_KEY_BACKSPACE, 0, "");
  sendKey("Delete", Dali::DALI_KEY_DELETE, 0, "");
  DALI_TEST_CHECK(sendKey("z", 122, 0, "z"));
  sendKey("Home", Dali::DALI_KEY_HOME, 0, "");
  controller->KeyboardFocusGainEvent();
  sendKey("Back", Dali::DALI_KEY_BACK, 0, "", Dali::KeyEvent::UP);
  END_TEST;
}

int UtcDaliTextControllerFocusFilterAndImeMatrixP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetSelectionEnabled(true);
  controller->SetMultiLineEnabled(true);
  controller->SetText("first second third");
  controller->Relayout(Size(220.0f, 100.0f));
  auto& impl = Text::Controller::Impl::GetImplementation(*controller);

  impl.mEventData->mState = Text::EventData::TEXT_PANNING;
  impl.mEventData->mPreviousState = Text::EventData::INTERRUPTED;
  controller->KeyboardFocusGainEvent();
  DALI_TEST_EQUALS(impl.mEventData->mPreviousState, Text::EventData::EDITING, TEST_LOCATION);
  impl.mEventData->mState = Text::EventData::SELECTING;
  impl.mEventData->mLeftSelectionPosition = 1u;
  impl.mEventData->mRightSelectionPosition = 5u;
  impl.mEventData->mPrimaryCursorPosition = 3u;
  controller->KeyboardFocusLostEvent();
  DALI_TEST_EQUALS(impl.mEventData->mState, Text::EventData::INACTIVE, TEST_LOCATION);
  DALI_TEST_EQUALS(impl.mEventData->mLeftSelectionPosition, 3u, TEST_LOCATION);
  controller->KeyboardFocusGainEvent();
  controller->Relayout(Size(220.0f, 100.0f));

  controller->SelectEvent(0.0f, 0.0f, Text::NONE);
  controller->Relayout(Size(220.0f, 100.0f));
  controller->SelectEvent(1u, 5u, Text::RANGE);
  controller->Relayout(Size(220.0f, 100.0f));

  Text::InputFilter filter;
  filter.SetAllowPattern("[0-9]");
  filter.SetDenyPattern("[7]");
  controller->SetInputFilter(filter);
  auto sendKey = [&](const char* keyName, int code, const char* keyString)
  {
    Dali::KeyEvent event = Dali::KeyEvent::New();
    event.SetKeyName(keyName);
    event.SetLogicalKey(keyName);
    event.SetKeyString(keyString);
    event.SetKeyCode(code);
    event.SetState(Dali::KeyEvent::DOWN);
    return controller->KeyEvent(event);
  };
  DALI_TEST_CHECK(sendKey("x", 120, "x"));
  DALI_TEST_CHECK(sendKey("7", 55, "7"));
  DALI_TEST_CHECK(sendKey("4", 52, "4"));
  controller->Relayout(Size(220.0f, 100.0f));
  controller->SetInputFilter(Text::InputFilter::None());

  namespace Im = Dali::Integration::InputMethodContext;
  auto getResult = controller->OnInputMethodContextEvent(
    inputMethodContext, Im::EventData(Im::GET_SURROUNDING, Dali::String(), 0, 0));
  DALI_TEST_CHECK(getResult.update);
  auto privateResult = controller->OnInputMethodContextEvent(
    inputMethodContext, Im::EventData(Im::PRIVATE_COMMAND, Dali::String(), 0, 0));
  DALI_TEST_CHECK(privateResult.update);
  controller->OnInputMethodContextEvent(inputMethodContext, Im::EventData(Im::SELECTION_SET, 2, 2));
  controller->OnInputMethodContextEvent(inputMethodContext, Im::EventData(Im::SELECTION_SET, 1, 4));
  controller->OnInputMethodContextEvent(
    inputMethodContext, Im::EventData(Im::COMMIT, Dali::String("Q"), 0, 0));
  controller->Relayout(Size(220.0f, 100.0f));
  controller->OnInputMethodContextEvent(
    inputMethodContext, Im::EventData(Im::DELETE_SURROUNDING, Dali::String(), -1, 1));
  controller->Relayout(Size(220.0f, 100.0f));
  controller->OnInputMethodContextEvent(
    inputMethodContext, Im::EventData(Im::VOID, Dali::String(), 0, 0));
  END_TEST;
}

int UtcDaliTextControllerAnchorClickAndHitTestP(void)
{
  struct AnchorObserver : Dali::Ui::Integration::Text::AnchorControlInterface
  {
    bool AnchorClicked(uint32_t, std::string&) override
    {
      return false;
    }

    void EmitAnchorClicked(const std::string& clickedHref) override
    {
      ++clickedCount;
      lastHref = clickedHref;
    }

    uint32_t clickedCount{0u};
    std::string lastHref;
  };

  UiTestApplication application;
  AnchorObserver observer;
  Text::ControllerPtr controller = Text::Controller::New();
  controller->SetAnchorControlInterface(&observer);
  controller->SetText("anchored text");
  controller->Relayout(Size(240.0f, 60.0f));

  auto& impl = Text::Controller::Impl::GetImplementation(*controller);
  auto& logicalModel = *impl.mModel->mLogicalModel;
  const Text::Length textLength = static_cast<Text::Length>(logicalModel.mText.Count());
  Text::ColorRun colorRun;
  colorRun.characterRun.characterIndex = 0u;
  colorRun.characterRun.numberOfCharacters = textLength;
  colorRun.color = Color::BLUE;
  const uint32_t colorRunIndex = static_cast<uint32_t>(logicalModel.mColorRuns.Count());
  logicalModel.mColorRuns.PushBack(colorRun);
  Text::UnderlinedCharacterRun underlineRun;
  underlineRun.characterRun.characterIndex = 0u;
  underlineRun.characterRun.numberOfCharacters = textLength;
  const uint32_t underlineRunIndex = static_cast<uint32_t>(logicalModel.mUnderlinedCharacterRuns.Count());
  logicalModel.mUnderlinedCharacterRuns.PushBack(underlineRun);

  Text::Anchor anchor{};
  anchor.startIndex = 0u;
  anchor.endIndex = textLength;
  anchor.href = new char[8]{'t', 'e', 's', 't', ':', '/', '/', '\0'};
  anchor.colorRunIndex = colorRunIndex;
  anchor.underlinedCharacterRunIndex = underlineRunIndex;
  anchor.isMarkupClickedColorSet = true;
  anchor.markupClickedColor = Color::RED;
  logicalModel.mAnchors.PushBack(anchor);

  std::string href;
  DALI_TEST_CHECK(!controller->AnchorClickEvent(textLength, href));
  DALI_TEST_CHECK(controller->AnchorClickEvent(1u, href));
  DALI_TEST_EQUALS(href, std::string("test://"), TEST_LOCATION);
  DALI_TEST_CHECK(logicalModel.mAnchors[0u].isClicked);
  DALI_TEST_EQUALS(logicalModel.mColorRuns[colorRunIndex].color, Color::RED, TEST_LOCATION);
  DALI_TEST_EQUALS(logicalModel.mUnderlinedCharacterRuns[underlineRunIndex].properties.color,
                   Color::RED, TEST_LOCATION);
  DALI_TEST_CHECK(controller->AnchorClickEvent(1u, href));
  controller->AnchorEvent(5.0f, 10.0f);
  DALI_TEST_CHECK(observer.clickedCount > 0u);
  DALI_TEST_EQUALS(observer.lastHref, std::string("test://"), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTextControllerTapAndPopupSelectionP(void)
{
  UiTestApplication application;
  Text::ControllerPtr controller = Text::Controller::New();
  Text::DecoratorPtr decorator = Text::Decorator::New(*controller, *controller);
  InputMethodContext inputMethodContext;
  controller->EnableTextInput(decorator, inputMethodContext);
  controller->SetSelectionEnabled(true);
  controller->SetText("tap popup selection");
  controller->Relayout(Size(240.0f, 60.0f));
  controller->KeyboardFocusGainEvent();
  controller->Relayout(Size(240.0f, 60.0f));

  auto& impl = Text::Controller::Impl::GetImplementation(*controller);
  auto& popupCallback = static_cast<TextSelectionPopupCallbackInterface&>(*controller);
  popupCallback.TextPopupButtonTouched(Text::InputCommandType::NONE);
  popupCallback.TextPopupButtonTouched(Text::InputCommandType::SELECT);
  controller->Relayout(Size(240.0f, 60.0f));
  controller->SetSelectionEnabled(false);
  popupCallback.TextPopupButtonTouched(Text::InputCommandType::SELECT);
  controller->SetSelectionEnabled(true);

  impl.mEventData->mState = Text::EventData::EDITING_WITH_POPUP;
  controller->TapEvent(1u, 10.0f, 10.0f);
  controller->Relayout(Size(240.0f, 60.0f));
  impl.mEventData->mState = Text::EventData::EDITING_WITH_PASTE_POPUP;
  controller->TapEvent(1u, 20.0f, 10.0f);
  controller->Relayout(Size(240.0f, 60.0f));
  controller->TapEvent(2u, 20.0f, 10.0f);
  controller->Relayout(Size(240.0f, 60.0f));
  std::string text;
  controller->GetText(text);
  DALI_TEST_EQUALS(text, std::string("tap popup selection"), TEST_LOCATION);
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(
    Text::LEFT_SELECTION_HANDLE_MARKER, Text::HANDLE_PRESSED, 10.0f, 10.0f);
  static_cast<Text::Decorator::ControllerInterface&>(*controller).DecorationEvent(
    Text::RIGHT_SELECTION_HANDLE_MARKER, Text::HANDLE_PRESSED, 10.0f, 10.0f);
  Text::ControllerPtr withoutInput = Text::Controller::New();
  static_cast<TextSelectionPopupCallbackInterface&>(*withoutInput).TextPopupButtonTouched(
    Text::InputCommandType::NONE);
  END_TEST;
}
