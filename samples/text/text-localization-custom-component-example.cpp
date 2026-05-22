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

// Demonstrates localizing a composite component without exposing its internal views.
// The binding is registered on LocalizedCard itself, and callbacks update the card
// through semantic APIs. Internal views are stored in attachment data.
//
// Key bindings:
//   1 - Set locale to en_US
//   2 - Set locale to ko_KR
//   3 - Set locale to ar_AE
//   B - Toggle bypass on/off
//   D - Toggle default domain
//   R - Manual RefreshBindings()
//   M - Set manual text on default card
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

constexpr float STACK_SPACING    = 10.0f;
constexpr float STACK_PADDING    = 20.0f;
constexpr float TITLE_FONT_SIZE  = 22.0f;
constexpr float LABEL_FONT_SIZE  = 22.0f;
constexpr float DESC_FONT_SIZE   = 16.0f;
constexpr float STATUS_FONT_SIZE = 16.0f;
constexpr float HELP_FONT_SIZE   = 14.0f;

constexpr uint32_t COLOR_DARK_TEXT        = 0x222222;
constexpr uint32_t COLOR_MID_GRAY         = 0x808080;
constexpr uint32_t COLOR_PAGE_BACKGROUND  = 0xEEF2F7;
constexpr uint32_t COLOR_PANEL_BACKGROUND = 0xFFFFFF;
constexpr uint32_t COLOR_SEPARATOR        = 0xCBD5E1;
constexpr uint32_t COLOR_CARD_BACKGROUND  = 0xF7F9FC;
constexpr uint32_t COLOR_CARD_TITLE       = 0x1F2937;
constexpr uint32_t COLOR_CARD_DESCRIPTION = 0x4B5563;
constexpr uint32_t COLOR_CARD_ACTION      = 0x2563EB;

// ---------------------------------------------------------------------------
// Card shadow helper
// ---------------------------------------------------------------------------

Property::Map CreateCardShadowMap()
{
  Property::Map transform;
  transform.Add(Ui::Visual::Transform::Property::OFFSET, Vector2(4.0f, 4.0f));
  transform.Add(Ui::Visual::Transform::Property::OFFSET_POLICY,
                Vector2(static_cast<float>(Ui::Visual::Transform::Policy::ABSOLUTE),
                        static_cast<float>(Ui::Visual::Transform::Policy::ABSOLUTE)));

  Property::Map shadow;
  shadow.Add(Ui::VisualBasePropertyIndex::TYPE, Ui::VisualType::COLOR);
  shadow.Add(Ui::VisualBasePropertyIndex::MIX_COLOR, Vector4(0.0f, 0.0f, 0.0f, 0.18f));
  shadow.Add(Ui::VisualBasePropertyIndex::TRANSFORM, transform);

  return shadow;
}

// ---------------------------------------------------------------------------
// LocalizedCard - lightweight handle derived from StackLayout with attachment
// ---------------------------------------------------------------------------

class LocalizedCard : public StackLayout
{
public:
  struct Data
  {
    Data(Label title, ImageView flag, Label description, Label action)
    : titleLabel(title),
      flagImage(flag),
      descriptionLabel(description),
      actionLabel(action)
    {
    }

    Label     titleLabel;
    ImageView flagImage;
    Label     descriptionLabel;
    Label     actionLabel;
  };

  LocalizedCard() = default;

  static LocalizedCard New()
  {
    LocalizedCard card(StackLayout::New(StackOrientation::VERTICAL));
    card.Initialize();
    return card;
  }

  static LocalizedCard DownCast(BaseHandle handle)
  {
    StackLayout layout = StackLayout::DownCast(handle);
    return layout && layout.GetAttachment<Data>(GetDataId()) ? LocalizedCard(layout) : LocalizedCard();
  }

  void Initialize()
  {
    SetSpacing(6.0f)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetPadding(Extents(18, 18, 16, 16))
      .SetBackgroundColor(UiColor(COLOR_CARD_BACKGROUND))
      .SetCornerRadius(16.0f);

    SetProperty(View::Property::SHADOW, CreateCardShadowMap());

    StackLayout titleRow = StackLayout::New(StackOrientation::HORIZONTAL)
      .SetSpacing(6.0f)
      .SetRequestedHeight(WRAP_CONTENT);

    Label titleLabel = Label::New()
      .SetFontSize(LABEL_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_CARD_TITLE))
      .SetMultiLine(true);

