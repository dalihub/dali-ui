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

#include <dali-ui-foundation/dali-ui-foundation.h>

#include <clocale>
#include <cstdio>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
#if defined(_WIN32)
constexpr int MESSAGE_LOCALE_CATEGORY = LC_ALL;
#else
constexpr int MESSAGE_LOCALE_CATEGORY = LC_MESSAGES;
#endif

constexpr float STACK_SPACING              = 12.0f;
constexpr float STACK_PADDING              = 22.0f;
constexpr float HEADER_FONT_SIZE           = 24.0f;
constexpr float HELP_FONT_SIZE             = 14.0f;
constexpr float TITLE_FONT_SIZE            = 22.0f;
constexpr float DESCRIPTION_FONT_SIZE      = 18.0f;
constexpr float STATUS_FONT_SIZE           = 15.0f;
constexpr float GRADIENT_ANIMATION_SECONDS = 3.2f;

constexpr uint32_t COLOR_PAGE_BACKGROUND  = 0xEEF2F7;
constexpr uint32_t COLOR_PANEL_BACKGROUND = 0xFFFFFF;
constexpr uint32_t COLOR_CARD_BACKGROUND  = 0xF8FAFC;
constexpr uint32_t COLOR_DARK_TEXT        = 0x111827;
constexpr uint32_t COLOR_MUTED_TEXT       = 0x4B5563;

void ApplyDescriptionGradient(Label label)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  gradient.SetStartOffset(-0.18f);
  gradient.SetStopNodes({
    Gradient::StopNode(0.0f, UiColor(0.48f, 0.24f, 0.86f, 1.0f)),
    Gradient::StopNode(0.25f, UiColor(0.92f, 0.18f, 0.62f, 1.0f)),
    Gradient::StopNode(0.50f, UiColor(0.34f, 0.46f, 0.95f, 1.0f)),
    Gradient::StopNode(0.75f, UiColor(0.98f, 0.70f, 0.18f, 1.0f)),
    Gradient::StopNode(1.0f, UiColor(0.10f, 0.62f, 0.86f, 1.0f)),
  });

  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradient(gradient);
}

Text::StyledText BuildDescriptionStyledText(const Dali::String& markup)
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(markup);

  // The PO strings mark solid muted ranges as <annotation style='muted'>...</annotation>.
  const uint32_t annotationCount = builder.GetAnnotationCount();
  for(uint32_t annotationIndex = 0u; annotationIndex < annotationCount; ++annotationIndex)
  {
    const Text::AnnotationSpan annotation = builder.GetAnnotationAt(annotationIndex);
    const uint32_t             startIndex = builder.GetAnnotationStartIndexAt(annotationIndex);
    const uint32_t             endIndex   = builder.GetAnnotationEndIndexAt(annotationIndex);

    if(annotation.GetKey() == "style")
    {
      if(annotation.GetValue() == "muted")
      {
        builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(COLOR_MUTED_TEXT)), startIndex, endIndex);
      }
    }
  }

  return builder.Build();
}

class LocalizedCard : public StackLayout
{
public:
  struct Data
  {
    Data(Label title, Label description)
    : titleLabel(title),
      descriptionLabel(description)
    {
    }

    Label titleLabel;
    Label descriptionLabel;
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

  void SetTitle(const Dali::String& text)
  {
    if(Data* data = GetAttachment<Data>(GetDataId()))
    {
      data->titleLabel.SetText(text);
    }
  }

  void SetDescription(const Dali::String& text)
  {
    if(Data* data = GetAttachment<Data>(GetDataId()))
    {
      data->descriptionLabel.SetStyledText(BuildDescriptionStyledText(text));
    }
  }

  void StartDescriptionGradientAnimation(Animation animation)
  {
    if(Data* data = GetAttachment<Data>(GetDataId()))
    {
      ApplyDescriptionGradient(data->descriptionLabel);
      data->descriptionLabel.Animate(animation)
        .TextGradientStartOffset(0.62f, Duration(GRADIENT_ANIMATION_SECONDS));
    }
  }

private:
  explicit LocalizedCard(StackLayout layout)
  : StackLayout(layout)
  {
  }

  void Initialize()
  {
    SetSpacing(8.0f);
    SetRequestedWidth(MATCH_PARENT);
    SetRequestedHeight(WRAP_CONTENT);
    SetPadding(Extents(20, 20, 18, 18));
    SetBackgroundColor(UiColor(COLOR_CARD_BACKGROUND));
    SetCornerRadius(16.0f);
    SetShadow(Shadow(0.0f,
                     Vector2(4.0f, 4.0f),
                     UiColor(0.0f, 0.0f, 0.0f, 0.16f),
                     Vector2::ZERO));

    Label titleLabel = Label::New();
    titleLabel.SetFontSize(TITLE_FONT_SIZE);
    titleLabel.SetTextColor(UiColor(COLOR_DARK_TEXT));
    titleLabel.SetMultiLine(true);
    titleLabel.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);

    Label descriptionLabel = Label::New();
    descriptionLabel.SetFontSize(DESCRIPTION_FONT_SIZE);
    descriptionLabel.SetTextColor(UiColor(COLOR_MUTED_TEXT));
    descriptionLabel.SetMultiLine(true);
    descriptionLabel.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
    ApplyDescriptionGradient(descriptionLabel);

    Add(titleLabel);
    Add(descriptionLabel);

