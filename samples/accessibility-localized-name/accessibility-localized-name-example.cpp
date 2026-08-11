/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Compares two accessibility-name localization strategies using real PO/MO
// resources and the platform accessibility bridge:
//
//   1. Resolve three resource IDs and compose them in several orders per
//      accessibility request.
//   2. Bind one complete-sentence resource ID directly to the View.
//
// With Screen Reader enabled, move between the buttons or press 1-4 to move key
// focus. The platform automatically follows key focus with accessibility
// highlight. Buttons/keys 5 and 6 select English or Korean for this process.
// Press R to refresh localization after changing the system language.

#include <dali-ui-components/public-api/components-ui-config.h>
#include <dali-ui-components/public-api/text-button.h>
#include <dali-ui-foundation/extension-api/view.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/focus-manager/focus-manager.h>
#include <dali-ui-foundation/public-api/layouts/stack-layout.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali/integration-api/debug.h>
#include <dali/public-api/adaptor-framework/application.h>
#include <dali/public-api/adaptor-framework/key.h>

#include "../text/text-localization-locale.h"

#include <clocale>
#include <cstdlib>
#include <string>

namespace
{
using namespace Dali;
using namespace Dali::Ui;

constexpr const char* LOCALIZATION_DOMAIN = "accessibility-localized-name";
constexpr const char* LOCALE_DIRECTORY    = RESOURCES_DIR "locale";

constexpr const char* WIFI_RESOURCE_ID     = "IDS_WIFI";
constexpr const char* CONNECT_RESOURCE_ID  = "IDS_CONNECT";
constexpr const char* COMPLETE_RESOURCE_ID = "IDS_COMPLETE";
constexpr const char* SENTENCE_RESOURCE_ID = "IDS_WIFI_CONNECT_COMPLETE";

#if defined(_WIN32)
constexpr int MESSAGE_LOCALE_CATEGORY = LC_ALL;
#else
constexpr int MESSAGE_LOCALE_CATEGORY = LC_MESSAGES;
#endif

Label CreateVisualLabel(const char* text, float fontSize, uint32_t color)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(color));
  label.SetMultiLine(true);
  label.SetAccessibilityHidden(true);
  return label;
}

void AppendPart(std::string& destination, const Dali::String& part)
{
  if(!destination.empty())
  {
    destination += ' ';
  }
  destination += part.CStr();
}
} // unnamed namespace

class AccessibilityLocalizedNameController : public Dali::ConnectionTracker
{
public:
  explicit AccessibilityLocalizedNameController(Dali::Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &AccessibilityLocalizedNameController::OnInit);
  }

