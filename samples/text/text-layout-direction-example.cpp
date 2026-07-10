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

class TextLayoutController : public ConnectionTracker
{
public:
  explicit TextLayoutController(Application& application)
  : mApplication(application)
  {
    mApplication.InitSignal().Connect(this, &TextLayoutController::OnInit);
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

    StackLayout header = StackLayout::New(StackOrientation::HORIZONTAL);
    header.SetSpacing(STACK_SPACING);
    header.SetRequestedWidth(MATCH_PARENT);
    Label dirLabel = Label::New("Dir");
    dirLabel.SetRequestedWidth(40.0f);
    Label modeLabel = Label::New("Mode");
    modeLabel.SetRequestedWidth(100.0f);
    header.Add(dirLabel);
    header.Add(modeLabel);
    header.Add(Label::New("Result"));

    root.Add(Label::New("Text Layout Direction Mode Example"));
    root.Add(header);
    root.Add(CreateLayoutDirection("LTR", "CONTENTS", "Hello world", Text::LayoutDirectionMode::CONTENTS));
    root.Add(CreateLayoutDirection("LTR", "INHERIT", "Hello world", Text::LayoutDirectionMode::INHERIT));
    root.Add(CreateLayoutDirection("LTR", "LOCALE", "Hello world", Text::LayoutDirectionMode::LOCALE));
    root.Add(CreateLayoutDirection("RTL", "CONTENTS", "שלום עולם", Text::LayoutDirectionMode::CONTENTS));
    root.Add(CreateLayoutDirection("RTL", "INHERIT", "שלום עולם", Text::LayoutDirectionMode::INHERIT));
    root.Add(CreateLayoutDirection("RTL", "LOCALE", "שלום עולם", Text::LayoutDirectionMode::LOCALE));
    root.Add(CreateLayoutDirection("MIX", "CONTENTS", "שלום עולם, Hello world", Text::LayoutDirectionMode::CONTENTS));
    root.Add(CreateLayoutDirection("MIX", "INHERIT", "שלום עולם, Hello world", Text::LayoutDirectionMode::INHERIT));
    root.Add(CreateLayoutDirection("MIX", "LOCALE", "שלום עולם, Hello world", Text::LayoutDirectionMode::LOCALE));
    window.Add(root);

    window.KeyEventSignal().Connect(this, &TextLayoutController::OnKeyEvent);
  }

  View CreateLayoutDirection(Dali::String textTypeDesc, Dali::String layoutModeDesc, Dali::String text, Text::LayoutDirectionMode mode)
  {
    StackLayout row = StackLayout::New(StackOrientation::HORIZONTAL);
    row.SetSpacing(STACK_SPACING);
    row.SetRequestedWidth(MATCH_PARENT);
    Label textTypeLabel = Label::New(textTypeDesc);
    textTypeLabel.SetRequestedWidth(40.0f);
    Label layoutModeLabel = Label::New(layoutModeDesc);
    layoutModeLabel.SetRequestedWidth(100.0f);
    Label resultLabel = Label::New(text);
    resultLabel.SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f));
    resultLabel.SetLayoutDirectionMode(mode);
    resultLabel.SetTextColor(UiColor(0xEFEFEF));
    resultLabel.SetBackgroundColor(UiColor(0x020202));
    row.Add(textTypeLabel);
    row.Add(layoutModeLabel);
    row.Add(resultLabel);
    return row;
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
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application application = Application::New(&argc, &argv);
  UiConfig::New().Apply();

  TextLayoutController controller(application);
  application.MainLoop();

  return 0;
}
