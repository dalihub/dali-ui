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
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/object/type-registry-helper.h>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
using Dali::Ui::Label;
using Dali::Ui::View;
namespace Accessibility = Dali::Ui::Accessibility;

bool Equals(Dali::StringView value, const char* text)
{
  const std::string expected(text);
  return value.Size() == expected.size() &&
         std::string(value.Data(), value.Size()) == expected;
}

bool Localize(Dali::StringView resourceId, Dali::StringView domain, Dali::String& result)
{
  if(Equals(resourceId, "IDS_A11Y_NAME") && Equals(domain, "sample"))
  {
    result = "Localized account name";
    return true;
  }
  if(Equals(resourceId, "IDS_A11Y_DESCRIPTION"))
  {
    result = "Localized account description";
    return true;
  }
  return false;
}

void AddResult(std::ostringstream& report, bool& passed, const char* label, bool result)
{
  report << (result ? "PASS " : "FAIL ") << label << '\n';
  passed = passed && result;
}

class AccessibilityDemoViewImpl : public Dali::Ui::ViewImpl
{
public:
  using Ptr = Dali::IntrusivePtr<AccessibilityDemoViewImpl>;

  enum class RequestedMode
  {
    DYNAMIC,
    EMPTY,
    FALLBACK
  };

  enum class DefaultNameMode
  {
    VALUE,
    EMPTY,
    FALLBACK
  };

  enum class DefaultDescriptionMode
  {
    VALUE,
    EMPTY,
    FALLBACK
  };

  static Ptr New()
  {
    return Ptr(new AccessibilityDemoViewImpl());
  }

  void SetRequestedMode(RequestedMode mode)
  {
    mRequestedMode = mode;
  }

  void SetDefaultNameMode(DefaultNameMode mode)
  {
    mDefaultNameMode = mode;
  }

  void SetDefaultDescriptionMode(DefaultDescriptionMode mode)
  {
    mDefaultDescriptionMode = mode;
  }

  bool OnAccessibilityActivate() override
  {
    ++mActivateCount;
    std::cout << "virtual activate\n";
    return true;
  }

  bool OnAccessibilityEscape() override
  {
    ++mEscapeCount;
    std::cout << "virtual escape\n";
    return true;
  }

  bool OnAccessibilityValueChange(bool isIncreased) override
  {
    mValueChangeBalance += isIncreased ? 1 : -1;
    ++mValueChangeCount;
    std::cout << "virtual value change: " << (isIncreased ? "increment" : "decrement") << '\n';
    return true;
  }

  bool OnAccessibilityScrollToChild(View child) override
  {
    ++mScrollToChildCount;
    mLastScrolledChild = child;
    std::cout << "virtual scroll to child\n";
    return static_cast<bool>(child);
  }

  bool OnAccessibilityPan(Dali::PanGesture gesture) override
  {
    ++mPanCount;
    return true;
  }

  bool OnAccessibilityZoom() override
  {
    ++mZoomCount;
    return true;
  }

  bool OnAccessibilityRequestName(Dali::String& value) override
  {
    return ResolveRequestedValue(value, "Requested account name");
  }

  bool OnAccessibilityRequestDefaultName(Dali::String& value) override
  {
    if(mDefaultNameMode == DefaultNameMode::FALLBACK)
    {
      return false;
    }
    value = mDefaultNameMode == DefaultNameMode::VALUE ? "View default account name" : "";
    return true;
  }

  bool OnAccessibilityRequestDescription(Dali::String& value) override
  {
    return ResolveRequestedValue(value, "Requested account description");
  }

  bool OnAccessibilityRequestDefaultDescription(Dali::String& value) override
  {
    if(mDefaultDescriptionMode == DefaultDescriptionMode::FALLBACK)
    {
      return false;
    }
    value = mDefaultDescriptionMode == DefaultDescriptionMode::VALUE ? "View default account description" : "";
    return true;
  }

  bool OnAccessibilityRequestValue(Dali::String& value) override
  {
    return ResolveRequestedValue(value, "Requested account value");
  }

