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
  void OnInit(Application application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    StackLayout root = StackLayout::New(StackOrientation::VERTICAL);
    root.SetSpacing(STACK_SPACING);
    root.SetRequestedWidth(MATCH_PARENT);
    root.SetRequestedHeight(MATCH_PARENT);
    root.SetPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING));

    mAnchorLabel      = CreateStyledTextLabel("클릭하면 사이트로 이동 합니다: <a href='https://www.samsung.com'>Samsung Electronics</a>");
    mAnchorColorLabel = CreateStyledTextLabel("색깔이 다른 Anchor: <a href='https://research.samsung.com'>Samsung Research</a>");
    root.AddChildren({
      Label::New("Text Markup Example"),
      CreateStyledTextLabel("<font size='24' family='SamsungOneUI_700'>Markup </font><color value='red'>Text</color>"),
      mAnchorLabel,
      mAnchorColorLabel,
    });
    window.Add(root);

    mAnchorColorLabel.SetAnchorColor(UiColor(0xFF0000));
    mAnchorColorLabel.SetAnchorClickedColor(UiColor(0x00FF00));

    PrintLabelInfo(mAnchorLabel, "Anchor Label 1");
    PrintLabelInfo(mAnchorColorLabel, "Anchor Label 2");

    mAnchorLabel.AnchorClickedSignal().Connect(this, &TextController::OnAnchorClicked);
    mAnchorColorLabel.AnchorClickedSignal().Connect(this, &TextController::OnAnchorClicked);

    window.KeyEventSignal().Connect(this, &TextController::OnKeyEvent);
  }

  void OnAnchorClicked(View view, const Dali::String& href)
  {
    Label label = Label::DownCast(view);
    if(label)
    {
    }
  }

  Label CreateStyledTextLabel(Dali::String text)
  {
    Label label = Label::New();
    label.SetStyledText(Text::StyledText::FromMarkup(text));
    label.SetFontSize(20.0f);
    label.SetFontFamily("SamsungOneUI_400");
    return label;
  }

  void PrintLabelInfo(Label label, const char* title)
  {
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
    }
  }

private:
  Application& mApplication;
  Label        mAnchorLabel;
  Label        mAnchorColorLabel;
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextController controller(application);
  application.MainLoop();

  return 0;
}