private:
  void OnInit(Dali::Application application)
  {
    // TextButton reads its visual and interaction styles from the components
    // configuration. It includes the foundation configuration as well.
    Dali::Ui::Components::UiConfig::New().Apply();

    auto localization = Dali::Ui::UiLocalizationManager::Get();
    const bool domainRegistered = localization.RegisterDomain(LOCALIZATION_DOMAIN, LOCALE_DIRECTORY);
    DALI_LOG_RELEASE_INFO("[LOCALIZATION DOMAIN] domain=%s localePath=\"%s\" registered=%s\n",
                          LOCALIZATION_DOMAIN,
                          LOCALE_DIRECTORY,
                          domainRegistered ? "true" : "false");
    LogLocalizationState("startup");

    auto window = application.GetWindow();
    window.SetBackgroundColor(Dali::Ui::UiColor(0xF4F7FA));

    auto root = Dali::Ui::StackLayout::New(Dali::Ui::StackOrientation::VERTICAL);
    root.SetRequestedWidth(Dali::Ui::MATCH_PARENT);
    root.SetRequestedHeight(Dali::Ui::MATCH_PARENT);
    root.SetSpacing(7.0f);
    root.SetPadding(Dali::Extents(16, 16, 16, 16));

    root.Add(CreateVisualLabel("Accessibility localized-name composition", 25.0f, 0x17212Bu));
    root.Add(CreateVisualLabel(
      "Press 5/6 to select English/Korean in this app, then swipe between "
      "the test buttons or press 1-4. Press R to refresh localization.",
      15.0f,
      0x4E5D6Cu));

    mEnglishLocaleButton = CreateTestButton("5  Set app locale: English (en_US)", 0xDFF4FFu);
    mEnglishLocaleButton.ClickedSignal().Connect(
      this,
      [this](Dali::Ui::View, Dali::Ui::InputEvent)
    {
      SetSampleLocale("en_US", "en_US.UTF-8", "en-US");
    });

    mKoreanLocaleButton = CreateTestButton("6  Set app locale: Korean (ko_KR)", 0xFFE6EFu);
    mKoreanLocaleButton.ClickedSignal().Connect(
      this,
      [this](Dali::Ui::View, Dali::Ui::InputEvent)
    {
      SetSampleLocale("ko_KR", "ko_KR.UTF-8", "ko-KR");
    });

    mFragmentButton = CreateTestButton("1  Compose IDS_WIFI + IDS_CONNECT + IDS_COMPLETE", 0xDDEEFFu);
    Dali::Ui::Extension::View::SetAccessibilityRequestNameCallback(
      mFragmentButton,
      Dali::Ui::Callback<bool(Dali::Ui::View, Dali::String&)>::New(
        this,
        &AccessibilityLocalizedNameController::OnRequestWifiConnectCompleteName));

    mCompleteFirstButton = CreateTestButton("2  Compose IDS_COMPLETE + IDS_CONNECT + IDS_WIFI", 0xFFF0D8u);
    Dali::Ui::Extension::View::SetAccessibilityRequestNameCallback(
      mCompleteFirstButton,
      Dali::Ui::Callback<bool(Dali::Ui::View, Dali::String&)>::New(
        this,
        &AccessibilityLocalizedNameController::OnRequestCompleteConnectWifiName));

    mConnectFirstButton = CreateTestButton("3  Compose IDS_CONNECT + IDS_WIFI + IDS_COMPLETE", 0xF0E3F7u);
    Dali::Ui::Extension::View::SetAccessibilityRequestNameCallback(
      mConnectFirstButton,
      Dali::Ui::Callback<bool(Dali::Ui::View, Dali::String&)>::New(
        this,
        &AccessibilityLocalizedNameController::OnRequestConnectWifiCompleteName));

    mSentenceButton = CreateTestButton("4  Bind IDS_WIFI_CONNECT_COMPLETE", 0xE3F3E6u);
    mSentenceButton.SetTranslatableAccessibilityName(SENTENCE_RESOURCE_ID, LOCALIZATION_DOMAIN);
    Dali::Ui::Extension::View::SetAccessibilityRequestNameCallback(
      mSentenceButton,
      Dali::Ui::Callback<bool(Dali::Ui::View, Dali::String&)>::New(
        this,
        &AccessibilityLocalizedNameController::OnRequestSentenceName));

    ConfigureButton(mFragmentButton);
    ConfigureButton(mCompleteFirstButton);
    ConfigureButton(mConnectFirstButton);
    ConfigureButton(mSentenceButton);
    root.Add(mEnglishLocaleButton);
    root.Add(mKoreanLocaleButton);
    root.Add(mFragmentButton);
    root.Add(mCompleteFirstButton);
    root.Add(mConnectFirstButton);
    root.Add(mSentenceButton);

    mStatus = CreateVisualLabel("", 13.0f, 0x34495Eu);
    root.Add(mStatus);

    window.Add(root);
    window.KeyEventSignal().Connect(this, &AccessibilityLocalizedNameController::OnKeyEvent);

    SetKeyFocus(mFragmentButton);
  }

  Dali::Ui::TextButton CreateTestButton(const char* visualText, uint32_t backgroundColor)
  {
    auto button = Dali::Ui::TextButton::New(visualText);
    button.SetRequestedWidth(Dali::Ui::MATCH_PARENT);
    button.SetRequestedHeight(58.0f);
    button.SetFontSize(16.0f);
    button.SetTextColor(Dali::Ui::UiColor(0x17212Bu));
    button.SetBackgroundColor(Dali::Ui::UiColor(backgroundColor));
    button.SetPadding(Dali::Extents(16, 16, 10, 10));
    button.SetFocusable(true);
    return button;
  }

  void ConfigureButton(Dali::Ui::TextButton button)
  {
    button.ClickedSignal().Connect(this, [this](Dali::Ui::View view, Dali::Ui::InputEvent)
    {
      SetKeyFocus(view);
    });

    button.AccessibilityHighlightedSignal().Connect(
      this,
      [this](Dali::Ui::View view, bool highlighted)
    {
      if(highlighted)
      {
        mCurrentView = view;
        UpdateStatus();
      }
    });
  }

  Dali::String ComposeFragmentName(const char* firstResourceId,
                                   const char* secondResourceId,
                                   const char* thirdResourceId) const
  {
    auto localization = Dali::Ui::UiLocalizationManager::Get();

    std::string result;
    AppendPart(result, localization.GetLocalizedString(firstResourceId, LOCALIZATION_DOMAIN));
    AppendPart(result, localization.GetLocalizedString(secondResourceId, LOCALIZATION_DOMAIN));
    AppendPart(result, localization.GetLocalizedString(thirdResourceId, LOCALIZATION_DOMAIN));
    return Dali::String(result.c_str());
  }

  void LogRequestedName(const char* buttonId, const Dali::String& result) const
  {
    const char* locale = std::setlocale(MESSAGE_LOCALE_CATEGORY, nullptr);
    DALI_LOG_RELEASE_INFO("[A11Y NAME REQUEST] button=%s locale=\"%s\" text=\"%s\"\n",
                          buttonId,
                          locale ? locale : "(unknown)",
                          result.CStr());
  }

  bool OnRequestWifiConnectCompleteName(Dali::Ui::View, Dali::String& result)
  {
    result = ComposeFragmentName(WIFI_RESOURCE_ID, CONNECT_RESOURCE_ID, COMPLETE_RESOURCE_ID);
    LogRequestedName("1", result);
    return true;
  }

  bool OnRequestCompleteConnectWifiName(Dali::Ui::View, Dali::String& result)
  {
    result = ComposeFragmentName(COMPLETE_RESOURCE_ID, CONNECT_RESOURCE_ID, WIFI_RESOURCE_ID);
    LogRequestedName("2", result);
    return true;
  }

  bool OnRequestConnectWifiCompleteName(Dali::Ui::View, Dali::String& result)
  {
    result = ComposeFragmentName(CONNECT_RESOURCE_ID, WIFI_RESOURCE_ID, COMPLETE_RESOURCE_ID);
    LogRequestedName("3", result);
    return true;
  }

  bool OnRequestSentenceName(Dali::Ui::View view, Dali::String& result)
  {
    // SetTranslatableAccessibilityName() stores the resolved binding in the
    // View's accessibility-name property. Return that exact value so button 4
    // is logged at the same name-request point as buttons 1-3.
    result = view.GetAccessibilityName();
    LogRequestedName("4", result);
    return true;
  }

  void LogLocalizationState(const char* event) const
  {
    auto localization = Dali::Ui::UiLocalizationManager::Get();

    const auto wifi     = localization.GetLocalizedString(WIFI_RESOURCE_ID, LOCALIZATION_DOMAIN);
    const auto connect  = localization.GetLocalizedString(CONNECT_RESOURCE_ID, LOCALIZATION_DOMAIN);
    const auto complete = localization.GetLocalizedString(COMPLETE_RESOURCE_ID, LOCALIZATION_DOMAIN);
    const auto sentence = localization.GetLocalizedString(SENTENCE_RESOURCE_ID, LOCALIZATION_DOMAIN);

    const char* activeLocale = std::setlocale(MESSAGE_LOCALE_CATEGORY, nullptr);
#if defined(_WIN32)
    const char* language = Dali::Ui::Samples::GetMessageLocale();
#else
    const char* language = std::getenv("LANGUAGE");
#endif

    DALI_LOG_RELEASE_INFO("[LOCALIZATION LOOKUP] event=%s selected=%s activeLocale=\"%s\" "
                          "LANGUAGE=\"%s\" IDS_WIFI=\"%s\" IDS_CONNECT=\"%s\" "
                          "IDS_COMPLETE=\"%s\" IDS_WIFI_CONNECT_COMPLETE=\"%s\"\n",
                          event,
                          mSelectedLocale.c_str(),
                          activeLocale ? activeLocale : "(unknown)",
                          language ? language : "(unset)",
                          wifi.CStr(),
                          connect.CStr(),
                          complete.CStr(),
                          sentence.CStr());
  }

  const char* GetButtonId(Dali::Ui::View view) const
  {
    if(view == mFragmentButton)
    {
      return "1";
    }
    if(view == mCompleteFirstButton)
    {
      return "2";
    }
    if(view == mConnectFirstButton)
    {
      return "3";
    }
    if(view == mSentenceButton)
    {
      return "4";
    }
    return "non-test";
  }

  void SetSampleLocale(const char* catalogLocale,
                       const char* posixLocale,
                       const char* windowsLocale)
  {
#if defined(_WIN32)
    (void)posixLocale;
    const bool  catalogLocaleSet   = Dali::Ui::Samples::SetMessageLocale(catalogLocale);
    const char* localeResult       = std::setlocale(LC_ALL, windowsLocale);
    const bool  requestedLocaleSet = localeResult != nullptr;
#else
    (void)windowsLocale;
    const bool  catalogLocaleSet   = ::setenv("LANGUAGE", catalogLocale, 1) == 0;
    const char* localeResult       = std::setlocale(LC_MESSAGES, posixLocale);
    const bool  requestedLocaleSet = localeResult != nullptr;
    if(!localeResult)
    {
      // Keep the current valid POSIX locale while LANGUAGE selects this
      // sample's catalog. Some target images do not install every *.UTF-8
      // locale even though the corresponding gettext catalog is available.
      localeResult = std::setlocale(LC_MESSAGES, "");
    }
#endif

    mSelectedLocale             = catalogLocale;
    mPosixLocaleSetSucceeded    = requestedLocaleSet;

    Dali::Ui::UiLocalizationManager::Get().RefreshBindings();
    const auto localizedWifi = Dali::Ui::UiLocalizationManager::Get().GetLocalizedString(
      WIFI_RESOURCE_ID,
      LOCALIZATION_DOMAIN);
    mLocaleSetSucceeded = catalogLocaleSet && std::string(localizedWifi.CStr()) != WIFI_RESOURCE_ID;

    DALI_LOG_RELEASE_INFO("[LOCALE SWITCH] catalog=%s locale=%s result=%s\n",
                          catalogLocale,
                          localeResult ? localeResult : "(not available)",
                          mLocaleSetSucceeded ? "success" : "failed");

    LogLocalizationState("locale-switch");
    UpdateStatus();
  }

  void SetKeyFocus(Dali::Ui::View view)
  {
    if(!view)
    {
      return;
    }

    mCurrentView = view;
    const bool focusSet = Dali::Ui::FocusManager::Get().SetCurrentFocusView(view);
    UpdateStatus();

    DALI_LOG_RELEASE_INFO("[KEY FOCUS] button=%s result=%s\n",
                          GetButtonId(view),
                          focusSet ? "set" : "unchanged-or-rejected");
  }

  void RefreshLocalization()
  {
    Dali::Ui::UiLocalizationManager::Get().RefreshBindings();
    LogLocalizationState("manual-refresh");
    UpdateStatus();
  }

  void UpdateStatus()
  {
    if(!mStatus)
    {
      return;
    }

    const auto wifiConnectComplete = ComposeFragmentName(
      WIFI_RESOURCE_ID,
      CONNECT_RESOURCE_ID,
      COMPLETE_RESOURCE_ID);
    const auto completeConnectWifi = ComposeFragmentName(
      COMPLETE_RESOURCE_ID,
      CONNECT_RESOURCE_ID,
      WIFI_RESOURCE_ID);
    const auto connectWifiComplete = ComposeFragmentName(
      CONNECT_RESOURCE_ID,
      WIFI_RESOURCE_ID,
      COMPLETE_RESOURCE_ID);
    const auto sentence = Dali::Ui::UiLocalizationManager::Get().GetLocalizedString(
      SENTENCE_RESOURCE_ID,
      LOCALIZATION_DOMAIN);

    const char* locale = std::setlocale(MESSAGE_LOCALE_CATEGORY, nullptr);
    if(!locale)
    {
      locale = "(unknown)";
    }

    std::string status = "Current locale: ";
    status += locale;
    status += "\nApp locale selection: ";
    status += mSelectedLocale;
    if(!mLocaleSetSucceeded)
    {
      status += " (catalog lookup failed)";
    }
    else if(!mPosixLocaleSetSucceeded)
    {
      status += " (catalog override; POSIX locale unavailable)";
    }
    status += "\n1  WIFI + CONNECT + COMPLETE: ";
    status += wifiConnectComplete.CStr();
    status += "\n2  COMPLETE + CONNECT + WIFI: ";
    status += completeConnectWifi.CStr();
    status += "\n3  CONNECT + WIFI + COMPLETE: ";
    status += connectWifiComplete.CStr();
    status += "\n4  Complete sentence: ";
    status += sentence.CStr();
    mStatus.SetText(status.c_str());
  }

  void OnKeyEvent(Dali::Window, Dali::KeyEvent event)
  {
    if(event.GetState() != Dali::KeyEvent::UP)
    {
      return;
    }

    if(Dali::IsKey(event, Dali::DALI_KEY_ESCAPE) || Dali::IsKey(event, Dali::DALI_KEY_BACK))
    {
      mApplication.Quit();
    }
    else if(event.GetKeyName() == "1")
    {
      SetKeyFocus(mFragmentButton);
    }
    else if(event.GetKeyName() == "2")
    {
      SetKeyFocus(mCompleteFirstButton);
    }
    else if(event.GetKeyName() == "3")
    {
      SetKeyFocus(mConnectFirstButton);
    }
    else if(event.GetKeyName() == "4")
    {
      SetKeyFocus(mSentenceButton);
    }
    else if(event.GetKeyName() == "5")
    {
      SetSampleLocale("en_US", "en_US.UTF-8", "en-US");
    }
    else if(event.GetKeyName() == "6")
    {
      SetSampleLocale("ko_KR", "ko_KR.UTF-8", "ko-KR");
    }
    else if(event.GetKeyName() == "r" || event.GetKeyName() == "R")
    {
      RefreshLocalization();
    }
  }

private:
  Dali::Application&   mApplication;
  Dali::Ui::TextButton mEnglishLocaleButton;
  Dali::Ui::TextButton mKoreanLocaleButton;
  Dali::Ui::TextButton mFragmentButton;
  Dali::Ui::TextButton mCompleteFirstButton;
  Dali::Ui::TextButton mConnectFirstButton;
  Dali::Ui::TextButton mSentenceButton;
  Dali::Ui::Label      mStatus;
  Dali::Ui::View       mCurrentView;
  std::string          mSelectedLocale{"platform/system"};
  bool                 mLocaleSetSucceeded{true};
  bool                 mPosixLocaleSetSucceeded{true};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  auto                                 application = Dali::Application::New(&argc, &argv);
  AccessibilityLocalizedNameController controller(application);
  application.MainLoop();
  return 0;
}