  int GetActivateCount() const
  {
    return mActivateCount;
  }
  int GetEscapeCount() const
  {
    return mEscapeCount;
  }
  int GetValueChangeCount() const
  {
    return mValueChangeCount;
  }
  int GetValueChangeBalance() const
  {
    return mValueChangeBalance;
  }
  int GetScrollToChildCount() const
  {
    return mScrollToChildCount;
  }
  int GetPanCount() const
  {
    return mPanCount;
  }
  int GetZoomCount() const
  {
    return mZoomCount;
  }
  View GetLastScrolledChild() const
  {
    return mLastScrolledChild;
  }

protected:
  ~AccessibilityDemoViewImpl() override = default;

private:
  AccessibilityDemoViewImpl() = default;

  bool ResolveRequestedValue(Dali::String& value, const char* dynamicValue)
  {
    if(mRequestedMode == RequestedMode::FALLBACK)
    {
      return false;
    }
    value = mRequestedMode == RequestedMode::DYNAMIC ? dynamicValue : "";
    return true;
  }

  RequestedMode          mRequestedMode{RequestedMode::DYNAMIC};
  DefaultNameMode        mDefaultNameMode{DefaultNameMode::FALLBACK};
  DefaultDescriptionMode mDefaultDescriptionMode{DefaultDescriptionMode::FALLBACK};
  View                   mLastScrolledChild;
  int                    mActivateCount{0};
  int                    mEscapeCount{0};
  int                    mValueChangeCount{0};
  int                    mValueChangeBalance{0};
  int                    mScrollToChildCount{0};
  int                    mPanCount{0};
  int                    mZoomCount{0};
};

View CreateAccessibilityDemoView(AccessibilityDemoViewImpl*& implementation)
{
  auto impl      = AccessibilityDemoViewImpl::New();
  implementation = impl.Get();
  View view(*impl);
  impl->Initialize();
  return view;
}

Dali::BaseHandle CreateRegisteredAccessibilityDemoView()
{
  AccessibilityDemoViewImpl* implementation = nullptr;
  return CreateAccessibilityDemoView(implementation);
}

DALI_TYPE_REGISTRATION_BEGIN(AccessibilityDemoViewImpl, Dali::Ui::View, CreateRegisteredAccessibilityDemoView)
DALI_TYPE_REGISTRATION_END()

class PerViewAccessibilityCallbackHandler
{
public:
  bool OnActivate(View view)
  {
    RecordView(view);
    ++mActivateCount;
    std::cout << "callback activate\n";
    return mActionResult;
  }

  bool OnEscape(View view)
  {
    RecordView(view);
    ++mEscapeCount;
    std::cout << "callback escape\n";
    return mActionResult;
  }

  bool OnValueChange(View view, bool isIncreased)
  {
    RecordView(view);
    ++mValueChangeCount;
    mValueChangeBalance += isIncreased ? 1 : -1;
    std::cout << "callback value change: " << (isIncreased ? "increment" : "decrement") << '\n';
    return mActionResult;
  }

  bool OnScrollToChild(View view, View child)
  {
    RecordView(view);
    ++mScrollToChildCount;
    mLastScrolledChild = child;
    std::cout << "callback scroll to child\n";
    return mActionResult;
  }

  bool OnRequestName(View view, Dali::String& value)
  {
    RecordView(view);
    ++mRequestNameCount;
    value = "Callback name";
    return mHandleDynamicRequests;
  }

  bool OnRequestDefaultName(View view, Dali::String& value)
  {
    RecordView(view);
    ++mRequestDefaultNameCount;
    value = "Callback default name";
    return true;
  }

  bool OnRequestDescription(View view, Dali::String& value)
  {
    RecordView(view);
    ++mRequestDescriptionCount;
    value = "Callback description";
    return mHandleDynamicRequests;
  }

  bool OnRequestDefaultDescription(View view, Dali::String& value)
  {
    RecordView(view);
    ++mRequestDefaultDescriptionCount;
    value = "Callback default description";
    return true;
  }

  bool OnRequestValue(View view, Dali::String& value)
  {
    RecordView(view);
    ++mRequestValueCount;
    value = "Callback value";
    return mHandleDynamicRequests;
  }

  void SetExpectedView(View view)
  {
    mExpectedView = view;
  }

  void SetHandleDynamicRequests(bool handled)
  {
    mHandleDynamicRequests = handled;
  }

  bool ReceivedExpectedView() const
  {
    return mReceivedExpectedView;
  }

  int GetActivateCount() const
  {
    return mActivateCount;
  }

  int GetEscapeCount() const
  {
    return mEscapeCount;
  }