    SetAttachment(GetDataId(), Dali::MakeUnique<Data>(titleLabel, descriptionLabel));
  }

  static AttachmentId GetDataId()
  {
    static AttachmentId id = AttachmentId::Alloc();
    return id;
  }
};

Label CreatePanelLabel(const char* text, float fontSize)
{
  Label label = Label::New(text);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(COLOR_DARK_TEXT));
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  label.SetMultiLine(true);
  label.SetPadding(Extents(10, 10, 10, 10));
  label.SetBackgroundColor(UiColor(COLOR_PANEL_BACKGROUND));
  return label;
}

} // anonymous namespace

class TextGradientLocalizationController : public ConnectionTracker
{
public:
  explicit TextGradientLocalizationController(Application& application)
  : mApplication(application),
    mCurrentLocale("en_US.UTF-8")
  {
    mApplication.InitSignal().Connect(this, &TextGradientLocalizationController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_PAGE_BACKGROUND));

    UiLocalizationManager manager = UiLocalizationManager::Get();
    manager.RegisterDomain(TEXT_DOMAIN_GRADIENT_LOCALIZATION, TEXT_LOCALE_DIR_GRADIENT_LOCALIZATION);

    window.Add(CreateContents());
    StartGradientAnimation();
    SetLocale("en_US.UTF-8");

    window.KeyEventSignal().Connect(this, &TextGradientLocalizationController::OnKeyEvent);
  }

  View CreateContents()
  {
    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetSpacing(STACK_SPACING);
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);
    contents.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    mCard        = CreateCard();
    mStatusLabel = CreateStatusLabel();

    contents.Add(CreateHeaderLabel());
    contents.Add(CreateHelpLabel());
    contents.Add(mCard);
    contents.Add(mStatusLabel);
    return contents;
  }

  Label CreateHeaderLabel()
  {
    return CreatePanelLabel("Gradient Localization Example", HEADER_FONT_SIZE);
  }

  Label CreateHelpLabel()
  {
    return CreatePanelLabel("Keys: 1=en_US, 2=ko_KR, 3=ar_AE, R=refresh, ESC/BACK=quit", HELP_FONT_SIZE);
  }

  Label CreateStatusLabel()
  {
    return CreatePanelLabel("", STATUS_FONT_SIZE);
  }

  LocalizedCard CreateCard()
  {
    LocalizedCard card = LocalizedCard::New();

    UiLocalizationManager::Get().SetBindingResource(
      card,
      "CardTitle",
      "IDS_TEXT_GRADIENT_LOCALIZATION_CARD_TITLE",
      TEXT_DOMAIN_GRADIENT_LOCALIZATION,
      LocalizedStringCallback::New(this, &TextGradientLocalizationController::ApplyCardLocalization));

    return card;
  }

  void ApplyCardLocalization(BaseHandle target, const Dali::String& title)
  {
    LocalizedCard card = LocalizedCard::DownCast(target);
    if(!card)
    {
      return;
    }

    UiLocalizationManager manager = UiLocalizationManager::Get();
    card.SetTitle(title);
    card.SetDescription(manager.GetLocalizedString("IDS_TEXT_GRADIENT_LOCALIZATION_CARD_DESCRIPTION", TEXT_DOMAIN_GRADIENT_LOCALIZATION));
  }

  void StartGradientAnimation()
  {
    mGradientAnimation = Animation::New(GRADIENT_ANIMATION_SECONDS);
    mGradientAnimation.SetLooping(true);
    mGradientAnimation.SetLoopingMode(Animation::AUTO_REVERSE);
    mCard.StartDescriptionGradientAnimation(mGradientAnimation);
    mGradientAnimation.Play();
  }

  void SetLocale(const std::string& locale)
  {
    mCurrentLocale = locale;

    const char* result = setlocale(MESSAGE_LOCALE_CATEGORY, locale.c_str());
    if(!result)
    {
      std::printf("setlocale(LC_MESSAGES, \"%s\") failed\n", locale.c_str());
    }

    UiLocalizationManager::Get().RefreshBindings();
    UpdateStatusLabel();
  }

  void RefreshLocalization()
  {
    UiLocalizationManager::Get().RefreshBindings();
    UpdateStatusLabel();
  }

  void UpdateStatusLabel()
  {
    const char* lcMsg = setlocale(MESSAGE_LOCALE_CATEGORY, nullptr);
    if(!lcMsg)
    {
      lcMsg = "(null)";
    }

    std::string status;
    status += "Locale: " + mCurrentLocale + "\n";
    status += "LC_MESSAGES: " + std::string(lcMsg);
    mStatusLabel.SetText(Dali::String(status.c_str()));
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
    else if(event.GetKeyName() == "r" || event.GetKeyName() == "R")
    {
      RefreshLocalization();
    }
  }

private:
  Application&  mApplication;
  LocalizedCard mCard;
  Label         mStatusLabel;
  Animation     mGradientAnimation;
  std::string   mCurrentLocale;

  static constexpr const char* TEXT_DOMAIN_GRADIENT_LOCALIZATION     = "text-gradient-localization-po";
  static constexpr const char* TEXT_LOCALE_DIR_GRADIENT_LOCALIZATION = RESOURCES_DIR "locale/gradient";
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextGradientLocalizationController controller(application);
  application.MainLoop();

  return 0;
}
