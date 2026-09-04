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

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

Label MakeLabel(const Dali::String& text, float size, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(size);
  label.SetTextColor(UiColor(color));
  label.SetMultiLine(true);
  return label;
}

TextButton MakeButton(const Dali::String& text)
{
  TextButton button = TextButton::New(text);
  button.SetRequestedWidth(MATCH_PARENT);
  button.SetRequestedHeight(56.0f);
  return button;
}

} // namespace

class ToastExample : public ConnectionTracker
{
public:
  explicit ToastExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &ToastExample::Create);
  }

private:
  void Create(Application application)
  {
    Components::UiConfig::New().Apply();
    mWindow = application.GetWindow();
    mWindow.SetBackgroundColor(Vector4(0.94f, 0.94f, 0.96f, 1.0f));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Insets(32.0f, 32.0f, 32.0f, 32.0f));
    root.SetSpacing(12.0f);
    root.Add(MakeLabel("Toast Sample", 30.0f, 0x202124u));
    root.Add(MakeLabel("Most buttons repost one Toast instance. Post the indefinite Toast, then post the custom Toast to verify distinct-instance coexistence. The first action click expands and the second dismisses.", 17.0f, 0x55565Bu));

    TextButton shortButton = MakeButton("Post SHORT toast (1.5 s)");
    shortButton.ConnectClickedSignal(this, &ToastExample::PostShort);
    root.Add(shortButton);

    TextButton longButton = MakeButton("Post LONG toast (2.5 s)");
    longButton.ConnectClickedSignal(this, &ToastExample::PostLong);
    root.Add(longButton);

    TextButton indefiniteButton = MakeButton("Post INDEFINITE toast with action");
    indefiniteButton.ConnectClickedSignal(this, &ToastExample::PostIndefinite);
    root.Add(indefiniteButton);

    TextButton noActionButton = MakeButton("Post toast without action");
    noActionButton.ConnectClickedSignal(this, &ToastExample::PostWithoutAction);
    root.Add(noActionButton);

    TextButton customButton = MakeButton("Post custom-style toast");
    customButton.ConnectClickedSignal(this, &ToastExample::PostCustom);
    root.Add(customButton);

    TextButton dismissButton = MakeButton("Dismiss current toast");
    dismissButton.ConnectClickedSignal(this, &ToastExample::DismissCurrent);
    root.Add(dismissButton);

    mStatus = MakeLabel("Shown: 0 | Hidden: 0 | Action(default): 0 | Action(expanded): 0", 16.0f, 0x0057B8u);
    root.Add(mStatus);
    mLastEvent = MakeLabel("Last event: none", 16.0f, 0x444449u);
    root.Add(mLastEvent);

    mDefaultToast = Toast::New("Saved successfully", Toast::Duration::INDEFINITE);
    ConnectSignals(mDefaultToast);

    ToastStyle customStyle = ToastStyle::Default()
                               .Configure()
                               .SetBackgroundColor(UiColor(0x23465Du))
                               .SetTextColor(UiColor(0xFFFFFFu))
                               .SetCornerRadius(Vector4(20.0f, 20.0f, 20.0f, 20.0f))
                               .Build();
    mCustomToast = Toast::New("Custom ToastStyle is active", Toast::Duration::LONG, customStyle);
    ConnectSignals(mCustomToast);

    mWindow.Add(root);
  }

  void ConnectSignals(Toast toast)
  {
    toast.ShownSignal().Connect(this, &ToastExample::OnShown);
    toast.HiddenSignal().Connect(this, &ToastExample::OnHidden);
    toast.ActionButtonClickedSignal().Connect(this, &ToastExample::OnAction);
  }

  void PrepareDefault(const Dali::String& text, uint32_t duration, const Dali::String& actionText)
  {
    mCurrentToast = mDefaultToast;
    mDefaultToast.SetText(text);
    mDefaultToast.SetDuration(duration);
    mDefaultToast.SetActionButtonText(actionText);
    mDefaultToast.Post(mWindow);
  }

  void PostShort(View, InputEvent)
  {
    PrepareDefault("A short informational Toast", Toast::Duration::SHORT, "Details");
  }

  void PostLong(View, InputEvent)
  {
    PrepareDefault("A longer notification gives the user more reading time", Toast::Duration::LONG, "Review");
  }

  void PostIndefinite(View, InputEvent)
  {
    PrepareDefault("Tap Action once to expand this message. Tap it again to confirm and dismiss.", Toast::Duration::INDEFINITE, "Action");
  }

  void PostWithoutAction(View, InputEvent)
  {
    PrepareDefault("This Toast has no action button", Toast::Duration::LONG, "");
  }

  void PostCustom(View, InputEvent)
  {
    mCurrentToast = mCustomToast;
    mCustomToast.Post(mWindow);
  }

  void DismissCurrent(View, InputEvent)
  {
    if(mCurrentToast)
    {
      mCurrentToast.Dismiss();
    }
  }

  void OnShown(Toast)
  {
    ++mShownCount;
    mLastEvent.SetText("Last event: ShownSignal");
    UpdateStatus();
  }

  void OnHidden(Toast)
  {
    ++mHiddenCount;
    mLastEvent.SetText("Last event: HiddenSignal");
    UpdateStatus();
  }

  void OnAction(Toast, Toast::ActionStage stage)
  {
    if(stage == Toast::ActionStage::CONFIRM_IN_DEFAULT_MODE)
    {
      ++mDefaultActionCount;
      mLastEvent.SetText("Last event: action in default mode (expanding)");
    }
    else
    {
      ++mExpandedActionCount;
      mLastEvent.SetText("Last event: action in expanded mode (dismissing)");
    }
    UpdateStatus();
  }

  void UpdateStatus()
  {
    const std::string status = "Shown: " + std::to_string(mShownCount) +
                               " | Hidden: " + std::to_string(mHiddenCount) +
                               " | Action(default): " + std::to_string(mDefaultActionCount) +
                               " | Action(expanded): " + std::to_string(mExpandedActionCount);
    mStatus.SetText(status.c_str());
  }

private:
  Application& mApplication;
  Window       mWindow;
  Toast        mDefaultToast;
  Toast        mCustomToast;
  Toast        mCurrentToast;
  Label        mStatus;
  Label        mLastEvent;
  uint32_t     mShownCount{0u};
  uint32_t     mHiddenCount{0u};
  uint32_t     mDefaultActionCount{0u};
  uint32_t     mExpandedActionCount{0u};
};

int main(int argc, char** argv)
{
  Application  application = Application::New(&argc, &argv);
  ToastExample example(application);
  application.MainLoop();
  return 0;
}
