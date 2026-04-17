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
#include <dali/integration-api/debug.h>
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
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .SetSpacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("Font Variation Example")
          .SetFontSize(20.0f),

        Label::New("Press '1' to clear, '2' to change variation, '3' to set via string")
          .SetFontSize(14.0f)
          .SetBackgroundColor(UiColor(0xE0E0E0))
          .SetPadding(Extents(10, 10, 10, 10)),

        Label::New("Weight 200 + Width 62.5")
          .SetFontSize(16.0f)
          .SetBackgroundColor(UiColor(0xF0F0F0))
          .SetPadding(Extents(10, 10, 10, 10)),

        Label::New(SAMPLE_TEXT)
          .SetFontFamily(VARIABLE_FONT_FAMILY)
          .SetFontVariation("wght=200,wdth=62.5")
          .SetFontSize(20.0f)
          .SetRequestedWidth(MATCH_PARENT)
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .SetPadding(Extents(10, 10, 10, 10))
          .As(mLabel200),

        Label::New("Weight 300 + Width 65")
          .SetFontSize(16.0f)
          .SetBackgroundColor(UiColor(0xF0F0F0))
          .SetPadding(Extents(10, 10, 10, 10)),

        Label::New(SAMPLE_TEXT)
          .SetFontFamily(VARIABLE_FONT_FAMILY)
          .SetFontVariation("wght=300,wdth=65")
          .SetFontSize(20.0f)
          .SetRequestedWidth(MATCH_PARENT)
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .SetPadding(Extents(10, 10, 10, 10))
          .As(mLabel300),

        Label::New("Weight 400 + Width 70")
          .SetFontSize(16.0f)
          .SetBackgroundColor(UiColor(0xF0F0F0))
          .SetPadding(Extents(10, 10, 10, 10)),

        Label::New(SAMPLE_TEXT)
          .SetFontFamily(VARIABLE_FONT_FAMILY)
          .SetFontVariation("wght=400,wdth=70")
          .SetFontSize(20.0f)
          .SetRequestedWidth(MATCH_PARENT)
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .SetPadding(Extents(10, 10, 10, 10))
          .As(mLabel400),

        // Width variation
        Label::New("Weight 500 + Width 80")
          .SetFontSize(16.0f)
          .SetBackgroundColor(UiColor(0xF0F0F0))
          .SetPadding(Extents(10, 10, 10, 10)),

        Label::New(SAMPLE_TEXT)
          .SetFontFamily(VARIABLE_FONT_FAMILY)
          .SetFontVariation("wght=500,wdth=80")
          .SetFontSize(20.0f)
          .SetRequestedWidth(MATCH_PARENT)
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .SetPadding(Extents(10, 10, 10, 10))
          .As(mLabel500),

        Label::New("Weight 600 + Width 90")
          .SetFontSize(16.0f)
          .SetBackgroundColor(UiColor(0xF0F0F0))
          .SetPadding(Extents(10, 10, 10, 10)),

        Label::New(SAMPLE_TEXT)
          .SetFontFamily(VARIABLE_FONT_FAMILY)
          .SetFontVariation("wght=600,wdth=90")
          .SetFontSize(20.0f)
          .SetRequestedWidth(MATCH_PARENT)
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .SetPadding(Extents(10, 10, 10, 10))
          .As(mLabel600),

        // Using FontVariationAxis API
        Label::New("Weight 700 + Width 100")
          .SetFontSize(16.0f)
          .SetBackgroundColor(UiColor(0xF0F0F0))
          .SetPadding(Extents(10, 10, 10, 10)),

        Label::New(SAMPLE_TEXT)
          .SetFontFamily(VARIABLE_FONT_FAMILY)
          .SetFontSize(20.0f)
          .SetRequestedWidth(MATCH_PARENT)
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .SetPadding(Extents(10, 10, 10, 10))
          .As(mLabel700),

        InputField::New()
          .SetText(SAMPLE_TEXT)
          .SetFontFamily(VARIABLE_FONT_FAMILY)
          .SetFontSize(20.0f)
          .SetRequestedWidth(MATCH_PARENT)
          .SetBackgroundColor(UiColor(0xEFEFEF))
          .SetPadding(Extents(10, 10, 10, 10))
          .As(mField)
        }));

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
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title);
    DALI_LOG_ERROR("Text            : %s\n", label.GetText().CStr());

    Dali::Vector<Text::FontVariationAxis> axes = label.GetFontVariation();
    Dali::String                          variationString = Text::FontVariation::ToString(axes);
    DALI_LOG_ERROR("FontVariation   : %s\n", variationString.CStr());

    for(std::size_t i = 0u; i < axes.Count(); i++)
    {
      DALI_LOG_ERROR("  Axis[%d]       : tag='%s', value=%.1f\n",
                     i, axes[i].GetTag().CStr(), axes[i].GetValue());
    }
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

  void PrintFontVariationInfo(InputField field, const char* title)
  {
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title);
    DALI_LOG_ERROR("Text            : %s\n", field.GetText().CStr());

    Dali::Vector<Text::FontVariationAxis> axes = field.GetFontVariation();
    Dali::String                          variationString = Text::FontVariation::ToString(axes);
    DALI_LOG_ERROR("FontVariation   : %s\n", variationString.CStr());

    for(std::size_t i = 0u; i < axes.Count(); i++)
    {
      DALI_LOG_ERROR("  Axis[%d]       : tag='%s', value=%.1f\n",
                     i, axes[i].GetTag().CStr(), axes[i].GetValue());
    }
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
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

  void OnKeyEvent(const KeyEvent& event)
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

      DALI_LOG_ERROR("Cleared all font variations\n");
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

      DALI_LOG_ERROR("Changed all font variations to wght=300, wdth=100\n");
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

      DALI_LOG_ERROR("Changed all font variations via string API\n");
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