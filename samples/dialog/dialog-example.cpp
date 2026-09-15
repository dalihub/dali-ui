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

// Interactive sample for Ui::AlertDialog (and the Dialog API it inherits).
//
// The AlertDialog shows a title, a message and two action buttons via the
// convenience API (SetTitle / SetMessage / AddActionButton). Tapping an action
// button updates the status line. The control buttons drive the inherited Dialog
// API on the same dialog:
//   - Spacing +/-  -> Dialog::SetSpacing
//   - Align        -> Dialog::SetLayoutAlignment (FILL -> START -> CENTER -> END)
//   - Toggle Msg   -> AlertDialog::SetMessage(text) / SetMessage("")
//   - Transition   -> Fade / Slide / Scale (content only)
//   - Scrim        -> Default / NoScrim (input remains modal)
//   - Animation    -> On / Off for all close paths
//   - Speed        -> Normal / Slow for visual inspection
//
// Post uses the selected settings; actions dismiss only this dialog.
// Use DialogPostOptions for scrim styling; do not manually parent this dialog.
// Custom scrim Views require a separate manually owned DialogContainer path.
// Press Escape or Back to request dismissal, or quit when no dialog is posted.

#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/public-api/dialog/alert-dialog.h>
#include <dali-ui-components/public-api/dialog/dialog.h>
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/devel-api/adaptor-framework/application.h>
#include "dialog-transition.h"

#include <functional>
#include <string>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::View;

namespace
{
const LayoutAlignment ALIGNMENTS[]  = {LayoutAlignment::FILL, LayoutAlignment::START, LayoutAlignment::CENTER, LayoutAlignment::END};
const char* const     ALIGN_NAMES[] = {"FILL", "START", "CENTER", "END"};
const char* const     MESSAGE_TEXT  = "This action cannot be undone.";
const char* const     TRANSITION_NAMES[] = {"Fade", "Slide", "Scale"};

const char* ReasonName(DialogDismissReason reason)
{
  switch(reason)
  {
    case DialogDismissReason::BACK: return "Back";
    case DialogDismissReason::SCRIM: return "Scrim";
    case DialogDismissReason::PROGRAMMATIC: return "Programmatic";
    case DialogDismissReason::HOST_REMOVED: return "Host removed";
  }
  return "Unknown";
}
} // namespace

