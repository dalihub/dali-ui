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

// Interactive Switch verification sample.
//
// Demonstrates the default, initially selected, custom-style, and disabled
// states. Every active Switch has an independent callback counter. The first
// control can also be changed programmatically. Use touch to tap/drag a switch,
// or focus one and press Enter.

#include <dali-ui-components/dali-ui-components.h>
#include <dali-ui-foundation/dali-ui-foundation.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

Label MakeLabel(const Dali::String& text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  return label;
}

Dali::String MakeStatus(const char* name, bool selected, uint32_t callbackCount)
{
  const std::string text = std::string(name) + ": " + (selected ? "ON" : "OFF") +
                           "  |  SelectionChanged callbacks: " + std::to_string(callbackCount);
  return Dali::String(text.c_str());
}

} // namespace

class SwitchExample : public ConnectionTracker
{
public:
  explicit SwitchExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &SwitchExample::Create);
  }

  void Create(Application application)
  {
    Components::UiConfig::New().Apply();

    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetSpacing(10.0f);
    root.SetPadding(Extents(24, 24, 24, 24));

    root.Add(MakeLabel("Switch Sample", 24.0f, 0x202124u));
    root.Add(MakeLabel("Tap or drag. With keyboard focus, press Enter to toggle. The circular dim/recoil effect is targeted to the handle.", 14.0f, 0x5F6368u));

    mDefaultStatus = MakeLabel(MakeStatus("Default", false, 0u), 15.0f, 0x303238u);
    root.Add(mDefaultStatus);
    mDefaultSwitch = Switch::New();
    mDefaultSwitch.SetAccessibilityName("Default switch");
    mDefaultSwitch.SelectionChangedSignal().Connect(this, &SwitchExample::OnDefaultChanged);
    root.Add(mDefaultSwitch);

    TextButton programmaticToggle = TextButton::New("Toggle default programmatically");
    programmaticToggle.SetRequestedWidth(340.0f);
    programmaticToggle.SetRequestedHeight(48.0f);
    programmaticToggle.SetBackgroundColor(UiColor(0xE4E4E7u));
    programmaticToggle.ClickedSignal().Connect(this, &SwitchExample::OnProgrammaticToggle);
    root.Add(programmaticToggle);

    mSelectedStatus = MakeLabel(MakeStatus("Initially selected", true, 0u), 15.0f, 0x303238u);
    root.Add(mSelectedStatus);
    mSelectedSwitch = Switch::New();
    mSelectedSwitch.SetAccessibilityName("Initially selected switch");
    mSelectedSwitch.SetSelected(true);
    mSelectedSwitch.SelectionChangedSignal().Connect(this, &SwitchExample::OnSelectedChanged);
    root.Add(mSelectedSwitch);

    // Configure() clones the default UiStyle. The resulting handle exercises the
    // same Dali-UI style path as CheckBoxStyle and TextButtonStyle.
    SwitchStyle customStyle = SwitchStyle::Default()
                                .Configure()
                                .SetTrackSize(72.0f, 40.0f)
                                .SetThumbPadding(Insets(4.0f, 4.0f, 4.0f, 4.0f))
                                .SetTrackColors(UiColor(0xB7B7BBu), UiColor(0x0BB075u))
                                .Build();

    mCustomStatus = MakeLabel(MakeStatus("Custom UiStyle", false, 0u), 15.0f, 0x303238u);
    root.Add(mCustomStatus);
    mCustomSwitch = Switch::New(customStyle);
    mCustomSwitch.SetAccessibilityName("Custom style switch");
    mCustomSwitch.SelectionChangedSignal().Connect(this, &SwitchExample::OnCustomChanged);
    root.Add(mCustomSwitch);

    root.Add(MakeLabel("Disabled (input and Enter are ignored)", 15.0f, 0x77777Cu));
    Switch disabledSwitch = Switch::New();
    disabledSwitch.SetSelected(true);
    disabledSwitch.SetEnabled(false);
    disabledSwitch.SetAccessibilityName("Disabled switch");
    root.Add(disabledSwitch);

    mLastEvent = MakeLabel("Last callback: none", 14.0f, 0x387AFFu);
    root.Add(mLastEvent);

    window.Add(root);
  }

private:
  void OnDefaultChanged(View /*view*/, bool selected, InputEvent event)
  {
    ++mDefaultCallbackCount;
    mDefaultStatus.SetText(MakeStatus("Default", selected, mDefaultCallbackCount));
    mLastEvent.SetText(event.IsProgrammatic() ? "Last callback: Default (programmatic or drag)" : "Last callback: Default (tap/Enter)");
  }

  void OnSelectedChanged(View /*view*/, bool selected, InputEvent event)
  {
    ++mSelectedCallbackCount;
    mSelectedStatus.SetText(MakeStatus("Initially selected", selected, mSelectedCallbackCount));
    mLastEvent.SetText(event.IsProgrammatic() ? "Last callback: Initially selected (programmatic or drag)" : "Last callback: Initially selected (tap/Enter)");
  }

  void OnCustomChanged(View /*view*/, bool selected, InputEvent event)
  {
    ++mCustomCallbackCount;
    mCustomStatus.SetText(MakeStatus("Custom UiStyle", selected, mCustomCallbackCount));
    mLastEvent.SetText(event.IsProgrammatic() ? "Last callback: Custom UiStyle (programmatic or drag)" : "Last callback: Custom UiStyle (tap/Enter)");
  }

  void OnProgrammaticToggle(View /*view*/, InputEvent /*event*/)
  {
    mDefaultSwitch.SetSelected(!mDefaultSwitch.IsSelected());
  }

private:
  Application& mApplication;

  Switch mDefaultSwitch;
  Switch mSelectedSwitch;
  Switch mCustomSwitch;

  Label mDefaultStatus;
  Label mSelectedStatus;
  Label mCustomStatus;
  Label mLastEvent;

  uint32_t mDefaultCallbackCount{0u};
  uint32_t mSelectedCallbackCount{0u};
  uint32_t mCustomCallbackCount{0u};
};

int main(int argc, char** argv)
{
  Application   application = Application::New(&argc, &argv);
  SwitchExample example(application);
  application.MainLoop();
  return 0;
}
