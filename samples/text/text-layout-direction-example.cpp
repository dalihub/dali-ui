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
#include <dali-ui-foundation/public-api/stack-layout-params.h>

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
  void OnInit(Application& application)
  {
    Window window = application.GetWindow();
    window.SetBackgroundColor(UiColor(0xFFFFFF));

    window.Add(StackLayout::New(StackOrientation::VERTICAL)
      .Spacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .SetRequestedHeight(MATCH_PARENT)
      .SetViewPadding(Extents(STACK_PADDING, STACK_PADDING, STACK_PADDING, STACK_PADDING))
      .Contents({
        Label::New("Text Layout Direction Mode Example"),
        StackLayout::New(StackOrientation::HORIZONTAL)
        .Spacing(STACK_SPACING)
        .SetRequestedWidth(MATCH_PARENT)
        .Contents({
          Label::New("Dir")
          .SetRequestedWidth(40.0f),
          Label::New("Mode")
          .SetRequestedWidth(100.0f),
          Label::New("Result")
        }),
        CreateLayoutDirection("LTR", "CONTENTS", "Hello world", Text::LayoutDirectionMode::CONTENTS),
        CreateLayoutDirection("LTR", "INHERIT", "Hello world", Text::LayoutDirectionMode::INHERIT),
        CreateLayoutDirection("LTR", "LOCALE", "Hello world", Text::LayoutDirectionMode::LOCALE),
        CreateLayoutDirection("RTL", "CONTENTS", "שלום עולם", Text::LayoutDirectionMode::CONTENTS),
        CreateLayoutDirection("RTL", "INHERIT", "שלום עולם", Text::LayoutDirectionMode::INHERIT),
        CreateLayoutDirection("RTL", "LOCALE", "שלום עולם", Text::LayoutDirectionMode::LOCALE),
        CreateLayoutDirection("MIX", "CONTENTS", "שלום עולם, Hello world", Text::LayoutDirectionMode::CONTENTS),
        CreateLayoutDirection("MIX", "INHERIT", "שלום עולם, Hello world", Text::LayoutDirectionMode::INHERIT),
        CreateLayoutDirection("MIX", "LOCALE", "שלום עולם, Hello world", Text::LayoutDirectionMode::LOCALE),
      })
    );

    window.KeyEventSignal().Connect(this, &TextLayoutController::OnKeyEvent);
  }

  View CreateLayoutDirection(Dali::String textTypeDesc, Dali::String layoutModeDesc, Dali::String text, Text::LayoutDirectionMode mode)
  {
    return StackLayout::New(StackOrientation::HORIZONTAL)
      .Spacing(STACK_SPACING)
      .SetRequestedWidth(MATCH_PARENT)
      .Contents({
        Label::New(textTypeDesc)
        .SetRequestedWidth(40.0f),
        Label::New(layoutModeDesc)
        .SetRequestedWidth(100.0f),
        Label::New(text)
        .SetLayoutParams(StackLayoutParams::New().SetWeight(1.0f))
        .SetLayoutDirectionMode(mode)
        .SetTextColor(UiColor(0xEFEFEF))
        .SetBackgroundColor(UiColor(0x020202))
      });
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