  int GetValueChangeCount() const
  {
    return mValueChangeCount;
  }

  int GetValueChangeBalance() const
  {
    return mValueChangeBalance;
  }

  int GetScrollToChildCount() const
  {
    return mScrollToChildCount;
  }

  int GetRequestNameCount() const
  {
    return mRequestNameCount;
  }

  int GetRequestDefaultNameCount() const
  {
    return mRequestDefaultNameCount;
  }

  int GetRequestDescriptionCount() const
  {
    return mRequestDescriptionCount;
  }

  int GetRequestDefaultDescriptionCount() const
  {
    return mRequestDefaultDescriptionCount;
  }

  int GetRequestValueCount() const
  {
    return mRequestValueCount;
  }

  View GetLastScrolledChild() const
  {
    return mLastScrolledChild;
  }

private:
  void RecordView(View view)
  {
    mReceivedExpectedView = mReceivedExpectedView && view == mExpectedView;
  }

  View mExpectedView;
  View mLastScrolledChild;
  int  mActivateCount{0};
  int  mEscapeCount{0};
  int  mValueChangeCount{0};
  int  mValueChangeBalance{0};
  int  mScrollToChildCount{0};
  int  mRequestNameCount{0};
  int  mRequestDefaultNameCount{0};
  int  mRequestDescriptionCount{0};
  int  mRequestDefaultDescriptionCount{0};
  int  mRequestValueCount{0};
  bool mReceivedExpectedView{true};
  bool mActionResult{true};
  bool mHandleDynamicRequests{true};
};

class AccessibilityViewApiController : public Dali::ConnectionTracker
{
public:
  explicit AccessibilityViewApiController(Dali::Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AccessibilityViewApiController::OnInit);
  }

