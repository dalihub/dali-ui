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

#include <cstdio>
#include <string>

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING   = 6.0f;
constexpr float STACK_PADDING   = 12.0f;
constexpr float BUTTON_HEIGHT   = 36.0f;
constexpr float INPUT_EDITOR_HEIGHT = 200.0f;
constexpr float BUTTON_SPACING  = 4.0f;
constexpr float COLOR_PREVIEW_SIZE = 24.0f;

constexpr uint32_t COLOR_DARK_TEXT    = 0x222222;
constexpr uint32_t COLOR_DARK_GRAY    = 0x404040;
constexpr uint32_t COLOR_LIGHT_BLUE   = 0xADD8E6;
constexpr uint32_t COLOR_MAGENTA      = 0xFF00FF;
constexpr uint32_t COLOR_BLUE         = 0x00AAFF;
constexpr uint32_t COLOR_GREEN        = 0x00CC66;
constexpr uint32_t COLOR_BLACK        = 0x000000;
constexpr uint32_t COLOR_WHITE        = 0xFFFFFF;
constexpr uint32_t COLOR_BUTTON_BG    = 0xE0E0E0;

Label CreateButton(const char* text, uint32_t bgColor)
{
  Label button = Label::New(text);
  button.SetFontSize(11.0f);
  button.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  button.SetVerticalTextAlignment(Text::Alignment::CENTER);
  button.SetBackgroundColor(UiColor(bgColor));
  button.SetRequestedWidth(0.0f);
  button.SetRequestedHeight(BUTTON_HEIGHT);
  button.SetPadding(Extents(4, 4, 4, 4));
  button.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
  return button;
}

View CreateButtonRow(std::initializer_list<Label> buttons)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(WRAP_CONTENT);
  row.SetSpacing(BUTTON_SPACING);

  for(auto& btn : buttons)
  {
    row.Add(btn);
  }
  return row;
}

Label CreateSectionLabel(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(12.0f);
  label.SetTextColor(UiColor(COLOR_DARK_GRAY));
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetRequestedHeight(WRAP_CONTENT);
  label.SetPadding(Extents(0, 0, 4, 4));
  return label;
}

Label CreateStateValueLabel(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(12.0f);
  label.SetTextColor(UiColor(COLOR_DARK_TEXT));
  label.SetRequestedWidth(WRAP_CONTENT);
  label.SetRequestedHeight(COLOR_PREVIEW_SIZE);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}

View CreateStateRow(const char* title, Label valueLabel)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(WRAP_CONTENT);
  row.SetSpacing(8.0f);
  row.SetPadding(Extents(0, 0, 2, 2));

  Label titleLabel = Label::New(title);
  titleLabel.SetFontSize(12.0f);
  titleLabel.SetTextColor(UiColor(COLOR_DARK_GRAY));
  titleLabel.SetRequestedWidth(WRAP_CONTENT);
  titleLabel.SetRequestedHeight(COLOR_PREVIEW_SIZE);
  titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
  row.Add(titleLabel);

  row.Add(valueLabel);
  return row;
}

View CreateColorStateRow(const char* title, View colorView)
{
  StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
  row.SetRequestedWidth(MATCH_PARENT);
  row.SetRequestedHeight(WRAP_CONTENT);
  row.SetSpacing(8.0f);
  row.SetPadding(Extents(0, 0, 2, 2));

  Label titleLabel = Label::New(title);
  titleLabel.SetFontSize(12.0f);
  titleLabel.SetTextColor(UiColor(COLOR_DARK_GRAY));
  titleLabel.SetRequestedWidth(WRAP_CONTENT);
  titleLabel.SetRequestedHeight(COLOR_PREVIEW_SIZE);
  titleLabel.SetVerticalTextAlignment(Text::Alignment::CENTER);
  row.Add(titleLabel);

  row.Add(colorView);
  return row;
}

