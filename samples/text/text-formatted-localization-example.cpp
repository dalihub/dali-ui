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

// Demonstrates localized format lookup, application-side positional printf
// formatting, and quantity-dependent plural lookup using resource-ID-based
// PO/MO entries.
//
// Host-side manual test keys:
//   1 - Set locale to en_US
//   2 - Set locale to ko_KR
//   3 - Set locale to pl_PL
//   ESC/BACK - Quit

#include <dali-ui-foundation/dali-ui-foundation.h>
#include "text-localization-locale.h"

#include <clocale>
#include <cstdio>
#include <string>
#include <vector>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING   = 10.0f;
constexpr float STACK_PADDING   = 20.0f;
constexpr float TITLE_FONT_SIZE = 20.0f;
constexpr float LABEL_FONT_SIZE = 18.0f;
constexpr float CASE_FONT_SIZE   = 15.0f;
constexpr float HELP_FONT_SIZE   = 14.0f;

constexpr uint32_t COLOR_WHITE      = 0xFFFFFF;
constexpr uint32_t COLOR_DARK_TEXT  = 0x222222;
constexpr uint32_t COLOR_MID_GRAY   = 0x6B7280;
constexpr uint32_t COLOR_LIGHT_BLUE = 0xEAF4FF;
constexpr uint32_t COLOR_LIGHT_GRAY = 0xF2F2F2;

constexpr const char* USER_NAME   = "Alex";
constexpr const char* DEVICE_NAME = "Living Room TV";
constexpr const char* FILE_NAME   = "holiday.mp4";

constexpr int    CURRENT_DOWNLOAD     = 3;
constexpr int    TOTAL_DOWNLOADS      = 10;
constexpr int    EPISODE_NUMBER       = 3;
constexpr int    START_HOUR           = 21;
constexpr int    START_MINUTE         = 7;
constexpr double DOWNLOAD_PERCENT     = 30.0;
constexpr double AVAILABLE_STORAGE_GB = 128.75;

/**
 * @brief Formats a trusted localized printf string using positional arguments.
 *
 * The PO catalog is application-owned and each conversion specifier must match
 * the corresponding variadic argument type. Negative formatting results are
 * made visible in the sample UI instead of returning a truncated result.
 */
template<typename... Arguments>
Dali::String FormatString(const Dali::String& format, Arguments... arguments)
{
  const char* formatString = format.CStr();
  if(!formatString)
  {
    return Dali::String("[formatting error]");
  }

#if defined(_WIN32)
  // The MSVC positional printf family uses the _p APIs.
  const int requiredLength = _scprintf_p(formatString, arguments...);
#else
  const int requiredLength = std::snprintf(nullptr, 0u, formatString, arguments...);
#endif

  if(requiredLength < 0)
  {
    return Dali::String("[formatting error]");
  }

  std::vector<char> buffer(static_cast<std::size_t>(requiredLength) + 1u);

#if defined(_WIN32)
  const int writtenLength = _sprintf_p(buffer.data(), buffer.size(), formatString, arguments...);
#else
  const int writtenLength = std::snprintf(buffer.data(), buffer.size(), formatString, arguments...);
#endif

  if(writtenLength != requiredLength)
  {
    return Dali::String("[formatting error]");
  }

  return Dali::String(buffer.data());
}

} // namespace

class TextFormattedLocalizationController : public ConnectionTracker
{
public:
  explicit TextFormattedLocalizationController(Application& application)
  : mApplication(application),
    mCurrentLocale("en_US.UTF-8")
  {
    mApplication.InitSignal().Connect(this, &TextFormattedLocalizationController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_WHITE));

    UiLocalizationManager manager = UiLocalizationManager::Get();
    manager.RegisterDomain(TEXT_FORMATTED_LOCALIZATION_DOMAIN, TEXT_FORMATTED_LOCALIZATION_LOCALE_DIR);
    manager.SetDefaultDomain(TEXT_FORMATTED_LOCALIZATION_DOMAIN);

    window.Add(CreateContents());
    SetLocale("en_US", "en_US.UTF-8", "en-US");

