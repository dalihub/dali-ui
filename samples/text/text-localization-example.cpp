/* Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// Demonstrates UiLocalizationManager and Label translatableText
// using an override-based localized string provider.
//
// Key bindings:
//   D - Toggle default domain (domainA <-> domainB)
//   B - Toggle bypass on/off
//   R - Manual RefreshBindings()
//   M - Set manual text on title label
//   ESC/BACK - Quit

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <dali/integration-api/string-utils.h>

#include <string>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToStdString;

namespace
{
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 20.0f;

constexpr float TITLE_FONT_SIZE    = 32.0f;
constexpr float LABEL_FONT_SIZE    = 24.0f;
constexpr float STATUS_FONT_SIZE   = 18.0f;
constexpr float HELP_FONT_SIZE     = 16.0f;

constexpr uint32_t COLOR_WHITE      = 0xFFFFFF;
constexpr uint32_t COLOR_DARK_TEXT  = 0x222222;
constexpr uint32_t COLOR_MID_GRAY   = 0x808080;
constexpr uint32_t COLOR_LIGHT_BLUE = 0xEAF4FF;
constexpr uint32_t COLOR_LIGHT_RED  = 0xFFE8E8;
constexpr uint32_t COLOR_LIGHT_GRAY = 0xF2F2F2;

bool LocalizationOverride(StringView resourceId, StringView domain, Dali::String& outString)
{
  const std::string id  = ToStdString(resourceId);
  const std::string dom = ToStdString(domain);

  if(id == "IDS_TITLE")
  {
    if(dom == "domainA")
    {
      outString = "Title - Domain A";
    }
    else if(dom == "domainB")
    {
      outString = "Title - Domain B";
    }
    else
    {
      outString = "Title - Default";
    }
    return true;
  }

  if(id == "IDS_SUBTITLE")
  {
    if(dom == "domainA")
    {
      outString = "Subtitle - Domain A";
    }
    else if(dom == "domainB")
    {
      outString = "Subtitle - Domain B";
    }
    else
    {
      outString = "Subtitle - Default";
    }
    return true;
  }

  if(id == "IDS_EXPLICIT_DOMAIN")
  {
    if(dom == "domainA")
    {
      outString = "Explicit Domain Label - Domain A";
    }
    else if(dom == "domainB")
    {
      outString = "Explicit Domain Label - Domain B";
    }
    else
    {
      outString = "Explicit Domain Label - Default";
    }
    return true;
  }

  if(id == "IDS_DIRECT_BINDING")
  {
    if(dom == "domainA")
    {
      outString = "Direct Binding Label - Domain A";
    }
    else if(dom == "domainB")
    {
      outString = "Direct Binding Label - Domain B";
    }
    else
    {
      outString = "Direct Binding Label - Default";
    }
    return true;
  }

  if(id == "IDS_DIRECT_EXPLICIT_DOMAIN")
  {
    if(dom == "domainA")
    {
      outString = "Direct Explicit Binding - Domain A";
    }
    else if(dom == "domainB")
    {
      outString = "Direct Explicit Binding - Domain B";
    }
    else
    {
      outString = "Direct Explicit Binding - Default";
    }
    return true;
  }

  return false;
}

} // namespace

class TextLocalizationController : public ConnectionTracker
{
public:
  explicit TextLocalizationController(Application& application)
  : mApplication(application),
    mCurrentDomain("domainA"),
    mBypassEnabled(false)
  {
    mApplication.InitSignal().Connect(this, &TextLocalizationController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_WHITE));

    UiLocalizationManager manager = UiLocalizationManager::Get();
    manager.SetLocalizedStringOverride(&LocalizationOverride);
    manager.SetDefaultDomain(ToDaliStringView(mCurrentDomain));

    window.Add(CreateContents());

    UpdateStatusLabel();

    window.KeyEventSignal().Connect(this, &TextLocalizationController::OnKeyEvent);
  }

  View CreateContents()
  {
    return StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        CreateHeaderLabel(),
        CreateHelpLabel(),
        CreateSeparator(),
        CreateTitleLabel().As(mTitleLabel),
        CreateSubtitleLabel().As(mSubtitleLabel),
        CreateExplicitDomainLabel().As(mExplicitDomainLabel),
        CreateDirectBindingLabel(),
        CreateDirectExplicitDomainBindingLabel(),
        CreateStatusLabel().As(mStatusLabel),
      });
  }

  Label CreateHeaderLabel()
  {
    return Label::New("UiLocalizationManager Label Example")
      .SetFontSize(TITLE_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_GRAY));
  }

  Label CreateHelpLabel()
  {
    return Label::New("Keys: D=domain, B=bypass, R=refresh, M=manual text, ESC/BACK=quit")
      .SetFontSize(HELP_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(0xEFEFEF));
  }

  View CreateSeparator()
  {
    return View::New()
      .SetBackgroundColor(UiColor(COLOR_MID_GRAY))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(2.0f);
  }

  Label CreateTitleLabel()
  {
    return Label::New()
      .SetTranslatableText("IDS_TITLE")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_RED));
  }

  Label CreateSubtitleLabel()
  {
    return Label::New()
      .SetTranslatableText("IDS_SUBTITLE")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_BLUE));
  }

  Label CreateExplicitDomainLabel()
  {
    return Label::New()
      .SetTranslatableText("IDS_EXPLICIT_DOMAIN", "domainA")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(0xF0F0F0));
  }

  Label CreateDirectBindingLabel()
  {
    // Demonstrates generic UiLocalizationManager binding without Label::SetTranslatableText().
    // The label is created first, then SetBindingResource() is called directly.
    auto label = Label::New()
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(0xFFF7E6));

    UiLocalizationManager::Get().SetBindingResource(
      label,
      "Text",
      "IDS_DIRECT_BINDING",
      LocalizedStringCallback::New(this, &TextLocalizationController::ApplyDirectBindingText));

    return label;
  }

  void ApplyDirectBindingText(BaseHandle target, const Dali::String& text)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetText(text);
    }
  }

  Label CreateDirectExplicitDomainBindingLabel()
  {
    // Demonstrates generic UiLocalizationManager binding with an explicit domain.
    // This label uses domainA directly, so default domain changes should not affect it.
    // Compare with CreateDirectBindingLabel() which uses the default domain.
    auto label = Label::New()
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(20, 20, 20, 20))
      .SetBackgroundColor(UiColor(0xEFFFF2));

    UiLocalizationManager::Get().SetBindingResource(
      label,
      "Text",
      "IDS_DIRECT_EXPLICIT_DOMAIN",
      "domainA",
      LocalizedStringCallback::New(this, &TextLocalizationController::ApplyDirectExplicitDomainBindingText));

    return label;
  }

  void ApplyDirectExplicitDomainBindingText(BaseHandle target, const Dali::String& text)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetText(text);
    }
  }

  Label CreateStatusLabel()
  {
    return Label::New()
      .SetFontSize(STATUS_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(0xFAFAFA));
  }

  void UpdateStatusLabel()
  {
    const std::string status =
      "DefaultDomain: " + mCurrentDomain +
      " | Bypass: " + std::string(mBypassEnabled ? "ON" : "OFF") +
      " | Title resourceId: " + ToStdString(mTitleLabel.GetTranslatableText()) +
      " | Explicit label domain: domainA" +
      " | Direct binding: IDS_DIRECT_BINDING" +
      " | Direct explicit: IDS_DIRECT_EXPLICIT_DOMAIN/domainA";

    mStatusLabel.SetText(Dali::String(status.c_str()));
  }

  void ToggleDomain()
  {
    mCurrentDomain = (mCurrentDomain == "domainA") ? "domainB" : "domainA";

    UiLocalizationManager::Get().SetDefaultDomain(ToDaliStringView(mCurrentDomain));

    // mExplicitDomainLabel has explicit domainA, so it should not change
    // when only default domain changes.
    UpdateStatusLabel();
  }

  void ToggleBypass()
  {
    mBypassEnabled = !mBypassEnabled;

    UiLocalizationManager::Get().SetBypassEnabled(mBypassEnabled);
    UpdateStatusLabel();
  }

  void RefreshLocalization()
  {
    UiLocalizationManager::Get().RefreshBindings();
    UpdateStatusLabel();
  }

  void SetManualText()
  {
    mTitleLabel.SetText("Manual Text");
    UpdateStatusLabel();
  }

  void OnKeyEvent(Window window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "d" || event.GetKeyName() == "D")
    {
      ToggleDomain();
    }
    else if(event.GetKeyName() == "b" || event.GetKeyName() == "B")
    {
      ToggleBypass();
    }
    else if(event.GetKeyName() == "r" || event.GetKeyName() == "R")
    {
      RefreshLocalization();
    }
    else if(event.GetKeyName() == "m" || event.GetKeyName() == "M")
    {
      SetManualText();
    }
  }

private:
  Application& mApplication;
  Label        mTitleLabel;
  Label        mSubtitleLabel;
  Label        mExplicitDomainLabel;
  Label        mStatusLabel;
  std::string  mCurrentDomain;
  bool         mBypassEnabled;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);

  TextLocalizationController controller(application);
  application.MainLoop();

  return 0;
}
