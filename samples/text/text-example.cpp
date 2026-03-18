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
#include <dali-ui-foundation/public-api/stack-layout.h>

using namespace Dali;
using namespace Dali::Ui;

/**
 * StackLayout sample: a vertical stack with fixed-height and weighted children.
 * - Root is a StackLayout that fills the window.
 * - Three rows: top (fixed), middle (weight 1), bottom (fixed).
 * - Press Escape or Back to quit.
 */
class StackLayoutController : public ConnectionTracker
{
public:
  StackLayoutController(Application& application)
    : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &StackLayoutController::Create);
  }

  void Create(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(Color::WHITE);

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .Spacing(10.0f)
      .SetLayoutWidth(LayoutDimension::MatchParent)
      .SetLayoutHeight(LayoutDimension::MatchParent)
      .SetViewPadding(Extents(20, 20, 20, 20))
      .Contents({
        Label::New("Simple Label")
          .SetFontSize(20),

        CreateLabel("Hello world", "SamsungOneUI_400", 40.0f)
          .SetLayoutWidth(LayoutDimension::WrapContent)
          .SetLayoutHeight(100.0f)
          .SetViewPadding(Extents(10, 10, 10, 10))
          .BackgroundColor(Color::RED)
          .SetTextColor(Color::ORANGE)
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER)
          .As(mLabel),

        CreateLabel("Hello world, this is a multi-line enabled long long text", "Ubuntu Mono", 30.0f)
          .SetMinimumWidth(100)
          .SetMinimumHeight(100)
          .SetMaximumWidth(600)
          .SetMaximumHeight(300)
          .SetViewPadding(Extents(20, 20, 20, 20))
          .BackgroundColor(Color::DARK_GRAY)
          .SetTextColor(Color::RED)
          .SetMultiLine(true)
          .As(mLabel2),

        CreateLabel("변화는 한 순간에 일어나지 않습니다. 매일의 작은 실천이 모여 지속가능한 삶을 이루는 것. 이것이 우리가 꿈꾸는 지속가능성입니다.", "SamsungOneUI_700", 20.0f)
          .SetLayoutWidth(LayoutDimension::MatchParent)
          .SetLayoutHeight(150.0f)
          .SetViewPadding(Extents(20, 20, 20, 20))
          .BackgroundColor(Color::BLACK)
          .SetTextColor(Color::WHITE)
          .SetMultiLine(true)
          .As(mLabel3),

         View::New()
          .BackgroundColor(Color::BLUE)
          .SetLayoutWidth(LayoutDimension::MatchParent)
          .SetLayoutHeight(30.0f),

        Label::New("Label Alignment")
          .BackgroundColor(Color::GRAY)
          .SetFontSize(10)
          .SetLayoutWidth(LayoutDimension::MatchParent)
          .SetLayoutHeight(30.0f)
          .SetHorizontalTextAlignment(Text::Alignment::START)
          .SetVerticalTextAlignment(Text::Alignment::START),

        Label::New("Label Alignment")
          .BackgroundColor(Color::GRAY)
          .SetFontSize(10)
          .SetLayoutWidth(LayoutDimension::MatchParent)
          .SetLayoutHeight(30.0f)
          .SetHorizontalTextAlignment(Text::Alignment::CENTER)
          .SetVerticalTextAlignment(Text::Alignment::CENTER),

        Label::New("Label Alignment")
          .BackgroundColor(Color::GRAY)
          .SetFontSize(10)
          .SetLayoutWidth(LayoutDimension::MatchParent)
          .SetLayoutHeight(30.0f)
          .SetHorizontalTextAlignment(Text::Alignment::END)
          .SetVerticalTextAlignment(Text::Alignment::END),
        }));


    Print(mLabel, "Label 1");
    Print(mLabel2, "Label 2");
    Print(mLabel3, "Label 3");

    window.KeyEventSignal().Connect(this, &StackLayoutController::OnKeyEvent);
  }

  Label CreateLabel(Dali::String text, Dali::String fontFamily, float fontSize)
  {
    return Label::New()
          .SetText(text)
          .SetFontFamily(fontFamily)
          .SetFontSize(fontSize);
  }

  void Print(Label label, Dali::String title)
  {
    DALI_LOG_ERROR("----------------------------------------------------------------\n");
    DALI_LOG_ERROR("%s\n", title.CStr());

    DALI_LOG_ERROR("Text       : %s\n", label.GetText().CStr());
    DALI_LOG_ERROR("FontFamily : %s\n", label.GetFontFamily().CStr());
    DALI_LOG_ERROR("Fontsize   : %f\n", label.GetFontSize());
    DALI_LOG_ERROR("MultiLine  : %d\n", label.IsMultiLine());
    DALI_LOG_ERROR("TextColor  : %.2f, %.2f, %.2f, %.2f\n", label.GetTextColor().r, label.GetTextColor().g, label.GetTextColor().b, label.GetTextColor().a);
    DALI_LOG_ERROR("H Align    : %d\n", label.GetHorizontalTextAlignment());
    DALI_LOG_ERROR("V Align    : %d\n", label.GetVerticalTextAlignment());

    DALI_LOG_ERROR("Natural    : %f\n", label.GetNaturalSize().x, label.GetNaturalSize().y);
    DALI_LOG_ERROR("H for W    : 100, %f\n", label.GetHeightForWidth(100));

    DALI_LOG_ERROR("----------------------------------------------------------------\n");
  }

  void OnKeyEvent(const KeyEvent& event)
  {
    if (event.GetState() == KeyEvent::DOWN)
    {
      if (IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
      {
        mApplication.Quit();
      }
    }
    else if (event.GetState() == KeyEvent::UP)
    {
      if (event.GetKeyName() == "1")
      {
        if (mLabel.GetTextColor() == Color::ORANGE)
        {
          mLabel.BackgroundColor(Color::ORANGE);
          mLabel.SetTextColor(Color::RED);
          mLabel2.BackgroundColor(Color::RED);
          mLabel2.SetTextColor(Color::DARK_GRAY);          
          mLabel3.BackgroundColor(Color::WHITE);
          mLabel3.SetTextColor(Color::BLACK);
        }
        else
        {
          mLabel.BackgroundColor(Color::RED);
          mLabel.SetTextColor(Color::ORANGE);
          mLabel2.BackgroundColor(Color::DARK_GRAY);
          mLabel2.SetTextColor(Color::RED);          
          mLabel3.BackgroundColor(Color::BLACK);
          mLabel3.SetTextColor(Color::WHITE);
        }
      }
    }
  }

private:
  Application& mApplication;
  Label mLabel;
  Label mLabel2;
  Label mLabel3;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  StackLayoutController controller(application);
  application.MainLoop();
  return 0;
}
