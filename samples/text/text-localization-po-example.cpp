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
// using real gettext PO/MO resources (no override).
//
// Key bindings:
//   1 - Set locale to en_US
//   2 - Set locale to ko_KR
//   3 - Set locale to ar_AE
//   B - Toggle bypass on/off
//   D - Toggle default domain (text-localization-po <-> text-localization-po-alt)
//   R - Manual RefreshBindings()
//   M - Set manual text on common title label
//   ESC/BACK - Quit

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <dali/integration-api/string-utils.h>

#include <clocale>
#include <cstdio>
#include <string>

// for desktop test
#include <dali/devel-api/adaptor-framework/application-devel.h>

using namespace Dali;
using namespace Dali::Ui;

using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToStdString;

namespace
{

constexpr float STACK_SPACING   = 10.0f;
constexpr float STACK_PADDING  = 20.0f;

constexpr float TITLE_FONT_SIZE  = 20.0f;
constexpr float LABEL_FONT_SIZE  = 20.0f;
constexpr float STATUS_FONT_SIZE = 16.0f;
constexpr float HELP_FONT_SIZE   = 14.0f;

constexpr uint32_t COLOR_WHITE       = 0xFFFFFF;
constexpr uint32_t COLOR_DARK_TEXT   = 0x222222;
constexpr uint32_t COLOR_MID_GRAY   = 0x808080;
constexpr uint32_t COLOR_LIGHT_BLUE = 0xEAF4FF;
constexpr uint32_t COLOR_LIGHT_RED  = 0xFFE8E8;
constexpr uint32_t COLOR_LIGHT_GREEN = 0xE8FFE8;
constexpr uint32_t COLOR_LIGHT_GRAY = 0xF2F2F2;
constexpr uint32_t COLOR_LIGHT_YELLOW = 0xFFF7E6;
constexpr uint32_t COLOR_LIGHT_CYAN   = 0xE0FFFF;

} // namespace

class TextLocalizationPoController : public ConnectionTracker
{
public:
  explicit TextLocalizationPoController(Application& application)
  : mApplication(application),
    mCurrentDomain(TEXT_DOMAIN_DEFAULT),
    mCurrentLocale("en_US.UTF-8")
  {
    mApplication.InitSignal().Connect(this, &TextLocalizationPoController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_WHITE));

    UiLocalizationManager manager = UiLocalizationManager::Get();

    // Register both domains with their locale directories
    manager.RegisterDomain(TEXT_DOMAIN_DEFAULT, TEXT_LOCALE_DIR_DEFAULT);
    manager.RegisterDomain(TEXT_DOMAIN_ALT, TEXT_LOCALE_DIR_ALT);

    // Set initial default domain
    manager.SetDefaultDomain(TEXT_DOMAIN_DEFAULT);

    window.Add(CreateContents());

    // Set initial locale
    SetLocale("en_US.UTF-8");