    ImageView flagImage = ImageView::New()
      .SetRequestedHeight(20.0f)
      .SetLayoutParams(StackLayoutParams::New().SetAlignment(LayoutAlignment::CENTER));

    titleRow.Add(titleLabel);
    titleRow.Add(flagImage);

    Label descriptionLabel = Label::New()
      .SetFontSize(DESC_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_CARD_DESCRIPTION))
      .SetMultiLine(true);

    Label actionLabel = Label::New()
      .SetFontSize(DESC_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_CARD_ACTION));

    Add(titleRow);
    Add(descriptionLabel);
    Add(actionLabel);

    SetAttachment(GetDataId(), Dali::MakeUnique<Data>(titleLabel, flagImage, descriptionLabel, actionLabel));
  }

  void SetTitle(const Dali::String& text)
  {
    Data* data = GetAttachment<Data>(GetDataId());
    if(data)
    {
      data->titleLabel.SetText(text);
    }
  }

  void SetDescription(const Dali::String& text)
  {
    Data* data = GetAttachment<Data>(GetDataId());
    if(data)
    {
      data->descriptionLabel.SetText(text);
    }
  }

  void SetActionText(const Dali::String& text)
  {
    Data* data = GetAttachment<Data>(GetDataId());
    if(data)
    {
      data->actionLabel.SetText(text);
    }
  }

  void SetFlagImageUrl(const Dali::String& url)
  {
    Data* data = GetAttachment<Data>(GetDataId());
    if(data)
    {
      data->flagImage.SetResourceUrl(url);
    }
  }

  DALI_UI_CHAIN_STACKLAYOUT_METHODS(LocalizedCard)

private:
  explicit LocalizedCard(StackLayout layout)
  : StackLayout(layout)
  {
  }

  static AttachmentId GetDataId()
  {
    static AttachmentId id = AttachmentId::Alloc();
    return id;
  }
};

} // anonymous namespace

// ---------------------------------------------------------------------------
// Sample controller
// ---------------------------------------------------------------------------

class TextLocalizationCustomComponentController : public ConnectionTracker
{
public:
  explicit TextLocalizationCustomComponentController(Application& application)
  : mApplication(application),
    mCurrentDomain(TEXT_DOMAIN_DEFAULT),
    mCurrentLocale("en_US.UTF-8"),
    mBypassEnabled{false}
  {
    mApplication.InitSignal().Connect(this, &TextLocalizationCustomComponentController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_PAGE_BACKGROUND));

    UiLocalizationManager manager = UiLocalizationManager::Get();

    manager.RegisterDomain(TEXT_DOMAIN_DEFAULT, TEXT_LOCALE_DIR_DEFAULT);
    manager.RegisterDomain(TEXT_DOMAIN_ALT, TEXT_LOCALE_DIR_ALT);
    manager.SetDefaultDomain(TEXT_DOMAIN_DEFAULT);

    window.Add(CreateContents());

    SetLocale("en_US.UTF-8");

