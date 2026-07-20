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
#include <dali-ui-foundation/public-api/views/view-accessibility-types.h>
#include <dali/dali.h>
#include <dali/devel-api/atspi-interfaces/accessible.h>
#include <dali/devel-api/atspi-interfaces/action.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>

#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace
{
using Dali::Ui::InteractiveView;
using Dali::Ui::Label;
using Dali::Ui::SelectableView;
using Dali::Ui::View;
namespace Accessibility       = Dali::Ui::Accessibility;
namespace ExportAccessibility = Dali::Integration::Accessibility;

void SetGeometry(View view, float x, float y, float width, float height)
{
  view.SetRequestedX(x);
  view.SetRequestedY(y);
  view.SetRequestedWidth(width);
  view.SetRequestedHeight(height);
}

void SetAccessibility(View view, Accessibility::Role role, const char* name, const char* description, std::initializer_list<Accessibility::State> states)
{
  view.SetAccessibilityRole(role);
  view.SetAccessibilityName(name);
  view.SetAccessibilityDescription(description);
  for(auto state : states)
  {
    view.AddAccessibilityState(state);
  }
}

class MockAccessibilityClient
{
public:
  struct Snapshot
  {
    bool                                             exists{false};
    std::string                                      name;
    std::string                                      description;
    std::string                                      value;
    uint32_t                                         role{0u};
    uint64_t                                         states{0u};
    bool                                             hidden{false};
    std::size_t                                      relationCount{0u};
    std::size_t                                      actionCount{0u};
    std::vector<std::pair<std::string, std::string>> attributes;
  };

  Snapshot Dump(const std::string& label, View view)
  {
    Snapshot snapshot;
    auto*    accessible = Dali::Accessibility::Accessible::Get(view);
    if(!accessible)
    {
      std::cout << label << ": no accessible object\n";
      return snapshot;
    }

    snapshot.exists        = true;
    snapshot.name          = accessible->GetName();
    snapshot.description   = accessible->GetDescription();
    snapshot.value         = accessible->GetValue();
    snapshot.role          = static_cast<uint32_t>(accessible->GetRole());
    snapshot.states        = accessible->GetStates().GetRawData64();
    snapshot.hidden        = accessible->IsHidden();
    snapshot.relationCount = accessible->GetRelationSet().size();
    const auto attributes  = accessible->GetAttributes();
    snapshot.attributes.reserve(attributes.size());
    for(const auto& attribute : attributes)
    {
      snapshot.attributes.push_back(attribute);
    }

    auto action = accessible->GetFeature<Dali::Accessibility::Action>();
    if(action)
    {
      snapshot.actionCount = action->GetActionCount();
    }

    std::cout << label << '\n';
    std::cout << "  name: " << snapshot.name << '\n';
    std::cout << "  description: " << snapshot.description << '\n';
    std::cout << "  value: " << snapshot.value << '\n';
    std::cout << "  role: " << snapshot.role << '\n';
    std::cout << "  states: 0x" << std::hex << snapshot.states << std::dec << '\n';
    std::cout << "  hidden: " << (snapshot.hidden ? "true" : "false") << '\n';
    std::cout << "  relations: " << snapshot.relationCount << '\n';

    for(const auto& attribute : snapshot.attributes)
    {
      std::cout << "  attribute: " << attribute.first << '=' << attribute.second << '\n';
    }

    if(action)
    {
      std::cout << "  actions: " << snapshot.actionCount << '\n';
      for(std::size_t index = 0u; index < snapshot.actionCount; ++index)
      {
        std::cout << "    [" << index << "] " << action->GetActionName(index) << '\n';
      }
    }

    return snapshot;
  }

  bool Activate(const std::string& label, View view)
  {
    auto* accessible = Dali::Accessibility::Accessible::Get(view);
    if(!accessible)
    {
      std::cout << label << ": no accessible action target\n";
      return false;
    }

    auto action = accessible->GetFeature<Dali::Accessibility::Action>();
    if(!action)
    {
      std::cout << label << ": no action feature\n";
      return false;
    }

    const bool result = action->DoAction("activate");
    std::cout << label << " activate result: " << (result ? "true" : "false") << '\n';
    return result;
  }
};

bool HasState(uint64_t states, ExportAccessibility::State state)
{
  return (states & (uint64_t{1u} << static_cast<uint32_t>(state))) != 0u;
}

bool HasAttribute(const MockAccessibilityClient::Snapshot& snapshot, const std::string& name, const std::string& value)
{
  for(const auto& attribute : snapshot.attributes)
  {
    if(attribute.first == name && attribute.second == value)
    {
      return true;
    }
  }
  return false;
}

void AddCheck(std::ostringstream& report, bool& allPassed, const char* label, bool passed)
{
  report << (passed ? "PASS " : "FAIL ") << label << '\n';
  allPassed = allPassed && passed;
}

class AccessibilityTest : public Dali::ConnectionTracker
{
public:
  explicit AccessibilityTest(Dali::Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AccessibilityTest::OnInit);
  }

private:
  void OnInit(Dali::Application application)
  {
    auto window = application.GetWindow();
    window.SetBackgroundColor(Dali::Color::WHITE);

    mTitle = Label::New("DALi UI Accessibility Test");
    SetGeometry(mTitle, 32.0f, 28.0f, 520.0f, 54.0f);
    mTitle.SetBackgroundColor(Dali::Ui::UiColor(0xF5F5F5));
    SetAccessibility(mTitle,
                     Accessibility::Role::HEADER,
                     "Accessibility test title",
                     "Static heading exposed as text for accessibility clients",
                     {Accessibility::State::ENABLED});
    window.Add(mTitle);

    mAction = InteractiveView::New();
    SetGeometry(mAction, 32.0f, 112.0f, 300.0f, 72.0f);
    mAction.SetBackgroundColor(Dali::Ui::UiColor(0xD6EAF8));
    mAction.SetAutomationId("primary-action");
    SetAccessibility(mAction,
                     Accessibility::Role::BUTTON,
                     "Primary action",
                     "Default accessibility activate moves focus to this view",
                     {Accessibility::State::ENABLED});
    window.Add(mAction);

    auto actionLabel = Label::New("Primary action");
    SetGeometry(actionLabel, 20.0f, 18.0f, 260.0f, 36.0f);
    mAction.Add(actionLabel);

    mSelected = SelectableView::New();
    SetGeometry(mSelected, 32.0f, 208.0f, 300.0f, 72.0f);
    mSelected.SetBackgroundColor(Dali::Ui::UiColor(0xD5F5E3));
    mSelected.SetSelected(true);
    SetAccessibility(mSelected,
                     Accessibility::Role::CHECK_BOX,
                     "Selected option",
                     "Selectable view with selected and checked state bits",
                     {Accessibility::State::ENABLED, Accessibility::State::SELECTED, Accessibility::State::CHECKED});
    window.Add(mSelected);

    auto selectedLabel = Label::New("Selected option");
    SetGeometry(selectedLabel, 20.0f, 18.0f, 260.0f, 36.0f);
    mSelected.Add(selectedLabel);

    mProgress = View::New();
    SetGeometry(mProgress, 32.0f, 304.0f, 300.0f, 54.0f);
    mProgress.SetBackgroundColor(Dali::Ui::UiColor(0xFADBD8));
    mProgress.SetAccessibilityValue("60%");
    SetAccessibility(mProgress,
                     Accessibility::Role::PROGRESS_BAR,
                     "Loading progress",
                     "Value property is available to accessibility clients",
                     {Accessibility::State::ENABLED, Accessibility::State::BUSY});
    window.Add(mProgress);

    mDialog = View::New();
    SetGeometry(mDialog, 372.0f, 112.0f, 300.0f, 168.0f);
    mDialog.SetBackgroundColor(Dali::Ui::UiColor(0xFCF3CF));
    mDialog.SetAccessibilityModal(true);
    SetAccessibility(mDialog,
                     Accessibility::Role::DIALOG,
                     "Modal dialog",
                     "Modal view exposed with dialog role",
                     {Accessibility::State::ENABLED});
    window.Add(mDialog);

    auto dialogLabel = Label::New("Modal dialog");
    SetGeometry(dialogLabel, 20.0f, 18.0f, 260.0f, 36.0f);
    mDialog.Add(dialogLabel);

    mHidden = View::New();
    SetGeometry(mHidden, 372.0f, 304.0f, 300.0f, 54.0f);
    mHidden.SetBackgroundColor(Dali::Ui::UiColor(0xEBEDEF));
    mHidden.SetAccessibilityHidden(true);
    SetAccessibility(mHidden,
                     Accessibility::Role::NOTIFICATION,
                     "Hidden notification",
                     "This view is intentionally hidden from accessibility traversal",
                     {Accessibility::State::ENABLED});
    window.Add(mHidden);

    mResult = Label::New();
    SetGeometry(mResult, 32.0f, 406.0f, 640.0f, 238.0f);
    mResult.SetBackgroundColor(Dali::Ui::UiColor(0xF4F6F7));
    mResult.SetTextColor(Dali::Ui::UiColor(0x111111));
    mResult.SetMultiLine(true);
    mResult.SetProperty(Label::Property::FONT_SIZE, 16.0f);
    window.Add(mResult);

    RunAccessibilityCheck();
  }

  void RunAccessibilityCheck()
  {
    auto title     = mClient.Dump("title", mTitle);
    auto button    = mClient.Dump("button", mAction);
    bool activated = mClient.Activate("button", mAction);
    auto selected  = mClient.Dump("selected", mSelected);
    auto progress  = mClient.Dump("progress", mProgress);
    auto dialog    = mClient.Dump("dialog", mDialog);
    auto hidden    = mClient.Dump("hidden", mHidden);

    auto focused = Dali::Ui::FocusManager::Get().GetCurrentFocusView();
    std::cout << "focused after activate: " << (focused == mAction ? "primary-action" : "none/other") << '\n';

    bool               allPassed = true;
    std::ostringstream report;
    report << "Accessibility refactoring test\n";
    report << "No interaction required. The mock client already queried this UI.\n\n";

    AddCheck(report,
             allPassed,
             "title exposes name and header role",
             title.exists && title.name == "Accessibility test title" &&
               title.role == static_cast<uint32_t>(ExportAccessibility::Role::HEADER));
    AddCheck(report,
             allPassed,
             "button exposes action metadata",
             button.exists && button.name == "Primary action" && button.actionCount > 0u &&
               button.role == static_cast<uint32_t>(ExportAccessibility::Role::PUSH_BUTTON) &&
               HasAttribute(button, "automationId", "primary-action"));
    AddCheck(report, allPassed, "mock activate moves focus to button", activated && focused == mAction);
    AddCheck(report,
             allPassed,
             "selected view exports checked/selected states",
             selected.exists && HasState(selected.states, ExportAccessibility::State::CHECKED) &&
               HasState(selected.states, ExportAccessibility::State::SELECTED));
    AddCheck(report,
             allPassed,
             "progress exports value and busy state",
             progress.exists && progress.value == "60%" &&
               HasState(progress.states, ExportAccessibility::State::BUSY));
    AddCheck(report,
             allPassed,
             "dialog exports modal role/state",
             dialog.exists && dialog.role == static_cast<uint32_t>(ExportAccessibility::Role::DIALOG) &&
               HasState(dialog.states, ExportAccessibility::State::MODAL));
    AddCheck(report, allPassed, "hidden object stays hidden", hidden.exists && hidden.hidden);

    report << '\n'
           << (allPassed ? "Overall: PASS" : "Overall: FAIL") << '\n';
    report << "Close the app when done.";
    mResult.SetText(report.str().c_str());
  }

  Dali::Application&      mApplication;
  MockAccessibilityClient mClient;
  Label                   mTitle;
  InteractiveView         mAction;
  SelectableView          mSelected;
  View                    mProgress;
  View                    mDialog;
  View                    mHidden;
  Label                   mResult;
};
} // namespace

int main(int argc, char** argv)
{
  auto               app    = Dali::Application::New(&argc, &argv);
  Dali::Ui::UiConfig config = Dali::Ui::UiConfig::New();
  config.SetDefaultStateEffectForInteractive(Dali::Ui::OverlayEffect::Plain());
  config.Apply();
  AccessibilityTest test(app);
  app.MainLoop();
  return 0;
}