    window.KeyEventSignal().Connect(this, &TextLocalizationPoController::OnKeyEvent);
  }

  View CreateContents()
  {
    ScrollView scroll = ScrollView::New()
      .SetScrollDirection(ScrollDirection::Vertical)
      .SetRequestedWidth(MATCH_PARENT)
      .SetMaximumHeight(400.0f);

    auto bindingContent = StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .Children({
        CreateSectionLabel("Default Domain Labels:"),
        CreateCommonTitleLabel().As(mCommonTitleLabel),
        CreateDefaultTitleLabel().As(mDefaultTitleLabel),
        CreateDefaultHelloLabel().As(mDefaultHelloLabel),
        CreateDefaultThanksLabel().As(mDefaultThanksLabel),
        CreateSeparator(),
        CreateSectionLabel("Explicit Domain Labels:"),
        CreateExplicitDefaultLabel().As(mExplicitDefaultLabel),
        CreateExplicitAltLabel().As(mExplicitAltLabel),
        CreateSeparator(),
        CreateSectionLabel("Direct Binding Labels:"),
        CreateDirectBindingLabel(),
        CreateDirectAltBindingLabel(),
        CreateSeparator(),
        CreateSectionLabel("Image URL Binding:"),
        CreateFlagImageView(),
        CreateSeparator(),
        CreateSectionLabel("InputField / InputEditor Placeholder:"),
        CreateLocalizedInputField(),
        CreateLocalizedInputEditor()
      });

    scroll.SetContent(bindingContent);

    return StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        CreateHeaderLabel(),
        CreateHelpLabel(),
        CreateSeparator(),
        scroll,
        CreateSeparator(),
        CreateSectionLabel("GetLocalizedString Lookup:"),
        CreateLookupLabel().As(mLookupLabel),
        CreateSeparator(),
        CreateStatusLabel().As(mStatusLabel)
      });
  }

  Label CreateHeaderLabel()
  {
    return Label::New("UiLocalizationManager PO/MO Example")
      .SetFontSize(TITLE_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_GRAY));
  }

  Label CreateHelpLabel()
  {
    return Label::New("Keys: 1=en_US, 2=ko_KR, 3=ar_AE, B=bypass, D=toggle default domain, R=refresh, M=manual text, ESC/BACK=quit")
      .SetFontSize(HELP_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS)
      .SetMultiLine(true)
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

  Label CreateSectionLabel(const char* text)
  {
    return Label::New(text)
      .SetFontSize(STATUS_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_MID_GRAY))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  }

  // --- Default domain labels ---

  Label CreateCommonTitleLabel()
  {
    // Uses IDS_COMMON_TITLE which exists in both domains.
    // When default domain changes, this label should switch between DEFAULT/ALT.
    return Label::New()
      .SetTranslatableText("IDS_COMMON_TITLE")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_RED));
  }

  Label CreateDefaultTitleLabel()
  {
    return Label::New()
      .SetTranslatableText("IDS_PO_TITLE")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_BLUE));
  }

  Label CreateDefaultHelloLabel()
  {
    return Label::New()
      .SetTranslatableText("IDS_PO_HELLO")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_BLUE));
  }

  Label CreateDefaultThanksLabel()
  {
    return Label::New()
      .SetTranslatableText("IDS_PO_THANKS")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_BLUE));
  }

  // --- Explicit domain labels ---

  Label CreateExplicitDefaultLabel()
  {
    // Explicit default domain - should not change when default domain toggles
    return Label::New()
      .SetTranslatableText("IDS_PO_HELLO", TEXT_DOMAIN_DEFAULT)
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_GREEN));
  }

  Label CreateExplicitAltLabel()
  {
    // Explicit alt domain - should not change when default domain toggles
    return Label::New()
      .SetTranslatableText("IDS_ALT_HELLO", TEXT_DOMAIN_ALT)
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_GREEN));
  }

  // --- Direct binding labels ---

  Label CreateDirectBindingLabel()
  {
    // Direct binding using default domain
    auto label = Label::New()
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_YELLOW));

    UiLocalizationManager::Get().SetBindingResource(
      label,
      "Text",
      "IDS_PO_DIRECT",
      LocalizedStringCallback::New(this, &TextLocalizationPoController::ApplyDirectBindingText));

    return label;
  }

  Label CreateDirectAltBindingLabel()
  {
    // Direct binding with explicit alt domain
    auto label = Label::New()
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_CYAN));

    UiLocalizationManager::Get().SetBindingResource(
      label,
      "Text",
      "IDS_ALT_DIRECT",
      TEXT_DOMAIN_ALT,
      LocalizedStringCallback::New(this, &TextLocalizationPoController::ApplyDirectAltBindingText));

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

  void ApplyDirectAltBindingText(BaseHandle target, const Dali::String& text)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetText(text);
    }
  }

  // --- Image URL binding ---

  ImageView CreateFlagImageView()
  {
    // Default domain image URL binding - locale changes update the flag image
    auto imageView = ImageView::New()
      .SetRequestedWidth(100.0f)
      .SetRequestedHeight(50.0f)
      .SetBackgroundColor(UiColor(0xFAFAFA));

    UiLocalizationManager::Get().SetBindingResource(
      imageView,
      "ResourceUrl",
      "IDS_FLAG_IMAGE_URL",
      TEXT_DOMAIN_DEFAULT,
      LocalizedStringCallback::New(this, &TextLocalizationPoController::ApplyImageResourceUrl));

    return imageView;
  }

  void ApplyImageResourceUrl(BaseHandle target, const Dali::String& url)
  {
    ImageView imageView = ImageView::DownCast(target);
    if(!imageView)
    {
      return;
    }

    // In bypass mode, url will be the msgid (e.g. "IDS_FLAG_IMAGE_URL") which is not a valid image.
    // This is expected behavior - bypass shows the raw resource ID.
    std::string resourceUrl = RESOURCES_DIR;
    resourceUrl += ToStdString(url);

    imageView.SetResourceUrl(resourceUrl.c_str());
  }

  // --- InputField / InputEditor localization ---

  InputField CreateLocalizedInputField()
  {
    // InputField with translatable placeholder using default domain
    return InputField::New()
      .SetTranslatablePlaceholder("IDS_INPUT_FIELD_PLACEHOLDER")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_YELLOW));
  }

  InputEditor CreateLocalizedInputEditor()
  {
    // InputEditor with translatable placeholder using default domain
    return InputEditor::New()
      .SetTranslatablePlaceholder("IDS_INPUT_EDITOR_PLACEHOLDER")
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(80.0f)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_LIGHT_CYAN));
  }

  // --- Lookup label ---

  Label CreateLookupLabel()
  {
    return Label::New()
      .SetFontSize(STATUS_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS)
      .SetMultiLine(true)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(0xFAFAFA));
  }

  void UpdateLookupLabel()
  {
    UiLocalizationManager manager = UiLocalizationManager::Get();

    Dali::String defaultLookup = manager.GetLocalizedString("IDS_PO_LOOKUP", TEXT_DOMAIN_DEFAULT);
    Dali::String altLookup     = manager.GetLocalizedString("IDS_ALT_LOOKUP", TEXT_DOMAIN_ALT);
    Dali::String commonLookup  = manager.GetLocalizedString("IDS_COMMON_TITLE");

    std::string text;
    text += "DEFAULT: " + std::string(defaultLookup.CStr()) + "\n";
    text += "ALT: " + std::string(altLookup.CStr()) + "\n";
    text += "COMMON (default domain): " + std::string(commonLookup.CStr());

    mLookupLabel.SetText(Dali::String(text.c_str()));
  }

  // --- Status label ---

  Label CreateStatusLabel()
  {
    return Label::New()
      .SetFontSize(STATUS_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS)
      .SetMultiLine(true)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(0xFAFAFA));
  }

  void UpdateStatusLabel()
  {
    const char* lcMsg = setlocale(LC_MESSAGES, nullptr);
    if(!lcMsg)
    {
      lcMsg = "(null)";
    }

    std::string status;
    status += "Locale: " + mCurrentLocale + "\n";
    status += "DefaultDomain: " + mCurrentDomain + "\n";
    status += "Bypass: " + std::string(mBypassEnabled ? "ON" : "OFF") + "\n";
    status += "LC_MESSAGES: " + std::string(lcMsg);

    mStatusLabel.SetText(Dali::String(status.c_str()));
  }

  // --- Actions ---

  /**
   * @brief Sets the locale for Ubuntu desktop sample testing.
   *
   * This is a desktop-only helper. Real applications cannot use
   * DevelApplication to change or simulate the system locale.
   *
   * On target devices, verify localization by changing the system language
   * in Settings and letting the platform locale changed signal refresh the
   * bindings.
   */
  void SetLocale(const std::string& locale)
  {
    mCurrentLocale = locale;

    const char* result = setlocale(LC_MESSAGES, locale.c_str());
    if(!result)
    {
      std::printf("setlocale(LC_MESSAGES, \"%s\") failed\n", locale.c_str());
    }

    Dali::DevelApplication::SetApplicationLocale(mApplication, locale);

    UpdateLookupLabel();
    UpdateStatusLabel();

    std::printf("LC_MESSAGES=%s\n", setlocale(LC_MESSAGES, nullptr));
  }

  void ToggleBypass()
  {
    mBypassEnabled = !mBypassEnabled;

    // SetBypassEnabled() internally calls RefreshBindings()
    UiLocalizationManager::Get().SetBypassEnabled(mBypassEnabled);

    UpdateLookupLabel();
    UpdateStatusLabel();
  }

  void ToggleDefaultDomain()
  {
    if(mCurrentDomain == TEXT_DOMAIN_DEFAULT)
    {
      mCurrentDomain = TEXT_DOMAIN_ALT;
    }
    else
    {
      mCurrentDomain = TEXT_DOMAIN_DEFAULT;
    }

    UiLocalizationManager::Get().SetDefaultDomain(ToDaliStringView(mCurrentDomain));

    UpdateLookupLabel();
    UpdateStatusLabel();
  }

  void RefreshLocalization()
  {
    UiLocalizationManager::Get().RefreshBindings();
    UpdateLookupLabel();
    UpdateStatusLabel();
  }

  void SetManualText()
  {
    mCommonTitleLabel.SetText("Manual Text");
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

    if(event.GetKeyName() == "1")
    {
      SetLocale("en_US.UTF-8");
    }
    else if(event.GetKeyName() == "2")
    {
      SetLocale("ko_KR.UTF-8");
    }
    else if(event.GetKeyName() == "3")
    {
      SetLocale("ar_AE.UTF-8");
    }
    else if(event.GetKeyName() == "b" || event.GetKeyName() == "B")
    {
      ToggleBypass();
    }
    else if(event.GetKeyName() == "d" || event.GetKeyName() == "D")
    {
      ToggleDefaultDomain();
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
  Label        mCommonTitleLabel;
  Label        mDefaultTitleLabel;
  Label        mDefaultHelloLabel;
  Label        mDefaultThanksLabel;
  Label        mExplicitDefaultLabel;
  Label        mExplicitAltLabel;
  Label        mLookupLabel;
  Label        mStatusLabel;
  std::string  mCurrentDomain;
  std::string  mCurrentLocale;
  bool         mBypassEnabled{false};

  static constexpr const char* TEXT_DOMAIN_DEFAULT  = "text-localization-po";
  static constexpr const char* TEXT_DOMAIN_ALT       = "text-localization-po-alt";
  static constexpr const char* TEXT_LOCALE_DIR_DEFAULT = RESOURCES_DIR "locale/default";
  static constexpr const char* TEXT_LOCALE_DIR_ALT     = RESOURCES_DIR "locale/alternate";
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextLocalizationPoController controller(application);
  application.MainLoop();

  return 0;
}