const char* ToString(Text::FontWeight weight)
{
  switch(weight)
  {
    case Text::FontWeight::THIN:
      return "Thin";
    case Text::FontWeight::EXTRA_LIGHT:
      return "Extra Light";
    case Text::FontWeight::LIGHT:
      return "Light";
    case Text::FontWeight::DEMI_LIGHT:
      return "Demi Light";
    case Text::FontWeight::BOOK:
      return "Book";
    case Text::FontWeight::NORMAL:
      return "Normal";
    case Text::FontWeight::MEDIUM:
      return "Medium";
    case Text::FontWeight::SEMI_BOLD:
      return "Semi Bold";
    case Text::FontWeight::BOLD:
      return "Bold";
    case Text::FontWeight::EXTRA_BOLD:
      return "Extra Bold";
    case Text::FontWeight::BLACK:
      return "Black";
    default:
      return "Unknown";
  }
}

const char* ToString(Text::FontWidth width)
{
  switch(width)
  {
    case Text::FontWidth::ULTRA_CONDENSED:
      return "Ultra Condensed";
    case Text::FontWidth::EXTRA_CONDENSED:
      return "Extra Condensed";
    case Text::FontWidth::CONDENSED:
      return "Condensed";
    case Text::FontWidth::SEMI_CONDENSED:
      return "Semi Condensed";
    case Text::FontWidth::NORMAL:
      return "Normal";
    case Text::FontWidth::SEMI_EXPANDED:
      return "Semi Expanded";
    case Text::FontWidth::EXPANDED:
      return "Expanded";
    case Text::FontWidth::EXTRA_EXPANDED:
      return "Extra Expanded";
    case Text::FontWidth::ULTRA_EXPANDED:
      return "Ultra Expanded";
    default:
      return "Unknown";
  }
}

const char* ToString(Text::FontSlant slant)
{
  switch(slant)
  {
    case Text::FontSlant::NORMAL:
      return "Normal";
    case Text::FontSlant::ITALIC:
      return "Italic";
    case Text::FontSlant::OBLIQUE:
      return "Oblique";
    default:
      return "Unknown";
  }
}

std::string FormatFontSize(float fontSize)
{
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%.0f", fontSize);
  return std::string(buffer);
}

} // namespace

