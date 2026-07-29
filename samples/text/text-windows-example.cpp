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

#include <dali-ui-foundation/dali-ui-foundation.h>
#include "text-localization-locale.h"

#include <clocale>
#include <cstdio>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{

constexpr float LABEL_FONT_SIZE  = 25.0f;
constexpr float STACK_SPACING    = 10.0f;
constexpr float STACK_PADDING    = 24.0f;

constexpr uint32_t COLOR_BACKGROUND = 0xF1F5F9;
constexpr uint32_t COLOR_TEXT       = 0x172033;
constexpr uint32_t COLOR_MUTED_TEXT = 0x4B5563;

constexpr const char* SAMPLE_TEXT             = "Hello World 안녕하세요 다국어 텍스트";
constexpr const char* FONT_FAMILY_LIGHT       = "SamsungOneUI_300";
constexpr const char* FONT_FAMILY_REGULAR     = "SamsungOneUI_400";
constexpr const char* FONT_FAMILY_MEDIUM      = "SamsungOneUI_500";
constexpr const char* FONT_FAMILY_SEMIBOLD    = "SamsungOneUI_600";
constexpr const char* FONT_FAMILY_BOLD        = "SamsungOneUI_700";
constexpr const char* LOCALIZATION_DOMAIN     = "text-windows-example";
constexpr const char* LOCALIZATION_LOCALE_DIR = RESOURCES_DIR "locale/windows";

/*
 * Windows fontconfig layout used by windows-dependencies
 * ------------------------------------------------------
 * The installed runtime environment configures fontconfig with:
 *   FONTCONFIG_PATH=<DALI_WINDOWS_SDK_ROOT>/share/dali
 *   FONTCONFIG_FILE=<DALI_WINDOWS_SDK_ROOT>/share/dali/fonts.conf
 * The installed set-dali-runtime-env.ps1 script sets both values.
 *
 * An application or test can use an isolated font root by setting both
 * environment variables before DALi text is created, for example:
 *   FONTCONFIG_PATH=D:/business-fonts
 *   FONTCONFIG_FILE=D:/business-fonts/fonts.conf
 *
 * The installed fonts.conf keeps C:/Windows/Fonts and also resolves these
 * paths relative to the directory that contains fonts.conf:
 *   <FONTCONFIG_PATH>/fonts/
 *   <FONTCONFIG_PATH>/fontconfig/conf.d/
 *
 * Copy application/test .ttf or .otf files into the first directory. Copy
 * configuration snippets such as 99-tizen.conf into the conf.d directory.
 * Included configuration filenames must start with an ASCII digit and end in
 * ".conf"; they are processed in sorted filename order by fontconfig.
 *
 * A snippet must be a valid fontconfig XML document with a <fontconfig> root.
 * The root fonts.conf already scans the prefix-local fonts directory, so an
 * alias-only 99-tizen.conf needs no <dir>. If another snippet adds a relative
 * font directory, remember that its base is that snippet's own directory; the
 * shared font directory is "../../fonts" from fontconfig/conf.d.
 *
 * A font file's filename is not its font family name. The aliases in
 * 99-tizen.conf must map SamsungOneUI_300 through SamsungOneUI_700 to family
 * names actually declared by the copied font files. Restart the application
 * after changing fonts or configuration so DALi/fontconfig rebuilds its font
 * state. Keep the vcpkg x64-windows/bin directory on PATH for fontconfig and
 * its runtime dependencies.
 */

Label CreateLabel(const char* text, const char* fontFamily, float fontSize)
{
  Label label = Label::New(text);
  label.SetFontFamily(fontFamily);
  label.SetFontSize(fontSize);
  label.SetTextColor(UiColor(COLOR_TEXT));
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetMultiLine(true);
  label.SetLayoutDirectionMode(Text::LayoutDirectionMode::CONTENTS);
  return label;
}

void ApplyTextGradient(Label label)
{
  Gradient::Linear gradient(Vector2(-0.5f, 0.0f), Vector2(0.5f, 0.0f));
  gradient.SetUnits(Gradient::Units::OBJECT_BOUNDING_BOX);
  gradient.SetSpreadMethod(Gradient::SpreadMethod::REFLECT);
  gradient.SetStopNodes({
    Gradient::StopNode(0.0f, UiColor(0.18f, 0.35f, 0.90f, 1.0f)),
    Gradient::StopNode(0.5f, UiColor(0.82f, 0.20f, 0.58f, 1.0f)),
    Gradient::StopNode(1.0f, UiColor(0.10f, 0.65f, 0.60f, 1.0f)),
  });

  label.SetTextGradientBoundsMode(Text::GradientBoundsMode::CONTENT_BOUND);
  label.SetTextGradient(gradient);
}

Text::StyledText BuildLocalizedStyledText(const Dali::String& markup)
{
  Text::StyledTextBuilder builder = Text::StyledTextBuilder::FromMarkup(markup);

  const uint32_t annotationCount = builder.GetAnnotationCount();
  for(uint32_t index = 0u; index < annotationCount; ++index)
  {
    const Text::AnnotationSpan annotation = builder.GetAnnotationAt(index);
    if(annotation.GetKey() == "style" && annotation.GetValue() == "muted")
    {
      const uint32_t startIndex = builder.GetAnnotationStartIndexAt(index);
      const uint32_t endIndex   = builder.GetAnnotationEndIndexAt(index);

      Text::FontAttributes attributes;
      attributes.SetFamily(FONT_FAMILY_REGULAR);
      builder.SetSpan(Text::FontSpan::New(attributes), startIndex, endIndex);
      builder.SetSpan(Text::ForegroundColorSpan::New(UiColor(COLOR_MUTED_TEXT)), startIndex, endIndex);
    }
  }

  return builder.Build();
}

} // anonymous namespace

