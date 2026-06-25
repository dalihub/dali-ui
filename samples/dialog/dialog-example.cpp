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
// convenience API (SetTitle / SetMessage / SetActionButtons). Tapping an action
// button updates the status line. The control buttons drive the inherited Dialog
// API on the same dialog:
//   - Spacing +/-  -> Dialog::SetSpacing
//   - Align        -> Dialog::SetLayoutAlignment (FILL -> START -> CENTER -> END)
//   - Toggle Msg   -> AlertDialog::SetMessage(text) / SetMessage("")
//
// Press Escape or Back to quit.

#include <dali-ui-components/public-api/dialog/alert-dialog.h>
#include <dali-ui-components/public-api/dialog/dialog.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/interactive-view.h>
#include <dali-ui-foundation/public-api/label.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

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
    root.SetSpacing(12.0f);
    root.SetPadding(Extents(24, 24, 24, 24));

    root.Add(MakeText("AlertDialog Sample", 24.0f, 48.0f));

    // The AlertDialog under test (uses the convenience + inherited Dialog API).
    mAlert = AlertDialog::New();
    mAlert.SetBackgroundColor(UiColor(0xFFFFFFu));
    mAlert.SetRequestedHeight(300.0f);
    mAlert.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    mAlert.SetSpacing(8.0f);
    mAlert.SetTitle("Delete item?");
    mAlert.SetMessage(MESSAGE_TEXT);
    mAlert.SetActionButtons({{"Cancel", [this]() { mLastAction = "Cancel"; UpdateStatus(); }},
                             {"OK", [this]() { mLastAction = "OK"; UpdateStatus(); }}});
    root.Add(mAlert);

    mStatus = MakeText("", 18.0f, 36.0f);
    root.Add(mStatus);

    StackLayout buttons = StackLayout::New(StackOrientation::HORIZONTAL);
    buttons.SetRequestedWidth(MATCH_PARENT);
    buttons.SetRequestedHeight(80.0f);
    buttons.SetSpacing(8.0f);
    buttons.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    buttons.Add(MakeButton("Spacing +", UiColor(0x3367D6u), [this]() { ChangeSpacing(+8.0f); }));
    buttons.Add(MakeButton("Spacing -", UiColor(0x3367D6u), [this]() { ChangeSpacing(-8.0f); }));
    buttons.Add(MakeButton("Align", UiColor(0x00897Bu), [this]() { CycleAlignment(); }));
    buttons.Add(MakeButton("Toggle Msg", UiColor(0xD81B60u), [this]() { ToggleMessage(); }));
    root.Add(buttons);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &DialogExample::OnKeyEvent);

    UpdateStatus();
  }

private:
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
    std::string text = "spacing: " + std::to_string(static_cast<int>(mAlert.GetSpacing())) +
                       "   align: " + ALIGN_NAMES[mAlignIndex] +
                       "   last action: " + (mLastAction.empty() ? "-" : mLastAction);
    mStatus.SetText(text.c_str());
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
    label.SetRequestedPositionX(12.0f);
    label.SetRequestedPositionY(28.0f);
    button.AddChildren({label});

    button.ConnectClickedSignal(this, [onClicked](View, InputEvent) { onClicked(); });
    return button;
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
  }

  Application& mApplication;
  AlertDialog  mAlert;
  Label        mStatus;
  int          mAlignIndex{0};
  std::string  mLastAction;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  DialogExample test(application);
  application.MainLoop();
  return 0;
}