class DialogExample : public ConnectionTracker
{
public:
  explicit DialogExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &DialogExample::Create);
  }

  ~DialogExample() = default;

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(8.0f);
    root.SetPadding(Insets(24.0f, 24.0f, 24.0f, 24.0f));

    root.Add(MakeText("Dialog Transition Sample", 22.0f, 40.0f));

    // The AlertDialog under test (uses the convenience + inherited Dialog API).
    mAlert = AlertDialog::New();
    // Keep the OneUI content-height layout; choose a width for this sample window.
    mAlert.SetRequestedWidth(440.0f);
    mAlert.SetLayoutParams(AbsoluteLayoutParams::New()
      .SetBounds(LayoutRect(0.5f, 0.5f, WRAP_CONTENT, WRAP_CONTENT))
      .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));
    mAlert.SetTitle("Delete item?");
    mAlert.SetMessage(MESSAGE_TEXT);
    TextButton cancelButton = mAlert.AddActionButton("Cancel");
    cancelButton.ConnectClickedSignal(this, [this](View, InputEvent)
    { Dismiss("Cancel"); });
    TextButton okButton = mAlert.AddActionButton("OK");
    okButton.ConnectClickedSignal(this, [this](View, InputEvent)
    { Dismiss("OK"); });

    mStatus = MakeText("", 15.0f, 112.0f);
    mStatus.SetMultiLine(true);
    root.Add(mStatus);
    auto hint = MakeText("Select settings, then Post. Close with Cancel/OK, Back or scrim.\nSlide moves only the dialog; watch the stationary scrim fade.", 14.0f, 60.0f);
    hint.SetMultiLine(true);
    root.Add(hint);

    // Connect before Post: unanimated lifecycle signals may be synchronous.
    mAlert.ShownSignal().Connect(this, [this](Dialog) {
      ++mShownCount;
      mPhase = "Shown";
      UpdateStatus();
    });
    mAlert.HiddenSignal().Connect(this, [this](Dialog, DialogDismissReason reason) {
      ++mHiddenCount;
      mClosing = false;
      mPhase = "Hidden";
      mLastReason = ReasonName(reason);
      UpdateStatus();
    });
    mAlert.DismissRequestedSignal().Connect(this, [this](Dialog, DialogDismissReason) {
      mClosing = true;
      mPhase = "Closing";
      UpdateStatus();
      return false;
    });

    StackLayout buttons = StackLayout::New(StackOrientation::HORIZONTAL);
    buttons.SetRequestedWidth(MATCH_PARENT);
    buttons.SetRequestedHeight(80.0f);
    buttons.SetSpacing(8.0f);
    buttons.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    buttons.Add(MakeButton("Spacing +", UiColor(0x3367D6u), [this]()
    { ChangeSpacing(+8.0f); }));
    buttons.Add(MakeButton("Spacing -", UiColor(0x3367D6u), [this]()
    { ChangeSpacing(-8.0f); }));
    buttons.Add(MakeButton("Align", UiColor(0x00897Bu), [this]()
    { CycleAlignment(); }));
    buttons.Add(MakeButton("Toggle Msg", UiColor(0xD81B60u), [this]()
    { ToggleMessage(); }));
    root.Add(buttons);
    StackLayout transition = MakeButtonRow();
    transition.Add(MakeButton("Transition", UiColor(0x7B1FA2u), [this]() {
      mModeIndex = (mModeIndex + 1) % 3;
      UpdateStatus();
    }));
    transition.Add(MakeButton("Scrim", UiColor(0x3367D6u), [this]() {
      mNoScrim = !mNoScrim;
      UpdateStatus();
    }));
    root.Add(transition);
    StackLayout timing = MakeButtonRow();
    timing.Add(MakeButton("Animation", UiColor(0x455A64u), [this]() {
      mAnimated = !mAnimated;
      UpdateStatus();
    }));
    timing.Add(MakeButton("Speed", UiColor(0x455A64u), [this]() {
      mSlow = !mSlow;
      UpdateStatus();
    }));
    root.Add(timing);
    StackLayout presentation = MakeButtonRow();
    presentation.Add(MakeButton("Post", UiColor(0x00897Bu), [this]() { Post(); }));
    root.Add(presentation);

    mNavigator = Navigator::New();
    mNavigator.SetRequestedWidth(MATCH_PARENT);
    mNavigator.SetRequestedHeight(MATCH_PARENT);
    mNavigator.Push(root, false);
    window.Add(mNavigator);
    window.KeyEventSignal().Connect(this, &DialogExample::OnKeyEvent);

    UpdateStatus();
  }