class TextWindowsController : public ConnectionTracker
{
public:
  explicit TextWindowsController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextWindowsController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(COLOR_BACKGROUND));

    UiLocalizationManager manager = UiLocalizationManager::Get();
    manager.RegisterDomain(LOCALIZATION_DOMAIN, LOCALIZATION_LOCALE_DIR);

    StackLayout contents = StackLayout::New(StackOrientation::VERTICAL);
    contents.SetSpacing(STACK_SPACING);
    contents.SetPadding(Extents(static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING), static_cast<int16_t>(STACK_PADDING)));
    contents.SetRequestedWidth(MATCH_PARENT);
    contents.SetRequestedHeight(MATCH_PARENT);

    contents.Add(CreateLabel(SAMPLE_TEXT, FONT_FAMILY_LIGHT, LABEL_FONT_SIZE));
    contents.Add(CreateLabel(SAMPLE_TEXT, FONT_FAMILY_REGULAR, LABEL_FONT_SIZE));
    contents.Add(CreateLabel(SAMPLE_TEXT, FONT_FAMILY_MEDIUM, LABEL_FONT_SIZE));
    contents.Add(CreateLabel(SAMPLE_TEXT, FONT_FAMILY_SEMIBOLD, LABEL_FONT_SIZE));
    contents.Add(CreateLabel(SAMPLE_TEXT, FONT_FAMILY_BOLD, LABEL_FONT_SIZE));

    mLocalizedLabel = CreateLabel("", FONT_FAMILY_SEMIBOLD, LABEL_FONT_SIZE);
    ApplyTextGradient(mLocalizedLabel);
    contents.Add(mLocalizedLabel);

    manager.SetBindingResource(
      mLocalizedLabel,
      "StyledText",
      "IDS_TEXT_WINDOWS_LOCALIZED_SENTENCE",
      LOCALIZATION_DOMAIN,
      LocalizedStringCallback::New(this, &TextWindowsController::ApplyLocalizedText));

    window.Add(contents);
    SetLocale("ko_KR", "ko_KR.UTF-8", "ko-KR");

    window.KeyEventSignal().Connect(this, &TextWindowsController::OnKeyEvent);
  }

  void ApplyLocalizedText(BaseHandle target, const Dali::String& markup)
  {
    Label label = Label::DownCast(target);
    if(label)
    {
      label.SetStyledText(BuildLocalizedStyledText(markup));
    }
  }

  void SetLocale(const char* catalogLocale, const char* posixLocale, const char* windowsLocale)
  {
    mCatalogLocale = catalogLocale;

#if defined(_WIN32)
    /*
     * MSVC has no LC_MESSAGES category. In the gettext 0.19 build supplied by
     * windows-dependencies, dgettext() selects catalogs from LANGUAGE, LC_ALL,
     * LC_MESSAGES, or LANG; changing only the MSVC CRT locale is insufficient.
     *
     * Set LANGUAGE in both the Win32 process environment and the executable's
     * CRT environment because libintl is a DLL. setlocale() separately receives
     * the Windows BCP-47 locale name. A Windows appfw backend should
     * establish equivalent process state before emitting LocaleChangedSignal.
     *
     * LANGUAGE is updated through the shared helper. LC_ALL is intentionally
     * left untouched in the process environment because it also affects
     * numeric, monetary, and date formatting.
     */
    (void)posixLocale;
    const bool messageLocaleSet = Samples::SetMessageLocale(catalogLocale);
    const char* localeResult    = setlocale(LC_ALL, windowsLocale);
    if(!messageLocaleSet)
    {
      std::printf("Failed to set the Windows gettext locale environment\n");
    }
#else
    (void)windowsLocale;
    const bool messageLocaleSet = setenv("LANGUAGE", catalogLocale, 1) == 0;
    const char* localeResult    = setlocale(LC_MESSAGES, posixLocale);
    (void)messageLocaleSet;
#endif

    if(!localeResult)
    {
      std::printf("setlocale failed for catalog locale \"%s\"\n", catalogLocale);
    }

    UiLocalizationManager::Get().RefreshBindings();
#if defined(_WIN32)
    std::printf("[text-windows-example] catalog=%s, CRT LC_ALL=%s\n",
                mCatalogLocale.c_str(),
                localeResult ? localeResult : "(not available)");
#else
    std::printf("[text-windows-example] catalog=%s, LC_MESSAGES=%s\n",
                mCatalogLocale.c_str(),
                localeResult ? localeResult : "(not available)");
#endif
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
      SetLocale("ar_AE", "ar_AE.UTF-8", "ar-AE");
    }
  }

private:
  Application& mApplication;
  Label        mLocalizedLabel;
  std::string  mCatalogLocale;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextWindowsController controller(application);
  application.MainLoop();

  return 0;
}
