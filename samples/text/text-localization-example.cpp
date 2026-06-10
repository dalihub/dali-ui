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

// Apps can only use public headers in platform builds.
// #include <dali/integration-api/string-utils.h>

#include <clocale>
#include <string>

// test
// Apps can only use public headers in platform builds.
// #include <dali/devel-api/adaptor-framework/application-devel.h>

using namespace Dali;
using namespace Dali::Ui;

// Apps can only use public headers in platform builds.
// using Dali::Integration::ToDaliStringView;
// using Dali::Integration::ToStdString;

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

std::string ToStdStringPublic(StringView stringView)
{
  return std::string(stringView.Data(), stringView.Size());
}

bool LocalizationOverride(StringView resourceId, StringView domain, Dali::String& outString)
{
  const std::string id  = ToStdStringPublic(resourceId);
  const std::string dom = ToStdStringPublic(domain);

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
    manager.SetDefaultDomain(mCurrentDomain.c_str());

    window.Add(CreateContents());

    UpdateStatusLabel();

    window.KeyEventSignal().Connect(this, &TextLocalizationController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetSpacing(STACK_SPACING);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    mTitleLabel          = CreateTitleLabel();
    mSubtitleLabel       = CreateSubtitleLabel();
    mExplicitDomainLabel = CreateExplicitDomainLabel();
    mStatusLabel         = CreateStatusLabel();

    contents.Add(CreateHeaderLabel());
    contents.Add(CreateHelpLabel());
    contents.Add(CreateSeparator());
    contents.Add(mTitleLabel);
    contents.Add(mSubtitleLabel);
    contents.Add(mExplicitDomainLabel);
    contents.Add(CreateDirectBindingLabel());
    contents.Add(CreateDirectExplicitDomainBindingLabel());
    contents.Add(mStatusLabel);
    return contents;
  }

  Label CreateHeaderLabel()
  {
    Label label = Label::New("UiLocalizationManager Label Example");
    label.SetFontSize(TITLE_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(10, 10, 10, 10));
    label.SetBackgroundColor(UiColor(COLOR_LIGHT_GRAY));
    return label;
  }

  Label CreateHelpLabel()
  {
    Label label = Label::New("Keys: D=domain, B=bypass, R=refresh, M=manual text, ESC/BACK=quit");
    label.SetFontSize(HELP_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(10, 10, 10, 10));
    label.SetBackgroundColor(UiColor(0xEFEFEF));
    return label;
  }

  View CreateSeparator()
  {
    View separator = View::New();
    separator.SetBackgroundColor(UiColor(COLOR_MID_GRAY));
    separator.SetRequestedWidth(MATCH_PARENT);
    separator.SetRequestedHeight(2.0f);
    return separator;
  }

  Label CreateTitleLabel()
  {
    Label label = Label::New();
    label.SetTranslatableText("IDS_TITLE");
    label.SetFontSize(LABEL_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(20, 20, 20, 20));
    label.SetBackgroundColor(UiColor(COLOR_LIGHT_RED));
    return label;
  }

  Label CreateSubtitleLabel()
  {
    Label label = Label::New();
    label.SetTranslatableText("IDS_SUBTITLE");
    label.SetFontSize(LABEL_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(20, 20, 20, 20));
    label.SetBackgroundColor(UiColor(COLOR_LIGHT_BLUE));
    return label;
  }

  Label CreateExplicitDomainLabel()
  {
    Label label = Label::New();
    label.SetTranslatableText("IDS_EXPLICIT_DOMAIN", "domainA");
    label.SetFontSize(LABEL_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(20, 20, 20, 20));
    label.SetBackgroundColor(UiColor(0xF0F0F0));
    return label;
  }

  Label CreateDirectBindingLabel()
  {
    // Demonstrates generic UiLocalizationManager binding without Label::SetTranslatableText().
    // The label is created first, then SetBindingResource() is called directly.
    auto label = Label::New();
    label.SetFontSize(LABEL_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(20, 20, 20, 20));
    label.SetBackgroundColor(UiColor(0xFFF7E6));

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
    auto label = Label::New();
    label.SetFontSize(LABEL_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(20, 20, 20, 20));
    label.SetBackgroundColor(UiColor(0xEFFFF2));

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
    Label label = Label::New();
    label.SetFontSize(STATUS_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetPadding(Extents(10, 10, 10, 10));
    label.SetBackgroundColor(UiColor(0xFAFAFA));
    return label;
  }

  void UpdateStatusLabel()
  {
    const std::string status =
      "DefaultDomain: " + mCurrentDomain +
      " | Bypass: " + std::string(mBypassEnabled ? "ON" : "OFF") +
      " | Title resourceId: " + ToStdStringPublic(mTitleLabel.GetTranslatableText()) +
      " | Explicit label domain: domainA" +
      " | Direct binding: IDS_DIRECT_BINDING" +
      " | Direct explicit: IDS_DIRECT_EXPLICIT_DOMAIN/domainA";

    mStatusLabel.SetText(Dali::String(status.c_str()));
  }

  void ToggleDomain()
  {
    mCurrentDomain = (mCurrentDomain == "domainA") ? "domainB" : "domainA";

    UiLocalizationManager::Get().SetDefaultDomain(mCurrentDomain.c_str());

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
    else if(event.GetKeyName() == "1")
    {
      SetLocale("en_US");
    }
    else if(event.GetKeyName() == "2")
    {
      SetLocale("ko_KR");
    }
    else if(event.GetKeyName() == "3")
    {
      SetLocale("ar_AE");
    }
    else if(event.GetKeyName() == "4")
    {
      SetLocale("ja_JP");
    }
  }

  void SetLocale(Dali::String locale)
  {
    // for test.
    setlocale(LC_MESSAGES, locale.CStr());
    // Apps can only use public headers in platform builds.
    // Dali::DevelApplication::SetApplicationLocale(mApplication, locale.CStr());
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
  UiConfig::New().Apply();

  TextLocalizationController controller(application);
  application.MainLoop();

  return 0;
}
