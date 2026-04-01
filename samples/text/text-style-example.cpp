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
} // namespace

class TextController : public ConnectionTracker
{
public:
  explicit TextController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextController::OnInit);
  }

private:
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .Spacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetViewPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Children({
        Label::New("Text Style Example"),
        Label::New("Italic Lorem ipsum dolor sit amet.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSlant(Text::FontSlant::ITALIC)
          .SetFontSize(20)
          .As(mSlantLabel),
        Label::New("Bold Lorem ipsum dolor sit amet.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontWeight(Text::FontWeight::BOLD)
          .SetFontSize(20)
          .As(mWeightLabel),
        Label::New("Semi condensed Lorem ipsum dolor sit amet.")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontWidth(Text::FontWidth::SEMI_CONDENSED)
          .SetFontSize(20)
          .As(mWidthLabel),
        Label::New("Underline Label")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetUnderline(Text::Underline().SetColor(UiColor(0xFF0000))),
        Label::New("Dashed Underline Label")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetRequestedWidth(MATCH_PARENT)
          .As(mUnderlineLabel),
        Label::New("Underline Label")
          .SetBackgroundColor(UiColor(0xefefef))
          .SetViewPadding(Extents(10, 10, 10, 10))
          .SetFontSize(20)
          .SetUnderline(Text::Underline().SetThickness(1)),
        }));

    Text::Underline dashed = Text::Underline()
                             .SetColor(UiColor(0x0088FF))
                             .SetThickness(2)
                             .SetType(Text::Underline::Type::DASHED)
                             .SetDashLength(4.0f)
                             .SetDashGap(4.0f);
    mUnderlineLabel.SetUnderline(dashed);

    PrintLabelInfo(mSlantLabel, "Slant Label");
    PrintLabelInfo(mWeightLabel, "Weight Label");
    PrintLabelInfo(mWidthLabel, "Width Label");

    PlayAnimation(mUnderlineLabel, Dali::Color::RED, Dali::Color::BLUE);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  void PlayAnimation(Label label, Vector4 startColor, Vector4 endColor)
  {
    label.SetProperty(Label::Property::TEXT_COLOR, startColor);

    Animation animation = Animation::New(1.f);
    animation.AnimateTo(Property(label, Label::Property::TEXT_COLOR), endColor, AlphaFunction::SIN);
    animation.SetLoopCount(0);
    animation.Play();
  }

  void PrintLabelInfo(Label label, const char* title)
  {
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title);
    DALI_LOG_ERROR("Text       : %s\n", label.GetText().CStr());
    DALI_LOG_ERROR("FontWeight : %s\n", TextAbstraction::FontWeight::Name[label.GetFontWeight()]);
    DALI_LOG_ERROR("FontWidth  : %s\n", TextAbstraction::FontWidth::Name[label.GetFontWidth()]);
    DALI_LOG_ERROR("FontSlant  : %s\n", TextAbstraction::FontSlant::Name[label.GetFontSlant()]);
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() != KeyEvent::UP)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      DALI_LOG_ERROR("DALI_KEY_ESCAPE:%d, DALI_KEY_BACK:%d\n", IsKey(event, Dali::DALI_KEY_ESCAPE), IsKey(event, Dali::DALI_KEY_BACK));
      mApplication.Quit();
      return;
    }

    if(event.GetKeyName() == "1")
    {
      mUnderlineLabel.ResetUnderline();
    }
  }

private:
  Application& mApplication;
  Label        mWeightLabel;
  Label        mWidthLabel;
  Label        mSlantLabel;
  Label        mUnderlineLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