class TypingStyleExample : public ConnectionTracker
{
public:
  explicit TypingStyleExample(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TypingStyleExample::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xF5F5F5));

    // Main container
    StackLayout mainContainer = StackLayout::New(StackOrientation::VERTICAL);
    mainContainer.SetRequestedWidth(MATCH_PARENT);
    mainContainer.SetRequestedHeight(MATCH_PARENT);
    mainContainer.SetSpacing(STACK_SPACING);
    mainContainer.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    // Title
    Label titleLabel = Label::New("InputEditor Typing Style Example");
    titleLabel.SetFontSize(18.0f);
    titleLabel.SetTextColor(UiColor(COLOR_DARK_TEXT));
    titleLabel.SetRequestedWidth(MATCH_PARENT);
    titleLabel.SetRequestedHeight(WRAP_CONTENT);
    mainContainer.Add(titleLabel);

    // Description
    Label descLabel = Label::New("Move cursor or select text, then press a style button.");
    descLabel.SetFontSize(12.0f);
    descLabel.SetTextColor(UiColor(COLOR_DARK_GRAY));
    descLabel.SetRequestedWidth(MATCH_PARENT);
    descLabel.SetRequestedHeight(WRAP_CONTENT);
    mainContainer.Add(descLabel);

    // InputEditor with markup
    mInputEditor = InputEditor::New();
    mInputEditor.SetMarkupEnabled(true);
    mInputEditor.SetText("<font family='DejaVu Sans'><color value='#FF00FF'>Magenta</color> <color value='#00AAFF'>Blue</color> <color value='#00CC66'>Green</color> text</font>");
    mInputEditor.SetFontSize(20.0f);
    mInputEditor.SetTextColor(UiColor(COLOR_DARK_TEXT));
    mInputEditor.SetCursorWidth(2);
    mInputEditor.SetCursorColor(UiColor(COLOR_DARK_TEXT));
    mInputEditor.SetSelectionColor(UiColor(COLOR_LIGHT_BLUE));
    mInputEditor.SetTextHandleEnabled(true);
    mInputEditor.SetTextHandleColor(UiColor(0x000080));
    mInputEditor.SetRequestedWidth(MATCH_PARENT);
    mInputEditor.SetRequestedHeight(INPUT_EDITOR_HEIGHT);
    mInputEditor.SetBackgroundColor(UiColor(COLOR_WHITE));
    mInputEditor.SetPadding(Extents(12, 12, 12, 12));
    mInputEditor.SetMaximumLength(500);
    mInputEditor.SetLineHeight(1.5f);
    mInputEditor.SetFocusable(true);
    mainContainer.Add(mInputEditor);

    // Connect TypingStyleChangedSignal
    mInputEditor.TypingStyleChangedSignal().Connect(this, &TypingStyleExample::OnTypingStyleChanged);

    // Current Typing Style section
    mainContainer.Add(CreateSectionLabel("Current Typing Style:"));

    // Color preview row
    mTypingColorView = View::New();
    mTypingColorView.SetRequestedWidth(COLOR_PREVIEW_SIZE);
    mTypingColorView.SetRequestedHeight(COLOR_PREVIEW_SIZE);
    mTypingColorView.SetBackgroundColor(mInputEditor.GetTypingTextColor().GetRgba());
    mainContainer.Add(CreateColorStateRow("Color:", mTypingColorView));

    // Font Family row
    mTypingFontFamilyLabel = CreateStateValueLabel(mInputEditor.GetTypingFontFamily().CStr());
    mainContainer.Add(CreateStateRow("Font Family:", mTypingFontFamilyLabel));

    // Font Size row
    mTypingFontSizeLabel = CreateStateValueLabel(FormatFontSize(mInputEditor.GetTypingFontSize()).c_str());
    mainContainer.Add(CreateStateRow("Font Size:", mTypingFontSizeLabel));

    // Font Weight row
    mTypingFontWeightLabel = CreateStateValueLabel(ToString(mInputEditor.GetTypingFontWeight()));
    mainContainer.Add(CreateStateRow("Font Weight:", mTypingFontWeightLabel));

    // Font Width row
    mTypingFontWidthLabel = CreateStateValueLabel(ToString(mInputEditor.GetTypingFontWidth()));
    mainContainer.Add(CreateStateRow("Font Width:", mTypingFontWidthLabel));

    // Font Slant row
    mTypingFontSlantLabel = CreateStateValueLabel(ToString(mInputEditor.GetTypingFontSlant()));
    mainContainer.Add(CreateStateRow("Font Slant:", mTypingFontSlantLabel));

    // Scrollable button area
    StackLayout buttonContainer = StackLayout::New(StackOrientation::VERTICAL);
    buttonContainer.SetRequestedWidth(MATCH_PARENT);
    buttonContainer.SetRequestedHeight(WRAP_CONTENT);
    buttonContainer.SetSpacing(STACK_SPACING);
    buttonContainer.SetBackgroundColor(UiColor(0xEFEFEF));

    // Color buttons section
    buttonContainer.Add(CreateSectionLabel("Typing Text Color:"));
    Label magentaBtn = CreateButton("Magenta", COLOR_BUTTON_BG);
    Label blueBtn = CreateButton("Blue", COLOR_BUTTON_BG);
    Label greenBtn = CreateButton("Green", COLOR_BUTTON_BG);
    Label blackBtn = CreateButton("Black", COLOR_BUTTON_BG);

    magentaBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnMagentaButtonTouched);
    blueBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnBlueButtonTouched);
    greenBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnGreenButtonTouched);
    blackBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnBlackButtonTouched);

    buttonContainer.Add(CreateButtonRow({magentaBtn, blueBtn, greenBtn, blackBtn}));

    // Font family buttons section
    buttonContainer.Add(CreateSectionLabel("Typing Font Family:"));
    Label sansBtn = CreateButton("Sans", COLOR_BUTTON_BG);
    Label serifBtn = CreateButton("Serif", COLOR_BUTTON_BG);
    Label monoBtn = CreateButton("Mono", COLOR_BUTTON_BG);

    sansBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnSansButtonTouched);
    serifBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnSerifButtonTouched);
    monoBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnMonoButtonTouched);

    buttonContainer.Add(CreateButtonRow({sansBtn, serifBtn, monoBtn}));

    // Font size buttons section
    buttonContainer.Add(CreateSectionLabel("Typing Font Size:"));
    Label smallBtn = CreateButton("Small", COLOR_BUTTON_BG);
    Label mediumBtn = CreateButton("Medium", COLOR_BUTTON_BG);
    Label largeBtn = CreateButton("Large", COLOR_BUTTON_BG);

    smallBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnSmallFontSizeButtonTouched);
    mediumBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnMediumFontSizeButtonTouched);
    largeBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnLargeFontSizeButtonTouched);

    buttonContainer.Add(CreateButtonRow({smallBtn, mediumBtn, largeBtn}));

    // Font weight buttons section
    buttonContainer.Add(CreateSectionLabel("Typing Font Weight:"));
    Label normalWeightBtn = CreateButton("Normal", COLOR_BUTTON_BG);
    Label boldBtn = CreateButton("Bold", COLOR_BUTTON_BG);

    normalWeightBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnNormalWeightButtonTouched);
    boldBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnBoldButtonTouched);

    buttonContainer.Add(CreateButtonRow({normalWeightBtn, boldBtn}));

    // Font width buttons section
    buttonContainer.Add(CreateSectionLabel("Typing Font Width:"));
    Label normalWidthBtn = CreateButton("Normal", COLOR_BUTTON_BG);
    Label condensedBtn = CreateButton("Condensed", COLOR_BUTTON_BG);

    normalWidthBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnNormalWidthButtonTouched);
    condensedBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnCondensedButtonTouched);

    buttonContainer.Add(CreateButtonRow({normalWidthBtn, condensedBtn}));

    // Font slant buttons section
    buttonContainer.Add(CreateSectionLabel("Typing Font Slant:"));
    Label normalSlantBtn = CreateButton("Normal", COLOR_BUTTON_BG);
    Label italicBtn = CreateButton("Italic", COLOR_BUTTON_BG);

    normalSlantBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnNormalSlantButtonTouched);
    italicBtn.TouchEventSignal().Connect(this, &TypingStyleExample::OnItalicButtonTouched);

    buttonContainer.Add(CreateButtonRow({normalSlantBtn, italicBtn}));

    // ScrollView for style buttons
    ScrollView scrollView = ScrollView::New();
    scrollView.SetScrollDirection(ScrollDirection::Vertical);
    scrollView.SetRequestedWidth(MATCH_PARENT);
    scrollView.SetRequestedHeight(0.0f);
    scrollView.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f).SetAlignment(LayoutAlignment::FILL));
    scrollView.SetContent(buttonContainer);

    mainContainer.Add(scrollView);

    window.Add(mainContainer);

    window.KeyEventSignal().Connect(this, &TypingStyleExample::OnKeyEvent);
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
  }

  void OnTypingStyleChanged(View view, Text::TypingStyle::Mask mask)
  {
    (void)view; // Unused parameter

    if(mask & Text::TypingStyle::TEXT_COLOR)
    {
      UpdateTypingColorView();
    }
    if(mask & Text::TypingStyle::FONT_FAMILY)
    {
      UpdateTypingFontFamilyLabel();
    }
    if(mask & Text::TypingStyle::FONT_SIZE)
    {
      UpdateTypingFontSizeLabel();
    }
    if(mask & Text::TypingStyle::FONT_WEIGHT)
    {
      UpdateTypingFontWeightLabel();
    }
    if(mask & Text::TypingStyle::FONT_WIDTH)
    {
      UpdateTypingFontWidthLabel();
    }
    if(mask & Text::TypingStyle::FONT_SLANT)
    {
      UpdateTypingFontSlantLabel();
    }
  }

  void UpdateTypingColorView()
  {
    mTypingColorView.SetBackgroundColor(mInputEditor.GetTypingTextColor().GetRgba());
  }

  void UpdateTypingFontFamilyLabel()
  {
    mTypingFontFamilyLabel.SetText(mInputEditor.GetTypingFontFamily());
  }

  void UpdateTypingFontSizeLabel()
  {
    mTypingFontSizeLabel.SetText(FormatFontSize(mInputEditor.GetTypingFontSize()).c_str());
  }

  void UpdateTypingFontWeightLabel()
  {
    mTypingFontWeightLabel.SetText(ToString(mInputEditor.GetTypingFontWeight()));
  }

  void UpdateTypingFontWidthLabel()
  {
    mTypingFontWidthLabel.SetText(ToString(mInputEditor.GetTypingFontWidth()));
  }

  void UpdateTypingFontSlantLabel()
  {
    mTypingFontSlantLabel.SetText(ToString(mInputEditor.GetTypingFontSlant()));
  }

  // Color button handlers
  bool OnMagentaButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingTextColor(UiColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f)));
      UpdateTypingColorView();
    }
    return true;
  }

  bool OnBlueButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingTextColor(UiColor(Vector4(0.0f, 0.67f, 1.0f, 1.0f)));
      UpdateTypingColorView();
    }
    return true;
  }

  bool OnGreenButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingTextColor(UiColor(Vector4(0.0f, 0.8f, 0.4f, 1.0f)));
      UpdateTypingColorView();
    }
    return true;
  }

  bool OnBlackButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingTextColor(UiColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)));
      UpdateTypingColorView();
    }
    return true;
  }

  // Font family button handlers
  bool OnSansButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontFamily("DejaVu Sans");
      UpdateTypingFontFamilyLabel();
    }
    return true;
  }

  bool OnSerifButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontFamily("DejaVu Serif");
      UpdateTypingFontFamilyLabel();
    }
    return true;
  }

  bool OnMonoButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontFamily("DejaVu Sans Mono");
      UpdateTypingFontFamilyLabel();
    }
    return true;
  }

  // Font size button handlers
  bool OnSmallFontSizeButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontSize(14.0f);
      UpdateTypingFontSizeLabel();
    }
    return true;
  }

  bool OnMediumFontSizeButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontSize(20.0f);
      UpdateTypingFontSizeLabel();
    }
    return true;
  }

  bool OnLargeFontSizeButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontSize(28.0f);
      UpdateTypingFontSizeLabel();
    }
    return true;
  }

  // Font weight button handlers
  bool OnNormalWeightButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontWeight(Text::FontWeight::NORMAL);
      UpdateTypingFontWeightLabel();
    }
    return true;
  }

  bool OnBoldButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontWeight(Text::FontWeight::BOLD);
      UpdateTypingFontWeightLabel();
    }
    return true;
  }

  // Font width button handlers
  bool OnNormalWidthButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontWidth(Text::FontWidth::NORMAL);
      UpdateTypingFontWidthLabel();
    }
    return true;
  }

  bool OnCondensedButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontWidth(Text::FontWidth::CONDENSED);
      UpdateTypingFontWidthLabel();
    }
    return true;
  }

  // Font slant button handlers
  bool OnNormalSlantButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontSlant(Text::FontSlant::NORMAL);
      UpdateTypingFontSlantLabel();
    }
    return true;
  }

  bool OnItalicButtonTouched(Actor actor, TouchEvent touch)
  {
    if(touch.GetState(0) == PointState::UP)
    {
      mInputEditor.SetTypingFontSlant(Text::FontSlant::ITALIC);
      UpdateTypingFontSlantLabel();
    }
    return true;
  }

private:
  Application& mApplication;
  InputEditor mInputEditor;
  View mTypingColorView;
  Label mTypingFontFamilyLabel;
  Label mTypingFontSizeLabel;
  Label mTypingFontWeightLabel;
  Label mTypingFontWidthLabel;
  Label mTypingFontSlantLabel;
};

int main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();
  TypingStyleExample example(application);
  application.MainLoop();
  return 0;
}