private:
  void Post()
  {
    const auto size = mNavigator.GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    if(size.x <= 0.0f || size.y <= 0.0f)
    {
      mLastPost = "Host not ready";
      UpdateStatus();
      return;
    }
    const float restingY = DialogTransitionSample::CenteredY(mAlert, Vector2(size.x, size.y));
    DialogPostOptions options;
    options.animated = mAnimated;
    if(mNoScrim) options.containerStyle = DialogContainerStyle::NoScrimPreset();
    options.transitionSpec = DialogTransitionSample::CreateSpec(this,
      static_cast<DialogTransitionSample::Mode>(mModeIndex), mSlow ? 1.4f : 0.35f, restingY);
    // Back and scrim use Navigator's switch; button Dismiss also passes the flag.
    mNavigator.SetModalTransitionAnimationEnabled(mAnimated);
    if(!mAlert.IsPosted()) mPhase = "Posting";
    mLastPost = mAlert.Post(mNavigator, options) ? "Accepted" : "Rejected";
    UpdateStatus();
  }

  void Dismiss(const char* action)
  {
    if(mClosing || !mAlert.IsPosted()) return;
    mClosing = true;
    mPhase = "Closing";
    mLastAction = action;
    mAlert.Dismiss(mAnimated);
    UpdateStatus();
  }

  void ChangeSpacing(float delta)
  {
    float spacing = mAlert.GetSpacing() + delta;
    if(spacing < 0.0f)
    {
      spacing = 0.0f;
    }
    mAlert.SetSpacing(spacing);
    UpdateStatus();
  }

  void CycleAlignment()
  {
    mAlignIndex = (mAlignIndex + 1) % 4;
    mAlert.SetLayoutAlignment(ALIGNMENTS[mAlignIndex]);
    UpdateStatus();
  }

  void ToggleMessage()
  {
    mAlert.SetMessage(mAlert.GetMessage().Empty() ? MESSAGE_TEXT : "");
    UpdateStatus();
  }

  void UpdateStatus()
  {
    std::string text = std::string(TRANSITION_NAMES[mModeIndex]) +
      " | Scrim: " + (mNoScrim ? "None" : "Default") +
      " | Animation: " + (mAnimated ? "On" : "Off") +
      "\nSpeed: " + (mSlow ? "Slow (1.4s)" : "Normal (0.35s)") +
      " | Post: " + mLastPost +
      "\n" + mPhase + " | Posted: " + (mAlert.IsPosted() ? "Yes" : "No") +
      " | Shown/Hidden: " + std::to_string(mShownCount) + "/" + std::to_string(mHiddenCount) +
      "\nReason: " + mLastReason + " | Action: " + (mLastAction.empty() ? "-" : mLastAction) +
      "\nSpacing: " + std::to_string(static_cast<int>(mAlert.GetSpacing())) + " | Align: " + ALIGN_NAMES[mAlignIndex];
    mStatus.SetText(text.c_str());
  }

  StackLayout MakeButtonRow()
  {
    auto row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetRequestedWidth(MATCH_PARENT);
    row.SetRequestedHeight(64.0f);
    row.SetSpacing(8.0f);
    row.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    return row;
  }

  Label MakeText(const char* text, float fontSize, float height)
  {
    Label label = Label::New(text);
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(height);
    label.SetFontSize(fontSize);
    label.SetTextColor(UiColor(0x202124u));
    label.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    return label;
  }

  InteractiveView MakeButton(const char* text, const UiColor& color, std::function<void()> onClicked)
  {
    InteractiveView button = InteractiveView::New();
    button.SetBackgroundColor(color);
    button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));

    Label label = Label::New(text);
    label.SetFontSize(15.0f);
    label.SetTextColor(UiColor(0xFFFFFFu));
    label.SetRequestedX(12.0f);
    label.SetRequestedY(20.0f);
    button.Add(label);

    button.ConnectClickedSignal(this, [onClicked](View, InputEvent)
    { onClicked(); });
    return button;
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        if(mAlert.IsPosted())
        {
          // Do not pop the background page while a modal is already closing.
          if(!mClosing) mNavigator.NavigateBack();
        }
        else mApplication.Quit();
      }
    }
  }

  Application& mApplication;
  Navigator    mNavigator;
  AlertDialog  mAlert;
  Label        mStatus;
  int          mAlignIndex{0};
  int          mModeIndex{1};
  bool         mNoScrim{false};
  bool         mAnimated{true};
  bool         mSlow{false};
  bool         mClosing{false};
  unsigned     mShownCount{0u};
  unsigned     mHiddenCount{0u};
  std::string  mLastPost{"-"};
  std::string  mLastReason{"-"};
  std::string  mPhase{"Idle"};
  std::string  mLastAction;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application          application = Application::New(&argc, &argv);
  Components::UiConfig config      = Components::UiConfig::New();
  config.SetDefaultStateEffectForInteractive(OverlayEffect::Plain());
  config.Apply();
  DialogExample test(application);
  application.MainLoop();
  return 0;
}