private:
  void OnInit(Dali::Application application)
  {
    auto window = application.GetWindow();
    window.SetBackgroundColor(Dali::Color::WHITE);

    mSubject = CreateAccessibilityDemoView(mSubjectImpl);
    mSubject.SetRequestedX(20.0f);
    mSubject.SetRequestedY(20.0f);
    mSubject.SetRequestedWidth(440.0f);
    mSubject.SetRequestedHeight(104.0f);
    mSubject.SetBackgroundColor(Dali::Ui::UiColor(0xD6EAF8));
    mSubject.SetFocusable(true);
    window.Add(mSubject);

    auto subjectLabel = Label::New("Accessibility View API subject");
    subjectLabel.SetRequestedX(16.0f);
    subjectLabel.SetRequestedY(28.0f);
    subjectLabel.SetRequestedWidth(408.0f);
    subjectLabel.SetRequestedHeight(48.0f);
    mSubject.Add(subjectLabel);

    mRelated = View::New();
    mRelated.SetAccessibilityRole(Accessibility::Role::TEXT);
    mRelated.SetAccessibilityName("Related help text");
    window.Add(mRelated);

    mResult = Label::New();
    mResult.SetRequestedX(20.0f);
    mResult.SetRequestedY(140.0f);
    mResult.SetRequestedWidth(440.0f);
    mResult.SetRequestedHeight(640.0f);
    mResult.SetTextColor(Dali::Ui::UiColor(0x111111));
    mResult.SetMultiLine(true);
    mResult.SetProperty(Label::Property::FONT_SIZE, 11.0f);
    window.Add(mResult);

    ConfigureSignals();
    RunChecks();
    Dali::Ui::FocusManager::Get().SetCurrentFocusView(mSubject);
  }

  void ConfigureSignals()
  {
    mSubject.AccessibilityReadingStatusChangedSignal().Connect(this, [this](View view, Accessibility::ReadingStatus status)
    {
      mReadingSourceMatches = mReadingSourceMatches && view == mSubject;
      mReadingStatuses.push_back(status);
      std::cout << "reading status: " << static_cast<uint32_t>(status) << '\n';
    });
    mSubject.AccessibilityHighlightedSignal().Connect(this, [this](View view, bool highlighted)
    {
      mHighlightSourceMatches = mHighlightSourceMatches && view == mSubject;
      ++mHighlightCount;
      mLastHighlightState = highlighted;
      std::cout << "highlight changed: " << (highlighted ? "true" : "false") << '\n';
    });
    mSubject.FocusChangedSignal().Connect(this, [this](View, bool focused)
    {
      if(focused)
      {
        const bool grabbed = Dali::Ui::Extension::View::GrabAccessibilityHighlight(mSubject);
        std::cout << "focus-triggered accessibility highlight: "
                  << (grabbed ? "grabbed" : "accessibility bridge inactive") << '\n';
      }
    });
  }

  void RunPerViewCallbackChecks(std::ostringstream& report, bool& passed)
  {
    View callbackView = View::New();
    callbackView.SetAccessibilityRole(Accessibility::Role::BUTTON);
    callbackView.SetAccessibilityName("Stored name");
    callbackView.SetAccessibilityDescription("Stored description");
    callbackView.SetAccessibilityValue("Stored value");

    auto* accessible     = Dali::Accessibility::Accessible::Get(callbackView);
    auto* viewAccessible = dynamic_cast<Dali::Ui::ViewAccessible*>(accessible);
    if(!viewAccessible)
    {
      AddResult(report, passed, "per-View callback accessible object", false);
      return;
    }

    PerViewAccessibilityCallbackHandler handler;
    handler.SetExpectedView(callbackView);

    Dali::Ui::Extension::View::SetAccessibilityActivateCallback(
      callbackView,
      Dali::Ui::Callback<bool(View)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnActivate));
    Dali::Ui::Extension::View::SetAccessibilityEscapeCallback(
      callbackView,
      Dali::Ui::Callback<bool(View)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnEscape));
    Dali::Ui::Extension::View::SetAccessibilityValueChangeCallback(
      callbackView,
      Dali::Ui::Callback<bool(View, bool)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnValueChange));
    Dali::Ui::Extension::View::SetAccessibilityScrollToChildCallback(
      callbackView,
      Dali::Ui::Callback<bool(View, View)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnScrollToChild));
    Dali::Ui::Extension::View::SetAccessibilityRequestNameCallback(
      callbackView,
      Dali::Ui::Callback<bool(View, Dali::String&)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnRequestName));
    Dali::Ui::Extension::View::SetAccessibilityRequestDefaultNameCallback(
      callbackView,
      Dali::Ui::Callback<bool(View, Dali::String&)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnRequestDefaultName));
    Dali::Ui::Extension::View::SetAccessibilityRequestDescriptionCallback(
      callbackView,
      Dali::Ui::Callback<bool(View, Dali::String&)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnRequestDescription));
    Dali::Ui::Extension::View::SetAccessibilityRequestDefaultDescriptionCallback(
      callbackView,
      Dali::Ui::Callback<bool(View, Dali::String&)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnRequestDefaultDescription));
    Dali::Ui::Extension::View::SetAccessibilityRequestValueCallback(
      callbackView,
      Dali::Ui::Callback<bool(View, Dali::String&)>::New(&handler, &PerViewAccessibilityCallbackHandler::OnRequestValue));

    Dali::Property::Map attributes;
    const bool          activated   = callbackView.DoAction("activate", attributes);
    const bool          escaped     = callbackView.DoAction("escape", attributes);
    const bool          incremented = callbackView.DoAction("increment", attributes);
    const bool          decremented = callbackView.DoAction("decrement", attributes);
    const bool          scrolled    = viewAccessible->ScrollToChild(mRelated);
    AddResult(report, passed, "per-View action callbacks without subclassing",
              activated && escaped && incremented && decremented && scrolled &&
                handler.GetActivateCount() == 1 && handler.GetEscapeCount() == 1 &&
                handler.GetValueChangeCount() == 2 && handler.GetValueChangeBalance() == 0 &&
                handler.GetScrollToChildCount() == 1 && handler.GetLastScrolledChild() == mRelated);

    AddResult(report, passed, "per-View dynamic request callbacks",
              accessible->GetName() == "Callback name" &&
                accessible->GetDescription() == "Callback description" &&
                accessible->GetValue() == "Callback value" &&
                handler.GetRequestNameCount() == 1 && handler.GetRequestDescriptionCount() == 1 &&
                handler.GetRequestValueCount() == 1);

    handler.SetHandleDynamicRequests(false);
    callbackView.SetAccessibilityName("");
    callbackView.SetAccessibilityDescription("");
    AddResult(report, passed, "per-View default request callbacks",
              accessible->GetName() == "Callback default name" &&
                accessible->GetDescription() == "Callback default description" &&
                handler.GetRequestDefaultNameCount() == 1 &&
                handler.GetRequestDefaultDescriptionCount() == 1 &&
                handler.ReceivedExpectedView());

    // Member-function callback targets must outlive their registrations.
    Dali::Ui::Extension::View::SetAccessibilityActivateCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityEscapeCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityValueChangeCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityScrollToChildCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityRequestNameCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityRequestDefaultNameCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityRequestDescriptionCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityRequestDefaultDescriptionCallback(callbackView, {});
    Dali::Ui::Extension::View::SetAccessibilityRequestValueCallback(callbackView, {});

    callbackView.SetAccessibilityName("Restored stored name");
    callbackView.SetAccessibilityDescription("Restored stored description");
    callbackView.SetAccessibilityValue("Restored stored value");
    AddResult(report, passed, "empty callbacks restore existing dispatch",
              !callbackView.DoAction("activate", attributes) &&
                !callbackView.DoAction("escape", attributes) &&
                !callbackView.DoAction("increment", attributes) &&
                !viewAccessible->ScrollToChild(mRelated) &&
                accessible->GetName() == "Restored stored name" &&
                accessible->GetDescription() == "Restored stored description" &&
                accessible->GetValue() == "Restored stored value" &&
                handler.GetActivateCount() == 1 && handler.GetEscapeCount() == 1 &&
                handler.GetValueChangeCount() == 2 && handler.GetScrollToChildCount() == 1);
  }

  void RunChecks()
  {
    bool               passed = true;
    std::ostringstream report;
    report << "Accessibility View API checks\n\n";

    mSubject.SetAccessibilityName("Static name");
    mSubject.SetAccessibilityDescription("Static description");
    mSubject.SetAccessibilityValue("Static value");
    mSubject.SetAccessibilityRole(Accessibility::Role::BUTTON);
    AddResult(report, passed, "basic strings and role",
              mSubject.GetAccessibilityName() == "Static name" &&
                mSubject.GetAccessibilityDescription() == "Static description" &&
                mSubject.GetAccessibilityValue() == "Static value" &&
                mSubject.GetAccessibilityRole() == Accessibility::Role::BUTTON);

    mSubject.SetAccessibilityHidden(true);
    mSubject.SetAccessibilityHighlightable(false);
    mSubject.SetAccessibilityScrollable(true);
    mSubject.SetAccessibilityModal(true);
    mSubject.SetAutomationId("phase2-subject");
    AddResult(report, passed, "flags and automation id",
              mSubject.IsAccessibilityHidden() && !mSubject.IsAccessibilityHighlightable() &&
                mSubject.IsAccessibilityScrollable() && mSubject.IsAccessibilityModal() &&
                mSubject.GetAutomationId() == "phase2-subject");
    mSubject.SetAccessibilityHidden(false);
    mSubject.ResetAccessibilityHighlightable();

    mSubject.AddAccessibilityState(Accessibility::State::ENABLED);
    mSubject.AddAccessibilityState(Accessibility::State::BUSY);
    mSubject.RemoveAccessibilityState(Accessibility::State::BUSY);
    AddResult(report, passed, "state Add/Remove/Has",
              mSubject.HasAccessibilityState(Accessibility::State::ENABLED) &&
                !mSubject.HasAccessibilityState(Accessibility::State::BUSY));

    mSubject.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::NAME);
    mSubject.AddAccessibilityReadingInfo(Accessibility::ReadingInfo::DESCRIPTION);
    mSubject.RemoveAccessibilityReadingInfo(Accessibility::ReadingInfo::DESCRIPTION);
    AddResult(report, passed, "reading info Add/Remove/Has",
              mSubject.HasAccessibilityReadingInfo(Accessibility::ReadingInfo::NAME) &&
                !mSubject.HasAccessibilityReadingInfo(Accessibility::ReadingInfo::DESCRIPTION));

    mSubject.AddAccessibilityRelation(Accessibility::RelationType::DESCRIBED_BY, mRelated);
    AddResult(report, passed, "View relation", mSubject.HasAccessibilityRelation(Accessibility::RelationType::DESCRIBED_BY, mRelated));

    mSubject.SetAccessibilityName("Hello world");
    mSubject.SetAccessibilityDescription("Bonjour world");
    const bool nameSpan        = mSubject.AddAccessibilityNameLanguageSpan(0u, 5u, "en-US");
    const bool descriptionSpan = mSubject.AddAccessibilityDescriptionLanguageSpan(0u, 7u, "fr-FR");
    AddResult(report, passed, "scalar language spans", nameSpan && descriptionSpan);

    mSubject.SetRequestInitialAccessibilityHighlight(true);
    mSubject.SetAccessibilityCollectionContainer(true);
    mSubject.SetAccessibilityCollectionIndex(4);
    mSubject.AppendAccessibilityAttribute("sample-key", "sample-value");
    AddResult(report, passed, "typed and raw attributes",
              mSubject.IsInitialAccessibilityHighlightRequested() &&
                mSubject.IsAccessibilityCollectionContainer() &&
                mSubject.GetAccessibilityCollectionIndex() == 4);

    auto localization = Dali::Ui::UiLocalizationManager::Get();
    localization.SetLocalizedStringOverride(Localize);
    mSubject.SetTranslatableAccessibilityName("IDS_A11Y_NAME", "sample");
    mSubject.SetTranslatableAccessibilityDescription("IDS_A11Y_DESCRIPTION");
    localization.RefreshBindings();
    AddResult(report, passed, "per-View translation binding",
              mSubject.GetTranslatableAccessibilityName() == "IDS_A11Y_NAME" &&
                mSubject.GetTranslatableAccessibilityDescription() == "IDS_A11Y_DESCRIPTION" &&
                mSubject.GetAccessibilityName() == "Localized account name" &&
                mSubject.GetAccessibilityDescription() == "Localized account description");

    auto* accessible = Dali::Accessibility::Accessible::Get(mSubject);
    AddResult(report, passed, "requested text virtuals",
              accessible && accessible->GetName() == "Requested account name" &&
                accessible->GetDescription() == "Requested account description" &&
                accessible->GetValue() == "Requested account value");

    mSubjectImpl->SetRequestedMode(AccessibilityDemoViewImpl::RequestedMode::EMPTY);
    AddResult(report, passed, "handled empty requested values",
              accessible && accessible->GetName().empty() &&
                accessible->GetDescription().empty() && accessible->GetValue().empty());

    mSubjectImpl->SetRequestedMode(AccessibilityDemoViewImpl::RequestedMode::FALLBACK);
    AddResult(report, passed, "requested virtual fallback",
              accessible && accessible->GetName() == "Localized account name" &&
                accessible->GetDescription() == "Localized account description" &&
                accessible->GetValue() == "Static value");

    mSubjectImpl->SetDefaultNameMode(AccessibilityDemoViewImpl::DefaultNameMode::VALUE);
    AddResult(report, passed, "explicit name beats default-name hook",
              accessible && accessible->GetName() == "Localized account name");

    mSubject.ClearTranslatableAccessibilityName();
    mSubject.SetAccessibilityName("");
    mSubject.SetProperty(Dali::Actor::Property::NAME, "Actor fallback name");
    AddResult(report, passed, "default-name hook",
              accessible && accessible->GetName() == "View default account name");

    mSubjectImpl->SetDefaultNameMode(AccessibilityDemoViewImpl::DefaultNameMode::EMPTY);
    AddResult(report, passed, "handled empty default name",
              accessible && accessible->GetName().empty());

    mSubjectImpl->SetDefaultNameMode(AccessibilityDemoViewImpl::DefaultNameMode::FALLBACK);
    AddResult(report, passed, "default-name fallback to Actor name",
              accessible && accessible->GetName() == "Actor fallback name");

    mSubjectImpl->SetDefaultDescriptionMode(AccessibilityDemoViewImpl::DefaultDescriptionMode::VALUE);
    AddResult(report, passed, "explicit description beats default hook",
              accessible && accessible->GetDescription() == "Localized account description");

    mSubject.ClearTranslatableAccessibilityDescription();
    mSubject.SetAccessibilityDescription("");
    AddResult(report, passed, "default-description hook",
              accessible && accessible->GetDescription() == "View default account description");

    mSubjectImpl->SetDefaultDescriptionMode(AccessibilityDemoViewImpl::DefaultDescriptionMode::EMPTY);
    AddResult(report, passed, "handled empty default description",
              accessible && accessible->GetDescription().empty());

    mSubjectImpl->SetDefaultDescriptionMode(AccessibilityDemoViewImpl::DefaultDescriptionMode::FALLBACK);
    AddResult(report, passed, "default-description raw fallback",
              accessible && accessible->GetDescription().empty());

    Dali::Property::Map attributes;
    const bool          activated   = mSubject.DoAction("activate", attributes);
    const bool          escaped     = mSubject.DoAction("escape", attributes);
    const bool          incremented = mSubject.DoAction("increment", attributes);
    const bool          decremented = mSubject.DoAction("decrement", attributes);
    AddResult(report, passed, "action virtual dispatch",
              activated && escaped && incremented && decremented &&
                mSubjectImpl->GetActivateCount() == 1 && mSubjectImpl->GetEscapeCount() == 1 &&
                mSubjectImpl->GetValueChangeCount() == 2 && mSubjectImpl->GetValueChangeBalance() == 0);

    const bool scrolled = mSubjectImpl->OnAccessibilityScrollToChild(mRelated);
    AddResult(report, passed, "scroll-to-child virtual dispatch",
              scrolled && mSubjectImpl->GetScrollToChildCount() == 1 &&
                mSubjectImpl->GetLastScrolledChild() == mRelated);

    const bool panned = mSubjectImpl->OnAccessibilityPan(Dali::PanGesture{});
    const bool zoomed = mSubjectImpl->OnAccessibilityZoom();
    AddResult(report, passed, "pan and zoom virtuals",
              panned && zoomed && mSubjectImpl->GetPanCount() == 1 && mSubjectImpl->GetZoomCount() == 1);

    RunPerViewCallbackChecks(report, passed);

    mSubject.DoAction("ReadingStarted", attributes);
    mSubject.DoAction("ReadingSkipped", attributes);
    mSubject.DoAction("ReadingPaused", attributes);
    mSubject.DoAction("ReadingResumed", attributes);
    mSubject.DoAction("ReadingCancelled", attributes);
    mSubject.DoAction("ReadingStopped", attributes);
    const std::vector<Accessibility::ReadingStatus> expectedStatuses{
      Accessibility::ReadingStatus::STARTED,
      Accessibility::ReadingStatus::SKIPPED,
      Accessibility::ReadingStatus::PAUSED,
      Accessibility::ReadingStatus::RESUMED,
      Accessibility::ReadingStatus::CANCELLED,
      Accessibility::ReadingStatus::STOPPED};
    AddResult(report, passed, "unified reading status signal", mReadingSourceMatches && mReadingStatuses == expectedStatuses);

    mSubject.AccessibilityHighlightedSignal().Emit(mSubject, true);
    mSubject.AccessibilityHighlightedSignal().Emit(mSubject, false);
    AddResult(report, passed, "highlight signal", mHighlightSourceMatches && mHighlightCount == 2 && !mLastHighlightState);

    mSubject.ClearAccessibilityStates();
    mSubject.ClearAccessibilityReadingInfo();
    mSubject.ClearAccessibilityRelations();
    mSubject.ClearAccessibilityNameLanguageSpans();
    mSubject.ClearAccessibilityDescriptionLanguageSpans();
    mSubject.ClearAccessibilityCollectionIndex();
    mSubject.RemoveAccessibilityAttribute("sample-key");
    mSubject.ClearAccessibilityAttributes();
    mSubject.ClearTranslatableAccessibilityName();
    mSubject.ClearTranslatableAccessibilityDescription();
    localization.ClearLocalizedStringOverride();

    report << '\n'
           << (passed ? "Overall: PASS" : "Overall: FAIL") << '\n';
    report << "See stdout for virtual, callback, and reading-status dispatch.";
    const std::string reportText = report.str();
    std::cout << '\n'
              << reportText << '\n';
    mResult.SetText(reportText.c_str());
  }

  Dali::Application&                        mApplication;
  View                                      mSubject;
  View                                      mRelated;
  Label                                     mResult;
  AccessibilityDemoViewImpl*                mSubjectImpl{nullptr};
  std::vector<Accessibility::ReadingStatus> mReadingStatuses;
  bool                                      mReadingSourceMatches{true};
  bool                                      mHighlightSourceMatches{true};
  int                                       mHighlightCount{0};
  bool                                      mLastHighlightState{false};
};
} // namespace

int DALI_EXPORT_API main(int argc, char** argv)
{
  auto                           application = Dali::Application::New(&argc, &argv);
  AccessibilityViewApiController controller(application);
  application.MainLoop();
  return 0;
}