    window.KeyEventSignal().Connect(this, &TextFormattedLocalizationController::OnKeyEvent);
  }

  View CreateContents()
  {
    ScrollView scroll = ScrollView::New();
    scroll.SetScrollDirection(ScrollDirection::Vertical);
    scroll.SetRequestedWidth(MATCH_PARENT);
    scroll.SetMaximumHeight(520.0f);

    StackLayout cases = StackLayout::New(StackOrientation::VERTICAL);
    cases.SetSpacing(STACK_SPACING);
    cases.SetRequestedWidth(MATCH_PARENT);
    cases.SetRequestedHeight(WRAP_CONTENT);

    AddCase(cases, "IDS_CONNECTION_REORDER — String argument reordering", mConnectionLabel);
    AddCase(cases, "IDS_DOWNLOAD_PROGRESS — Mixed types / escaped percent", mDownloadProgressLabel);
    AddCase(cases, "IDS_STORAGE_AVAILABLE — LC_NUMERIC floating-point", mStorageLabel);
    AddCase(cases, "IDS_EPISODE_START — Positional width / zero padding", mZeroPaddingLabel);
    AddCase(cases, "IDS_UNREAD_MESSAGE — Plural lookup for 1 / 2 / 5", mUnreadMessagesLabel);
    BindLocalizedFormats();
    scroll.SetContent(cases);

    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetSpacing(STACK_SPACING);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));

    mHeaderLabel = CreateHeaderLabel();
    mStatusLabel = CreateStatusLabel();

    contents.Add(mHeaderLabel);
    contents.Add(CreateHelpLabel());
    contents.Add(CreateSeparator());
    contents.Add(scroll);
    contents.Add(CreateSeparator());
    contents.Add(mStatusLabel);
    return contents;
  }

  Label CreateHeaderLabel()
  {
    Label label = Label::New();
    label.SetTranslatableText("IDS_COMMON_TITLE", TEXT_FORMATTED_LOCALIZATION_DOMAIN);
    label.SetFontSize(TITLE_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    label.SetPadding(Extents(10, 10, 10, 10));
    label.SetBackgroundColor(UiColor(COLOR_LIGHT_GRAY));
    return label;
  }

  Label CreateHelpLabel()
  {
    Label label = Label::New("Host test keys: 1=en_US, 2=ko_KR, 3=pl_PL, ESC/BACK=quit");
    label.SetFontSize(HELP_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    label.SetMultiLine(true);
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

  Label CreateCaseTitle(const char* text)
  {
    Label label = Label::New(text);
    label.SetFontSize(CASE_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_MID_GRAY));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    return label;
  }

  Label CreateResultLabel()
  {
    Label label = Label::New();
    label.SetFontSize(LABEL_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    label.SetMultiLine(true);
    label.SetPadding(Extents(10, 10, 10, 10));
    label.SetBackgroundColor(UiColor(COLOR_LIGHT_BLUE));
    return label;
  }

  Label CreateStatusLabel()
  {
    Label label = Label::New();
    label.SetFontSize(HELP_FONT_SIZE);
    label.SetTextColor(UiColor(COLOR_DARK_TEXT));
    label.SetRequestedWidth(MATCH_PARENT);
    label.SetRequestedHeight(WRAP_CONTENT);
    label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    label.SetMultiLine(true);
    return label;
  }

  void AddCase(StackLayout& cases, const char* title, Label& resultLabel)
  {
    resultLabel = CreateResultLabel();
    cases.Add(CreateCaseTitle(title));
    cases.Add(resultLabel);
  }

  void BindLocalizedFormats()
  {
    UiLocalizationManager manager = UiLocalizationManager::Get();

    // The manager supplies the localized msgstr to each callback whenever the
    // locale is refreshed. The application formats that string in the callback.
    manager.SetBindingResource(
      mConnectionLabel,
      "Text",
      "IDS_CONNECTION_REORDER",
      TEXT_FORMATTED_LOCALIZATION_DOMAIN,
      LocalizedStringCallback::New(this, &TextFormattedLocalizationController::ApplyConnectionFormat));

    manager.SetBindingResource(
      mDownloadProgressLabel,
      "Text",
      "IDS_DOWNLOAD_PROGRESS",
      TEXT_FORMATTED_LOCALIZATION_DOMAIN,
      LocalizedStringCallback::New(this, &TextFormattedLocalizationController::ApplyDownloadProgressFormat));

    manager.SetBindingResource(
      mStorageLabel,
      "Text",
      "IDS_STORAGE_AVAILABLE",
      TEXT_FORMATTED_LOCALIZATION_DOMAIN,
      LocalizedStringCallback::New(this, &TextFormattedLocalizationController::ApplyStorageFormat));

    manager.SetBindingResource(
      mZeroPaddingLabel,
      "Text",
      "IDS_EPISODE_START",
      TEXT_FORMATTED_LOCALIZATION_DOMAIN,
      LocalizedStringCallback::New(this, &TextFormattedLocalizationController::ApplyZeroPaddingFormat));

    manager.SetBindingResource(
      mUnreadMessagesLabel,
      "Text",
      "IDS_UNREAD_MESSAGE",
      TEXT_FORMATTED_LOCALIZATION_DOMAIN,
      LocalizedStringCallback::New(this, &TextFormattedLocalizationController::ApplyUnreadMessagesFormat));
  }

  void ApplyConnectionFormat(BaseHandle target, const Dali::String& format)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetText(FormatString(format, USER_NAME, DEVICE_NAME));
    }
  }

  void ApplyUnreadMessagesFormat(BaseHandle target, const Dali::String& /*localizedString*/)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      // The binding provides locale-change refreshes. Plural lookup is
      // repeated here because selection also depends on quantity.
      constexpr uint32_t quantities[] = {1u, 2u, 5u};

      UiLocalizationManager manager = UiLocalizationManager::Get();
      std::string           result;

      for(uint32_t quantity : quantities)
      {
        Dali::String format = manager.GetLocalizedPluralString(
          "IDS_UNREAD_MESSAGE",
          "IDS_PLURAL_UNREAD_MESSAGE",
          quantity,
          TEXT_FORMATTED_LOCALIZATION_DOMAIN);

        Dali::String line = FormatString(format, USER_NAME, static_cast<int>(quantity));
        if(!result.empty())
        {
          result += '\n';
        }
        result += line.CStr();
      }

      label.SetText(Dali::String(result.c_str()));
    }
  }

  void ApplyDownloadProgressFormat(BaseHandle target, const Dali::String& format)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetText(FormatString(format, FILE_NAME, CURRENT_DOWNLOAD, TOTAL_DOWNLOADS, DOWNLOAD_PERCENT));
    }
  }

  void ApplyStorageFormat(BaseHandle target, const Dali::String& format)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetText(FormatString(format, DEVICE_NAME, AVAILABLE_STORAGE_GB));
    }
  }

  void ApplyZeroPaddingFormat(BaseHandle target, const Dali::String& format)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetText(FormatString(format, EPISODE_NUMBER, START_HOUR, START_MINUTE));
    }
  }

  void UpdateStatusLabel()
  {
    std::string status = "Locale: " + mCurrentLocale;
    status += "\nMessage locale: ";
    status += Samples::GetMessageLocale();
    status += "\nNumeric locale: ";
    const char* numericLocale = std::setlocale(LC_NUMERIC, nullptr);
    status += numericLocale ? numericLocale : "(null)";
    if(!mMessageLocaleSet || !mNumericLocaleSet)
    {
      status += "\nLocale setup failed:";
      if(!mMessageLocaleSet)
      {
        status += " message";
      }
      if(!mNumericLocaleSet)
      {
        status += " numeric";
      }
    }
    mStatusLabel.SetText(Dali::String(status.c_str()));
  }

  /**
  * @brief Switches locale explicitly for manual testing on Linux and Windows hosts.
  *
  * The 1/2/3 key path and the direct setlocale()/LANGUAGE updates below are
  * specific to this standalone host sample. They are not a locale-management
  * pattern for Tizen device applications.
  *
  * In the Tizen product path, system locale changes are handled by the
  * platform locale infrastructure. UiLocalizationManager listens to
  * Adaptor::LocaleChangedSignal() and refreshes registered bindings when that
  * signal is received. The callbacks can then call
  * GetLocalizedString()/GetLocalizedPluralString(), which use the active
  * gettext locale and catalog.
  *
  * This host test calls RefreshBindings() directly only to apply the locale
  * changed by the sample's 1/2/3 keys immediately.
  */
  void SetLocale(const char* catalogLocale, const char* posixLocale, const char* windowsLocale)
  {
#if defined(_WIN32)
    // gettext uses the catalog locale while the MSVC CRT accepts BCP-47 names.
    (void)posixLocale;
    mCurrentLocale    = windowsLocale;
    mMessageLocaleSet = Samples::SetMessageLocale(catalogLocale);
    mNumericLocaleSet = std::setlocale(LC_NUMERIC, windowsLocale) != nullptr;
#else
    (void)catalogLocale;
    (void)windowsLocale;
    mCurrentLocale    = posixLocale;
    mMessageLocaleSet = Samples::SetMessageLocale(posixLocale);
    mNumericLocaleSet = std::setlocale(LC_NUMERIC, posixLocale) != nullptr;
#endif

    if(!mMessageLocaleSet)
    {
      std::printf("Failed to set message locale to \"%s\"\n", mCurrentLocale.c_str());
    }
    if(!mNumericLocaleSet)
    {
      std::printf("Failed to set numeric locale to \"%s\"\n", mCurrentLocale.c_str());
    }

    UiLocalizationManager::Get().RefreshBindings();
    UpdateStatusLabel();
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "1")
    {
      SetLocale("en_US", "en_US.UTF-8", "en-US");
    }
    else if(event.GetKeyName() == "2")
    {
      SetLocale("ko_KR", "ko_KR.UTF-8", "ko-KR");
    }
    else if(event.GetKeyName() == "3")
    {
      SetLocale("pl_PL", "pl_PL.UTF-8", "pl-PL");
    }
  }

private:
  Application& mApplication;
  Label        mHeaderLabel;
  Label        mConnectionLabel;
  Label        mUnreadMessagesLabel;
  Label        mDownloadProgressLabel;
  Label        mStorageLabel;
  Label        mZeroPaddingLabel;
  Label        mStatusLabel;
  std::string  mCurrentLocale;
  bool         mMessageLocaleSet{false};
  bool         mNumericLocaleSet{false};

  static constexpr const char* TEXT_FORMATTED_LOCALIZATION_DOMAIN     = "text-formatted-localization";
  static constexpr const char* TEXT_FORMATTED_LOCALIZATION_LOCALE_DIR = RESOURCES_DIR "locale/formatted";
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextFormattedLocalizationController controller(application);
  application.MainLoop();

  return 0;
}
