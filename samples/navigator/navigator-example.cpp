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

// Interactive sample for Ui::Navigator (+ DialogContainer + AlertDialog).
//
//   - Push Page / Pop Page : Navigator::Push / Navigator::Pop (fade transition)
//   - Show Dialog          : Navigator::PushModal of a DialogContainer holding an
//                            AlertDialog. Tap an action button OR tap the scrim
//                            (outside the card) to dismiss it.
//   - Back / Escape        : Navigator::NavigateBack (dismisses modal, else pops;
//                            quits when nothing is left to go back to)

#include <dali-ui-components/public-api/dialog/alert-dialog.h>
#include <dali-ui-components/public-api/dialog/dialog-container.h>
#include <dali-ui-components/public-api/navigator/navigator.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/views/interactive-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/layouts/absolute-layout-params.h>
#include <dali-ui-foundation/public-api/layouts/layout-types.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <functional>
#include <string>

using namespace Dali;
using namespace Dali::Ui;
using Dali::Ui::View;

namespace
{
const UiColor PAGE_COLORS[] = {UiColor(0x1565C0u), UiColor(0x2E7D32u), UiColor(0x6A1B9Au), UiColor(0xEF6C00u), UiColor(0xC62828u)};
constexpr int PAGE_COLOR_COUNT = 5;
} // namespace

class NavigatorExample : public ConnectionTracker
{
public:
  explicit NavigatorExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &NavigatorExample::Create);
  }

  ~NavigatorExample() = default;

  void Create(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    const auto windowSize = window.GetPositionSize();
    mWindowW              = static_cast<float>(windowSize.width);
    mWindowH              = static_cast<float>(windowSize.height);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(12.0f);
    root.SetPadding(Extents(24, 24, 24, 24));

    root.Add(MakeText("Navigator Sample", 24.0f, 48.0f));

    mNavigator = Navigator::New();
    mNavigator.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    mNavigator.TransitionFinishedSignal().Connect(this, &NavigatorExample::OnTransitionFinished);
    root.Add(mNavigator);

    mStatus = MakeText("", 18.0f, 36.0f);
    root.Add(mStatus);

    StackLayout buttons = StackLayout::New(StackOrientation::HORIZONTAL);
    buttons.SetRequestedWidth(MATCH_PARENT);
    buttons.SetRequestedHeight(80.0f);
    buttons.SetSpacing(8.0f);
    buttons.SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::FILL));
    buttons.Add(MakeButton("Push Page", UiColor(0x3367D6u), [this]() { PushPage(); }));
    buttons.Add(MakeButton("Pop Page", UiColor(0x5F6368u), [this]() { mNavigator.Pop(); UpdateStatus(); }));
    buttons.Add(MakeButton("Show Dialog", UiColor(0x00897Bu), [this]() { ShowDialog(); }));
    root.Add(buttons);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &NavigatorExample::OnKeyEvent);

    PushPage(); // start with one page
  }

private:
  void PushPage()
  {
    View page = View::New();
    page.SetBackgroundColor(PAGE_COLORS[(mNextPageNumber - 1) % PAGE_COLOR_COUNT]);

    Label label = Label::New(("Page " + std::to_string(mNextPageNumber)).c_str());
    label.SetFontSize(28.0f);
    label.SetTextColor(UiColor(0xFFFFFFu));
    label.SetRequestedPositionX(24.0f);
    label.SetRequestedPositionY(24.0f);
    page.AddChildren({label});

    ++mNextPageNumber;
    mNavigator.Push(page); // Navigator sizes the page to fill itself
    UpdateStatus();
  }

  void ShowDialog()
  {
    AlertDialog alert = AlertDialog::New();
    alert.SetBackgroundColor(UiColor(0xFFFFFFu));
    alert.SetSpacing(8.0f);
    alert.SetTitle("Delete item?");
    alert.SetMessage("This action cannot be undone.");
    alert.SetActionButtons({{"Cancel", [this]() { mNavigator.PopModal(); }},
                            {"Delete", [this]() { mNavigator.PopModal(); }}});

    // Center the card over the scrim.
    const float dialogWidth  = 600.0f;
    const float dialogHeight = 340.0f;
    alert.SetLayoutParams(AbsoluteLayoutParams::New()
                            .SetBounds(LayoutRect(0.5f, 0.5f, dialogWidth, dialogHeight))
                            .SetFlags(AbsoluteLayoutFlags::POSITION_PROPORTIONAL));

    DialogContainer container = DialogContainer::New();
    container.SetModalContent(alert);

    mNavigator.PushModal(container);
    UpdateStatus();
  }

  void OnTransitionFinished(Navigator /*navigator*/)
  {
    UpdateStatus();
  }

  void UpdateStatus()
  {
    const std::string text = "nav pages: " + std::to_string(mNavigator.GetNavigationStackCount()) +
                             "   modals: " + std::to_string(mNavigator.GetModalStackCount());
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
    label.SetFontSize(16.0f);
    label.SetTextColor(UiColor(0xFFFFFFu));
    label.SetRequestedPositionX(16.0f);
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
        if(!mNavigator.NavigateBack())
        {
          mApplication.Quit();
        }
      }
    }
  }

  Application& mApplication;
  Navigator    mNavigator;
  Label        mStatus;
  int          mNextPageNumber{1};
  float        mWindowW{0.0f};
  float        mWindowH{0.0f};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  NavigatorExample test(application);
  application.MainLoop();
  return 0;
}