    window.KeyEventSignal().Connect(this, &TextLocalizationCustomComponentController::OnKeyEvent);
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
        CreateSectionLabel("Default-domain card:"),
        CreateDefaultCard().As(mDefaultCard),
        CreateSeparator(),
        CreateSectionLabel("Explicit alt-domain card:"),
        CreateAltCard().As(mAltCard),
        CreateSeparator(),
        CreateStatusLabel().As(mStatusLabel),
      });
  }

  Label CreateHeaderLabel()
  {
    return Label::New("Localization Custom Component Example")
      .SetFontSize(TITLE_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_PANEL_BACKGROUND));
  }

  Label CreateHelpLabel()
  {
    return Label::New("Keys: 1=en_US, 2=ko_KR, 3=ar_AE, B=bypass, D=domain, R=refresh, M=manual, ESC/BACK=quit")
      .SetFontSize(HELP_FONT_SIZE)
      .SetTextColor(UiColor(COLOR_DARK_TEXT))
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(WRAP_CONTENT)
      .SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS)
      .SetMultiLine(true)
      .SetPadding(Extents(10, 10, 10, 10))
      .SetBackgroundColor(UiColor(COLOR_PANEL_BACKGROUND));
  }

  View CreateSeparator()
  {
    return View::New()
      .SetBackgroundColor(UiColor(COLOR_SEPARATOR))
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

  // --- Default-domain card ---

  LocalizedCard CreateDefaultCard()
  {
    auto card = LocalizedCard::New();

    // Binding on the card itself, not on internal labels.
    // The callback updates all internal labels via semantic APIs.
    UiLocalizationManager::Get().SetBindingResource(
      card,
      "CardTitle",
      "IDS_CARD_TITLE",
      LocalizedStringCallback::New(this, &TextLocalizationCustomComponentController::ApplyDefaultCardLocalization));

    return card;
  }

  void ApplyDefaultCardLocalization(BaseHandle target, const Dali::String& title)
  {
    LocalizedCard card = LocalizedCard::DownCast(target);
    if(!card)
    {
      return;
    }

    UiLocalizationManager manager = UiLocalizationManager::Get();

    const Dali::String flagImageUrl = manager.GetLocalizedString("IDS_FLAG_IMAGE_URL", TEXT_DOMAIN_DEFAULT);
    const Dali::String path = RESOURCES_DIR + flagImageUrl;
    card.SetFlagImageUrl(path);
    card.SetTitle(title);
    card.SetDescription(manager.GetLocalizedString("IDS_CARD_DESCRIPTION"));
    card.SetActionText(manager.GetLocalizedString("IDS_CARD_ACTION"));
  }

  // --- Explicit alt-domain card ---

  LocalizedCard CreateAltCard()
  {
    auto card = LocalizedCard::New();

    UiLocalizationManager::Get().SetBindingResource(
      card,
      "CardTitle",
      "IDS_ALT_CARD_TITLE",
      TEXT_DOMAIN_ALT,
      LocalizedStringCallback::New(this, &TextLocalizationCustomComponentController::ApplyAltCardLocalization));

    return card;
  }

  void ApplyAltCardLocalization(BaseHandle target, const Dali::String& title)
  {
    LocalizedCard card = LocalizedCard::DownCast(target);
    if(!card)
    {
      return;
    }

    UiLocalizationManager manager = UiLocalizationManager::Get();

    const Dali::String flagImageUrl = manager.GetLocalizedString("IDS_ALT_FLAG_IMAGE_URL", TEXT_DOMAIN_ALT);
    const Dali::String path = RESOURCES_DIR + flagImageUrl;
    card.SetFlagImageUrl(path);
    card.SetTitle(title);
    card.SetDescription(manager.GetLocalizedString("IDS_ALT_CARD_DESCRIPTION", TEXT_DOMAIN_ALT));
    card.SetActionText(manager.GetLocalizedString("IDS_ALT_CARD_ACTION", TEXT_DOMAIN_ALT));
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
      .SetBackgroundColor(UiColor(COLOR_PANEL_BACKGROUND));
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

    UpdateStatusLabel();

    std::printf("LC_MESSAGES=%s\n", setlocale(LC_MESSAGES, nullptr));
  }

  void ToggleBypass()
  {
    mBypassEnabled = !mBypassEnabled;

    // SetBypassEnabled() internally calls RefreshBindings()
    UiLocalizationManager::Get().SetBypassEnabled(mBypassEnabled);

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

    UpdateStatusLabel();
  }

  void RefreshLocalization()
  {
    UiLocalizationManager::Get().RefreshBindings();
    UpdateStatusLabel();
  }

  void SetManualText()
  {
    mDefaultCard.SetTitle("Manual Title");
    mDefaultCard.SetDescription("Manual Description");
    mDefaultCard.SetActionText("Manual Action");
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
  LocalizedCard mDefaultCard;
  LocalizedCard mAltCard;
  Label        mStatusLabel;
  std::string  mCurrentDomain;
  std::string  mCurrentLocale;
  bool         mBypassEnabled{false};

  static constexpr const char* TEXT_DOMAIN_DEFAULT    = "text-localization-po";
  static constexpr const char* TEXT_DOMAIN_ALT         = "text-localization-po-alt";
  static constexpr const char* TEXT_LOCALE_DIR_DEFAULT = RESOURCES_DIR "locale/default";
  static constexpr const char* TEXT_LOCALE_DIR_ALT     = RESOURCES_DIR "locale/alternate";
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextLocalizationCustomComponentController controller(application);
  application.MainLoop();

  return 0;
}
