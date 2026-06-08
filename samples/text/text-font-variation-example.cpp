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

using namespace Dali;
using namespace Dali::Ui;

namespace
{
constexpr float STACK_SPACING = 10.0f;
constexpr float STACK_PADDING = 20.0f;
const char* DEFAULT_FONT_FAMILY = "Ubuntu Mono";
const char* VARIABLE_FONT_FAMILY = "Noto Sans";
const char* SAMPLE_TEXT = "The quick brown fox jumps over the lazy dog";

Label CreateSectionLabel(const char* text)
{
  Label label = Label::New(text);
  label.SetFontSize(16.0f);
  label.SetBackgroundColor(UiColor(0xF0F0F0));
  label.SetPadding(Extents(10, 10, 10, 10));
  return label;
}

Label CreateVariationLabel(const char* variation)
{
  Label label = Label::New(SAMPLE_TEXT);
  label.SetFontFamily(VARIABLE_FONT_FAMILY);
  if(variation)
  {
    label.SetFontVariation(variation);
  }
  label.SetFontSize(20.0f);
  label.SetRequestedWidth(MATCH_PARENT);
  label.SetBackgroundColor(UiColor(0xEFEFEF));
  label.SetPadding(Extents(10, 10, 10, 10));
  return label;
}
} // namespace

class FontVariationController : public ConnectionTracker
{
public:
  explicit FontVariationController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &FontVariationController::OnInit);
  }

private:
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    Label titleLabel = Label::New("Font Variation Example");
    titleLabel.SetFontSize(20.0f);
    root.Add(titleLabel);

    Label instructionLabel = Label::New("Press '1' to clear, '2' to change variation, '3' to set via string");
    instructionLabel.SetFontSize(14.0f);
    instructionLabel.SetBackgroundColor(UiColor(0xE0E0E0));
    instructionLabel.SetPadding(Extents(10, 10, 10, 10));
    root.Add(instructionLabel);

    root.Add(CreateSectionLabel("Weight 200 + Width 62.5"));
    mLabel200 = CreateVariationLabel("wght=200,wdth=62.5");
    root.Add(mLabel200);

    root.Add(CreateSectionLabel("Weight 300 + Width 65"));
    mLabel300 = CreateVariationLabel("wght=300,wdth=65");
    root.Add(mLabel300);

    root.Add(CreateSectionLabel("Weight 400 + Width 70"));
    mLabel400 = CreateVariationLabel("wght=400,wdth=70");
    root.Add(mLabel400);

    root.Add(CreateSectionLabel("Weight 500 + Width 80"));
    mLabel500 = CreateVariationLabel("wght=500,wdth=80");
    root.Add(mLabel500);

    root.Add(CreateSectionLabel("Weight 600 + Width 90"));
    mLabel600 = CreateVariationLabel("wght=600,wdth=90");
    root.Add(mLabel600);

    root.Add(CreateSectionLabel("Weight 700 + Width 100"));
    mLabel700 = CreateVariationLabel(nullptr);
    root.Add(mLabel700);

    mField = InputField::New();
    mField.SetText(SAMPLE_TEXT);
    mField.SetFontFamily(VARIABLE_FONT_FAMILY);
    mField.SetFontSize(20.0f);
    mField.SetRequestedWidth(MATCH_PARENT);
    mField.SetBackgroundColor(UiColor(0xEFEFEF));
    mField.SetPadding(Extents(10, 10, 10, 10));
    root.Add(mField);

    window.Add(root);

    // Set font variation using FontVariationAxis API
    Dali::Vector<Text::FontVariationAxis> axes;
    axes.PushBack(Text::FontVariationAxis("wght", 700.0f));
    axes.PushBack(Text::FontVariationAxis("wdth", 100.0f));
    mLabel700.SetFontVariation(axes);
    mField.SetFontVariation(axes);

    // Print initial font variation settings
    PrintFontVariationInfo(mLabel200, "Weight 200 + Width 62.5");
    PrintFontVariationInfo(mLabel300, "Weight 300 + Width 65");
    PrintFontVariationInfo(mLabel400, "Weight 400 + Width 70");
    PrintFontVariationInfo(mLabel500, "Weight 500 + Width 80");
    PrintFontVariationInfo(mLabel600, "Weight 600 + Width 90");
    PrintFontVariationInfo(mLabel700, "Weight 700 + Width 100");
    PrintFontVariationInfo(mField, "Weight 700 + Width 100");

    window.KeyEventSignal().Connect(this, &FontVariationController::OnKeyEvent);
  }

  void PrintFontVariationInfo(Label label, const char* title)
  {
  }

  void PrintFontVariationInfo(InputField field, const char* title)
  {
  }

  void SetFontFamily(Dali::String family)
  {
    mLabel200.SetFontFamily(family);
    mLabel300.SetFontFamily(family);
    mLabel400.SetFontFamily(family);
    mLabel500.SetFontFamily(family);
    mLabel600.SetFontFamily(family);
    mLabel700.SetFontFamily(family);
    mField.SetFontFamily(family);
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
      // Clears font variation
      mLabel200.ClearFontVariation();
      mLabel300.ClearFontVariation();
      mLabel400.ClearFontVariation();
      mLabel500.ClearFontVariation();
      mLabel600.ClearFontVariation();
      mLabel700.ClearFontVariation();
      mField.ClearFontVariation();
      SetFontFamily(DEFAULT_FONT_FAMILY);
    }
    else if(event.GetKeyName() == "2")
    {
      // Change font variation using FontVariationAxis API
      Dali::Vector<Text::FontVariationAxis> axes;
      axes.PushBack(Text::FontVariationAxis("wght", 300.0f));
      axes.PushBack(Text::FontVariationAxis("wdth", 100.0f));

      mLabel200.SetFontVariation(axes);
      mLabel300.SetFontVariation(axes);
      mLabel400.SetFontVariation(axes);
      mLabel500.SetFontVariation(axes);
      mLabel600.SetFontVariation(axes);
      mLabel700.SetFontVariation(axes);
      mField.SetFontVariation(axes);
      SetFontFamily(VARIABLE_FONT_FAMILY);
    }
    else if(event.GetKeyName() == "3")
    {
      // Set font variation using string API
      mLabel200.SetFontVariation("wght=200,wdth=62.5");
      mLabel300.SetFontVariation("wght=300,wdth=65");
      mLabel400.SetFontVariation("wght=400,wdth=70");
      mLabel500.SetFontVariation("wght=500,wdth=80");
      mLabel600.SetFontVariation("wght=600,wdth=90");
      mLabel700.SetFontVariation("wght=700,wdth=100");
      mField.SetFontVariation("wght=700,wdth=100");
      SetFontFamily(VARIABLE_FONT_FAMILY);
    }
  }

private:
  Application& mApplication;
  Label        mLabel200;
  Label        mLabel300;
  Label        mLabel400;
  Label        mLabel500;
  Label        mLabel600;
  Label        mLabel700;
  InputField   mField;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  FontVariationController controller(application);
  application.MainLoop();

  return 0;
}
